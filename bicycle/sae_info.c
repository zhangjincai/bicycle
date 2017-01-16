#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "configs.h"
#include "lib_general.h"
#include "lib_zmalloc.h"
#include "sae_info.h"


#define BLK_BIT_ARRAY_N			lib_bit_to_byte(CONFS_STAKE_NUM_MAX + 1)


#define H_SWAP_L(A) 	(A&0x01)<<7 | (A&0x02) <<5 | \
	(A&0x04)<<3 | (A&0x08)<<1 | \
	(A&0x10)>>1 | (A&0x20)>>3 | \
	(A&0x40)>>5 | (A&0x80)>>7

/*
 * 记录锁桩信息
 */
struct sae_info_statistics
{
	unsigned char s_stat[17];			 			//锁桩全部状态
	unsigned char s_bicycle_exist_stat[17]; 			//车是否在桩状态
	unsigned char max;     							//支持最大锁桩数
	unsigned char online;    							//在线锁桩数
	unsigned char quantity;							//锁桩数量
	
	unsigned short status[CONFS_STAKE_NUM_MAX + 1];  	//存放锁桩状态
	
	lib_mutex_t *mutex;
	struct sae_info *info;
}__attribute__((packed));
typedef struct sae_info_statistics sae_info_statistics_t;


static sae_info_statistics_t sae_statistics;  				//锁桩信息统计


int sae_info_init(const unsigned char max, const unsigned char quantity)
{
	int i;
	
	memset(&sae_statistics, 0, sizeof(struct sae_info_statistics));
	
	sae_statistics.mutex = lib_mutex_create();
	if(sae_statistics.mutex == NULL)
		goto ERR;

	sae_statistics.info = (struct sae_info *)lib_zmalloc(sizeof(struct sae_info) * max);
	if(sae_statistics.info == NULL)
		goto ERR;

	sae_statistics.max = max;
	sae_statistics.quantity = quantity;
	sae_statistics.online = 0;
	memset(sae_statistics.s_stat, 0, sizeof(sae_statistics.s_stat));
	memset(sae_statistics.s_bicycle_exist_stat, 0, sizeof(sae_statistics.s_bicycle_exist_stat));

	for(i = 0; i < max; i++)
	{
		memset(&(sae_statistics.info[i].status), 0xfb, 2); 
	}

	for(i = 0; i < CONFS_STAKE_NUM_MAX + 1; i++)
	{
		memset(&(sae_statistics.status[i]), 0xfb, 2);  //BIT2为0,表示无车
	}
	
	return B_DEV_EOK;
ERR:
	if(sae_statistics.info == NULL)
		lib_zfree(sae_statistics.info);
	
	lib_mutex_destroy(sae_statistics.mutex);
	return B_DEV_ERROR;
}

void sae_info_destroy(void)
{
	if(sae_statistics.info == NULL)
		lib_zfree(sae_statistics.info);
	
	lib_mutex_destroy(sae_statistics.mutex);	
}

void sae_info_put(const unsigned char id, sae_info_t *info)
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);

	if(sae_statistics.info[idx].healthy_flag == 0)
		sae_statistics.online++;

	memcpy(&(sae_statistics.info[idx]), info, sizeof(sae_info_t));
	lib_set_bit(sae_statistics.s_stat, idx);  //按"位"设置桩机状态
	
	lib_mutex_unlock(sae_statistics.mutex);
}

void sae_info_del(const unsigned char id)
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);
	memset(&(sae_statistics.info[idx]), 0, sizeof(sae_info_t));
	
	if(sae_statistics.info[idx].healthy_flag == 1)
		sae_statistics.online--;
	
	sae_statistics.info[idx].healthy_flag = 0;  //锁桩不存在
	lib_clr_bit(sae_statistics.s_stat, idx);  //按"位"清除桩机状态
	lib_clr_bit(sae_statistics.s_bicycle_exist_stat, idx); //新增,为了兼容延迟还车
	
	lib_mutex_unlock(sae_statistics.mutex);
}

void sae_info_update(const unsigned char id, const unsigned short stat, const unsigned char s_sn[6])
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.info[idx].send_hb_times = 0;
	sae_statistics.info[idx].status = stat;
	strcpy(sae_statistics.info[idx].s_sn, s_sn);
	lib_mutex_unlock(sae_statistics.mutex);
}

void sae_info_send_hb_times_inc(const unsigned char id)
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.info[idx].send_hb_times++;
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_send_hb_times_inc_all(const unsigned char s_max)
{
	int i; 

	lib_mutex_lock(sae_statistics.mutex);
	for(i = 1; i < s_max; i++)
	{
		sae_statistics.info[i].send_hb_times++;
	}
	lib_mutex_unlock(sae_statistics.mutex);	
}

sae_info_t *sae_info_get_by_id(const unsigned char id)
{
	unsigned char idx = id;
	sae_info_t *info = NULL;
	 
	lib_mutex_lock(sae_statistics.mutex);
	info = &(sae_statistics.info[idx]);
	lib_mutex_unlock(sae_statistics.mutex);	

	return info;
}

void sae_info_set_quantity(const unsigned char quantity)
{
	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.quantity = quantity;
	lib_mutex_unlock(sae_statistics.mutex);		
}

unsigned char sae_info_get_quantity(void)
{
	unsigned char quantity = 0;
	
	lib_mutex_lock(sae_statistics.mutex);
	quantity = sae_statistics.quantity;
	lib_mutex_unlock(sae_statistics.mutex);	

	return quantity;
}

unsigned char sae_info_get_online(void)
{
	unsigned char online = 0;
	
	lib_mutex_lock(sae_statistics.mutex);
	online = sae_statistics.online;
	lib_mutex_unlock(sae_statistics.mutex);

	return online;
}

unsigned char sae_info_get_register_count(void)
{
	int i;
	unsigned char online = 0;
	unsigned char quantity = 0;
	
	lib_mutex_lock(sae_statistics.mutex);

	quantity = sae_statistics.quantity;
	for(i = 1; i <= quantity; i++)
	{
		if(lib_chk_bit(sae_statistics.s_stat, i) == 1)  //签到数
			online++;
	}

	lib_mutex_unlock(sae_statistics.mutex);

	return online;	
}

unsigned char sae_info_get_bicycle_online(void)
{
	int i;
	unsigned char online = 0;
	unsigned char quantity = 0;
	
	lib_mutex_lock(sae_statistics.mutex);

	quantity = sae_statistics.quantity;
	for(i = 1; i <= quantity; i++)
	{
		if(lib_chk_bit(sae_statistics.s_bicycle_exist_stat, i) == 1)  //在线
			online++;
	}

	lib_mutex_unlock(sae_statistics.mutex);

	return online;
}

void sae_info_set_online(const unsigned char id)
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	
	if(sae_statistics.info[idx].healthy_flag == 0)  //?
	{
		sae_statistics.online++;
	}
	
	lib_mutex_unlock(sae_statistics.mutex);
}

void sae_info_set_register(const unsigned char id)
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);

	sae_statistics.info[idx].healthy_flag = 1;  //锁桩存在
	lib_set_bit(sae_statistics.s_stat, idx);  //按"位"设置桩机状态
	
	lib_mutex_unlock(sae_statistics.mutex);
}

void sae_info_get_stat(unsigned char s_stat[17])
{
	lib_mutex_lock(sae_statistics.mutex);
	memcpy(s_stat, sae_statistics.s_stat, 17);
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_get_stat_for_center(unsigned char s_stat[17])
{
	unsigned char stat[17] = {0};	
	
	lib_mutex_lock(sae_statistics.mutex);
	memcpy(stat, sae_statistics.s_stat, 17);
	lib_mutex_unlock(sae_statistics.mutex);	

	int i;
	for(i = 0; i < 17; i++)
	{
		s_stat[i] = H_SWAP_L(stat[i]); //?
	}
}

void sae_info_get_bicycle_exist_stat(unsigned char s_stat[17])
{
	lib_mutex_lock(sae_statistics.mutex);
	memcpy(s_stat, sae_statistics.s_bicycle_exist_stat, 17);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_id(const unsigned char id, unsigned char s_id[4])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_id, s_id);
	sae_statistics.info[idx].id = id;
	sae_statistics.info[idx].send_hb_times = 0; //发送次清0
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_send_hb_times_zero(const unsigned char id)
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.info[idx].send_hb_times = 0; //发送次清0
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_set_ver(const unsigned char id, unsigned char s_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_ver, s_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_lock_id(const unsigned char id, unsigned char s_lock_id[4])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_lock_id, s_lock_id);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_lock_ver(const unsigned char id, unsigned char s_lock_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_lock_ver, s_lock_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_sn(const unsigned char id, unsigned char s_sn[7])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_sn, s_sn);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_total_blk_ver(const unsigned char id, unsigned char s_total_bl_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_total_bl_ver, s_total_bl_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_inc_blk_ver(const unsigned char id, unsigned char s_inc_bl_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_inc_bl_ver, s_inc_bl_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_dec_blk_ver(const unsigned char id, unsigned char s_dec_bl_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_dec_bl_ver, s_dec_bl_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_temp_blk_ver(const unsigned char id, unsigned char s_temp_bl_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_temp_bl_ver, s_temp_bl_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_stake_para_ver(const unsigned char id, unsigned char s_stake_para_ver[9])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_stake_para, s_stake_para_ver);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_set_psam(const unsigned char id, unsigned char s_psam[7])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_psam, s_psam);
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_set_single_stat(const unsigned char id, unsigned short stat)
{
	unsigned char idx = id;
	sae_status_t status;
		
	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.info[idx].status = stat;

	memcpy(&status, &stat, 2); //保存车是否在桩的状态
	if(status.bike == 1) //有车
		lib_set_bit(sae_statistics.s_bicycle_exist_stat, idx); 
	else if(status.bike == 0) //无车
		lib_clr_bit(sae_statistics.s_bicycle_exist_stat, idx); 
	
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_set_stat(const unsigned char id, unsigned short stat)
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.status[idx] = stat;
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_set_time(const unsigned char id, unsigned char s_time[13])
{
	unsigned char idx = id;
	
	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_time, s_time);
	lib_mutex_unlock(sae_statistics.mutex);		
}

void sae_info_get_all_statistics(void *statistics)
{
	lib_mutex_lock(sae_statistics.mutex);
	memcpy(statistics, &sae_statistics, sizeof(sae_info_statistics_t) - 8);
	lib_mutex_unlock(sae_statistics.mutex);	
}

void sae_info_get_all_info(struct sae_info *info)
{
	lib_mutex_lock(sae_statistics.mutex);
	memcpy(info, sae_statistics.info, sizeof(struct sae_info) * (CONFS_STAKE_NUM_MAX + 1));
	lib_mutex_unlock(sae_statistics.mutex);
}

void sae_info_set_register_time(const unsigned char id)
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);
	sae_statistics.info[idx].s_reg_time = time(NULL);
	lib_mutex_unlock(sae_statistics.mutex);		
}

unsigned char sae_info_get_online_count(void)
{
	int i;
	unsigned char count = 0;
	
	lib_mutex_lock(sae_statistics.mutex);

	for(i = 1; i < CONFS_STAKE_NUM_MAX + 1; i++)
	{
		if(lib_chk_bit(sae_statistics.s_stat, i) == 1)
			count++;
	}
	
	lib_mutex_unlock(sae_statistics.mutex);	

	return count;
}

void sae_info_set_phy_sn(const unsigned char id, unsigned char phy_sn[9])
{
	unsigned char idx = id;

	lib_mutex_lock(sae_statistics.mutex);
	strcpy(sae_statistics.info[idx].s_phy_sn, phy_sn);
	lib_mutex_unlock(sae_statistics.mutex);		
}










