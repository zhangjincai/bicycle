#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>

#include "lib_general.h"
#include "defines.h"
#include "device_config.h"
#include "var_data.h"
#include "sae_info.h"
#include "fn_hash.h"
#include "universal_file.h"
#include "utils.h"
#include "node_device.h"
#include "fn_stake.h"

//#define FN_DEF

#ifdef FN_DEF
#define FN_DBG(fmt, args...)	fprintf(stderr, fmt, ##args)
#else
#define FN_DBG(fmt, args...)
#endif

int fn_sae_reg(fn_stake_val_t *val);
int fn_sae_n_reg(fn_stake_val_t *val);
int fn_sae_n_bheart(fn_stake_val_t *val);
int fn_sae_id(fn_stake_val_t *val);
int fn_sae_ver(fn_stake_val_t *val);
int fn_sae_lock_id(fn_stake_val_t *val);
int fn_sae_lock_ver(fn_stake_val_t *val);
int fn_sae_psam(fn_stake_val_t *val);
int fn_sae_sn(fn_stake_val_t *val);
int fn_sae_total_ver(fn_stake_val_t *val);
int fn_sae_inc_ver(fn_stake_val_t *val);
int fn_sae_dec_ver(fn_stake_val_t *val);
int fn_sae_temp_ver(fn_stake_val_t *val);
int fn_sae_time(fn_stake_val_t *val);
int fn_sae_n_id(fn_stake_val_t *val);
int fn_sae_status(fn_stake_val_t *val);
int fn_sae_n_status(fn_stake_val_t *val);
int fn_sae_n_time(fn_stake_val_t *val);
int fn_sae_n_mac(fn_stake_val_t *val);
int fn_sae_phy_sn(fn_stake_val_t *val);
int fn_sae_control(fn_stake_val_t *val);
int fn_sae_para_ver(fn_stake_val_t *val);


/* 锁桩主动签到, 节点机应答 */
int fn_sae_reg(fn_stake_val_t *val)
{
	unsigned char id = val->s_addr;
	
	FN_DBG("fn_sae_reg:%s, id:%d\n", SAE_REG_W, id);

	fprintf(stderr, "fn_sae_reg:%s, id:%d\n", SAE_REG_W, id);

	device_config_t conf;
	char s_time[13] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};
	unsigned short nstat;
	unsigned char term_id[6] = {0};
			
	sae_info_set_register_time(id);  //注册时间
	sae_info_set_send_hb_times_zero(id);   //发送次数清0
	sae_info_set_online(id);  //设置锁桩存在  sae_info_set_online要先于sae_info_set_register执行
	sae_info_set_register(id); //设置注册

	device_config_get(&conf);
	utils_get_sys_time_s(s_time);
	ndev_info_stat_NOR_get(&nstat);

	s_var_data_t *var = (s_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_TRUE)  //带?号
	{	
		if(var != NULL)
		{
			s_var_data_set(var, SAE_REG_W, 2);
			s_var_data_set(var, "00", 2);
			s_var_data_char(var, '&');
		}
	}
	else if(val->sel == FN_SEL_FLASE)  //应答
	{	
		if(var != NULL)
		{
			s_var_data_set(var, SAE_REG_W, 2);  //应答
			s_var_data_set(var, "00", 2);
			s_var_data_char(var, '&');

			s_var_data_set(var, SAE_N_ID_W, 2);  //节点机编号
			sprintf(term_id, "%05d", conf.nconf.term_id); //转字符
			s_var_data_set(var, term_id, 5);
			s_var_data_char(var, '&');

			s_var_data_set(var, SAE_N_STATUS_W, 2);  //节点机状态
			s_var_data_b64_set(var, &nstat, 2);
			s_var_data_char(var, '&');

			s_var_data_set(var, SAE_N_TIME_W, 2);		//节点机时间
			s_var_data_set(var, s_time, 12);
			s_var_data_char(var, '&');

			lib_get_macaddr("eth1", macaddr);  //节点机MAC 地址
			lib_hex_to_str((unsigned char *)macaddr, 6, (unsigned char *)s_macaddr);
			s_var_data_set(var, SAE_N_MAC_W, 2);	
			s_var_data_set(var, s_macaddr, 12);
			s_var_data_char(var, '&');


			val->ack = 1;  //应答标志
		}
	}
		
	return B_DEV_EOK;
}

/* 节点机主动签到和桩机应答 */
int fn_sae_n_reg(fn_stake_val_t *val)
{
	unsigned char id = val->s_addr;
	
	FN_DBG("fn_sae_n_reg:%s, id:%d\n", SAE_N_REG_W, id);

	sae_info_set_register_time(id);  //注册时间
	sae_info_set_online(id);  //设置锁桩存在 sae_info_set_online要先于sae_info_set_register执行
	sae_info_set_register(id); //设置注册
	sae_info_set_send_hb_times_zero(id);   //发送次数清0
	
	return B_DEV_EOK;
}

/* 节点机主动心跳和桩机心跳应答 */
int fn_sae_n_bheart(fn_stake_val_t *val)  
{
	unsigned char id = val->s_addr;
	
	FN_DBG("fn_sae_n_bheart:%s, id:%d\n", SAE_N_BHEART_W, id);
	
	//fprintf(stderr, "fn_sae_n_bheart:%s, id:%d\n", SAE_N_BHEART_W, id);

	sae_info_set_send_hb_times_zero(id);   //发送次数清0
	

	return B_DEV_EOK;
}

int fn_sae_id(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_id:%s\n", SAE_ID_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_id(id, val->data);
	}

	//fprintf(stderr, "s_addr = %d\n", val->s_addr);

	return B_DEV_EOK;
}

int fn_sae_ver(fn_stake_val_t *val)
{	
	FN_DBG("fn_sae_ver:%s\n", SAE_VER_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		//fprintf(stderr, "%s:%s\n", SAE_VER_W, val->data);
		
		sae_info_set_ver(id, val->data);
	}
	
#if 0
	else if(val->sel == FN_SEL_TRUE)
	{
		sae_info_t *info = sae_info_get_by_id(id);
		if(info != NULL)
		{
			s_var_data_t *var = (s_var_data_t *)val->var_ptr;
			if(var != NULL)
			{
				s_var_data_set(var, SAE_VER_W, 2);
				s_var_data_set(var, info->s_ver, 8);
				s_var_data_char(var, '&');
			}	
		}
	}
#endif

	return B_DEV_EOK;	
}

int fn_sae_lock_id(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_lock_id:%s\n", SAE_LOCK_ID_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		//fprintf(stderr, "%s:%s\n", SAE_LOCK_ID_W, val->data);
		
		sae_info_set_lock_id(id, val->data);
	}
#if 0
	else if(val->sel == FN_SEL_TRUE)
	{
		sae_info_t *info = sae_info_get_by_id(id);
		if(info != NULL)
		{
			s_var_data_t *var = (s_var_data_t *)val->var_ptr;
			if(var != NULL)
			{
				s_var_data_set(var, SAE_LOCK_ID_W, 2);
				s_var_data_set(var, info->s_lock_id, 3);
				s_var_data_char(var, '&');
			}	
		}
	}
#endif

	return B_DEV_EOK;
}

int fn_sae_lock_ver(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_lock_ver:%s\n", SAE_LOCK_VER_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		//fprintf(stderr, "%s:%s\n", SAE_LOCK_VER_W, val->data);
		
		sae_info_set_lock_ver(id, val->data);
	}

#if 0
	else if(val->sel == FN_SEL_TRUE)
	{
		sae_info_t *info = sae_info_get_by_id(id);
		if(info != NULL)
		{
			s_var_data_t *var = (s_var_data_t *)val->var_ptr;
			if(var != NULL)
			{
				s_var_data_set(var, SAE_LOCK_VER_W, 2);
				s_var_data_set(var, info->s_lock_ver, 8);
				s_var_data_char(var, '&');
			}	
		}
	}	
#endif

	return B_DEV_EOK;
}

int fn_sae_psam(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_psam:%s\n", SAE_PSAM_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		//fprintf(stderr, "%s:%s\n", SAE_PSAM_W, val->data);
		
		sae_info_set_psam(id, val->data);
	}
	
	return B_DEV_EOK;
}

int fn_sae_sn(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_sn:%s\n", SAE_SN_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_sn(id, val->data);
	}

	return B_DEV_EOK;
}

int fn_sae_total_ver(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_total_ver:%s\n", SAE_TOTAL_BLK_VER_W);

	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_total_blk_ver(id, val->data);
	}

	return B_DEV_EOK;
}

int fn_sae_inc_ver(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_inc_ver:%s\n", SAE_INC_BLK_VER_W);

	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_inc_blk_ver(id, val->data);
	}

	return B_DEV_EOK;
}

int fn_sae_dec_ver(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_dec_ver:%s\n", SAE_DEC_BLK_VER_W);

	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_dec_blk_ver(id, val->data);
	}

	return B_DEV_EOK;
}

int fn_sae_temp_ver(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_temp_ver:%s\n", SAE_TEMP_BLK_VER_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_temp_blk_ver(id, val->data);
	}
	
	return B_DEV_EOK;
}

int fn_sae_time(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_time:%s\n", SAE_TIME_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_time(id, val->data);
	}
	
	return B_DEV_EOK;
}

int fn_sae_n_id(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_n_id:%s\n", SAE_N_ID_W);

	unsigned char id[6] = {0};
	device_config_t conf;

	
	if(val->sel == FN_SEL_TRUE)
	{	
		device_config_get(&conf);

		sprintf(id, "%05d", conf.nconf.term_id);
		fprintf(stderr, "ndev id:%s\n", id);
	
		s_var_data_t *var = (s_var_data_t *)val->var_ptr;
		if(var != NULL)
		{
			s_var_data_set(var, SAE_N_ID_W, 2);
			s_var_data_set(var, id, 5);
			s_var_data_char(var, '&');
		}
	}
	
	return B_DEV_EOK;
}

int fn_sae_status(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_status:%s\n", SAE_STATUS_W);
	
	char status[8] = {0};
	unsigned short stat = 0;
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		lib_b64_decode_hex(status, val->data, val->d_size);
		memcpy(&stat, status, 2);

		//fprintf(stderr, "stat:0x%02x\n", stat);
		
		sae_info_set_single_stat(id, stat); //保存锁桩状态
		sae_info_set_stat(id, stat); //保存锁桩状态
	}
	
	return B_DEV_EOK;
}

int fn_sae_n_status(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_n_status:%s\n", SAE_N_STATUS_W);

	ndev_info_t info;
	unsigned char stat[2] = {0};
	
	if(val->sel == FN_SEL_TRUE)
	{	
		s_var_data_t *var = (s_var_data_t *)val->var_ptr;
		if(var != NULL)
		{
			ndev_info_get(&info);
			memcpy(&stat, &(info.stat), 2);
			
			s_var_data_set(var, SAE_N_STATUS_W, 2);
			s_var_data_b64_set(var, stat, 2);
			s_var_data_char(var, '&');
		}
	}
		
	return B_DEV_EOK;
}

int fn_sae_n_time(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_n_time:%s\n", SAE_N_TIME_W);

	char s_time[13] = {0};

	if(val->sel == FN_SEL_TRUE)
	{	
		s_var_data_t *var = (s_var_data_t *)val->var_ptr;
		if(var != NULL)
		{
			utils_get_sys_time_s(s_time);
			
			s_var_data_set(var, SAE_N_TIME_W, 2);
			s_var_data_set(var, s_time, 12);
			s_var_data_char(var, '&');
		}
	}
	
	return B_DEV_EOK;
}

int fn_sae_n_mac(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_n_mac:%s\n", SAE_N_MAC_W);

	char macaddr[8] = {0};
	char s_macaddr[16] = {0};

	if(val->sel == FN_SEL_TRUE)
	{	
		s_var_data_t *var = (s_var_data_t *)val->var_ptr;
		if(var != NULL)
		{
			lib_get_macaddr("eth1", macaddr);  //节点机MAC 地址
			lib_hex_to_str(macaddr, 6, s_macaddr);
			s_var_data_set(var, SAE_N_MAC_W, 2);	
			s_var_data_set(var, s_macaddr, 12);
			s_var_data_char(var, '&');
		}
	}
	
	return B_DEV_EOK;
}

int fn_sae_phy_sn(fn_stake_val_t *val)  //锁桩物理编号
{
	FN_DBG("fn_sae_phy_sn:%s\n", SAE_PHY_SN_W);
	
	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_phy_sn(id, val->data);
	}

	return B_DEV_EOK;
}

int fn_sae_control(fn_stake_val_t *val) //锁桩控制
{
	FN_DBG("fn_sae_control:%s\n", SAE_CTRL_C);
	
	unsigned char id = val->s_addr;
	unsigned char ack_val[8] = {0};
	unsigned short s_stat = 0;
	
	memset(ack_val, 0xff, sizeof(ack_val));
	
	if(val->sel == FN_SEL_FLASE)
	{
		lib_b64_decode_hex(ack_val, val->data, val->d_size);
		
		if(ack_val[0] == 0) //成功
		{
			sae_info_t *sinfo = sae_info_get_by_id(id); //获取锁桩状态
			if(sinfo != NULL)
			{
				n_var_data_t *var = n_var_data_alloc();
				if(var != NULL)
				{
					memset(ack_val, 0x00, sizeof(ack_val));
					s_stat = htons(sinfo->status);
					ack_val[0] = id;
					memcpy(&ack_val[1], &s_stat, 2);
					
					n_var_data_hd_ack(var, NDEV_CTRL_REQ, g_sae_ctrl_sn);
					n_var_data_set(var, NDEV_S_LOCK_OP_W, 2);
					n_var_data_b64_set(var, ack_val, 3);
					n_var_data_char(var, '&');
					
					n_var_data_len(var);
					n_var_data_crc(var);

					ndev_tcp_send_to_server_with_escape(var->data, var->total_len, 200); //发送数据到中心
				}
			}
		}
	}

	return B_DEV_EOK;
}

int fn_sae_para_ver(fn_stake_val_t *val)
{
	FN_DBG("fn_sae_para_ver:%s\n", SAE_PARA_VER_W);
	//fprintf(stderr, "fn_sae_para_ver:%s\n", SAE_PARA_VER_W);

	unsigned char id = val->s_addr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		sae_info_set_stake_para_ver(id, val->data);
	}

	return B_DEV_EOK;	
}



