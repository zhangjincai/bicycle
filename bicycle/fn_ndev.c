#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>

#include "configs.h"
#include "defines.h"
#include "var_data.h"
#include "fn_hash.h"
#include "node_device.h"
#include "universal_file.h"
#include "lib_general.h"
#include "lib_blacklist.h"
#include "ndev_protocol.h"
#include "sae_protocol.h"
#include "utils.h"
#include "ndev_info.h"
#include "gui_info.h"
#include "device_config.h"
#include "lib_watchdog.h"
#include "gui_info.h"
#include "gps.h"
#include "utils.h"
#include "fn_ndev.h"


extern struct gps_rxbuf g_gps_rxbuf2;  //GPS数据


//#define FN_DEF

#ifdef FN_DEF
#define FN_DBG(fmt, args...)	fprintf(stderr, fmt, ##args)
#else
#define FN_DBG(fmt, args...)
#endif

int fn_ndev_reg(fn_ndev_val_t *val);
int fn_ndev_stat(fn_ndev_val_t *val);
int fn_ndev_time(fn_ndev_val_t *val);
int fn_ndev_s_total_num(fn_ndev_val_t *val);
int fn_ndev_s_online_num(fn_ndev_val_t *val);
int fn_ndev_total_ver(fn_ndev_val_t *val);
int fn_ndev_inc_ver(fn_ndev_val_t *val);
int fn_ndev_dec_ver(fn_ndev_val_t *val);
int fn_ndev_temp_ver(fn_ndev_val_t *val);
int fn_ndev_s_blk_attr(fn_ndev_val_t *val);
int fn_ndev_s_attr(fn_ndev_val_t *val);
int fn_ndev_yct_stat(fn_ndev_val_t *val);
int fn_ndev_btheart_return(fn_ndev_val_t *val);
int fn_ndev_site_name_ctrl(fn_ndev_val_t *val);
int fn_ndev_univ_update_ctrl(fn_ndev_val_t *val);
int fn_ndev_btheart_time_set(fn_ndev_val_t *val);
int fn_ndev_s_lock_op(fn_ndev_val_t *val);
int fn_ndev_load_serv_ip_set(fn_ndev_val_t *val);
int fn_ndev_load_serv_port_set(fn_ndev_val_t *val);
int fn_ndev_load_serv2_ip_set(fn_ndev_val_t *val);
int fn_ndev_load_serv2_port_set(fn_ndev_val_t *val);
int fn_ndev_device_reboot(fn_ndev_val_t *val);
int fn_ndev_ftp_set(fn_ndev_val_t *val);
int fn_ndev_except_handle(fn_ndev_val_t *val);
int fn_ndev_ftp_download(fn_ndev_val_t *val);
int fn_ndev_update_ctrl(fn_ndev_val_t *val);
//int fn_ndev_rent_info(fn_ndev_val_t *val);
int fn_ndev_sae_all_phy_info(fn_ndev_val_t *val);
int fn_ndev_gps_info(fn_ndev_val_t *val);
int fn_ndev_set_light_ctrl_time(fn_ndev_val_t *val);
int fn_ndev_set_site_QR_code(fn_ndev_val_t *val);



/* 节点机签到 */
int fn_ndev_reg(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_reg:%s\n", NDEV_REG_W);

	int ret = LIB_GE_ERROR;
	ndev_reg_ack_t reg;
	ndev_status_t status;
	memset(&reg, 0, sizeof(ndev_reg_ack_t));
	memset(&status, 0, sizeof(ndev_status_t));

	//printf("-----------------register recv size:%d\n", val->d_size);
	ret = lib_b64_decode_hex((char *)&reg, (char *)val->data,val->d_size); //b64解码
	if(ret > 0)
	{
		fprintf(stderr, "server status: 0x%02x\n", reg.status);
		lib_printf("server time", reg.serv_time, 7);

		if(reg.status == 00) //认证成功
		{
			lib_set_systime_bcd(reg.serv_time);  //设置节点机时间
			utils_set_systime_bcd(reg.serv_time);

			ndev_info_stat_get(&status);
			status.time = 1; //正常 
			ndev_info_stat_put(&status);
			
			ndev_set_conn_stat(NDEV_NETSTAT_REGISTERED);
			ndev_register_notify_put(NDEV_NOTIFY_REGISTERED);
			ndev_btheart_notify_put(NDEV_NOTIFY_BTHEART);
		}
	}
		
	return B_DEV_EOK;
}

/* 节点机状态 */
int fn_ndev_stat(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_stat:%s\n", NDEV_STAT_W);

	int ret = -1;
	char s_b64[128] = {0};
	unsigned short nstat;
	
	if(val->sel == FN_SEL_TRUE) //获取节点机时间
	{	
		n_var_data_t *var = (n_var_data_t *)val->var_ptr;
		if(var != NULL)
		{
			ndev_info_stat_NOR_get(&nstat);
			ret = lib_b64_encode_hex(s_b64,(char *)&nstat, sizeof(ndev_status_t));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_STAT_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;
}

/* 节点机时间 */
int fn_ndev_time(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_time:%s\n", NDEV_N_TIME_W);

	char s_time[13] = {0};

	if(val->sel == FN_SEL_TRUE) //获取节点机时间
	{	
		n_var_data_t *var = (n_var_data_t *)val->var_ptr;
		if(var != NULL)
		{
			utils_get_sys_time_s(s_time);
			
			n_var_data_set(var, NDEV_N_TIME_W, 2);
			n_var_data_set(var, s_time, 12);
			n_var_data_char(var, '&');
		}
	}
	else if(val->sel == FN_SEL_FLASE) //设置节点机时间
	{
		utils_set_sys_time_s(val->data);

		ndev_status_t nstatus;
		ndev_info_stat_get(&nstatus);
		nstatus.time = 1;   //时间同步状态
		ndev_info_stat_put(&nstatus);
	}
	
	return B_DEV_EOK;
}

/* 桩机总数量 */
int fn_ndev_s_total_num(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_s_total_num:%s\n", NDEV_S_TOTAL_NUM_W);
	
	return B_DEV_EOK;
}

/* 桩机在线数量 */
int fn_ndev_s_online_num(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_s_online_num:%s\n", NDEV_S_ONLINE_NUM_W);
	
	return B_DEV_EOK;
}

/* 总量黑名单版本 */
int fn_ndev_total_ver(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_total_ver:%s\n", NDEV_TOTAL_BLK_VER_W);
	
	return B_DEV_EOK;
}

/* 增量黑名单版本 */
int fn_ndev_inc_ver(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_inc_ver:%s\n", NDEV_INC_BLK_VER_W);
	
	return B_DEV_EOK;
}

/* 减量黑名单版本 */
int fn_ndev_dec_ver(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_dec_ver:%s\n", NDEV_DEC_BLK_VER_W);
	
	return B_DEV_EOK;
}

/* 临时黑名单版本 */
int fn_ndev_temp_ver(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_temp_ver:%s\n", NDEV_TEMP_BLK_VER_W);
	
	return B_DEV_EOK;
}

/* 桩机黑名单属性 */
int fn_ndev_s_blk_attr(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_s_blk_attr:%s\n", NDEV_S_BLK_ATTR_W);
	
	return B_DEV_EOK;
}

/* 桩机属性 */
int fn_ndev_s_attr(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_s_attr:%s\n", NDEV_S_ATTR_W);
	
	return B_DEV_EOK;
}

/* 羊城通读卡器状态 */
int fn_ndev_yct_stat(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_yct_stat:%s\n", NDEV_YCT_STAT_W);
	
	return B_DEV_EOK;	
}

/* 心跳回应 */
int fn_ndev_btheart_return(fn_ndev_val_t *val)
{
	//printf("-----------------btheart recv size:%d\n", val->d_size);
	
	FN_DBG("fn_ndev_btheart_return:%s\n", NDEV_BTHEART_RETURN_W);

	ndev_btheart_notify_put(NDEV_NOTIFY_BTHEART);
	
	return B_DEV_EOK;		
}

/* 终端名称 */
int fn_ndev_site_name_ctrl(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_site_name:%s\n", NDEV_SITE_NAME_W);

	int ret = -1;
	char s_b64[128] = {0};
	ndev_info_t ninfo;
	device_config_t config;
	char f_ack = 0;
	memset(&ninfo, 0, sizeof(ndev_info_t));
	
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			//printf("----------------------site_name,s_b64:%s\n", s_b64);
			SYS_LOG_INFO("%s: terminal name: %s", __FUNCTION__, s_b64); //终端名称写入日志 2017-01-06
			
			ndev_info_get(&ninfo);
			strcpy(ninfo.terminal_name, s_b64);
			ndev_info_put(&ninfo);

			device_config_get(&config);
			strcpy(config.nconf.site_name, s_b64);  //把站点名称保存在配置文件里面
			device_config_put(&config);

			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 0;
		}

		memset(s_b64, 0, sizeof(s_b64));
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_SITE_NAME_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}
	else if(val->sel == FN_SEL_TRUE)  //获取节点机终端名称
	{
		if(var != NULL)
		{
			ndev_info_get(&ninfo);
			ret = lib_b64_encode(s_b64, ninfo.terminal_name);
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_SITE_NAME_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}
	
	return B_DEV_EOK;	
}

int fn_ndev_univ_update_ctrl(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_univ_update:%s\n", NDEV_UNIV_UPDATE_W);

	if(val == NULL)
		return B_DEV_ERROR;

	int ret = -1;
	univ_file_hd_t hd;

	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_FLASE)
	{	
		memset(&hd, 0, sizeof(univ_file_hd_t));

		ret = lib_b64_decode_hex((char *)&hd, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			SYS_LOG_INFO("%s: file_seq:%d, total: %d, div_seq:%d, ufc:0x%02x", __FUNCTION__, ntohs(hd.file_seq), ntohs(hd.total), ntohs(hd.div_seq), hd.ufc);
			
			async_mutex_queue_data_t *async_mqd = (async_mutex_queue_data_t *)lib_zmalloc(sizeof(async_mutex_queue_data_t));
			if(async_mqd == NULL)
				return B_DEV_ERROR;
			
			univ_file_hd_t *t_hd = (univ_file_hd_t *)lib_zmalloc(sizeof(univ_file_hd_t));
			if(t_hd == NULL)
				return B_DEV_ERROR;

			lib_async_mutex_queue_t *async_mutex_queue = (lib_async_mutex_queue_t *)ndev_get_univ_async_mutex_queue();
			if(async_mutex_queue == NULL)
				return B_DEV_ERROR;

			memcpy(t_hd, &hd, sizeof(univ_file_hd_t)); //拷贝泛文件头部
			
			async_mqd->qid = NDEV_ASYNC_MUTEX_QDATA_UNIV;  //泛文件处理
			async_mqd->sn = val->sn;
			async_mqd->qptr = t_hd;
			lib_async_mutex_queue_put(async_mutex_queue, async_mqd);
		
			val->ack = 1;

			if(var != NULL) //确认报文
			{
				n_var_data_set(var, NDEV_UNIV_UPDATE_W, 2);
				n_var_data_b64_set(var, &hd, sizeof(univ_file_hd_t));
				n_var_data_char(var, '&');
			}
		}
		else
		{
			val->ack = 0;
		}
	}

	return B_DEV_EOK;	
}


/* 心跳时间 */
int fn_ndev_btheart_time_set(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_btheart_time_set:%s\n", NDEV_BTHEART_TIME_SET_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned char btheart_time = 0;
	device_config_t dev_conf;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 0;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memset(&dev_conf, 9, sizeof(device_config_t));
			memcpy(&btheart_time, s_b64, sizeof(btheart_time));

			SYS_LOG_NOTICE("Control Command btheart time set: %d\n", btheart_time);
			
			device_config_get(&dev_conf);
			dev_conf.nconf.heart_time = btheart_time;
			device_config_put(&dev_conf);

			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));	
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_BTHEART_TIME_SET_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;
}

/* 立即开锁 */
int fn_ndev_s_lock_op(fn_ndev_val_t *val) 
{
	FN_DBG("fn_ndev_s_lock_op:%s\n", NDEV_S_LOCK_OP_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned char txbuf[512] = {0};
	unsigned char lock_op[2] = {0};
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&lock_op, s_b64, sizeof(lock_op));

			/*
			 * lock ID:锁桩序号, lock:锁车操作, 00h:锁车, 01h:开锁
			 */
			SYS_LOG_NOTICE("Control Command stake lock ID: %d, lock: %02x\n", lock_op[0], lock_op[1]);

			sae_control_t ctrl;
			memset(&ctrl, 0, sizeof(sae_control_t));

			if(lock_op[1] == 0x00) //锁车
			{
				ctrl.f_lock = 2;
				ctrl.m_lock = 3;
			}
			else if(lock_op[1] == 0x01) //开锁
			{
				ctrl.f_lock = 1; 
				ctrl.m_lock = 3;
			}

			ret = sae_protocol_control_for_lock(txbuf, lock_op[0], &ctrl);
			if(ret > 0)
			{
				lib_printf_v2("For Lock Ctrl:", &ctrl, sizeof(sae_control_t), 16);
				lib_printf_v2("For Lock TXbuf:", txbuf, ret, 16);
				
				stake_ctos_put(lock_op[0], txbuf, ret); //发送给锁桩
				g_sae_ctrl_sn = val->sn; //保存流水号
			}

			//lock_op[1] = 0xff;  //模拟
			//val->ack = 1;
		}
		
		#if 0
		else
		{
			lock_op[1] = 0x00; 
			val->ack = 1;
		}
		#endif
		
		#if 0
		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &lock_op, sizeof(lock_op));  //应答
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_S_LOCK_OP_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
		#endif
	}

	return B_DEV_EOK;
}

/* 负载服务器IP */
int fn_ndev_load_serv_ip_set(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_load_serv_ip_set:%s\n", NDEV_LOAD_SERV_IP_SET_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned char serv_ip[16] = {0};
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 0;
	device_config_t dev_conf;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&serv_ip, s_b64, 8);
			unsigned int ipaddr = 0;
			memcpy(&ipaddr, &s_b64[4], 4);
			
			SYS_LOG_NOTICE("Control Command serv ip: %s\n", lib_iaddr_to_saddr(ipaddr));

			memset(&dev_conf, 0, sizeof(device_config_t));
			device_config_get(&dev_conf);
			memcpy(&(dev_conf.nconf.load_server1_ip), &ipaddr, 4);
			device_config_put(&dev_conf);
	
			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_LOAD_SERV_IP_SET_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;
}

/* 负载服务器端口 */
int fn_ndev_load_serv_port_set(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_load_serv_port_set:%s\n", NDEV_LOAD_SERV_PORT_SET_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned short serv_port = 0;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 1;
	device_config_t dev_conf;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&serv_port, s_b64, 2);
			SYS_LOG_NOTICE("Control Command serv port: %d\n", ntohs(serv_port));

			memset(&dev_conf, 0, sizeof(device_config_t));
			device_config_get(&dev_conf);
			dev_conf.nconf.load_server1_port = ntohs(serv_port);
			device_config_put(&dev_conf);

			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_LOAD_SERV_PORT_SET_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;	
}

/* 备份负载服务器IP */
int fn_ndev_load_serv2_ip_set(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_load_serv2_ip_set:%s\n", NDEV_LOAD_SERV2_IP_SET_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned char serv_ip[16] = {0};
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 0;
	device_config_t dev_conf;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&serv_ip, s_b64, 8);
			unsigned int ipaddr = 0;
			memcpy(&ipaddr, &s_b64[4], 4);
			
			SYS_LOG_NOTICE("Control Command serv2 ip: %s\n", lib_iaddr_to_saddr(ipaddr));

			memset(&dev_conf, 0, sizeof(device_config_t));
			device_config_get(&dev_conf);
			memcpy(&(dev_conf.nconf.load_server2_ip), &ipaddr, 4);
			device_config_put(&dev_conf);
	
			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   //回应
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_LOAD_SERV2_IP_SET_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;
}

/* 备份负载服务器端口 */
int fn_ndev_load_serv2_port_set(fn_ndev_val_t *val)  //没有测试
{
	FN_DBG("fn_ndev_load_serv2_port_set:%s\n", NDEV_LOAD_SERV2_PORT_SET_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned short serv_port = 0;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 1;
	device_config_t dev_conf;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&serv_port, s_b64, 2);
			SYS_LOG_NOTICE("Control Command serv2 port: %d\n", ntohs(serv_port));

			memset(&dev_conf, 0, sizeof(device_config_t));
			device_config_get(&dev_conf);
			dev_conf.nconf.load_server2_port = ntohs(serv_port);
			device_config_put(&dev_conf);

			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_LOAD_SERV2_PORT_SET_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;	
}

/* 重启设备 */
int fn_ndev_device_reboot(fn_ndev_val_t *val)  
{
	FN_DBG("fn_ndev_device_reboot:%s\n", NDEV_DEVICE_REBOOT_W);

	int ret = B_DEV_ERROR;
	char s_b64[64] = {0};
	unsigned char reboot = 0xff;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 0;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&reboot, s_b64, 1);
			
			SYS_LOG_NOTICE("Control Command Reboot: %d\n", reboot);

			if(reboot == 0) //重启设备
			{
				lib_wdt_system_reboot();	
			}

			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_DEVICE_REBOOT_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}	

	return B_DEV_EOK;	
}

/* FTP设置 */
int fn_ndev_ftp_set(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_ftp_set:%s\n", NDEV_FTP_SET_W);

	int ret = B_DEV_ERROR;
	char s_b64[128] = {0};
	ndev_ftp_server_t ftp;
	device_config_t dev_conf;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 0;
	
	if(val->sel == FN_SEL_FLASE)
	{
		memset(&ftp, 0, sizeof(ndev_ftp_server_t));
		
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&ftp, s_b64, sizeof(ndev_ftp_server_t));

			SYS_LOG_NOTICE("Control Command ftp ip address: %s\n", lib_iaddr_to_saddr(ftp.ipaddr));
			SYS_LOG_NOTICE("Control Command ftp port: %d\n", ntohs(ftp.port));
			SYS_LOG_NOTICE("Control Command ftp username: %s\n", ftp.username);
			SYS_LOG_NOTICE("Control Command ftp passwd: %s\n", ftp.passwd);

			memset(&dev_conf, 0, sizeof(device_config_t));
			device_config_get(&dev_conf);
			dev_conf.nconf.ftp_ip = ftp.ipaddr;
			dev_conf.nconf.ftp_port = ftp.port;
			strcpy(dev_conf.nconf.ftp_username, ftp.username);
			strcpy(dev_conf.nconf.ftp_password, ftp.passwd);
			device_config_put(&dev_conf);
				
			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_FTP_SET_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}


	return B_DEV_EOK;	
}

/* 异常处理 */
int fn_ndev_except_handle(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_except_handle:%s\n", NDEV_EXCEPT_HANDLE_W);
	fprintf(stderr, "n_ndev_except_handle:%s\n", NDEV_EXCEPT_HANDLE_W);

	int ret = B_DEV_ERROR;
	char s_b64[128] = {0};
	exception_handle_ack_t ack;
	memset(&ack, 0, sizeof(exception_handle_ack_t));

	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&ack, s_b64, sizeof(exception_handle_ack_t));
			gui_set_exception_handle(&ack, 1);  //设置异常处理应答报文
			
			//lib_printf_v2("-----------fn_ndev_except_handle---------", (unsigned char *)&ack, sizeof(exception_handle_ack_t), 16);
		}


		val->ack = 0;
	}

	return B_DEV_EOK;
}

/* FTP固件下载 */
int fn_ndev_ftp_download(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_ftp_download:%s\n", NDEV_FTP_DNL_W);


	int ret = B_DEV_ERROR;
	char s_b64[128] = {0};

	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			async_mutex_queue_data_t *async_mqd = (async_mutex_queue_data_t *)lib_zmalloc(sizeof(async_mutex_queue_data_t));
			if(async_mqd == NULL)
				return B_DEV_ERROR;
			
			ndev_ftp_download_ctrl_req_t *ftp_dnl_req = (ndev_ftp_download_ctrl_req_t *)lib_zmalloc(sizeof(ndev_ftp_download_ctrl_req_t));
			if(ftp_dnl_req == NULL)
			{
				lib_zfree(async_mqd);
				return B_DEV_ERROR;
			}

			memcpy(ftp_dnl_req, s_b64, sizeof(ndev_ftp_download_ctrl_req_t));

			lib_printf_v2("FTP DNL", s_b64, ret, 16);
			fprintf(stderr, "upsn=%d\n", ntohs(ftp_dnl_req->upsn));
			fprintf(stderr, "supplier=%d\n", ftp_dnl_req->supplier);
			fprintf(stderr, "fw_name:%s\n", ftp_dnl_req->fw_name);

			lib_async_mutex_queue_t *async_mutex_queue = (lib_async_mutex_queue_t *)ndev_get_univ_async_mutex_queue();
			if(async_mutex_queue == NULL)
			{
				lib_zfree(async_mqd);
				lib_zfree(ftp_dnl_req);
				return B_DEV_ERROR;
			} 
			
			async_mqd->qid = NDEV_ASYNC_MUTEX_QDATA_FTP_DNL; 
			async_mqd->sn = val->sn;
			async_mqd->qptr = ftp_dnl_req;
			lib_async_mutex_queue_put(async_mutex_queue, async_mqd);
		

		}

		val->ack = 0;
	}

	return B_DEV_EOK;
}

/* 固件更新控制 */
int fn_ndev_update_ctrl(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_update_ctrl:%s\n", NDEV_UPDATE_CTRL_W);

	int ret = B_DEV_ERROR;
	char s_b64[128] = {0};
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			lib_printf_v2("FW UP", s_b64, ret, 16);

			async_mutex_queue_data_t *async_mqd = (async_mutex_queue_data_t *)lib_zmalloc(sizeof(async_mutex_queue_data_t)); //异步数据
			if(async_mqd == NULL)
				return B_DEV_ERROR;
			
			ndev_fw_update_ctrl_req_t *fup_req = (ndev_fw_update_ctrl_req_t *)lib_zmalloc(sizeof(ndev_fw_update_ctrl_req_t));
			if(fup_req == NULL)
			{
				lib_zfree(async_mqd);
				return B_DEV_ERROR;
			}

			memcpy(fup_req, s_b64, sizeof(ndev_fw_update_ctrl_req_t));

			fprintf(stderr, "upsn=%d\n", ntohs(fup_req->upsn));
			fprintf(stderr, "supplier=%d\n", fup_req->supplier);
			fprintf(stderr, "upop=%d\n", fup_req->upop);

			lib_async_mutex_queue_t *async_mutex_queue = (lib_async_mutex_queue_t *)ndev_get_univ_async_mutex_queue(); 
			if(async_mutex_queue == NULL)
			{
				lib_zfree(async_mqd);
				lib_zfree(fup_req);
				return B_DEV_ERROR;
			}

			async_mqd->qid = NDEV_ASYNC_MUTEX_QDATA_FW_CTRL;  
			async_mqd->sn = val->sn;
			async_mqd->qptr = fup_req;
			lib_async_mutex_queue_put(async_mutex_queue, async_mqd);
			
			
		}

		val->ack = 0;
	}

	
	return B_DEV_EOK;
}

#if 0
/* 租还车记录信息 */
int fn_ndev_rent_info(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_rent_info:%s\n", NDEV_RENT_INFO_W);

	fprintf(stderr, "fn_ndev_rent_info:%s\n", NDEV_RENT_INFO_W);

	int ret = B_DEV_ERROR;
	char s_b64[B_DEV_RXBUF_SZ] = {0};
	ndev_rent_info_qry_ack_t ack;
	memset(&ack, 0, sizeof(ndev_rent_info_qry_ack_t));

	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memcpy(&ack, s_b64, sizeof(ndev_rent_info_qry_ack_t));
			gui_set_rent_info_qry(&ack, 1); 
			
			lib_printf_v2("-----------gui_set_rent_info_qry---------", (unsigned char *)&ack, sizeof(ndev_rent_info_qry_ack_t), 16);
		}

		val->ack = 0;
	}

	return B_DEV_EOK;	
}
#endif

/* 获取桩机属性 */
int fn_ndev_get_s_attr(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_get_s_attr:%s\n", NDEV_GET_S_ATTR_W);
	fprintf(stderr, "fn_ndev_get_s_attr:%s\n", NDEV_GET_S_ATTR_W);

	int ret = B_DEV_ERROR;
	char s_b64[128] = {0};
	unsigned char s_attr[128] = {0};
	unsigned char lock_id = 0;
	sae_info_t *sinfo = NULL;
	sae_attribute_t sattr;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			lock_id = s_b64[0];
			SYS_LOG_NOTICE("Get SAE attr lock ID: %d\n", lock_id);
			val->ack = 1;
		}
		else
		{
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			if(lock_id > CONFS_STAKE_NUM_MAX)
			{
				s_attr[0] = 0x00; //离线
				goto Done;
			}
			
			sinfo = sae_info_get_by_id(lock_id);  
			if(sinfo == NULL)
			{
				s_attr[0] = 0x00; //离线
				goto Done;
			}

			if(sinfo->healthy_flag == 0)
			{
				s_attr[0] = 0x00; //离线
				goto Done;
			}

			memset(&sattr, 0xfb, sizeof(sae_attribute_t));
			
			sattr.id = lock_id;		//桩机编号
			sattr.sn = atoi(sinfo->s_sn);		//桩机最新流水号
			utils_string_to_bcd(sinfo->s_phy_sn, sattr.physn);		//桩机物理编号
			utils_string_to_bcd(sinfo->s_psam, sattr.psam);			//PSAM卡编号
			utils_string_to_bcd(sinfo->s_ver, sattr.version);			//桩机版本号
			utils_string_to_bcd(sinfo->s_time,  sattr.time); //桩机时间
			utils_time_to_bcd_yymmddhhMMss(&(sinfo->s_reg_time), sattr.register_time);  //桩机注册时间
			sattr.lock_id = atoi(sinfo->s_lock_id);		//锁控编号
			utils_string_to_bcd(sinfo->s_lock_ver, sattr.lock_version);		 //锁控版本号
			utils_string_to_bcd(sinfo->s_total_bl_ver, sattr.total_blk_ver);	//总量黑名单版本号
			utils_string_to_bcd(sinfo->s_inc_bl_ver, sattr.inc_blk_ver);		//增量黑名单版本号
			utils_string_to_bcd(sinfo->s_dec_bl_ver, sattr.dec_blk_ver);	//减量黑名单版本号
			utils_string_to_bcd(sinfo->s_temp_bl_ver, sattr.temp_blk_ver);	//临时黑名单版本号
			memcpy(&(sattr.stat), &(sinfo->status), 2);
		
			memcpy(&s_attr[1], &sattr, sizeof(sae_attribute_t));
			s_attr[0] = 0x01; //在线
Done:			
			ret = lib_b64_encode_hex(s_b64, &s_attr, 51);  //应答
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_GET_S_ATTR_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}
	
	return B_DEV_EOK;	
}

/* 租还车记录信息 */
int fn_ndev_rent_info_explain(void *ptr, const unsigned int len)
{
	//lib_printf_v2("RENT INFO EXPLAIN",  ptr, len, 16);

	fprintf(stderr, "fn_ndev_rent_info_explain:%d\n", len);

	int ret = B_DEV_ERROR;
	char s_b64[B_DEV_RXBUF_SZ] = {0};
	ndev_rent_info_qry_ack_t ack;
	memset(&ack, 0, sizeof(ndev_rent_info_qry_ack_t));
	
	ret = lib_b64_decode_hex(s_b64, (char *)ptr, len); //b64解码
	if(ret > 0)
	{
		memcpy(&ack, s_b64, sizeof(ndev_rent_info_qry_ack_t));
		gui_set_rent_info_qry(&ack, 1); //拷贝租还车信息到全局变量
			
		//lib_printf_v2("-----------gui_set_rent_info_qry---------", (unsigned char *)&ack, sizeof(ndev_rent_info_qry_ack_t), 16);
		//fprintf(stderr, "%s\n", &s_b64[11]);
	}

	
	return B_DEV_EOK;	
}


int fn_ndev_sae_all_phy_info(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_sae_all_phy_info:%s\n", NDEV_GET_ALL_SAE_PHY_INFO_W);
	fprintf(stderr, "fn_ndev_sae_all_phy_info:%s\n", NDEV_GET_ALL_SAE_PHY_INFO_W);

	int quantity = 0;
	int n, id, j = 1, item = 0;
	int ret = B_DEV_ERROR;
	unsigned char phy_info[512] = {0};
	char s_b64[1024] = {0};
	unsigned char phy_id_hex[8] = {0};
	sae_info_t *sinfo = NULL;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		if(var != NULL)
		{
			val->ack = 1;  //应答
			
			quantity = sae_info_get_quantity();
			if(quantity == 0)
			{
				item = 0; //项数
			 	n = 1;  //数据长度
				goto Done;
			}

			if(quantity >=  CONFS_STAKE_NUM_MAX)
			{
				item = 0;
				n = 1;
				goto Done;
			}

			n = 1;
			for(id = 1; id <= quantity; id++)
			{		
				sinfo = sae_info_get_by_id(id);  
				if(sinfo != NULL)
				{
					if(sinfo->healthy_flag == 1) //锁桩存在
					{
						lib_str_to_hex(sinfo->s_phy_sn, 8, phy_id_hex);

						//fprintf(stderr, "-------------------------ID:%d,  ", id);
						//lib_printf(":", phy_id_hex, 4);

						phy_info[j] = id; //id
						j += 1;
						memcpy(&(phy_info[j]), phy_id_hex, 4);
						j += 4;

						item++; //项数
						n += 5; //数据长度
					}
				}
			}	
		}

		

Done:
		//fprintf(stderr, "******************* item=%d, n=%d\n", item, n);
		
		phy_info[0] = item;
		ret = lib_b64_encode_hex(s_b64, &phy_info, n);  //应答
		if(ret > 0)
		{
			//fprintf(stderr, "%s\n", s_b64);
			
			n_var_data_set(var, NDEV_GET_ALL_SAE_PHY_INFO_W, 2);
			n_var_data_set(var, s_b64, ret);
			n_var_data_char(var, '&');
		}
	}

	return B_DEV_EOK;
}

int fn_ndev_gps_info(fn_ndev_val_t *val)
{

	FN_DBG("fn_ndev_gps_info:%s\n", NDEV_GET_GPS_INFO_W);
	fprintf(stderr, "fn_ndev_gps_info:%s\n", NDEV_GET_GPS_INFO_W);
	
	int ret = B_DEV_ERROR;
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	gps_all_info_t gps;
	struct gui_gps_info gps_info; 
	
	memset(&gps, 0, sizeof(gps_all_info_t));
	memset(&gps_info, 0, sizeof(struct gui_gps_info));
	
	if(val->sel == FN_SEL_FLASE)
	{	
		if(var != NULL)
		{
			val->ack = 1;  //应答
			
			gps.vaild = '0';
			
			if(g_gps_rxbuf2.vaild != 1) //数据无效
			{
				goto Done;
			}
  
			memset(&gps_info, 0, sizeof(struct gui_gps_info));
			gps_get_info(g_gps_rxbuf2.string, &(gps_info.gga), &(gps_info.tm), &(gps_info.attr));

			if(gps_info.attr.gga == 1) //方位
			{		
				sprintf(gps.latitude, "%4.5f", gps_info.gga.latitude);
				gps.ns_indicator = gps_info.gga.ns_indicator;
		
				sprintf(gps.longitude, "%4.5f", gps_info.gga.longitude);
				gps.ew_indicator = gps_info.gga.ew_indicator;

				switch(gps_info.gga.status) //GPS 状态, 0=未定位，1=非差分定位，2=差分定位	
				{
					case 0:
						gps.status = '0';
						break;

					case 1:
						gps.status = '1';
						break;

					case 2:
						gps.status = '2';
						break;

					default:
						gps.status = '0';
				}
				
				sprintf(gps.satellite, "%02d", gps_info.gga.satellite); 	//正在使用的用于定位的卫星数量（00~12） 
				
				gps.vaild = '1';
			}

			if(gps_info.attr.tm == 1) //时间
			{	
				utils_tm_to_bcd_time(&(gps_info.tm), &(gps.times));  //时间

				gps.vaild = '1';
			}
		}
		
		
	}
	
Done:
	n_var_data_set(var, NDEV_GET_GPS_INFO_W, 2);
	n_var_data_set(var, &gps, sizeof(gps_all_info_t));
	n_var_data_char(var, '&');
	
	return B_DEV_EOK;
}

int fn_ndev_set_light_ctrl_time(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_set_light_ctrl_time:%s\n", NDEV_SET_LIGHT_CTRL_TIME_W);

	int ret = B_DEV_ERROR;
	light_ctrl_time_t ctrl_time;
	char s_b64[64] = {0};
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	char f_ack = 1;
	device_config_t dev_conf;
	
	if(val->sel == FN_SEL_FLASE)
	{
		ret = lib_b64_decode_hex(s_b64, (char *)val->data,val->d_size); //b64解码
		if(ret > 0)
		{
			memset(&ctrl_time, 0, sizeof(light_ctrl_time_t));
			memcpy(&ctrl_time, s_b64, 4);
			
			memset(&dev_conf, 0, sizeof(device_config_t));
			device_config_get(&dev_conf);
			memcpy(&dev_conf.nconf.light_ctrl_time_enable, &ctrl_time.enable, 2);
			memcpy(&dev_conf.nconf.light_ctrl_time_disable, &ctrl_time.disable, 2);
			device_config_put(&dev_conf);

			f_ack = 1;
			val->ack = 1;
		}
		else
		{
			f_ack = 0;
			val->ack = 1;
		}

		memset(s_b64, 0, sizeof(s_b64));   
		if(var != NULL)
		{
			ret = lib_b64_encode_hex(s_b64, &f_ack, sizeof(f_ack));
			if(ret > 0)
			{
				n_var_data_set(var, NDEV_SET_LIGHT_CTRL_TIME_W, 2);
				n_var_data_set(var, s_b64, ret);
				n_var_data_char(var, '&');
			}
		}
	}

	return B_DEV_EOK;	
}

/* 站点二维码 */
int fn_ndev_set_site_QR_code(fn_ndev_val_t *val)
{
	FN_DBG("fn_ndev_set_site_QR_code:%s\n", NDEV_SITE_QR_CODE_W);

	int ret = -1;
	device_config_t config;
	unsigned char f_ack = '0';
	
	n_var_data_t *var = (n_var_data_t *)val->var_ptr;
	
	if(val->sel == FN_SEL_FLASE)
	{
		if(strlen(val->data) < sizeof(config.nconf.site_QR_code))  //下发的字符串要小于存储空间
		{
			device_config_get(&config);
			//strcpy(config.nconf.site_QR_code, val->data);  
			memcpy(&config.nconf.site_QR_code, &val->data, sizeof(config.nconf.site_QR_code));

			//fprintf(stderr, "===================QR_code Recv:%s\n", val->data);
			
		#if 0 //CONFS_USING_TEST_BY_ZJC    	
			char QR_code[32] = {0};
			memset(QR_code, 0, sizeof(QR_code));
			sprintf(QR_code, "QR_code:%s\n", val->data);
		
			FILE *logfd;
			logfd = fopen("/opt/logpath/QR_code.txt", "a+");
			fwrite(&QR_code, strlen(QR_code), 1, logfd);
			fclose(logfd); //must!
		#endif
		
			//lib_printf("QR_code:", &config.nconf.site_QR_code, 32);
			
			device_config_put(&config);

			f_ack = '1';
			val->ack = 1;
		}
		else
		{
			f_ack = '0';
			val->ack = 0;
		}

		if(var != NULL)
		{
			n_var_data_set(var, NDEV_SITE_QR_CODE_W, 2);
			n_var_data_char(var, f_ack);
			n_var_data_char(var, '&');
		}
	}
	
	return B_DEV_EOK;	
}


/* 附近网点信息查询 add by zjc at 2016-11-03 */
int fn_ndev_nearby_site_info_explain(void *ptr, const unsigned int len)
{
	fprintf(stderr, "--------------fn_ndev_nearby_site_info_explain:%d\n", len);

	int ret = B_DEV_ERROR;
	char s_b64[B_DEV_RXBUF_SZ] = {0};
	ndev_nearby_site_info_qry_ack_t ack;
	memset(&ack, 0, sizeof(ndev_nearby_site_info_qry_ack_t));
	
	ret = lib_b64_decode_hex(s_b64, (char *)ptr, len); //b64解码
	if(ret > 0)
	{
		memcpy(&ack, s_b64, sizeof(ndev_nearby_site_info_qry_ack_t));
		gui_set_nearby_siteinfo_qry(&ack, 1); //拷贝附近网点信息到全局变量
	}

	
	return B_DEV_EOK;	
}
/* end of 附近网点信息查询 */

