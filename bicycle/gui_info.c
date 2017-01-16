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
#include "ndev_info.h"
#include "sae_info.h"
#include "node_device.h"
#include "universal_file.h"
#include "lib_wireless.h"
#include "ndev_protocol.h"
#include "gui_info.h"
#include "unity_file_handle.h"

extern lib_wl_t *g_bicycle_wl;
extern long g_last_time_power_on;
extern lib_unity_file_t *g_unity_file_db;


static struct gui_except_handle_ack g_ehndl_ack;  //保存异常处理应答包
static unsigned int g_ehndl_ack_f = 0;  //异常处理确认标志
static struct gui_rent_info_qry_ack g_rentinfo_qry_ack; //保存租还车记录应答包
static unsigned int g_rentinfo_qry_ack_f = 0;

static struct gui_nearby_site_info_qry_ack g_nearby_site_info_qry_ack; //保存附近网点信息应答包
static unsigned int g_nearby_siteinfo_qry_ack_f = 0;



void gui_get_ndev_stat(ndev_status_t *stat)
{
	ndev_info_t info;
	
	ndev_info_get(&info);
	memcpy(stat, &(info.stat), sizeof(struct ndev_status));
}

void gui_get_sae_stat(sae_status_t *stat, const unsigned char id)
{
	sae_info_t *info = NULL;

	info =  sae_info_get_by_id(id);
	if(info != NULL)
	{
		memcpy(stat, &(info->status), sizeof(sae_status_t));	
	}
}

void gui_get_ndev_version(char ver[32])
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	strcpy(ver, conf.nconf.appl_ver);
}

void gui_get_ndev_device_config(struct ndev_config *config)
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	memcpy(config, &(conf.nconf), sizeof(struct ndev_config));
}

void gui_get_stake_device_config(struct stake_config *config)
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	memcpy(config, &(conf.sconf), sizeof(struct stake_config));	
}




int gui_get_ndev_page_config(struct gui_ndev_page_config *config)
{
	if(config == LIB_GE_NULL)
		return LIB_GE_ERROR;

	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t)); //add by zjc at 2016-09-26

	device_config_get(&conf);

	/* 本地IP */
	strcpy(config->ip_conf.ipaddr, lib_iaddr_to_saddr(conf.nconf.local_ip));
	strcpy(config->ip_conf.subnet_mask, lib_iaddr_to_saddr(conf.nconf.local_submask));
	strcpy(config->ip_conf.default_gateway, lib_iaddr_to_saddr(conf.nconf.local_gateway));
	
	/* WIFI */
	strcpy(config->wifi_ip_conf.ipaddr, lib_inet_ntoa(conf.nconf.wifi_ip));
	strcpy(config->wifi_ip_conf.subnet_mask, lib_inet_ntoa(conf.nconf.wifi_submask));
	strcpy(config->wifi_ip_conf.default_gateway, lib_inet_ntoa(conf.nconf.wifi_gateway));
	
	/* 负载均衡服务器 */
	strcpy(config->center_ip_conf.load1_ipaddr, lib_iaddr_to_saddr(conf.nconf.load_server1_ip));
	strcpy(config->center_ip_conf.load2_ipaddr, lib_iaddr_to_saddr(conf.nconf.load_server2_ip));
	config->center_ip_conf.load1_port =  conf.nconf.load_server1_port;
	config->center_ip_conf.load2_port = conf.nconf.load_server2_port;

	/* ftp */
	strcpy(config->ftp_conf.ipaddr, lib_iaddr_to_saddr(conf.nconf.ftp_ip));
	strcpy(config->ftp_conf.user_name, conf.nconf.ftp_username);
	strcpy(config->ftp_conf.user_password, conf.nconf.ftp_password);
	config->ftp_conf.port = conf.nconf.ftp_port;

	/* 节点机参数配置 */
	config->n_param_conf.load_timeout = conf.nconf.load_timeout;
	config->n_param_conf.heart_time = conf.nconf.heart_time;
	config->n_param_conf.emerg_heart_time = conf.nconf.emerg_heart_time;
	config->n_param_conf.timer_gate_value = conf.nconf.timer_gate_value;
	config->n_param_conf.term_id = conf.nconf.term_id;

	/* 锁桩参数配置 */
	config->s_param_conf.can_baud_rate = conf.sconf.can_baud_rate;
	config->s_param_conf.heart_time = conf.sconf.heart_time;
	config->s_param_conf.quantity = conf.sconf.quantity;

	/* 网络接入方式 */
	config->access_conf.using_wireless = conf.nconf.using_wireless;

	/* 岭南通配置 */
	strcpy(config->lnt_conf.ipaddr, lib_iaddr_to_saddr(conf.nconf.lnt_ipaddr));
	config->lnt_conf.port = conf.nconf.lnt_port;

	return B_DEV_EOK;
}

int gui_get_ndev_access_pattern_config(struct gui_ndev_access_pattern_config *config)
{
	if(config == LIB_GE_NULL)
		return LIB_GE_ERROR;

	device_config_t conf;

	device_config_get(&conf);	

	/* 网络接入方式 */
	config->using_wireless = conf.nconf.using_wireless;

	return B_DEV_EOK;
}

int gui_set_ndev_ftp_config(struct gui_ndev_ftp_config *config)
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);

	conf.nconf.ftp_ip = lib_inet_addr(&(config->ipaddr));
	conf.nconf.ftp_port = config->port;
	strcpy(conf.nconf.ftp_username, config->user_name);
	strcpy(conf.nconf.ftp_password, config->user_password);

	device_config_put(&conf);

	return B_DEV_EOK;
}

int gui_set_ndev_center_config(struct gui_ndev_center_ip_config *config) //自行车中心配置
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);

	conf.nconf.load_server1_ip = lib_inet_addr(&(config->load1_ipaddr));
	conf.nconf.load_server1_port = config->load1_port;
	conf.nconf.load_server2_ip = lib_inet_addr(&(config->load2_ipaddr));
	conf.nconf.load_server2_port = config->load2_port;
	
	device_config_put(&conf);

	return B_DEV_EOK;	
}

int gui_set_ndev_wifi_config(struct gui_ndev_wifi_ip_config *config)	//WIFI配置
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);

	conf.nconf.wifi_ip = lib_inet_addr(&(config->ipaddr));
	conf.nconf.wifi_submask = lib_inet_addr(config->subnet_mask);
	conf.nconf.wifi_gateway = lib_inet_addr(config->default_gateway);
	
	device_config_put(&conf);

	#if 0
	lib_set_ipaddr("eth0", config->ipaddr);
	lib_set_netmask("eth0", config->subnet_mask);
	lib_set_gateway("eth0", config->default_gateway);
	#endif
	
	return B_DEV_EOK;			
}

int gui_set_ndev_local_config(struct gui_ndev_local_ip_config *config) //本地IP配置
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);

	conf.nconf.local_ip = lib_inet_addr(&(config->ipaddr));
	conf.nconf.local_submask = lib_inet_addr(config->subnet_mask);
	conf.nconf.local_gateway = lib_inet_addr(config->default_gateway);
	
	device_config_put(&conf);

	#if 0
	lib_set_ipaddr("eth1", config->ipaddr);
	lib_set_netmask("eth1", config->subnet_mask);
	lib_set_gateway("eth1", config->default_gateway);
	#endif
	
	return B_DEV_EOK;		
}

int gui_set_ndev_parameter_config(struct gui_ndev_parameter_config *config)  //节点机参数设置
{
	device_config_t conf;
	ndev_info_t ninfo;
	memset(&conf, 0, sizeof(device_config_t));
	memset(&ninfo, 0, sizeof(ndev_info_t));
	
	device_config_get(&conf);
	ndev_info_get(&ninfo);
		
	conf.nconf.load_timeout = config->load_timeout;
	conf.nconf.heart_time = config->heart_time;
	conf.nconf.emerg_heart_time = config->emerg_heart_time;
	conf.nconf.timer_gate_value = config->timer_gate_value;
	conf.nconf.term_id = config->term_id;
	ninfo.terminal_no = config->term_id;
	
	device_config_put(&conf);
	ndev_info_put(&ninfo);

	
	return B_DEV_EOK;
}

int gui_set_stake_parameter_config(struct gui_stake_parameter_config *config) //锁桩参数配置
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);

	conf.sconf.can_baud_rate = config->can_baud_rate;
	conf.sconf.heart_time = config->heart_time;
	conf.sconf.quantity = config->quantity;

	sae_info_set_quantity(conf.sconf.quantity);  //更新锁桩数量

	device_config_put(&conf);
	
	return B_DEV_EOK;		
}

int gui_set_ndev_access_pattern_config(struct gui_ndev_access_pattern_config *config)  //网络接入方式配置
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	
	conf.nconf.using_wireless = config->using_wireless;
	
	device_config_put(&conf);

	if(config->using_wireless == 1) //无线
		lib_wl_set_model(g_bicycle_wl, WL_NETWORK_MODEL_WIRELESS, 300);
	else if(config->using_wireless == 2)  //有线
		lib_wl_set_model(g_bicycle_wl, WL_NETWORK_MODEL_WIRED, 300);
		
	return B_DEV_EOK;	
}

int gui_set_lnt_config(struct gui_lnt_page_config *config)
{
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);

	conf.nconf.lnt_ipaddr= lib_inet_addr(&(config->ipaddr));
	conf.nconf.lnt_port = config->port;
	
	device_config_put(&conf);

	return B_DEV_EOK;
}

/* 异常处理 */
int gui_get_exception_handle_req(struct gui_except_handle_req *req)
{
	if(req == NULL)
		return B_DEV_ERROR;

	int ret = B_DEV_ERROR;
	unsigned char txbuf[1024] = {0};
	unsigned char esc_buf[1024] = {0};
	int esc_len = 0;

	memset(&g_ehndl_ack, 0, sizeof(gui_except_handle_ack_t));  //清空原数据

	g_ehndl_ack_f = 0;
	
	ret = ndev_protocol_exception_handle_req(txbuf, req, sizeof(struct gui_except_handle_req));
	if(ret > 0)
	{
		esc_len = uplink_escape(esc_buf, txbuf, ret); //上行转义
		ret = ndev_tcp_send_to_server(esc_buf, esc_len, 1000);
	}
	
	return ret;
}

int gui_set_exception_handle(struct gui_except_handle_ack *ack, const unsigned int flag)
{
	if(ack == NULL)
		return B_DEV_ERROR;

	g_ehndl_ack_f = flag;
	memset(&g_ehndl_ack, 0, sizeof(gui_except_handle_ack_t));
	memcpy(&g_ehndl_ack, ack, sizeof(gui_except_handle_ack_t));
		
	return B_DEV_EOK;
}

int gui_get_exception_handle_ack(struct gui_except_handle_ack *ack)
{
	if(ack == NULL)
		return B_DEV_ERROR;

	if(g_ehndl_ack_f == 1)
	{
		memcpy(ack, &g_ehndl_ack, sizeof(gui_except_handle_ack_t));

		memset(&g_ehndl_ack, 0, sizeof(gui_except_handle_ack_t));  //清空原数据
		
		g_ehndl_ack_f = 0;
		return B_DEV_EOK;
	}
	
	return B_DEV_ERROR;
}

int gui_get_sae_comparison_status(struct gui_sae_comparison_status comp[65])
{
	if(comp == NULL)
		return B_DEV_ERROR;

	int i;
	unsigned char quantity = 0;
	unity_file_config_t uconfig;
	device_config_t dev_conf;
	sae_info_t *sinfo = NULL;

	memset(&uconfig, 0, sizeof(unity_file_config_t));
	memset(&dev_conf, 0, sizeof(device_config_t));

	device_config_get(&dev_conf);
	lib_unity_file_config_select_data(g_unity_file_db, &uconfig);

	quantity = dev_conf.sconf.quantity;

	for(i = 1; i <= quantity; i++ )
	{
		sinfo = sae_info_get_by_id(i);
		if(sinfo != NULL)
		{
			if(strncmp((char *)uconfig.stake_fw_last_ver,  "00000000", 8) == 0)
				comp[i].fw = 0;
			{
				if(strncmp((char *)uconfig.stake_fw_last_ver, (char *)sinfo->s_ver, 8) == 0)   //锁桩版本
					comp[i].fw = 1;
				else
					comp[i].fw = 0;
			}

			if(strncmp((char *)uconfig.total_bl_last_ver,  "00000000", 8) == 0)  //版本为空
				comp[i].bl_total = 0;
			else
			{
				if(strncmp((char *)uconfig.total_bl_last_ver, (char *)sinfo->s_total_bl_ver, 8) == 0)  //总量黑名单
				{
					comp[i].bl_total = 1;
				}
				else
					comp[i].bl_total = 0;
			}

			if(strncmp((char *)uconfig.inc_bl_last_ver,  "00000000", 8) == 0)
				comp[i].bl_inc = 0;
			else
			{
				if(strncmp((char *)uconfig.inc_bl_last_ver, (char *)sinfo->s_inc_bl_ver, 8) == 0) //增量黑名单
					comp[i].bl_inc = 1;
				else
					comp[i].bl_inc = 0;
			}

			if(strncmp((char *)uconfig.dec_bl_last_ver,  "00000000", 8) == 0)
				comp[i].bl_dec = 0;
			else
			{
				if(strncmp((char *)uconfig.dec_bl_last_ver, (char *)sinfo->s_dec_bl_ver, 8) == 0)  //减量黑名单
					comp[i].bl_dec = 1;
				else
					comp[i].bl_dec = 0;
			}

			if(strncmp((char *)uconfig.temp_bl_last_ver,  "00000000", 8) == 0)
				comp[i].bl_temp = 0;
			else
			{
				if(strncmp((char *)uconfig.temp_bl_last_ver, (char *)sinfo->s_temp_bl_ver, 8) == 0) //临时黑名单
					comp[i].bl_temp = 1;
				else
					comp[i].bl_temp = 0;	
			}

			if(strncmp((char *)uconfig.stake_para_last_ver,  "00000000", 8) == 0)
				comp[i].stake_para = 0;
			else
			{
				if(strncmp((char *)uconfig.stake_para_last_ver, (char *)sinfo->s_stake_para, 8) == 0) //锁桩参数配置
					comp[i].stake_para = 1;
				else
					comp[i].stake_para = 0;	
			}
		}
	}
	

	return B_DEV_EOK;
}


int gui_get_access_state(struct gui_access_state *state)
{
	if(access == NULL)
		return LIB_GE_ERROR;

	device_config_t conf;

	memset(&conf, 0, sizeof(device_config_t));
	
	device_config_get(&conf);

	state->access = conf.nconf.using_wireless;
	state->connect_stat = ndev_get_conn_stat();

	return LIB_GE_EOK;
}


/*
 * 基本信息
 */
int gui_get_basic_info_page_config(struct gui_basic_info_page_config *config)
{
	if(config == LIB_GE_NULL)
		return LIB_GE_ERROR;

	int ret = LIB_UF_ERROR;
	device_config_t conf;
	ndev_info_t ninfo;
	
	memset(&conf, 0, sizeof(device_config_t));
	memset(&ninfo, 0, sizeof(ndev_info_t));

	device_config_get(&conf);
	ndev_info_get(&ninfo);
	
	SYS_LOG_INFO("%s: terminal name: %s", __FUNCTION__, ninfo.terminal_name); //终端名称写入日志 2017-01-06

	unity_file_config_t uconfig;
	memset(&uconfig, 0, sizeof(unity_file_config_t));

	ret = lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	if(ret == LIB_UF_OK)
	{
		strncpy(config->stake_firmware_ver, uconfig.stake_fw_last_ver, 8); 	//桩版本
		strncpy(config->total_blacklist_ver, uconfig.total_bl_last_ver, 8);
		strncpy(config->inc_blacklist_ver, uconfig.inc_bl_last_ver, 8);
		strncpy(config->dec_blacklist_ver, uconfig.dec_bl_last_ver, 8);
		strncpy(config->temp_blacklist_ver, uconfig.temp_bl_last_ver, 8);
		strncpy(config->stake_para, uconfig.stake_para_last_ver, 8);
		strncpy(config->unionpay_key, uconfig.unionpay_key_last_ver, 8);
		
		config->use_total_db = uconfig.use_total_bl_db;
		config->use_inc_db = uconfig.use_inc_bl_db;
		config->use_dec_db = uconfig.use_dec_bl_db;
		config->use_temp_db = uconfig.use_temp_bl_db;
		config->use_fw_db = uconfig.use_stake_fw_db;
		config->use_stake_para_db = uconfig.use_stake_para_db;
		config->use_unionpay_key_db = uconfig.use_unionpay_key_db;

		config->use_total_seq = uconfig.use_total_bl_seq;
		config->use_inc_seq = uconfig.use_inc_bl_seq;
		config->use_dec_seq = uconfig.use_dec_bl_seq;
		config->use_temp_seq = uconfig.use_temp_bl_seq;
		config->use_fw_seq = uconfig.use_stake_fw_seq;
		config->use_stake_para_seq = uconfig.use_stake_para_seq;
		config->use_unionpay_key_seq = uconfig.use_unionpay_key_seq;
	}
	
	strcpy(config->ndev_firmware_ver, conf.nconf.appl_ver);	 //节点机版本
	strcpy(config->yct_firmware_ver, "15011601"); 			//岭南通版本
	strcpy(config->terminal_name, ninfo.terminal_name); //终端名称
	
	config->terminal_no = conf.nconf.term_id;
	config->register_time = ninfo.register_time;  //最后一次注册时间
	strcpy(config->conn_ipaddr, ninfo.center_ip);
	config->conn_port = ninfo.center_port;
	config->last_time_power_on = g_last_time_power_on; //最后一次上电时间

	config->center_conn_stat = ndev_get_conn_stat();
	config->load_bal_stat = ndev_get_balance_stat();  //均衡服务器状态

	ndev_info_stat_put(&(ninfo.stat));
	memcpy(&(config->nstatus), &(ninfo.stat), sizeof(ndev_status_t));

	/* 3G信息 */
	ret = LIB_WL_ERROR;
	lib_wl_csq_sysinfo_t csysinfo; 
	enum WL_DIAL_STAT dial_stat;
	memset(&csysinfo, 0, sizeof(lib_wl_csq_sysinfo_t));
	ret = lib_wl_csq_sysinfo_get(g_bicycle_wl, &csysinfo, 300);
	if(ret == LIB_WL_EOK)
	{
		config->system_mode = csysinfo.sys_mode;
		config->uim_card_stat = csysinfo.sim_state;
		config->rssi = csysinfo.rssi;
		config->fer = csysinfo.fer;
	}

	config->dial_stat = lib_wl_dial_stat_get(g_bicycle_wl, 300);  //拨号状态
	
	return B_DEV_EOK;	
}

int gui_get_stake_info_page_config(struct gui_stake_info_page_config *config)  //获取锁桩信息
{
	if(config == LIB_GE_NULL)
		return LIB_GE_ERROR;
	
	char statistics[512] = {0};
	
	sae_info_get_all_statistics(&statistics);  //注意:如果结构体数据大小改变,statistics,sae_info_get_all_statistics要做相应调整，不然会内存越界
	memcpy(config, statistics, sizeof(struct gui_stake_info_page_config));

	return B_DEV_EOK;
}

/*
 * UI主页信息
 */
int gui_get_ndev_home_page_info(struct gui_ndev_home_page_info *info)
{
	if(info == LIB_GE_NULL)
		return LIB_GE_ERROR;

	int ret = B_DEV_ERROR;
	int i;
	ndev_info_t ninfo;
	unsigned char online = 0;
	unsigned char quantity = 0;
	unsigned char s_stat[17] = {0};
	enum WL_SIGNAL_LEVEL level = WL_SIGNAL_LEVEL0;
	device_config_t config;
		
	memset(&ninfo, 0, sizeof(ndev_info_t));

	ndev_info_get(&ninfo);
#if 0
	strcpy(info->terminal_name, ninfo.terminal_name);
#endif

	device_config_get(&config);
	strcpy(info->terminal_name, config.nconf.site_name);

	info->terminal_no = ninfo.terminal_no;
	info->network_type = ninfo.network_type;

	level = lib_wl_signal_level_get(g_bicycle_wl, 2000);
	info->signal_level = level;
	
	info->wifi_status = 0; //wifi状态
	info->gps_status = ninfo.stat.gps; //GPS状态
		
	if(ndev_get_conn_stat() >= NDEV_NETSTAT_SESSION)  //连接中心状态
		info->center_status = 1;
	else
		info->center_status = 0;

	info->yct_status = 0;   //连接羊城通中心状态

	quantity = sae_info_get_quantity();  //锁桩总数
	sae_info_get_stat(s_stat);
	for(i = 1; i <= quantity; i++)
	{
		if(lib_chk_bit(s_stat, i) == 1)
			online++;
	}

	if(online > 0)   //can总线状态
		info->canbus_status = 1;
	else
		info->canbus_status = 0;

	info->stake_online_num=  online;
	info->stake_total_num = quantity;

	/* 网点二维码 */
	strcpy(info->site_QR_code, config.nconf.site_QR_code);

	return B_DEV_EOK;
}

int gui_get_all_stake_info_config(struct gui_stake_all_info_page_config *config)
{
	if(config == LIB_GE_NULL)
		return LIB_GE_ERROR;

	sae_info_t sinfo[CONFS_STAKE_NUM_MAX + 1];
	memset(&sinfo, 0, sizeof(sae_info_t) * (CONFS_STAKE_NUM_MAX + 1));

	config->quantity = sae_info_get_quantity();

	sae_info_get_all_info(sinfo);
	memcpy(&(config->info), &sinfo, sizeof(sae_info_t) * (CONFS_STAKE_NUM_MAX + 1));	

	return B_DEV_EOK;
}

int gui_set_lnt_card_status(struct gui_lnt_card_status *stat)
{
	if(stat == NULL)
		return B_DEV_ERROR;

	ndev_status_t nstat;
	memset(&nstat, 0, sizeof(ndev_status_t));

	ndev_info_stat_get(&nstat);
	nstat.yct = stat->lnt_card_stat;
	ndev_info_stat_put(&nstat);

	return B_DEV_EOK;
}

int gui_set_admin_card_record(gui_admin_card_info_t *info)
{
	if(info == NULL)
		return B_DEV_ERROR;

	
	

	return B_DEV_EOK;
}


/* 租还车记录查询 */
int gui_get_rent_info_qry_req(struct gui_rent_info_qry_req *req)
{
	if(req == NULL)
		return B_DEV_ERROR;

	int ret = B_DEV_ERROR;
	unsigned char txbuf[1024] = {0};
	unsigned char esc_buf[1024] = {0};
	int esc_len = 0;

	memset(&g_rentinfo_qry_ack, 0, sizeof(gui_rent_info_qry_ack_t));  //清空原数据

	g_rentinfo_qry_ack_f = 0;
	
	ret = ndev_protocol_rent_info_qry_req(txbuf, req, sizeof(struct gui_rent_info_qry_req));
	if(ret > 0)
	{
		esc_len = uplink_escape(esc_buf, txbuf, ret); //上行转义
		ret = ndev_tcp_send_to_server(esc_buf, esc_len, 1000);
	}
	
	return ret;
}

int gui_set_rent_info_qry(struct gui_rent_info_qry_ack *ack, const unsigned int flag)
{
	if(ack == NULL)
		return B_DEV_ERROR;

	g_rentinfo_qry_ack_f = flag;
	
	memset(&g_rentinfo_qry_ack, 0, sizeof(gui_rent_info_qry_ack_t));
	memcpy(&g_rentinfo_qry_ack, ack, sizeof(gui_rent_info_qry_ack_t));
		
	return B_DEV_EOK;
}

int gui_get_rent_info_qry_ack(struct gui_rent_info_qry_ack *ack)
{
	if(ack == NULL)
		return B_DEV_ERROR;

	if(g_rentinfo_qry_ack_f == 1)
	{
		memcpy(ack, &g_rentinfo_qry_ack, sizeof(gui_rent_info_qry_ack_t));
		memset(&g_rentinfo_qry_ack, 0, sizeof(gui_rent_info_qry_ack_t));  //清空原数据
		
		g_rentinfo_qry_ack_f = 0;
		return B_DEV_EOK;
	}
	
	return B_DEV_ERROR;
}

int gui_light_ctrl_time_get(struct gui_light_ctrl_time *ctrl_time)
{
	if(ctrl_time == NULL)
		return B_DEV_ERROR;

	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	memcpy(&(ctrl_time->light_ctrl_time_enable), &(conf.nconf.light_ctrl_time_enable), 2);
	memcpy(&(ctrl_time->light_ctrl_time_disable), &(conf.nconf.light_ctrl_time_disable), 2);	

	return B_DEV_EOK;
}

int gui_light_ctrl_time_set(struct gui_light_ctrl_time *ctrl_time)
{
	if(ctrl_time == NULL)
		return B_DEV_ERROR;

	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));

	device_config_get(&conf);
	memcpy(&(conf.nconf.light_ctrl_time_enable), &(ctrl_time->light_ctrl_time_enable), 2);
	memcpy(&(conf.nconf.light_ctrl_time_disable), &(ctrl_time->light_ctrl_time_disable), 2);	
	device_config_put(&conf);
	
	return B_DEV_EOK;
}

int gui_delay_return_bicycle_ack(struct gui_delay_return_bicycle *bicycle)
{
	if(bicycle == NULL)
		return B_DEV_ERROR;

	int i;
	sae_info_t *info = NULL;
	int quantity =  (int)sae_info_get_quantity(); 

	quantity = sae_info_get_quantity();  //获取锁桩总数
	for(i = 1; i <= quantity; i++) 
	{
		info = sae_info_get_by_id(i); //获取锁桩信息
		if(info->send_hb_times >= 1)
			sae_info_del(i);
	}
	
	bicycle->quantity = quantity; 
	bicycle->register_count = (int)sae_info_get_register_count();
	bicycle->bicycle_online_count = (int)sae_info_get_bicycle_online();

	return B_DEV_EOK;
}




/* 附近网点查询 add by zjc at 2016-11-03 */
int gui_get_nearby_site_info_qry_req(struct gui_nearby_site_info_qry_req *req)
{
	if(req == NULL)
		return B_DEV_ERROR;

	int ret = B_DEV_ERROR;
	unsigned char txbuf[1024] = {0};
	unsigned char esc_buf[1024] = {0};
	int esc_len = 0;

	memset(&g_nearby_site_info_qry_ack, 0, sizeof(gui_nearby_site_info_qry_req_t));  //清空原数据

	g_nearby_siteinfo_qry_ack_f = 0;
	
	ret = ndev_protocol_nearby_siteinfo_qry_req(txbuf, req, sizeof(struct gui_nearby_site_info_qry_req));
	//lib_printf("-----------nearby_site_info--------------\n", &txbuf, ret);
	if(ret > 0)
	{
		esc_len = uplink_escape(esc_buf, txbuf, ret); //上行转义
		ret = ndev_tcp_send_to_server(esc_buf, esc_len, 1000);
	}
	
	return ret;
}

int gui_set_nearby_siteinfo_qry(struct gui_nearby_site_info_qry_ack *ack, const unsigned int flag)
{
	if(ack == NULL)
		return B_DEV_ERROR;

	g_nearby_siteinfo_qry_ack_f = flag;
	
	memset(&g_nearby_site_info_qry_ack, 0, sizeof(gui_nearby_site_info_qry_ack_t));
	memcpy(&g_nearby_site_info_qry_ack, ack, sizeof(gui_nearby_site_info_qry_ack_t));
		
	return B_DEV_EOK;
}

int gui_get_nearby_siteinfo_qry_ack(struct gui_nearby_site_info_qry_ack *ack)
{
	if(ack == NULL)
		return B_DEV_ERROR;

	if(g_nearby_siteinfo_qry_ack_f == 1)
	{
		memcpy(ack, &g_nearby_site_info_qry_ack, sizeof(gui_nearby_site_info_qry_ack_t));
		memset(&g_nearby_site_info_qry_ack, 0, sizeof(gui_nearby_site_info_qry_ack_t));  //清空原数据
		
		g_nearby_siteinfo_qry_ack_f = 0;
		return B_DEV_EOK;
	}
	
	return B_DEV_ERROR;
}
/* end of 附近网点查询*/




