#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>


#include "default.h"
#include "defines.h"
#include "lib_general.h"
#include "var_data.h"
#include "lib_zmalloc.h"
#include "ndev_protocol.h"
#include "node_device.h"
#include "utils.h"
#include "stake.h"
#include "sae_protocol.h"
#include "lib_ctos.h"
#include "unity_file_handle.h"


static void *__unity_file_request_thread(void *arg);
static void *__unity_file_broadcast_handle(void *arg);
static int __unity_file_verify(unity_file_hd_t *hd);
static void *__unity_file_broadcast_handle_to_sae(void *arg);

extern lib_unity_file_t *g_unity_file_db;

typedef struct unity_file_ufc
{
	unsigned int atmoic;
	unsigned short file_seq;
}unity_file_ufc_t;

#define UNITY_FILE_SZ			(7)


static unity_file_hd_t g_unity_file_hd[UNITY_FILE_SZ] = {{0,0,0,0,{0}},{0,0,0,0,{0}}, {0,0,0,0,{0}}, {0,0,0,0,{0}}, {0,0,0,0,{0}}, {0,0,0,0,{0}}, {0,0,0,0,{0}}};
static unity_file_ufc_t g_unity_file_ufc[UNITY_FILE_SZ] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

struct unity_file_notify
{
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;
	unsigned int notify;	
	unity_file_hd_t hd;
};


pthread_mutex_t *g_unity_file_hd_mutex;
struct unity_file_notify *g_unity_file_notify[UNITY_FILE_SZ];

static unity_file_info_t g_ufile_info;    
static pthread_mutex_t *g_ufile_info_mutex;

static void __unity_file_notify_init(void)
{
	int i;

	for(i = 0; i < UNITY_FILE_SZ; i++)
	{
		g_unity_file_notify[i] = (struct unity_file_notify *)malloc(sizeof(struct unity_file_notify));
		memset(g_unity_file_notify[i], 0, sizeof(struct unity_file_notify));
		
		g_unity_file_notify[i]->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
		g_unity_file_notify[i]->cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));

		pthread_mutex_init(g_unity_file_notify[i]->mutex, NULL);
		pthread_cond_init(g_unity_file_notify[i]->cond, NULL);
	}
}

static void __unity_file_notify_release(void)
{
	int i;

	for(i = 0; i < UNITY_FILE_SZ; i++)
	{
		pthread_mutex_destroy(g_unity_file_notify[i]->mutex);
		pthread_cond_destroy(g_unity_file_notify[i]->cond);

		free(g_unity_file_notify[i]->mutex);
		g_unity_file_notify[i]->mutex = NULL;

		free(g_unity_file_notify[i]->cond);
		g_unity_file_notify[i]->cond = NULL;
		
		free(g_unity_file_notify[i]);
		g_unity_file_notify[i] = NULL;
	}	
}


static void __unity_file_hd_get(unity_file_hd_t *hd, const unsigned char type)
{
	if(hd == NULL)
		return;

	pthread_mutex_lock(g_unity_file_hd_mutex);

	switch(type)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(hd, &g_unity_file_hd[0], sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(hd, &g_unity_file_hd[1], sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(hd, &g_unity_file_hd[2], sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(hd, &g_unity_file_hd[3], sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(hd, &g_unity_file_hd[4], sizeof(unity_file_hd_t));
		}
		break;
		
		case UF_TYPE_STAKE_PARA:
		{
			memcpy(hd, &g_unity_file_hd[5], sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_UNIONPAY_KEY:
		{
			memcpy(hd, &g_unity_file_hd[6], sizeof(unity_file_hd_t));
		}
		break;
	}

	pthread_mutex_unlock(g_unity_file_hd_mutex);	
}

static void __unity_file_hd_put(unity_file_hd_t *hd, const unsigned char type)
{
	if(hd == NULL)
		return;	

	pthread_mutex_lock(g_unity_file_hd_mutex);

	switch(type)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(&g_unity_file_hd[0], hd, sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(&g_unity_file_hd[1], hd, sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(&g_unity_file_hd[2], hd, sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(&g_unity_file_hd[3], hd, sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(&g_unity_file_hd[4], hd, sizeof(unity_file_hd_t));
		}
		break;
		
		case UF_TYPE_STAKE_PARA:
		{
			memcpy(&g_unity_file_hd[5], hd, sizeof(unity_file_hd_t));
		}
		break;

		case UF_TYPE_UNIONPAY_KEY:
		{
			memcpy(&g_unity_file_hd[6], hd, sizeof(unity_file_hd_t));
		}
		break;
	}
	
	pthread_mutex_unlock(g_unity_file_hd_mutex);
}

static void __unity_file_atmoic_set_val(const unsigned char type, const unsigned int val)
{
	switch(type)
	{
		case UF_TYPE_TOTAL_BL:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[0].atmoic), val);
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[1].atmoic), val);
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[2].atmoic), val);
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[3].atmoic), val);
		}
		break;
		
		case UF_TYPE_STAKE_FW:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[4].atmoic), val);
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[5].atmoic), val);
		}
		break;

		case UF_TYPE_UNIONPAY_KEY:
		{
			lib_gcc_atmoic_set(&(g_unity_file_ufc[6].atmoic), val);
		}
		break;
	}
}

static unsigned int __unity_file_atmoic_get_val(const unsigned char type)
{
	switch(type)
	{
		case UF_TYPE_TOTAL_BL:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[0].atmoic));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[1].atmoic));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[2].atmoic));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[3].atmoic));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[4].atmoic));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[5].atmoic));
		}
		break;

		case UF_TYPE_UNIONPAY_KEY:
		{
			return lib_gcc_atmoic_get(&(g_unity_file_ufc[6].atmoic));
		}
		break;
	}

	return UF_ATMOIC_END;
}


static void __unity_file_seq_set_val(const unsigned char type, const unsigned short seq)
{
	switch(type)
	{
		case UF_TYPE_TOTAL_BL:
		{
			g_unity_file_ufc[0].file_seq = seq;
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			g_unity_file_ufc[1].file_seq = seq;
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			g_unity_file_ufc[2].file_seq = seq;
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			g_unity_file_ufc[3].file_seq = seq;
		}
		break;
		
		case UF_TYPE_STAKE_FW:
		{
			g_unity_file_ufc[4].file_seq = seq;
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			g_unity_file_ufc[5].file_seq = seq;
		}
		break;

		case UF_TYPE_UNIONPAY_KEY:
		{
			g_unity_file_ufc[6].file_seq = seq;
		}
		break;
	}
}

static int __unity_file_notify_put(const unsigned int notify, unity_file_hd_t *hd, const unsigned char utype)
{
	if(hd == NULL)
		return B_DEV_ERROR;
	
	int i;
	
	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
			i = 0;
			break;
			
		case UF_TYPE_INCREMENT_BL:
			i = 1;
			break;

		case UF_TYPE_DECREMENT_BL:
			i = 2;
			break;

		case UF_TYPE_TEMPORARY_BL:
			i = 3;
			break;

		case UF_TYPE_STAKE_FW:
			i = 4;
			break;

		case UF_TYPE_STAKE_PARA:
			i = 5;
			break;

		case UF_TYPE_UNIONPAY_KEY:
			i = 6;

		default:
			return B_DEV_ERROR;
	}

	pthread_mutex_lock(g_unity_file_notify[i]->mutex);
	
	g_unity_file_notify[i]->notify = notify;
	memset(&(g_unity_file_notify[i]->hd), 0, sizeof(unity_file_hd_t));
	memcpy(&(g_unity_file_notify[i]->hd), hd, sizeof(unity_file_hd_t));

	pthread_mutex_unlock(g_unity_file_notify[i]->mutex);
	
	return pthread_cond_signal(g_unity_file_notify[i]->cond);
}

static int __unity_file_notify_timedwait(const unsigned int sec, unsigned int *notify, unity_file_hd_t *hd,  const unsigned char utype)
{
	if(hd == NULL)
		return B_DEV_ERROR;
	
	int err = B_DEV_ERROR;
	struct timespec tspec;
	
	memset(&tspec, 0, sizeof(struct timespec));
	tspec.tv_sec = time(NULL) + sec;
	tspec.tv_nsec = 0;

	int i;
	
	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
			i = 0;
			break;
			
		case UF_TYPE_INCREMENT_BL:
			i = 1;
			break;

		case UF_TYPE_DECREMENT_BL:
			i = 2;
			break;

		case UF_TYPE_TEMPORARY_BL:
			i = 3;
			break;

		case UF_TYPE_STAKE_FW:
			i = 4;
			break;

		case UF_TYPE_STAKE_PARA:
			i = 5;
			break;
			
		case UF_TYPE_UNIONPAY_KEY:
			i = 6;
			
		default:
			return B_DEV_ERROR;
	}
	
	pthread_mutex_lock(g_unity_file_notify[i]->mutex);

	err = pthread_cond_timedwait(g_unity_file_notify[i]->cond, g_unity_file_notify[i]->mutex, &tspec);
	if(err == ETIMEDOUT)
		err = B_DEV_ETIMEOUT;

	*notify = g_unity_file_notify[i]->notify;
	memcpy(hd, &(g_unity_file_notify[i]->hd), sizeof(unity_file_hd_t));	
	
	pthread_mutex_unlock(g_unity_file_notify[i]->mutex);

	return err;
}

enum UF_DB __get_idle_db(enum UF_TYPE utype)
{
	enum UF_DB ufdb = UF_DB_NOT;
	unity_file_config_t uconfig;
	memset(&uconfig, 0, sizeof(unity_file_config_t));

	int ret = lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	if(ret != LIB_UF_OK)
		return UF_DB_NOT;

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			if(uconfig.use_total_bl_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_total_bl_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_total_bl_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			if(uconfig.use_inc_bl_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_inc_bl_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_inc_bl_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			if(uconfig.use_dec_bl_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_dec_bl_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_dec_bl_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			if(uconfig.use_temp_bl_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_temp_bl_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_temp_bl_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			if(uconfig.use_stake_fw_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_stake_fw_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_stake_fw_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			if(uconfig.use_stake_para_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_stake_para_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_stake_para_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;
		
		case UF_TYPE_UNIONPAY_KEY:
		{
			if(uconfig.use_unionpay_key_db == UF_DB_IDLE)
				ufdb = UF_DB_A;
			else if(uconfig.use_unionpay_key_db == UF_DB_A)
				ufdb = UF_DB_B;
			else if(uconfig.use_unionpay_key_db == UF_DB_B)
				ufdb = UF_DB_A;	
		}
		break;
	}

	return ufdb;
}

static enum UF_DB __get_used_db(enum UF_TYPE utype)
{
	enum UF_DB ufdb = UF_DB_NOT;
	unity_file_config_t uconfig;
	memset(&uconfig, 0, sizeof(unity_file_config_t));

	int ret = lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	if(ret != LIB_UF_OK)
		return UF_DB_NOT;

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
			ufdb = uconfig.use_total_bl_db;
			break;

		case UF_TYPE_INCREMENT_BL:
			ufdb = uconfig.use_inc_bl_db;
			break;
			
		case UF_TYPE_DECREMENT_BL:
			ufdb = uconfig.use_dec_bl_db;
			break;
			
		case UF_TYPE_TEMPORARY_BL:
			ufdb = uconfig.use_temp_bl_db;
			break;
			
		case UF_TYPE_STAKE_FW:
			ufdb = uconfig.use_stake_fw_db;
			break;
			
		case UF_TYPE_STAKE_PARA:
			ufdb =  uconfig.use_stake_para_db;
			break;
			
		case UF_TYPE_UNIONPAY_KEY:
			ufdb =  uconfig.use_unionpay_key_db;
			break;
	}

	return ufdb;	
}

static void __set_used_db(unity_file_hd_t *hd)
{
	if(hd == NULL)
		return;
	
	unity_file_config_t uconfig;
	memset(&uconfig, 0, sizeof(unity_file_config_t));

	int ret = lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	if(ret != LIB_UF_OK)
		return;
	
	switch(hd->ufc)
	{
		case UF_TYPE_TOTAL_BL:
		{
			lib_unity_file_hd_n4_to_str(uconfig.total_bl_last_ver, hd); //版本
			uconfig.use_total_bl_seq = ntohs(hd->total); //分割总数
			
			if(uconfig.use_total_bl_db == UF_DB_IDLE)
			{
				uconfig.use_total_bl_db = UF_DB_A;
			}
			else if(uconfig.use_total_bl_db == UF_DB_A)
			{
				uconfig.use_total_bl_db = UF_DB_B;
			}
			else if(uconfig.use_total_bl_db == UF_DB_B)
			{
				uconfig.use_total_bl_db = UF_DB_A;
			}
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			lib_unity_file_hd_n4_to_str(uconfig.inc_bl_last_ver, hd);
			uconfig.use_inc_bl_seq = ntohs(hd->total);
			
			if(uconfig.use_inc_bl_db == UF_DB_IDLE)
			{
				uconfig.use_inc_bl_db = UF_DB_A;
			}
			else if(uconfig.use_inc_bl_db == UF_DB_A)
			{
				uconfig.use_inc_bl_db = UF_DB_B;
			}
			else if(uconfig.use_inc_bl_db == UF_DB_B)
			{
				uconfig.use_inc_bl_db = UF_DB_A;
			}
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			lib_unity_file_hd_n4_to_str(uconfig.dec_bl_last_ver, hd);
			uconfig.use_dec_bl_seq = ntohs(hd->total);
			
			if(uconfig.use_dec_bl_db == UF_DB_IDLE)
			{
				uconfig.use_dec_bl_db = UF_DB_A;
			}
			else if(uconfig.use_dec_bl_db == UF_DB_A)
			{
				uconfig.use_dec_bl_db = UF_DB_B;
			}
			else if(uconfig.use_dec_bl_db == UF_DB_B)
			{
				uconfig.use_dec_bl_db = UF_DB_A;
			}
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			lib_unity_file_hd_n4_to_str(uconfig.temp_bl_last_ver, hd);
			uconfig.use_temp_bl_seq = ntohs(hd->total);
			
			if(uconfig.use_temp_bl_db == UF_DB_IDLE)
			{
				uconfig.use_temp_bl_db = UF_DB_A;
			}
			else if(uconfig.use_temp_bl_db == UF_DB_A)
			{
				uconfig.use_temp_bl_db = UF_DB_B;
			}
			else if(uconfig.use_temp_bl_db == UF_DB_B)
			{
				uconfig.use_temp_bl_db = UF_DB_A;
			}
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			lib_unity_file_hd_n4_to_str(uconfig.stake_fw_last_ver, hd);
			uconfig.use_stake_fw_seq = ntohs(hd->total);
			
			if(uconfig.use_stake_fw_db == UF_DB_IDLE)
			{
				uconfig.use_stake_fw_db = UF_DB_A;
			}
			else if(uconfig.use_stake_fw_db == UF_DB_A)
			{
				uconfig.use_stake_fw_db = UF_DB_B;
			}
			else if(uconfig.use_stake_fw_db == UF_DB_B)
			{
				uconfig.use_stake_fw_db = UF_DB_A;
			}
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			lib_unity_file_hd_n4_to_str(uconfig.stake_para_last_ver, hd);
			uconfig.use_stake_para_seq = ntohs(hd->total);
			
			if(uconfig.use_stake_para_db == UF_DB_IDLE)
			{
				uconfig.use_stake_para_db = UF_DB_A;
			}
			else if(uconfig.use_stake_para_db == UF_DB_A)
			{
				uconfig.use_stake_para_db = UF_DB_B;
			}
			else if(uconfig.use_stake_para_db == UF_DB_B)
			{
				uconfig.use_stake_para_db = UF_DB_A;
			}	
		}
		break;
		
		case UF_TYPE_UNIONPAY_KEY:
		{
			lib_unity_file_hd_n4_to_str(uconfig.unionpay_key_last_ver, hd);
			uconfig.use_unionpay_key_seq = ntohs(hd->total);
			
			if(uconfig.use_unionpay_key_db == UF_DB_IDLE)
			{
				uconfig.use_unionpay_key_db = UF_DB_A;
			}
			else if(uconfig.use_unionpay_key_db == UF_DB_A)
			{
				uconfig.use_unionpay_key_db = UF_DB_B;
			}
			else if(uconfig.use_unionpay_key_db == UF_DB_B)
			{
				uconfig.use_unionpay_key_db = UF_DB_A;
			}	
		}
		break;
	}
		
	lib_unity_file_config_replace_data(g_unity_file_db, &uconfig);

	#if 0
	memset(&uconfig, 0, sizeof(unity_file_config_t));
	lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	#endif
}

static void __unity_file_ufc(ndev_rx_buf_t *nrxbuf, unity_file_st_t *ufile, const unsigned int ufile_len)
{
	int err = LIB_UF_ERROR;
	enum UF_TYPE utype = ufile->hd.ufc;   //文件类别
	unsigned short file_seq = ntohs(ufile->hd.file_seq);
	unsigned short div_seq = ntohs(ufile->hd.div_seq);
	unsigned short total_seq = ntohs(ufile->hd.total);
	
	enum UF_DB idle_uf_db = __get_idle_db(utype);
	if(idle_uf_db == UF_DB_NOT)
	{
		fprintf(stderr, "%s:get idle db failed!\n", __FUNCTION__);
		return;
	}
	
	fprintf(stderr, "receive unity file div_seq = %d\n", div_seq);

	if(ufile->hd.div_seq == UNIV_DIV_SEQ_BC)  //请求的版本,比中心版本旧,通知请求线程退出 (泛文件接收完毕)
	{
		SYS_LOG_INFO("%s: control center notify unity file thread quit, file_seq:%d, total: %d, div_seq:%d, ufc:0x%02x", __FUNCTION__, 
			file_seq, total_seq, div_seq, utype);

		fprintf(stderr, "%s: control center notify unity file thread quit, file_seq:%d, total: %d, div_seq:%d, ufc:0x%02x\n", __FUNCTION__, 
			file_seq, total_seq, div_seq, utype);
		
		__unity_file_notify_put(NDEV_NOTIFY_UNIV_QUIT, &(ufile->hd), utype);
		lib_msleep(200);
		__unity_file_notify_put(NDEV_NOTIFY_UNIV_QUIT, &(ufile->hd), utype);

		/* 清空备份数据库 */
		lib_unity_file_delete_table(g_unity_file_db, utype, idle_uf_db);

		return; //UNIV_DIV_SEQ_BC(0xffff)不应该插入到数据库 
	}

	err = lib_unity_file_replace_data(g_unity_file_db, utype, idle_uf_db, div_seq, ufile); //插入数据库
	if(err == LIB_UF_OK)
	{
		__unity_file_notify_put(NDEV_NOTIFY_UNIV_CONTINUE, &(ufile->hd), utype); //接收数据成功
	}
	else
	{
		SYS_LOG_INFO("%s: replace data file_seq:%d, total: %d, div_seq:%d, ufc:0x%02x error!", __FUNCTION__, file_seq, total_seq, div_seq, utype);
	}
}

/*
 * 泛文件下载请求
 */
static void *__unity_file_request_thread(void *arg)
{
	int err = B_DEV_ERROR;
	int n = 0;
	int ret_cnt = 0; 
	unsigned int notify = NDEV_NOTIFY_INIT;
	unsigned short div_seq = 0;
	unsigned char txbuf[512] = {0};
	unity_file_hd_t ack_ufhd, req_ufhd;
	struct unity_file_struct file_struct;
		
	unity_file_hd_t *p_ufhd = (unity_file_hd_t *)arg;
	if(p_ufhd == NULL)
		return lib_thread_exit((void *)NULL);

	__unity_file_hd_put(p_ufhd, p_ufhd->ufc); //保存泛文件头部

	memset(&req_ufhd, 0, sizeof(unity_file_hd_t));
	memcpy(&req_ufhd, p_ufhd, sizeof(unity_file_hd_t));
	div_seq = ntohs(req_ufhd.div_seq);
	
	lib_zfree(p_ufhd);

	
	/* 更新下载信息 */
	unity_file_info_struct_ndev_get(&file_struct, req_ufhd.ufc);
	file_struct.total =req_ufhd.total;
	file_struct.file_seq = req_ufhd.file_seq;
	file_struct.div_seq = req_ufhd.div_seq;
	file_struct.status = 1;
	time(&file_struct.start_time);
	unity_file_info_struct_ndev_put(&file_struct, req_ufhd.ufc); 
		

	while(1)
	{
		notify = NDEV_NOTIFY_INIT;
		memset(&ack_ufhd, 0, sizeof(unity_file_hd_t));

		memset(&txbuf, 0, sizeof(txbuf));
		n = ndev_protocol_univ_update_req(&txbuf, &req_ufhd, sizeof(unity_file_hd_t));
		if(n < 0)
		{
			ret_cnt++;  
			if(ret_cnt > 3)
				return lib_thread_exit((void *)NULL);
			
			lib_sleep(1);
			continue;
		}

		/* 更新下载信息 */
		unity_file_info_struct_ndev_get(&file_struct, req_ufhd.ufc);
		file_struct.total =req_ufhd.total;
		file_struct.file_seq = req_ufhd.file_seq;
		file_struct.div_seq = req_ufhd.div_seq;
		file_struct.status = 1;
		unity_file_info_struct_ndev_put(&file_struct, req_ufhd.ufc); 
		
		ret_cnt = 0;
		ndev_tcp_send_to_server_with_escape(txbuf, n, 1000); //发往中心

		err = __unity_file_notify_timedwait(15, &notify, &ack_ufhd, req_ufhd.ufc); //15秒
		if(err == B_DEV_ETIMEOUT) //等待超时
		{
			fprintf(stderr, "%s:__ufile_notify_timedwait\n", __FUNCTION__);
			continue;  //超时，继续请求，这里要限制一下请求次数，不然会导致线程一直不退出????????
		}
		
		utils_notify_printf(notify);

		switch(notify)
		{
			case NDEV_NOTIFY_UNIV_QUIT:
			{
				SYS_LOG_NOTICE("%s:notify = NDEV_NOTIFY_UNIV_QUIT, thread quit!\n", __FUNCTION__);
				fprintf(stderr, "%s:notify = NDEV_NOTIFY_UNIV_QUIT, thread quit!\n", __FUNCTION__);

				/* 如果中心通知节点机退出,要清除泛文件头 */

				unity_file_hd_t t_ufile_hd;
				memset(&t_ufile_hd, 0, sizeof(unity_file_hd_t));
				unity_file_hd_put(&t_ufile_hd, req_ufhd.ufc);
				
				return lib_thread_exit((void *)NULL);
			}
			break;

			case NDEV_NOTIFY_UNIV_CONTINUE:
			{
				fprintf(stderr, "Recv Notify NDEV_NOTIFY_UNIV_CONTINUE\n");
				
				if((req_ufhd.file_seq != ack_ufhd.file_seq) || (req_ufhd.div_seq != ack_ufhd.div_seq))   //中心下发的数据与请求数据不一致		
				{
					SYS_LOG_INFO("req_ufhd.file_seq = %d, ack_ufhd.file_seq = %d\n", ntohs(req_ufhd.file_seq), ntohs(ack_ufhd.file_seq));
					SYS_LOG_INFO("req_ufhd.div_seq = %d, ack_ufhd.div_seq = %d\n", ntohs(req_ufhd.div_seq), ntohs(ack_ufhd.div_seq));

					lib_sleep(30);
					continue;
				}

				if(ack_ufhd.div_seq == ack_ufhd.total) //接收完毕,开始发送给锁桩
				{
					SYS_LOG_INFO("%s: unity file receive complete, file_seq:%d,  total: %d, div_seq:%d, ufc:0x%02x", __FUNCTION__, ntohs(ack_ufhd.file_seq), 
								ntohs(ack_ufhd.total), ntohs(ack_ufhd.div_seq), ack_ufhd.ufc);


					/* 更新下载信息 */
					unity_file_info_struct_ndev_get(&file_struct, req_ufhd.ufc);
					file_struct.total =req_ufhd.total;
					file_struct.file_seq = req_ufhd.file_seq;
					file_struct.div_seq = req_ufhd.div_seq;
					file_struct.status = 2; //下载完毕
					time(&(file_struct.end_time));
					unity_file_info_struct_ndev_put(&file_struct, req_ufhd.ufc); 
					
					memcpy(&req_ufhd, &ack_ufhd, sizeof(unity_file_hd_t));
					req_ufhd.div_seq = UNIV_DIV_SEQ_BC;  //发送0xffff表示泛文件接收完毕
					memset(&txbuf, 0, sizeof(txbuf));
					n = ndev_protocol_univ_update_ack_bc(&txbuf, &req_ufhd, sizeof(unity_file_hd_t));
					if(n < 0)
						return lib_thread_exit((void *)NULL);	

					ndev_tcp_send_to_server_with_escape(txbuf, n, 1000);	//告诉中心接收完毕
				
					__unity_file_verify(&ack_ufhd); //校验数据

					return lib_thread_exit((void *)NULL);	
				}
				
				div_seq++; //分割序列递增
				req_ufhd.div_seq = htons(div_seq); //继续请求下一包数据

				fprintf(stderr, "Next Div Seq = %d\n", div_seq);
				continue;
			}
			break;
		}
	}

	return lib_thread_exit((void *)NULL);
}

/*
 * 泛文件下发
 */
static void *__unity_file_broadcast_handle(void *arg)
{		
	unity_file_hd_t *p_hd = (unity_file_hd_t *)arg;
	if(p_hd == NULL)
		return lib_thread_exit((void *)NULL);	

	unsigned char unity_file_f = p_hd->attr[0];
	
	int i, len;
	int err = LIB_UF_ERROR;
	unsigned char s_buf[512] = {0};
	unsigned char d_buf[512] = {0};
	unity_file_st_t ufile;
	unsigned char type = p_hd->ufc;
	unsigned short total = ntohs(p_hd->total);
	struct unity_file_struct file_struct;
		
	enum UF_DB uf_used_db = __get_used_db(p_hd->ufc);
	if(uf_used_db == UF_DB_NOT)
	{
		fprintf(stderr, "%s:get used db failed!\n", __FUNCTION__);
		return lib_thread_exit((void *)NULL);	
	}

	/* 广播泛文件信息 */
	if(unity_file_f == 0x55) //代表是节点机下载后广播
	{
		memset(&file_struct, 0, sizeof(file_struct));
		file_struct.total = p_hd->total;
		file_struct.file_seq = p_hd->file_seq;
		file_struct.div_seq = p_hd->div_seq;
		file_struct.status = 1; //?
		time(&(file_struct.start_time));
		unity_file_info_struct_broadcast_put(&file_struct, p_hd->ufc); 
	}
	else if(unity_file_f == 0xAA)
	{
		//printf("\n----------unity_file_f == 0xAA-------1------\n"); //for test
			
		memset(&file_struct, 0, sizeof(file_struct));
		file_struct.total = p_hd->total;
		file_struct.file_seq = p_hd->file_seq;
		file_struct.div_seq = p_hd->div_seq;
		file_struct.status = 1; //?
		time(&(file_struct.start_time));
		unity_file_info_struct_stake_put(&file_struct, p_hd->ufc); //锁桩下载
	}
	
	memset(d_buf, 0, sizeof(d_buf));
	len = sae_protocol_blacklist_version_broadcast(&d_buf); ///设备签到包中广播黑名单版本
	if(len > 0)
	{
		stake_ctos_put_priority(SAE_CAN_BROADCAST_ID, d_buf, len, CAN_PRIORITY_LOW);
		lib_sleep(5);
	}

	__unity_file_atmoic_set_val(p_hd->ufc, UF_ATMOIC_RUN);  //开启标志
	
	for(i = 1; i <= total; i++)  //这里要控制一下，打断旧版本下载
	{
		memset(s_buf, 0, sizeof(s_buf));
		memset(d_buf, 0, sizeof(d_buf));
		memset(&ufile, 0, sizeof(unity_file_st_t));

		if(__unity_file_atmoic_get_val(p_hd->ufc) == UF_ATMOIC_STOP)  //其他线程通知退出
			goto Done;
			

		err = lib_unity_file_select_data(g_unity_file_db, type , uf_used_db, i, &ufile);
		if(err == LIB_UF_OK)
		{
			memcpy(s_buf, &(ufile.hd), ufile.length);

			len = sae_protocol_blacklist_broadcast(d_buf, s_buf, ufile.length); //泛文件传输请求
			if(len > 0)
				stake_ctos_put_priority(SAE_CAN_BROADCAST_ID, d_buf, len, CAN_PRIORITY_LOW);
		}

		fprintf(stderr, "N->S:(broadcast)%s, type:0x%02x, total:%d, seq:%d\n", __FUNCTION__, type, total, i);

		///分割序号
		if(unity_file_f == 0x55) 
			unity_file_info_struct_broadcast_put_div_seq(ufile.hd.div_seq, ufile.hd.ufc);
		else if(unity_file_f == 0xAA) {
			unity_file_info_struct_stake_put_div_seq(ufile.hd.div_seq, ufile.hd.ufc);
			//printf("\n----------unity_file_f == 0xAA------2-------\n"); //for test
		}
		lib_msleep(800);
	}

	/* 广播泛文件信息 */
	if(unity_file_f == 0x55) 
	{
		memset(&file_struct, 0, sizeof(file_struct));
		unity_file_info_struct_broadcast_get(&file_struct, ufile.hd.ufc);
		file_struct.status = 2;
		time(&(file_struct.end_time));
		unity_file_info_struct_broadcast_put(&file_struct, ufile.hd.ufc); 
	}
	else if(unity_file_f == 0xAA) 
	{
		memset(&file_struct, 0, sizeof(file_struct));
		unity_file_info_struct_stake_get(&file_struct, ufile.hd.ufc);
		file_struct.status = 2;
		time(&(file_struct.end_time));
		unity_file_info_struct_stake_put(&file_struct, ufile.hd.ufc); 
	}

Done:
	__unity_file_atmoic_set_val(p_hd->ufc, UF_ATMOIC_INIT); //还原标志
	
	return lib_thread_exit((void *)NULL);	
}

static int __unity_file_unicast_handle(void *ptr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;

	sae_rx_buf_t *srxbuf = (sae_rx_buf_t *)ptr;
	if(srxbuf == NULL)
		return B_DEV_ERROR;

	unity_file_hd_t hd;
	memcpy(&hd, &(S_RX_DATA_PTR(srxbuf)), sizeof(unity_file_hd_t));

	int len;
	int err = LIB_UF_ERROR;
	unsigned short uidx = ntohs(hd.div_seq);  	//分割序列
	unsigned char d_buf[512] = {0};
	unity_file_st_t ufile;
	unsigned char s_addr = S_RX_SADDR_PTR(srxbuf);

	enum UF_TYPE utype = hd.ufc; //泛文件类型
	enum UF_DB udb = __get_used_db(utype); 
	if(udb == UF_DB_NOT)
		return B_DEV_ERROR;

	err = lib_unity_file_select_data(g_unity_file_db,  utype, udb, uidx, &ufile);
	if(err == LIB_UF_OK)
	{
		len = sae_protocol_firmware_unicast(s_addr, S_RX_SN_PTR(srxbuf), d_buf, &(ufile.hd), ufile.length);
		if(len > 0)
		{
			stake_ctos_put_priority(s_addr, d_buf, len, CAN_PRIORITY_HIGH);
			
			fprintf(stderr, "S->N:(unicast)%s,  ufc:0x%02x, seq:%d\n", __FUNCTION__, utype, uidx);
		}
	}
	
	return B_DEV_EOK;
}

/*
 * 泛文件检测
 */
static int __unity_file_verify(unity_file_hd_t *hd)
{
	if(hd == NULL)
		return B_DEV_ERROR;
	
	int err, i, n, recv_cnt = 0;
	unsigned short total = ntohs(hd->total);
	enum UF_TYPE utype = hd->ufc;
	unity_file_st_t ufile;
	pthread_t thr;

	memset(&ufile, 0, sizeof(unity_file_st_t));

	enum UF_DB ufdb = __get_idle_db(utype);
	if(ufdb == UF_DB_NOT)
		return B_DEV_ERROR;

	for(i = 1; i <= total; i++) 	//检查接收情况
	{
		err = lib_unity_file_select_data(g_unity_file_db, utype, ufdb, i, &ufile);
		if(err == LIB_UF_OK)
			recv_cnt++;
	}			

	fprintf(stderr, "Recv All Cnt: %d\n", recv_cnt);

	if(recv_cnt == total) 
	{
		/* 接收完毕才切换数据库 */
		__set_used_db(hd); //设置使用数据库标志

		/* 接收完毕，通知广播线程退出 2016-04-11 */
		__unity_file_atmoic_set_val(hd->ufc, UF_ATMOIC_STOP);
		lib_sleep(3);  //休眠，让其他线程可以退出

		
		hd->attr[0] = 0x55;  //代表是节点机广播
		lib_normal_thread_create(&thr, __unity_file_broadcast_handle, hd);
	}
	
	return B_DEV_EOK;
}



void unity_file_handle_init(void)
{
	__unity_file_notify_init();
	
	g_unity_file_hd_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(g_unity_file_hd_mutex, NULL);

	memset(&g_ufile_info, 0, sizeof(g_ufile_info));
	g_ufile_info_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(g_ufile_info_mutex, NULL);
}

void unity_file_handle_release(void)
{
	__unity_file_notify_release();

	pthread_mutex_destroy(g_unity_file_hd_mutex);
	free(g_unity_file_hd_mutex);
	g_unity_file_hd_mutex = NULL;

	pthread_mutex_destroy(g_ufile_info_mutex);
	free(g_ufile_info_mutex);
	g_ufile_info_mutex = NULL;
}





void unity_file_hd_get(unity_file_hd_t *hd, const unsigned char type)
{
	__unity_file_hd_get(hd, type);
}

void unity_file_hd_put(unity_file_hd_t *hd, const unsigned char type)
{
	__unity_file_hd_put(hd, type);
}

void unity_file_atmoic_set_val(const unsigned char type, const unsigned int val)
{
	__unity_file_atmoic_set_val(type, val);
}

unsigned int unity_file_atmoic_get_val(const unsigned char type)
{
	return __unity_file_atmoic_get_val(type);
}

int unity_file_notify_put(const unsigned int notify, unity_file_hd_t *hd, const unsigned char utype)
{
	return __unity_file_notify_put(notify, hd, utype);
}

int unity_file_notify_timedwait(const unsigned int sec, unsigned int *notify, unity_file_hd_t *hd,  const unsigned char utype)
{
	return __unity_file_notify_timedwait(sec, notify, hd, utype);
}



int unity_file_handle_to_ndev(void *ptr, const unsigned int size)
{
	if(ptr == NULL)
		return B_DEV_ERROR;

	int ret = B_DEV_ERROR;
	int data_len;
	unity_file_st_t ufile;
	int ufile_len = sizeof(unity_file_st_t) - 2;//-2:数据长度
	n_data_attr_t attr;
	
	memset(&ufile, 0, sizeof(unity_file_st_t));
	memset(&attr, 0, sizeof(n_data_attr_t));
	
	ndev_rx_buf_t *nrxbuf = (ndev_rx_buf_t *)ptr;
	if(nrxbuf == NULL)
		return B_DEV_ERROR;

	data_len = ntohs(N_RX_LEN_PTR(nrxbuf));
	if(data_len > ufile_len)  
	{
		SYS_LOG_ERR("%s: network receive data length(%d) > ufile MAX length(%s)", __FUNCTION__, data_len, ufile_len);
		return B_DEV_ERROR;
	}

	memcpy(&(ufile.hd), N_RX_DATA_PTR(nrxbuf), data_len); //拷贝泛文件头
	ufile.length = data_len; //泛文件实际长度


	unsigned short s_len = 0;
	unsigned char txbuf[512] = {0};
	unsigned char dev_attr = 0;
	
	switch(ufile.hd.ufc)
	{
		case UF_TYPE_STAKE_TRADE_RECORD: //锁桩交易记录
		{
			s_len = ntohs(N_RX_LEN_PTR(nrxbuf));
			dev_attr = N_RX_DATA_ATTR_PTR(nrxbuf);
			memcpy(&attr, &dev_attr, sizeof(dev_attr));
			if(attr.pass == 1)	//透传数据
			{
				ret = sae_protocol_trading_records_pass(N_RX_DEV_ADDR_PTR(nrxbuf), txbuf, N_RX_DATA_PTR(nrxbuf), s_len);
				if(ret > 0)
				{
					stake_ctos_put_priority(N_RX_DEV_ADDR_PTR(nrxbuf), txbuf, ret, CAN_PRIORITY_HIGH);
				}
			}			
		}
		break;

		case UF_TYPE_LOG_RECORD:  //日志集合
		{
			s_len = ntohs(N_RX_LEN_PTR(nrxbuf));
			dev_attr = N_RX_DATA_ATTR_PTR(nrxbuf);
			memcpy(&attr, &dev_attr, sizeof(dev_attr));
			if(attr.pass == 1)	//透传数据
			{
				ret = sae_protocol_trading_records_pass(N_RX_DEV_ADDR_PTR(nrxbuf), txbuf, N_RX_DATA_PTR(nrxbuf), s_len);
				if(ret > 0)
				{
					stake_ctos_put_priority(N_RX_DEV_ADDR_PTR(nrxbuf), txbuf, ret, CAN_PRIORITY_HIGH);
				}
			}	
		}
		break;

		case UF_TYPE_EX_CONSUME_RECORD: //异常处理扣费交易记录
		{
			fprintf(stderr, "UF_TYPE_EX_CONSUME_RECORD\n");
		}
		break;

		default:
			__unity_file_ufc(nrxbuf, &ufile, data_len);
	}
	return B_DEV_EOK;
}

int unity_file_request_handle(unity_file_hd_t *hd)
{
	if(hd == NULL)
		return B_DEV_ERROR;

	pthread_t thr;
	enum UF_TYPE utype = hd->ufc;   //文件类别
	enum UF_DB idle_uf_db = __get_idle_db(utype);
	if(idle_uf_db == UF_DB_NOT)
	{
		fprintf(stderr, "%s:get idle db failed!\n", __FUNCTION__);
		lib_zfree(hd);
		return B_DEV_ERROR;
	}

	unity_file_st_t ufile;
	memset(&ufile, 0, sizeof(unity_file_st_t));
	memcpy(&(ufile.hd), hd, sizeof(unity_file_hd_t));

	/* 清空备份数据库 */
	lib_unity_file_delete_table(g_unity_file_db, utype, idle_uf_db);
 	lib_unity_file_replace_data(g_unity_file_db, utype, idle_uf_db, 0, &ufile); //插入数据库

	hd->div_seq = htons(hd->div_seq + 1); //请求下一包
	
	lib_normal_thread_create(&thr, __unity_file_request_thread, hd);
	
	return B_DEV_EOK;
}



int unity_file_except_handle(void)
{
	int i;
	int err = LIB_UF_ERROR;
	enum UF_TYPE uf_type;
	enum UF_DB uf_db = UF_DB_NOT;
	unsigned int max_seq = 0;
	unsigned int total = 0;
	unity_file_st_t ufile;
	pthread_t thr[UNITY_FILE_SZ];
	
	for(i = 0; i < UNITY_FILE_SZ; i++)
	{
		switch(i)
		{
			case 0:
				uf_type = UF_TYPE_TOTAL_BL;
				break;

			case 1:
				uf_type = UF_TYPE_INCREMENT_BL;
				break;

			case 2:
				uf_type = UF_TYPE_DECREMENT_BL;
				break;

			case 3:
				uf_type = UF_TYPE_TEMPORARY_BL;
				break;		

			case 4:
				uf_type = UF_TYPE_STAKE_FW;
				break;	

			case 5:
				uf_type = UF_TYPE_STAKE_PARA;
				break;	

			case 6:
				uf_type = UF_TYPE_UNIONPAY_KEY;
				break;
		}

		uf_db = __get_used_db(uf_type); //在用数据库
		if(uf_db == UF_DB_NOT)
			continue;

		memset(&ufile, 0, sizeof(unity_file_st_t));
		err = lib_unity_file_select_data(g_unity_file_db, uf_type, uf_db, 0, &ufile); //读取在用数据库
		if(err == LIB_UF_OK)
		{
			__unity_file_hd_put(&(ufile.hd), uf_type); //有什么用?
		}

		uf_db = UF_DB_NOT;
		uf_db = __get_idle_db(uf_type); //空闲数据库
		if(uf_db == UF_DB_NOT)
			continue;

		memset(&ufile, 0, sizeof(unity_file_st_t));
		err = lib_unity_file_select_data(g_unity_file_db, uf_type, uf_db, 0, &ufile); //读取空闲数据库
		if(err == LIB_UF_OK)
		{	
			err = lib_unity_file_max(g_unity_file_db, uf_type, uf_db, &max_seq); //读取表项最大数
			if(err == LIB_UF_OK)
			{
				total = ntohs(ufile.hd.total);
				if((max_seq != UNIV_DIV_SEQ_BC) && (max_seq < total))
				{
					unity_file_hd_t *t_hd = (unity_file_hd_t *)lib_zmalloc(sizeof(unity_file_hd_t));
					if(t_hd == NULL)
						continue;

					memcpy(t_hd, &(ufile.hd), sizeof(unity_file_hd_t));
					t_hd->div_seq = htons(max_seq + 1); ////继续接收的泛文件数据包序列

					SYS_LOG_INFO("%s: unity file UFC = 0x%02x, MAX SEQ = %d", __FUNCTION__, t_hd->ufc, max_seq);
					fprintf(stderr, "%s: unity file UFC = 0x%02x, MAX SEQ = %d\n", __FUNCTION__, t_hd->ufc, max_seq);

					lib_normal_thread_create(&thr[i], __unity_file_request_thread, t_hd); //泛文件请求线程
				}
			}
		}
	}

	return B_DEV_EOK;
}

/*
 * 泛文件处理-锁桩
 */
int unity_file_handle_to_sae(void *ptr, const unsigned int size)
{
	if(ptr == NULL)
		return B_DEV_ERROR;

	sae_rx_buf_t *srxbuf = (sae_rx_buf_t *)ptr;
	if(srxbuf == NULL)
		return B_DEV_ERROR;

	int ret =  LIB_UF_ERROR;
	int i = 0;
	unity_file_hd_t hd;
	unity_file_st_t ufile;
	memcpy(&hd, &(S_RX_DATA_PTR(srxbuf)), sizeof(unity_file_hd_t));

	unsigned short ufile_seq = ntohs(hd.file_seq);  //文件序号
	unsigned short ufile_div_seq = ntohs(hd.div_seq);  //分割序列
	unsigned short ufile_total = ntohs(hd.total); //分割总数
	unsigned char ufile_ufc = hd.ufc; //文件类别
	unsigned int ufile_atmoic = UF_ATMOIC_END;
	pthread_t thr[UNITY_FILE_SZ];
	unsigned int s_len = 0;
		
	if(ufile_ufc == UF_TYPE_STAKE_TRADE_RECORD) //锁桩交易记录,透传方式 (节点机带桩机透传给中心)
	{
		s_len = ntohs(S_RX_LEN_PTR(srxbuf));
		ndev_trading_records_upload(S_RX_SADDR_PTR(srxbuf), &(S_RX_DATA_PTR(srxbuf)), s_len);  	
		return B_DEV_EOK;
	}

	if(ufile_ufc == UF_TYPE_LOG_RECORD)  //日志集合,透传 (节点机带桩机透传给中心)
	{
		s_len = ntohs(S_RX_LEN_PTR(srxbuf));
		ndev_trading_records_upload(S_RX_SADDR_PTR(srxbuf), &(S_RX_DATA_PTR(srxbuf)), s_len);  	
		return B_DEV_EOK;
	}


	fprintf(stderr, "%s: s_addr:0x%d, file_seq:%d, total:%d, div_seq:%d, ufc:0x%02x\n", __FUNCTION__, S_RX_SADDR_PTR(srxbuf), ufile_seq, ufile_total, ufile_div_seq, ufile_ufc);
	
	if(ufile_div_seq == UNIV_DIV_SEQ_BC) //请求广播 (UNIV_DIV_SEQ_BC:泛文件接收完毕)
	{
		ufile_atmoic = __unity_file_atmoic_get_val(ufile_ufc);

		fprintf(stderr, "Ufile Atmoic: %d, Ufile Ufc:0x%02x\n", ufile_atmoic, ufile_ufc);
		
		if(ufile_atmoic == UF_ATMOIC_INIT)
		{
			fprintf(stderr, "STAKE Request Broadcast [%02x]: file_seq:%d, total:%d, div_seq:%d, atmoic:%d\n", ufile_ufc, ufile_seq, ufile_total, ufile_div_seq, ufile_atmoic);

			unity_file_hd_t *p_hd = (unity_file_hd_t *)malloc(sizeof(unity_file_hd_t));
			if(p_hd == NULL)
				return B_DEV_ERROR;
			
			switch(ufile_ufc)
			{
				case UF_TYPE_TOTAL_BL:
					fprintf(stderr, "Request [UF_TYPE_TOTAL_BL] Broadcast\n");
					i = 0;
					break;

				case UF_TYPE_INCREMENT_BL:
					fprintf(stderr, "Request [UF_TYPE_INCREMENT_BL] Broadcast\n");
					i = 1;
					break;

				case UF_TYPE_DECREMENT_BL:
					fprintf(stderr, "Request [UF_TYPE_DECREMENT_BL] Broadcast\n");
					i = 2;
					break;

				case UF_TYPE_TEMPORARY_BL:
					fprintf(stderr, "Request [UF_TYPE_TEMPORARY_BL] Broadcast\n");
					i = 3;
					break;

				case UF_TYPE_STAKE_FW:
					fprintf(stderr, "Request [UF_TYPE_STAKE_FW] Broadcast\n");
					i = 4;
					break;

				case UF_TYPE_STAKE_PARA:
					fprintf(stderr, "Request [UF_TYPE_STAKE_PARA] Broadcast\n");
					i = 5;
					break;

				case UF_TYPE_UNIONPAY_KEY:
					fprintf(stderr, "Request [UF_TYPE_UNIONPAY_KEY] Broadcast\n");
					i = 6;
					break;
			}

			if(i >= UNITY_FILE_SZ)
			{
				free(p_hd);
				p_hd = NULL;
				return B_DEV_ERROR;
			}

			__unity_file_atmoic_set_val(ufile_ufc, UF_ATMOIC_STOP); //有新版本泛文件下发
			
			enum UF_DB uf_used_db = __get_used_db(ufile_ufc);
			if(uf_used_db == UF_DB_NOT) //没有可用泛文件
			{
				free(p_hd);
				p_hd = NULL;
				return B_DEV_ERROR;
			}
			
			ret = lib_unity_file_select_data(g_unity_file_db, ufile_ufc , uf_used_db, 0, &ufile);
			if(ret == LIB_UF_OK)
			{
				memcpy(p_hd, &(ufile.hd), sizeof(unity_file_hd_t));

				p_hd->attr[0] = 0xAA;  //代表是锁桩广播
				lib_normal_thread_create(&thr[i], __unity_file_broadcast_handle_to_sae, p_hd); //创建广播线程
			}
			else
			{
				free(p_hd);
				p_hd = NULL;
				return B_DEV_ERROR;
			}
		}
	}
	else
	{
		__unity_file_unicast_handle(ptr); //请求单播
	}
	
	return B_DEV_EOK;
}

static void *__unity_file_broadcast_handle_to_sae(void *arg)
{
	__unity_file_broadcast_handle(arg);
	
	unity_file_hd_t *p_hd = (unity_file_hd_t *)arg;
	if(p_hd == NULL)
		return lib_thread_exit((void *)NULL);	
	
	__unity_file_atmoic_set_val(p_hd->ufc, UF_ATMOIC_INIT);
	
	free(p_hd);
	p_hd = NULL;

	return lib_thread_exit((void *)NULL);	
}


int unity_file_info_put(unity_file_info_t *info)
{
	if(info == NULL)
		return B_DEV_ERROR;

	pthread_mutex_lock(g_ufile_info_mutex);
	memcpy(&g_ufile_info, info, sizeof(unity_file_info_t));
	pthread_mutex_unlock(g_ufile_info_mutex);
	
	return B_DEV_EOK;
}

int unity_file_info_get(unity_file_info_t *info)
{
	if(info == NULL)
		return B_DEV_ERROR;

	pthread_mutex_lock(g_ufile_info_mutex);
	memcpy(info, &g_ufile_info, sizeof(unity_file_info_t));
	pthread_mutex_unlock(g_ufile_info_mutex);
	
	return B_DEV_EOK;	
}

int unity_file_info_struct_ndev_put(struct unity_file_struct *file_struct, const enum UF_TYPE utype)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;

	pthread_mutex_lock(g_ufile_info_mutex);

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(&g_ufile_info.ndev_ufile_st[0], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(&g_ufile_info.ndev_ufile_st[1], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(&g_ufile_info.ndev_ufile_st[2], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(&g_ufile_info.ndev_ufile_st[3], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(&g_ufile_info.ndev_ufile_st[4], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			memcpy(&g_ufile_info.ndev_ufile_st[5], file_struct, sizeof(struct unity_file_struct));
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);

	return B_DEV_EOK;	
}

int unity_file_info_struct_ndev_get(struct unity_file_struct *file_struct, const enum UF_TYPE utype)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;	

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(file_struct, &g_ufile_info.ndev_ufile_st[0], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(file_struct, &g_ufile_info.ndev_ufile_st[1], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(file_struct, &g_ufile_info.ndev_ufile_st[2], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(file_struct, &g_ufile_info.ndev_ufile_st[3], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(file_struct, &g_ufile_info.ndev_ufile_st[4], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			memcpy(file_struct, &g_ufile_info.ndev_ufile_st[5], sizeof(struct unity_file_struct));
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);
	
	return B_DEV_EOK;	
}

int unity_file_info_struct_broadcast_put(struct unity_file_struct *file_struct, const enum UF_TYPE utype)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;

	pthread_mutex_lock(g_ufile_info_mutex);

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(&g_ufile_info.broadcast_ufile_st[0], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(&g_ufile_info.broadcast_ufile_st[1], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(&g_ufile_info.broadcast_ufile_st[2], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(&g_ufile_info.broadcast_ufile_st[3], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(&g_ufile_info.broadcast_ufile_st[4], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			memcpy(&g_ufile_info.broadcast_ufile_st[5], file_struct, sizeof(struct unity_file_struct));
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);

	return B_DEV_EOK;	
}

int unity_file_info_struct_broadcast_get(struct unity_file_struct *file_struct, const enum UF_TYPE utype)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;	

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(file_struct, &g_ufile_info.broadcast_ufile_st[0], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(file_struct, &g_ufile_info.broadcast_ufile_st[1], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(file_struct, &g_ufile_info.broadcast_ufile_st[2], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(file_struct, &g_ufile_info.broadcast_ufile_st[3], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(file_struct, &g_ufile_info.broadcast_ufile_st[4], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			memcpy(file_struct, &g_ufile_info.broadcast_ufile_st[5], sizeof(struct unity_file_struct));
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);
	
	return B_DEV_EOK;		
}

int unity_file_info_struct_broadcast_put_div_seq(const unsigned short div_seq, const enum UF_TYPE utype)
{
	pthread_mutex_lock(g_ufile_info_mutex);

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			g_ufile_info.broadcast_ufile_st[0].div_seq = div_seq;
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			g_ufile_info.broadcast_ufile_st[1].div_seq = div_seq;
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			g_ufile_info.broadcast_ufile_st[2].div_seq = div_seq;
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			g_ufile_info.broadcast_ufile_st[3].div_seq = div_seq;
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			g_ufile_info.broadcast_ufile_st[4].div_seq = div_seq;
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			g_ufile_info.broadcast_ufile_st[5].div_seq = div_seq;
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);

	return B_DEV_EOK;	
}

int unity_file_info_struct_stake_put(struct unity_file_struct *file_struct, const enum UF_TYPE utype)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;

	pthread_mutex_lock(g_ufile_info_mutex);

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(&g_ufile_info.stake_ufile_st[0], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(&g_ufile_info.stake_ufile_st[1], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(&g_ufile_info.stake_ufile_st[2], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(&g_ufile_info.stake_ufile_st[3], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(&g_ufile_info.stake_ufile_st[4], file_struct, sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			memcpy(&g_ufile_info.stake_ufile_st[5], file_struct, sizeof(struct unity_file_struct));
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);

	return B_DEV_EOK;	
}

int unity_file_info_struct_stake_get(struct unity_file_struct *file_struct, const enum UF_TYPE utype)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;	

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			memcpy(file_struct, &g_ufile_info.stake_ufile_st[0], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			memcpy(file_struct, &g_ufile_info.stake_ufile_st[1], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			memcpy(file_struct, &g_ufile_info.stake_ufile_st[2], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			memcpy(file_struct, &g_ufile_info.stake_ufile_st[3], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			memcpy(file_struct, &g_ufile_info.stake_ufile_st[4], sizeof(struct unity_file_struct));
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			memcpy(file_struct, &g_ufile_info.stake_ufile_st[5], sizeof(struct unity_file_struct));
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);
	
	return B_DEV_EOK;	
}

int unity_file_info_struct_stake_put_div_seq(const unsigned short div_seq, const enum UF_TYPE utype)
{
	pthread_mutex_lock(g_ufile_info_mutex);

	switch(utype)
	{
		case UF_TYPE_TOTAL_BL:
		{
			g_ufile_info.stake_ufile_st[0].div_seq = div_seq;
		}
		break;

		case UF_TYPE_INCREMENT_BL:
		{
			g_ufile_info.stake_ufile_st[1].div_seq = div_seq;
		}
		break;

		case UF_TYPE_DECREMENT_BL:
		{
			g_ufile_info.stake_ufile_st[2].div_seq = div_seq;
		}
		break;

		case UF_TYPE_TEMPORARY_BL:
		{
			g_ufile_info.stake_ufile_st[3].div_seq = div_seq;
		}
		break;

		case UF_TYPE_STAKE_FW:
		{
			g_ufile_info.stake_ufile_st[4].div_seq = div_seq;
		}
		break;

		case UF_TYPE_STAKE_PARA:
		{
			g_ufile_info.stake_ufile_st[5].div_seq = div_seq;
		}
		break;
		
		default:
			pthread_mutex_unlock(g_ufile_info_mutex);
			return B_DEV_ERROR;
	}

	pthread_mutex_unlock(g_ufile_info_mutex);

	return B_DEV_EOK;		
}

void unity_file_config_printf(void)
{
	fprintf(stderr, "\n----------------- unity fileversion ---------------\n");

	unity_file_config_t uconfig;
	memset(&uconfig, 0, sizeof(unity_file_config_t));

	int ret = lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	if(ret != LIB_UF_OK)
		return;

	char total_last_ver[16] = {0};
	char inc_last_ver[16] = {0};
	char dec_last_ver[16] = {0};
	char temp_last_ver[16] = {0};
	char stake_fw_last_ver[16] = {0};
	char stake_para_last_ver[16] = {0};
	char unionpay_key_last_ver[16] = {0};
	
	strncpy(total_last_ver, uconfig.total_bl_last_ver, 8);
	strncpy(inc_last_ver, uconfig.inc_bl_last_ver, 8);
	strncpy(dec_last_ver, uconfig.dec_bl_last_ver, 8);
	strncpy(temp_last_ver, uconfig.temp_bl_last_ver, 8);
	strncpy(stake_fw_last_ver, uconfig.stake_fw_last_ver, 8);
	strncpy(stake_para_last_ver, uconfig.stake_para_last_ver, 8);
	strncpy(unionpay_key_last_ver, uconfig.unionpay_key_last_ver, 8);

	fprintf(stderr, "total_last_ver: %s\n", total_last_ver);
	fprintf(stderr, "inc_last_ver: %s\n", inc_last_ver);
	fprintf(stderr, "dec_last_ver: %s\n", dec_last_ver);
	fprintf(stderr, "temp_last_ver: %s\n", temp_last_ver);	
	fprintf(stderr, "stake_fw_last_ver: %s\n", stake_fw_last_ver);
	fprintf(stderr, "stake_para_last_ver: %s\n", stake_para_last_ver);
	fprintf(stderr, "unionpay_key_last_ver: %s\n", unionpay_key_last_ver);	

	fprintf(stderr, "use_total_db: %d\n", uconfig.use_total_bl_db);
	fprintf(stderr, "use_inc_db: %d\n", uconfig.use_inc_bl_db);
	fprintf(stderr, "use_dec_db: %d\n", uconfig.use_dec_bl_db);
	fprintf(stderr, "use_temp_db: %d\n", uconfig.use_temp_bl_db);
	fprintf(stderr, "use_stake_fw_db: %d\n", uconfig.use_stake_fw_db);
	fprintf(stderr, "use_stake_para_db: %d\n", uconfig.use_stake_para_db);
	fprintf(stderr, "use_unionpay_key_db: %d\n", uconfig.use_unionpay_key_db);

	fprintf(stderr, "use_total_seq: %d\n", uconfig.use_total_bl_seq);
	fprintf(stderr, "use_inc_seq: %d\n", uconfig.use_inc_bl_seq);
	fprintf(stderr, "use_dec_seq: %d\n", uconfig.use_dec_bl_seq);
	fprintf(stderr, "use_temp_seq: %d\n", uconfig.use_temp_bl_seq);
	fprintf(stderr, "use_stake_fw_seq: %d\n", uconfig.use_stake_fw_seq);
	fprintf(stderr, "use_stake_para_seq: %d\n", uconfig.use_stake_para_seq);
	fprintf(stderr, "use_unionpay_key_seq: %d\n", uconfig.use_unionpay_key_seq);

	fprintf(stderr, "\n----------------- ----------------- ---------------\n");
}

int unity_file_info_struct_printf(struct unity_file_struct *file_struct)
{
	if(file_struct == NULL)
		return B_DEV_ERROR;

	fprintf(stderr, "\n----------------- unity file info ---------------\n");
	
	fprintf(stderr, "total:%d\n", ntohs(file_struct->total));
	fprintf(stderr, "file_seq:%d\n", ntohs(file_struct->file_seq));
	fprintf(stderr, "div_seq:%d\n", ntohs(file_struct->div_seq));
	fprintf(stderr, "status:%d\n", file_struct->status);

	fprintf(stderr, "start time:%s", asctime(gmtime(&file_struct->start_time)));
	fprintf(stderr, "end time:%s", asctime(gmtime(&file_struct->end_time)));

	
	fprintf(stderr, "\n----------------- ----------------- ---------------\n");
	
	return B_DEV_EOK;
}


