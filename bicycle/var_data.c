#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>
#include <arpa/inet.h>

#include "configs.h"
#include "defines.h"
#include "lib_general.h"
#include "device_config.h"
#include "var_data.h"


struct var_data
{
	lib_mem_pool_t *stake_mp;
	lib_mem_pool_t *ndev_mp;
	int mp_s_sem_id;
	int mp_n_sem_id;
	unsigned char stake_sn;
	unsigned char ndev_sn;
};

static struct var_data g_var_data;

#if 0
static unsigned short calc_crc16(unsigned char *ptr, const unsigned short length)
{
	assert(ptr);
	
	unsigned short quotient, i, j;
	unsigned short remainder, divisor = 0x1021;
	unsigned char data[1024] = {0};
	unsigned short t_data;

	memcpy(data, ptr, length);
	data[length] = 0;
	data[length + 1] = 0;
	remainder = 0;

	for(j = 0; j < (length + 2); j++)
	{
		t_data = data[j];
		for(i = 8; i>0; i--)
		{
			quotient = remainder & 0x8000;
			remainder <<= 1;
			if((t_data <<= 1) & 0x0100)
				remainder |= 1;
			if(quotient)
				remainder ^= divisor;
		}
	}
	
	return remainder;	
}
#endif

static inline unsigned char __get_stake_sn(void)
{
	if(g_var_data.stake_sn == B_DEV_CHAR_MAX)
	{
		g_var_data.stake_sn = 1;
		return g_var_data.stake_sn;
	}

	return ++g_var_data.stake_sn;
}

unsigned char get_stake_sn(void)
{
	return __get_stake_sn();
}

static inline unsigned char __get_ndev_sn(void)
{
	if(g_var_data.ndev_sn == B_DEV_CHAR_MAX)
	{
		g_var_data.ndev_sn = 1;
		return g_var_data.ndev_sn;
	}

	return ++g_var_data.ndev_sn;
}

unsigned char get_ndev_sn(void)
{
	return __get_ndev_sn();
}

static int __s_var_data_init(void)
{
	int key = -1;
	int err = -1;

	/* 锁桩协议封包内存池PV 锁*/
	key = lib_get_random_key("/proc/meminfo");
	err = lib_sem_init(&(g_var_data.mp_s_sem_id), key);
	if(err != LIB_GE_EOK)
	{
		fprintf(stderr, "stake memory pool sem init failed!\n");
		goto ERR;
	}

	/* 锁桩协议封包内存池*/
	g_var_data.stake_mp = lib_mp_create(S_VAR_DATA_NUM, sizeof(s_var_data_t) + 8);
	if(g_var_data.stake_mp == LIB_GE_NULL)
	{
		fprintf(stderr, "stake memory pool create failed!\n");
		goto ERR;	
	}

	return B_DEV_EOK;
ERR:
	if(g_var_data.mp_s_sem_id != -1)
		lib_sem_destroy(g_var_data.mp_s_sem_id);
	
	return B_DEV_ERROR;
}

static void __s_var_data_destroy(void)
{
	if(g_var_data.mp_s_sem_id != -1)
		lib_sem_destroy(g_var_data.mp_s_sem_id);
	
	if(g_var_data.stake_mp != NULL)
		lib_mp_destroy(g_var_data.stake_mp);
}

static int __n_var_data_init(void)
{
	int key = -1;
	int err = -1;
	
	/* 内存池PV 锁*/
	key = lib_get_random_key("/proc/cpuinfo");
	err = lib_sem_init(&g_var_data.mp_n_sem_id, key);
	if(err != LIB_GE_EOK)
	{
		fprintf(stderr, "node device memory pool sem init failed!\n");
		goto ERR;
	}

	/* 内存池*/
	g_var_data.ndev_mp = lib_mp_create(N_VAR_DATA_NUM, sizeof(n_var_data_t) + 8);
	if(g_var_data.ndev_mp == LIB_GE_NULL)
	{
		fprintf(stderr, "node device memory pool create failed!\n");
		goto ERR;	
	}

	return B_DEV_EOK;
ERR:
	if(g_var_data.mp_n_sem_id != -1)
		lib_sem_destroy(g_var_data.mp_n_sem_id);
	
	return B_DEV_ERROR;
}

static void __n_var_data_destroy(void)
{
	if(g_var_data.mp_n_sem_id != -1)
		lib_sem_destroy(g_var_data.mp_n_sem_id);
	
	if(g_var_data.ndev_mp != NULL)
		lib_mp_destroy(g_var_data.ndev_mp);
}



int var_data_init(void)
{
	memset(&g_var_data, 0, sizeof(struct var_data));
	
	if(__s_var_data_init() != B_DEV_EOK)
		return VAR_DATA_ERROR;
	
	if(__n_var_data_init() != B_DEV_EOK)
		return VAR_DATA_ERROR;
	
	return VAR_DATA_EOK;
}

void var_data_destroy(void)
{
	__s_var_data_destroy();
	__n_var_data_destroy();
}




void s_var_data_char(s_var_data_t *var, const unsigned char ch)
{
	var->data[var->idx] = ch;
	var->idx += 1;
	var->data_len += 1;
	var->total_len += 1;	
}

void s_var_data_short(s_var_data_t *var, const unsigned short sot)
{
	unsigned short m_sot = htons(sot);
	memcpy(&(var->data[var->idx]), &m_sot, 2);
	var->idx += 2;
	var->data_len += 2;
	var->total_len += 2;		
}

void s_var_data_int(s_var_data_t *var, const unsigned short it)
{
	unsigned int m_it = htonl(it);
	memcpy(&(var->data[var->idx]), &m_it, 4);
	var->idx += 4;
	var->data_len += 4;
	var->total_len += 4;		
}
void s_var_data_set(s_var_data_t *var, void *ptr, const unsigned int size)
{
	memcpy(&(var->data[var->idx]), ptr, size);
	var->idx += size;
	var->data_len += size;
	var->total_len += size;
}

void s_var_data_b64_set(s_var_data_t *var, void *ptr, const unsigned int size)
{
	char s_dst[128] = {0};
	int nsize = 0;

	nsize = lib_b64_encode_hex(s_dst, ptr, size);
	memcpy(&(var->data[var->idx]), s_dst, nsize);

	var->idx += nsize;
	var->data_len += nsize;
	var->total_len += nsize;
}

void s_var_data_b64_w_set(s_var_data_t *var, const unsigned char word[3], void *ptr, const unsigned int size)
{
	if(word == NULL)
		return;
	
	char s_dst[128] = {0};
	int nsize = 0;
	
	memcpy(&(var->data[var->idx]), word, 2);
	var->idx += 2;
	var->data_len += 2;
	var->total_len += 2;	
	
	nsize = lib_b64_encode_hex(s_dst, ptr, size);
	memcpy(&(var->data[var->idx]), s_dst, nsize);

	var->idx += nsize;
	var->data_len += nsize;
	var->total_len += nsize;	
}

/* 节点机和桩机应用数据帧组包 */
void s_var_data_hd_req(s_var_data_t *var, const unsigned char s_addr, const unsigned char d_addr, const unsigned char cmd)
{
	memset(var, 0, sizeof(s_var_data_t));
	
	var->data[0] = SAE_HD_H;
	var->data[1] = SAE_HD_L;
	var->data[2] = s_addr;
	var->data[3] = d_addr;
	var->data[4] = cmd;
	var->data[5] = __get_stake_sn();
	var->idx = S_VAR_DATA_IDX;
	var->total_len += S_VAR_DATA_IDX;  
}

void s_var_data_hd_ack(s_var_data_t *var, const unsigned char s_addr, const unsigned char d_addr, const unsigned char cmd, const unsigned char sn)
{
	memset(var, 0, sizeof(s_var_data_t));
	
	var->data[0] = SAE_HD_H;
	var->data[1] = SAE_HD_L;
	var->data[2] = s_addr;
	var->data[3] = d_addr;
	
	switch(cmd)
	{
		case SAE_DEV_REG_REQ:
			var->data[4] = SAE_DEV_REG_ACK;
			break;

		case SAE_DEV_CTRL_REQ:
			var->data[4] = SAE_DEV_CTRL_ACK;
			break;
			
		case SAE_FILE_TRANS_REQ:
			var->data[4] = SAE_FILE_TRANS_ACK;
			break;

		case SAE_PASS_REQ:
			var->data[4] = SAE_PASS_ACK;
			break;	
	}
	
	var->data[5] = sn;
	var->idx = S_VAR_DATA_IDX;
	var->total_len += S_VAR_DATA_IDX;  
}

void s_var_data_crc(s_var_data_t *var)
{
	unsigned short crc16, calc_crc16;
	unsigned short m_len = htons(var->data_len);
	memcpy(&(var->data[S_VAR_DATA_LEN_IDX]), &m_len, 2);
	
	calc_crc16 = lib_crc16_with_table((char *)var->data, var->total_len);
	crc16 = htons(calc_crc16);
	memcpy(&(var->data[var->idx]), &crc16, 2);
	
	var->total_len += 2;
	var->idx += 2;
}

s_var_data_t *s_var_data_alloc(void)
{
	s_var_data_t *var = NULL;
	
	lib_sem_p(g_var_data.mp_s_sem_id);
	var = lib_mp_alloc(g_var_data.stake_mp, sizeof(s_var_data_t), NULL);
	lib_sem_v(g_var_data.mp_s_sem_id);	

	return var;
}

int s_var_data_free(s_var_data_t *var)
{
	int ret = VAR_DATA_ERROR;
	
	lib_sem_p(g_var_data.mp_s_sem_id);
	ret = lib_mp_free(g_var_data.stake_mp, var);
	lib_sem_v(g_var_data.mp_s_sem_id);
	
	return ret;
}




void n_var_data_char(n_var_data_t *var, const unsigned char ch)
{
	var->data[var->idx] = ch;
	var->idx += 1;
	var->data_len += 1;
	var->total_len += 1;	
}

void n_var_data_short(n_var_data_t *var, const unsigned short sot)
{
	unsigned short m_sot = htons(sot);
	memcpy(&(var->data[var->idx]), &m_sot, 2);
	var->idx += 2;
	var->data_len += 2;
	var->total_len += 2;		
}

void n_var_data_int(n_var_data_t *var, const unsigned short it)
{
	unsigned int m_it = htonl(it);
	memcpy(&(var->data[var->idx]), &m_it, 4);
	var->idx += 4;
	var->data_len += 4;
	var->total_len += 4;		
}

void n_var_data_set(n_var_data_t *var, void *ptr, const unsigned int size)
{
	memcpy(&(var->data[var->idx]), ptr, size);
	var->idx += size;
	var->data_len += size;
	var->total_len += size;
}

void n_var_data_b64_set(n_var_data_t *var, void *ptr, const unsigned int size)
{
	char s_dst[1024] = {0};
	int nsize = 0;

	nsize = lib_b64_encode_hex(s_dst, ptr, size);
	memcpy(&(var->data[var->idx]), s_dst, nsize);

	var->idx += nsize;
	var->data_len += nsize;
	var->total_len += nsize;
}

/* 节点机和中心通信数据帧组包 */
void n_var_data_hd_req(n_var_data_t *var, const unsigned char cmd, const unsigned char dev_addr,  n_data_attr_t *attr)
{
	unsigned short term_id;
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	
	term_id = htons(conf.nconf.term_id);
	
	var->data[0] = NDEV_HD;           					//协议头部
	var->data[1] = __get_ndev_sn();  				//流水号
	memcpy(&(var->data[2]), &term_id, 2);				//终端编号
	var->data[4] = dev_addr;						//设备地址
	memcpy(&(var->data[N_VAR_ATTR_IDX]), attr, 1); 	//数据属性(控制位)
	var->data[6] = cmd;                 					//类别引导字
	
	var->idx = N_VAR_DATA_IDX;
	var->total_len = N_VAR_DATA_IDX; //总长
	var->data_len = 0;	//数据体部分长度
}

void n_var_data_hd_ack(n_var_data_t *var, const unsigned char cmd, const unsigned char sn)
{
	unsigned short term_id;
	device_config_t conf;
	n_data_attr_t attr;
	memset(&conf, 0, sizeof(device_config_t));
	memset(&attr, 0, sizeof(n_data_attr_t));
	
	device_config_get(&conf);

	term_id = htons(conf.nconf.term_id);
	
	var->data[0] = NDEV_HD;           					//协议头部
	var->data[1] = sn;  								//流水号
	memcpy(&(var->data[2]), &term_id, 2);				//终端编号           					

	switch(cmd)  //类别引导字
	{
		case NDEV_REGISTER_REQ:
			var->data[6] = NDEV_REGISTER_ACK;
			break;

		case NDEV_CTRL_REQ:
			var->data[6] = NDEV_CTRL_ACK;
			break;
			
		case NDEV_FILE_TRANS_REQ:
			var->data[6] = NDEV_FILE_TRANS_ACK;
			break;

		case NDEV_PASS_REQ:
			var->data[6] = NDEV_PASS_ACK;
			break;
	}

	/* 控制属性 */
	attr.pass = 0;
	attr.recv = 1;
	attr.broadcast = 0;
	memcpy(&(var->data[N_VAR_ATTR_IDX]), &attr, sizeof(n_data_attr_t));
	
	var->idx = N_VAR_DATA_IDX;
	var->total_len = N_VAR_DATA_IDX;
	var->data_len = 0;
}

void n_var_data_len(n_var_data_t *var)
{
	unsigned short m_len = htons(var->data_len);
	memcpy(&(var->data[N_VAR_DATA_LEN_IDX]), &(m_len), 2);
}

void n_var_data_crc(n_var_data_t *var)
{
	unsigned short c_crc16, crc16;
	
	c_crc16 = lib_crc16_with_table((char *)&(var->data[1]), var->data_len + 8);
	crc16 = htons(c_crc16);
	memcpy(&(var->data[var->idx]), &crc16, 2);
	var->idx += 2;
	var->data[var->idx]= NDEV_TAIL;
	var->total_len += 3;	
}

n_var_data_t *n_var_data_alloc(void)
{
	n_var_data_t *var = NULL;
	
	lib_sem_p(g_var_data.mp_n_sem_id);
	var = lib_mp_alloc(g_var_data.ndev_mp, sizeof(n_var_data_t), NULL);
	lib_sem_v(g_var_data.mp_n_sem_id);	

	return var;
}

int n_var_data_free(n_var_data_t *var)
{
	int ret = -1;
	
	lib_sem_p(g_var_data.mp_n_sem_id);
	ret = lib_mp_free(g_var_data.ndev_mp, var);
	lib_sem_v(g_var_data.mp_n_sem_id);
	
	return ret;
}










