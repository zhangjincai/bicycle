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
#include "defines.h"
#include "lib_general.h"
#include "lib_zmalloc.h"
#include "device_config.h"
#include "universal_file.h"
#include "var_data.h"
#include "ndev_info.h"
#include "utils.h"
#include "unity_file_handle.h"
#include "sae_info.h"
#include "sae_protocol.h"


extern lib_unity_file_t *g_unity_file_db;


/* 节点机主动呼叫锁桩签到 */
int sae_protocol_req_req(void *ptr, const unsigned char d_addr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	unsigned char id[6] = {0};
	device_config_t conf;
	s_var_data_t *var = NULL;
	unsigned short nstat;
	char s_time[13] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};
	
	var = s_var_data_alloc();
	if(var != NULL)
	{
		device_config_get(&conf);
		utils_get_sys_time_s(s_time);
		ndev_info_stat_NOR_get(&nstat);
		
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, SAE_DEV_REG_REQ);

		s_var_data_set(var, SAE_N_REG_W, 2);  //节点机主动签到和桩机应答
		s_var_data_set(var, "00", 2);
		s_var_data_char(var, '&');	
		
		s_var_data_set(var, SAE_N_ID_W, 2);  //节点机编号
		sprintf(id, "%05d", conf.nconf.term_id); //转字符
		s_var_data_set(var, id, 5);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_N_STATUS_W, 2);  //节点机状态
		s_var_data_b64_set(var, &nstat, 2);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_N_TIME_W, 2);		//节点机时间
		s_var_data_set(var, s_time, 12);
		s_var_data_char(var, '&');

		lib_get_macaddr("eth1", macaddr);  //节点机MAC 地址
		lib_hex_to_str(macaddr, 6, s_macaddr);
		s_var_data_set(var, SAE_N_MAC_W, 2);	
		s_var_data_set(var, s_macaddr, 12);
		s_var_data_char(var, '&');

		
		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}	

	return B_DEV_ERROR;	
}

/* 锁桩心跳请求 */
int sae_protocol_bheart_req(void *ptr, const unsigned char d_addr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	unsigned char temp[32] = {0};
	unsigned char id[6] = {0};
	device_config_t conf;
	unity_file_config_t uconfig;
	unsigned short nstat;
	unsigned char quantity = 0;
	s_var_data_t *var = NULL;
	char s_time[13] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};

	memset(&uconfig, 0, sizeof(unity_file_config_t));
	var = s_var_data_alloc();
	if(var != NULL)
	{
		device_config_get(&conf);
		utils_get_sys_time_s(s_time);
		ndev_info_stat_NOR_get(&nstat);
		lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
		
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, SAE_DEV_REG_REQ);

		s_var_data_set(var, SAE_N_BHEART_W, 2);  //节点机主动心跳和桩机心跳应答
		s_var_data_set(var, "00", 2);
		s_var_data_char(var, '&');	
		
		s_var_data_set(var, SAE_N_ID_W, 2);  		//节点机编号
		sprintf(id, "%05d", conf.nconf.term_id); 		//转字符
		s_var_data_set(var, id, 5);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_N_STATUS_W, 2);  //节点机状态
		s_var_data_b64_set(var, &nstat, 2);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_N_TIME_W, 2);	//节点机时间
		s_var_data_set(var, s_time, 12);
		s_var_data_char(var, '&');

		lib_get_macaddr("eth1", macaddr); 		 //节点机MAC 地址
		lib_hex_to_str(macaddr, 6, s_macaddr);
		s_var_data_set(var, SAE_N_MAC_W, 2);	
		s_var_data_set(var, s_macaddr, 12);
		s_var_data_char(var, '&');
		
		s_var_data_set(var, SAE_TOTAL_BLK_VER_W, 2);	  //总量黑名单
		s_var_data_set(var, uconfig.total_bl_last_ver, 8);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_INC_BLK_VER_W, 2);	  //增量黑名单
		s_var_data_set(var, uconfig.inc_bl_last_ver, 8);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_DEC_BLK_VER_W, 2);	  //减量黑名单
		s_var_data_set(var, uconfig.dec_bl_last_ver, 8);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_TEMP_BLK_VER_W, 2);	  //临时黑名单
		s_var_data_set(var, uconfig.temp_bl_last_ver, 8);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_PARA_VER_W, 2);	  	//锁桩配置版本
		s_var_data_set(var, uconfig.stake_para_last_ver, 8);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_VER_W, 2);	  //桩固件
		s_var_data_set(var, uconfig.stake_fw_last_ver, 8);
		s_var_data_char(var, '&');

		s_var_data_set(var, SAE_QUANTITY_W, 2);  		//锁桩总数
		quantity = sae_info_get_quantity();
		sprintf(temp, "%03d", quantity); 		//转字符
		s_var_data_set(var, temp, 3);
		s_var_data_char(var, '&');

	
		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}	

	return B_DEV_ERROR;
}

/* 节点机状态 */
int sae_protocol_ndev_stat(void *ptr, const unsigned char d_addr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;
	unsigned short nstat;
	
	var = s_var_data_alloc();
	if(var != NULL)
	{
		ndev_info_stat_NOR_get(&nstat);

		lib_printf("--------nstat-------", &nstat, 2);
		
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, SAE_DEV_REG_REQ);
		
		s_var_data_set(var, SAE_N_STATUS_W, 2);  //节点机状态
		s_var_data_b64_set(var, &nstat, 2);
		s_var_data_char(var, '&');

		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);	
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_blacklist_version_broadcast(void *ptr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
		
	int len = B_DEV_ERROR;
	unity_file_config_t uconfig;
	s_var_data_t *var = NULL;	
	
	var = s_var_data_alloc();
	if(var != NULL)
	{
		memset(&uconfig, 0, sizeof(unity_file_config_t));
		lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, SAE_CAN_BROADCAST_ID, SAE_DEV_REG_REQ);

		s_var_data_set(var, SAE_TOTAL_BLK_VER_W, 2);
		s_var_data_set(var, &(uconfig.total_bl_last_ver), 8);
		s_var_data_char(var, '&');
		
		s_var_data_set(var, SAE_INC_BLK_VER_W, 2);
		s_var_data_set(var, &(uconfig.inc_bl_last_ver), 8);
		s_var_data_char(var, '&');
		
		s_var_data_set(var, SAE_DEC_BLK_VER_W, 2);
		s_var_data_set(var, &(uconfig.dec_bl_last_ver), 8);
		s_var_data_char(var, '&');
		
		s_var_data_set(var, SAE_TEMP_BLK_VER_W, 2);
		s_var_data_set(var, &(uconfig.temp_bl_last_ver), 8);
		s_var_data_char(var, '&');

		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_firmware_version_broadcast(void *ptr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	unity_file_config_t uconfig;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		memset(&uconfig, 0, sizeof(unity_file_config_t));
		lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
		
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, SAE_CAN_BROADCAST_ID, SAE_DEV_REG_REQ);

		s_var_data_set(var, SAE_VER_W, 2);
		s_var_data_set(var, &(uconfig.stake_fw_last_ver), 8);
		s_var_data_char(var, '&');
	
		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_blacklist_broadcast(void *d_ptr, void *s_ptr, const unsigned int s_len)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	if(s_len == 0)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, SAE_CAN_BROADCAST_ID, SAE_FILE_TRANS_REQ);
		s_var_data_set(var, s_ptr, s_len);
		s_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_blacklist_unicast(const unsigned char d_addr, const unsigned char sn, void *d_ptr, void *s_ptr, const unsigned int s_len)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_ack(var, SAE_CAN_LOCAL_ID, d_addr, SAE_FILE_TRANS_REQ, sn);
		s_var_data_set(var, s_ptr, s_len);
		s_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_firmware_broadcast(void *d_ptr, void *s_ptr, const unsigned int s_len)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, SAE_CAN_BROADCAST_ID, SAE_FILE_TRANS_REQ);
		s_var_data_set(var, s_ptr, s_len);
		s_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_firmware_unicast(const unsigned char d_addr, const unsigned char sn, void *d_ptr, void *s_ptr, const unsigned int s_len)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_ack(var, SAE_CAN_LOCAL_ID, d_addr, SAE_FILE_TRANS_REQ, sn);
		s_var_data_set(var, s_ptr, s_len);
		s_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;
}

int sae_protocol_trading_records_pass(const unsigned char d_addr, void *d_ptr, void *s_ptr, const unsigned int s_len)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, SAE_FILE_TRANS_ACK);
		s_var_data_set(var, s_ptr, s_len);
		s_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;	
}

int sae_protocol_whole_pass(const unsigned char op, const unsigned char d_addr, void *d_ptr, void *s_ptr, const unsigned int s_len)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;	

	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, op);
		s_var_data_set(var, s_ptr, s_len);
		s_var_data_crc(var);
			
		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}

	return B_DEV_ERROR;	
}

int sae_protocol_led_refresh(void *ptr, const unsigned char d_addr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;
	char s_time[13] = {0};
	
	var = s_var_data_alloc();
	if(var != NULL)
	{
		utils_get_sys_time_s(s_time);
	
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, SAE_DEV_REG_REQ);

		s_var_data_set(var, SAE_N_TIME_W, 2);		//节点机时间
		s_var_data_set(var, s_time, 12);
		s_var_data_char(var, '&');

		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}	

	return B_DEV_ERROR;	
}

int sae_protocol_control_for_lock(void *ptr, const unsigned char d_addr,  sae_control_t *ctrl)
{
	if((ptr == NULL) || (ctrl == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	s_var_data_t *var = NULL;
	
	var = s_var_data_alloc();
	if(var != NULL)
	{
		s_var_data_hd_req(var, SAE_CAN_LOCAL_ID, d_addr, SAE_DEV_CTRL_REQ);

		s_var_data_set(var, SAE_CTRL_C, 2);		
		s_var_data_b64_set(var, ctrl, sizeof(sae_control_t));
		s_var_data_char(var, '&');

		s_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		s_var_data_free(var);
		
		return len;
	}	
	
	return B_DEV_ERROR;	
}


