#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "configs.h"
#include "defines.h"
#include "lib_general.h"
#include "lib_zmalloc.h"
#include "device_config.h"
#include "ndev_protocol.h"
#include "lib_wireless.h"
#include "fn_hash.h"
#include "fn_ndev.h"
#include "rx_buf.h"
#include "ndev_info.h"
#include "var_data.h"
#include "universal_file.h"
#include "unity_file_handle.h"
#include "lib_blacklist.h"
#include "lib_threadpool.h"
#include "sae_protocol.h"
#include "stake.h"
#include "lib_firmware.h"
#include "database.h"
#include "lib_ctos.h"
#include "lib_upgrade.h"
#include "ufile_ctrl_info.h"
#include "lib_unity_file.h"
#include "default.h"
#include "external_interface_board.h"
#include "node_device.h"

#include "lib_network_check.h"
#include "lib_lnt.h"
//#include <sys/ioctl.h>
//#include "gpio_ctrl.h"
  

#define	SYS_REBOOT_PATH		"/opt/logpath/network_reboot_times.txt" //记录自网络异常后系统重启次数

extern lib_wl_t *g_bicycle_wl; //无线

//char dail_stat[32] = {0};

typedef struct register_notify  //注册事件通知
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;
}register_notify_t;  

typedef struct btheart_notify  //心跳事件通知
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;
}btheart_notify_t;

typedef struct netstat_notify  //网络状态通知事件
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;	
}netstat_notify_t;

typedef struct ndev_notify
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;		
}ndev_notify_t;


static unsigned int g_ndev_sockfd = 0;    		 	//网络套接口
static unsigned int g_ndev_conn_stat = 0;  			//网络连接状态
static unsigned int g_ndev_balance_stat = 0;        //负载均衡状态
static unsigned int g_ndev_bheart_cnt = 0;			//网络心跳计数
static lib_ringbuffer_t *g_ndev_rb = NULL;			//网络接收缓冲区
static fn_hash_t *g_fn_hash_ndev = NULL;			//哈希解释
database_t *g_database = NULL;
lib_unity_file_t *g_unity_file_db = NULL;     //新版本泛文件数据库
static lib_async_mutex_queue_t *g_async_mutex_queue = NULL;
extern lib_wl_t *g_bicycle_wl; 
lib_upgrade_t *g_upgrade = NULL;
ndev_ftp_download_ctrl_info_t g_ftp_dnl_ctrl_info;  //FTP固件下载信息

static ndev_notify_t g_register_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NDEV_NOTIFY_INIT};
static ndev_notify_t g_btheart_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NDEV_NOTIFY_INIT};
static ndev_notify_t g_netstat_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NDEV_NOTIFY_INIT};


static void *__ndev_reg_thread(void *arg);
static void *__ndev_recv_thread(void *arg);
static void *__ndev_explain_thread(void *arg);
static void *__ndev_bheart_thread(void *arg);
static void *__ndev_unity_file_mng_thread(void *arg);
static int __tcp_send_to_server(void *ptr, const unsigned int len, const unsigned int msec);
static int __tcp_send_to_server_wait(void *ptr, const unsigned int len, const unsigned int msec);
static int __tcp_send_to_server_timedwait(void *ptr, const unsigned int len, const unsigned int msec, const unsigned int wait_time_sec);
static int __ndev_protocol_explain(ndev_rx_buf_t *rxbuf, const unsigned int len);
static void __ndev_word_analysis(ndev_rx_buf_t *buf, const unsigned int len);
static void __ndev_word_run(fn_ndev_val_t *val, const unsigned int len, unsigned char *ack_f);
static int __unity_file_ack_to_server(unity_file_st_t *univ, const unsigned char cmd, const unsigned char sn, const unsigned char result);
static int __unity_file_req_to_server(univ_file_ack_t *ack);
static void *__unity_file_except_handle(void *arg);
static int __is_data_attr_legal(const unsigned char dev_addr, n_data_attr_t *attr);
static void *__database_loop_handle(void *arg);
static int __uplink_escape(unsigned char *dest, unsigned char *src, const int slen);


/*
 * 判断数据属性是否合法
 */
static int __is_data_attr_legal(const unsigned char dev_addr, n_data_attr_t *attr)
{	
	/* 设备地址是节点机,属性是透传,表示数据有矛盾 */
	if((dev_addr == SAE_CAN_LOCAL_ID) && (attr->pass == 1))
		return B_DEV_FALSE;

	return B_DEV_TRUE;
}

static void __set_sockfd(const unsigned int sockfd)
{
	lib_gcc_atmoic_set(&g_ndev_sockfd, sockfd);
}

static unsigned int __get_sockfd(void)
{
	return lib_gcc_atmoic_get(&g_ndev_sockfd);
}

static void __set_conn_stat(const unsigned int stat)
{
	lib_gcc_atmoic_set(&g_ndev_conn_stat, stat);
}

static unsigned int __get_conn_stat(void)
{
	return lib_gcc_atmoic_get(&g_ndev_conn_stat);
}

static void __set_bheart_inc(void)
{
	lib_gcc_atmoic_inc(&g_ndev_bheart_cnt);
}

static void __set_bheart_cnt(const unsigned int cnt)
{
	lib_gcc_atmoic_set(&g_ndev_bheart_cnt, cnt);
}

static unsigned int __get_bheart_cnt(void)
{
	return lib_gcc_atmoic_get(&g_ndev_bheart_cnt);
}

void ndev_set_sockfd(const unsigned int sockfd)
{
	__set_sockfd(sockfd);
}

unsigned int ndev_get_sockfd(void)
{
	return __get_sockfd();
}

unsigned int ndev_get_conn_stat(void)
{
	return __get_conn_stat();
}

void ndev_set_conn_stat(const unsigned int stat)
{
	return __set_conn_stat(stat);
}

static void __set_balance_stat(const unsigned int stat)
{
	lib_gcc_atmoic_set(&g_ndev_balance_stat, stat);
}

static unsigned int __get_balance_stat(void)
{
	return lib_gcc_atmoic_get(&g_ndev_balance_stat);
}

unsigned int ndev_get_balance_stat(void)
{
	return __get_balance_stat();
}

static int __register_notify_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_register_notify.mutex);

	g_register_notify.notify = notify;

	pthread_mutex_unlock(&g_register_notify.mutex);

	return pthread_cond_signal(&g_register_notify.cond);
}

static int __register_notify_timedwait(const unsigned int msec, unsigned int *notify)
{
	int err = B_DEV_ERROR;
	struct timespec tspec;
	
	memset(&tspec, 0, sizeof(struct timespec));
	tspec.tv_sec = time(NULL) + msec / 1000;
	tspec.tv_nsec = msec * 1000 % 1000000;

	pthread_mutex_lock(&g_register_notify.mutex);

	err = pthread_cond_timedwait(&g_register_notify.cond, &g_register_notify.mutex, &tspec); 
	if(err == ETIMEDOUT)
		err = B_DEV_ETIMEOUT;

	*notify = g_register_notify.notify;
		
	pthread_mutex_unlock(&g_register_notify.mutex);

	return err;
}

static int __register_notify_wait(unsigned int *notify)
{
	int err = B_DEV_ERROR;

	pthread_mutex_lock(&g_register_notify.mutex);

	err = pthread_cond_wait(&g_register_notify.cond, &g_register_notify.mutex);
	*notify = g_register_notify.notify;
	
	pthread_mutex_unlock(&g_register_notify.mutex);

	return err;
}

static int __btheart_notify_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_btheart_notify.mutex);

	g_btheart_notify.notify = notify;

	pthread_mutex_unlock(&g_btheart_notify.mutex);

	return pthread_cond_signal(&g_btheart_notify.cond);
}

static int __btheart_notify_timedwait(const unsigned int msec, unsigned int *notify)
{
	int err = B_DEV_ERROR;
	struct timespec tspec;
	
	memset(&tspec, 0, sizeof(struct timespec));
	tspec.tv_sec = time(NULL) + msec / 1000;
	tspec.tv_nsec = msec * 1000 % 1000000;

	pthread_mutex_lock(&g_btheart_notify.mutex);

	err = pthread_cond_timedwait(&g_btheart_notify.cond, &g_btheart_notify.mutex, &tspec);
	if(err == ETIMEDOUT)
		err = B_DEV_ETIMEOUT;

	*notify = g_btheart_notify.notify;
		
	pthread_mutex_unlock(&g_btheart_notify.mutex);

	return err;
}

static int __btheart_notify_wait(unsigned int *notify)
{
	int err = B_DEV_ERROR;

	pthread_mutex_lock(&g_btheart_notify.mutex);

	err = pthread_cond_wait(&g_btheart_notify.cond, &g_btheart_notify.mutex);
	*notify = g_btheart_notify.notify;
	
	pthread_mutex_unlock(&g_btheart_notify.mutex);

	return err;
}

static int __netstat_notify_unicast_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_netstat_notify.mutex);

	g_netstat_notify.notify = notify;

	pthread_mutex_unlock(&g_netstat_notify.mutex);

	return pthread_cond_signal(&g_netstat_notify.cond);
}

static int __netstat_notify_broadcast_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_netstat_notify.mutex);

	g_netstat_notify.notify = notify;

	pthread_mutex_unlock(&g_netstat_notify.mutex);

	return pthread_cond_broadcast(&g_netstat_notify.cond);
}

static int __netstat_notify_timedwait(const unsigned int sec, unsigned int *notify)
{
	int err = B_DEV_ERROR;
	struct timespec tspec;
	
	memset(&tspec, 0, sizeof(struct timespec));
	tspec.tv_sec = time(NULL) + sec;
	tspec.tv_nsec = 0;

	pthread_mutex_lock(&g_netstat_notify.mutex);

	err = pthread_cond_timedwait(&g_netstat_notify.cond, &g_netstat_notify.mutex, &tspec);
	if(err == ETIMEDOUT)
		err = B_DEV_ETIMEOUT;

	*notify = g_netstat_notify.notify;
		
	pthread_mutex_unlock(&g_netstat_notify.mutex);

	return err;
}

static int __netstat_notify_wait(unsigned int *notify)
{
	int err = B_DEV_ERROR;

	pthread_mutex_lock(&g_netstat_notify.mutex);

	err = pthread_cond_wait(&g_netstat_notify.cond, &g_netstat_notify.mutex);
	*notify = g_netstat_notify.notify;
	
	pthread_mutex_unlock(&g_netstat_notify.mutex);

	return err;
}

static int __tcp_send_to_server(void *ptr, const unsigned int len, const unsigned int msec)
{
	int err = B_DEV_ERROR;
	int sockfd = 0;

	lib_printf_v2("----------------------tcp send-------------------", ptr, len, 16);

	sockfd = __get_sockfd();
	if(sockfd > 0)
		err = lib_tcp_writen(sockfd, ptr, len);

	//fprintf(stderr, "------------------------tcp send err: %d\n", err);

	return err;
}

static int __tcp_send_to_server_wait(void *ptr, const unsigned int len, const unsigned int msec)
{
	return __tcp_send_to_server(ptr, len, msec);
}

static int __tcp_send_to_server_timedwait(void *ptr, const unsigned int len, const unsigned int msec, const unsigned int wait_time_sec)
{
	int err = B_DEV_ERROR;
	unsigned int notify = NDEV_NOTIFY_INIT;

	/*
	 * 发送数据失败后,等待wait_time_sec 秒
	 * 网络重新正常后,重新发送数据到中心
	 */
	err = __tcp_send_to_server(ptr, len, msec);
	if(err < 0)
	{
		err = __netstat_notify_timedwait(wait_time_sec, &notify);
		if(err == B_DEV_ETIMEOUT)  //超时
			return B_DEV_ETIMEOUT;
	}

	if(notify == NDEV_NOTIFY_REGISTERED)     //收到注册成功事件通知
		return __tcp_send_to_server(ptr, len, msec);  

	return B_DEV_ERROR;
}

int ndev_tcp_send_to_server(void *ptr, const unsigned int len, const unsigned int msec)
{
	return __tcp_send_to_server(ptr, len, msec);
}

int ndev_tcp_send_to_server_with_escape(void *ptr, const unsigned int len, const unsigned int msec)
{
	unsigned char esc_buf[4096] = {0};
	int esc_len = 0;

	esc_len = __uplink_escape(esc_buf, ptr, len);
	return __tcp_send_to_server(esc_buf, esc_len, msec);
}

/*
 * 转义解析-- 下行:返回转义后数据长度
 */
static int __downlink_escape(unsigned char *dest, unsigned char *src, const int slen)
{
	if((dest == NULL) || (src == NULL))
		return B_DEV_ERROR;
	
	int idxI = 1, idxJ = 1;

	while(idxI <= (slen - 2))
	{
		if(src[idxI] == NDEV_ESC_F0H)
		{
			if(src[idxI + 1] == 0x01)
				dest[idxJ] = NDEV_HD;
			else if(src[idxI + 1] == 0x02)
				dest[idxJ] = NDEV_ESC_F0H;
			else if(src[idxI + 1] == 0x00)
				dest[idxJ] = NDEV_TAIL;

			idxI++;
		}
		else
			dest[idxJ] = src[idxI];

		idxI++;
		idxJ++;
	}

	dest[0] = NDEV_HD;
	dest[idxJ++] = NDEV_TAIL;

	return idxJ;
}

/*
 * 转义解析-- 上行:返回转义后数据长度
 */
static int __uplink_escape(unsigned char *dest, unsigned char *src, const int slen)
{
	if((dest == NULL) || (src == NULL))
		return B_DEV_ERROR;

	int idxI = 1, idxJ = 1;
	
	while(idxI <= (slen - 2))
	{
		switch(src[idxI])
		{
			case NDEV_HD:
			{
				dest[idxJ++] = NDEV_ESC_F0H;
				dest[idxJ++] = NDEV_ESC_01H;	
			}
			break;

			case NDEV_TAIL:
			{
				dest[idxJ++] = NDEV_ESC_F0H;
				dest[idxJ++] = NDEV_ESC_00H;				
			}
			break;

			case NDEV_ESC_F0H:
			{
				dest[idxJ++] = NDEV_ESC_F0H;
				dest[idxJ++] = NDEV_ESC_02H;					
			}
			break;
			
			default:
				dest[idxJ++] = src[idxI];
		}

		idxI++;
	}

	dest[0] = NDEV_HD;
	dest[idxJ++] = NDEV_TAIL;
		
	return idxJ;
}

int downlink_escape(unsigned char *dest, unsigned char *src, const int slen)
{
	return __downlink_escape(dest, src, slen);
}

int uplink_escape(unsigned char *dest, unsigned char *src, const int slen)
{
	return __uplink_escape(dest, src, slen);
}

int ndev_register_notify_put(const unsigned int notify)
{
	return __register_notify_put(notify);
}

int ndev_btheart_notify_put(const unsigned int notify)
{
	return __btheart_notify_put(notify);
}

static void __fn_ndev_register(void)
{
	fn_hash_register(g_fn_hash_ndev, NDEV_REG_W, fn_ndev_reg);						//设备注册
	fn_hash_register(g_fn_hash_ndev, NDEV_STAT_W, fn_ndev_stat);						//桩机状态
	fn_hash_register(g_fn_hash_ndev, NDEV_N_TIME_W, fn_ndev_time);					//节点机时间
	fn_hash_register(g_fn_hash_ndev, NDEV_S_TOTAL_NUM_W, fn_ndev_s_total_num);		//桩机总数量
	fn_hash_register(g_fn_hash_ndev, NDEV_S_ONLINE_NUM_W, fn_ndev_s_online_num);	//桩机在线数量
	fn_hash_register(g_fn_hash_ndev, NDEV_TOTAL_BLK_VER_W, fn_ndev_total_ver);		//总量黑名单版本
	fn_hash_register(g_fn_hash_ndev, NDEV_INC_BLK_VER_W, fn_ndev_inc_ver);			//增量黑名单版本
	fn_hash_register(g_fn_hash_ndev, NDEV_DEC_BLK_VER_W, fn_ndev_dec_ver);			//减量黑名单版本
	fn_hash_register(g_fn_hash_ndev, NDEV_TEMP_BLK_VER_W, fn_ndev_temp_ver);		//临时黑名单版本
	fn_hash_register(g_fn_hash_ndev, NDEV_S_BLK_ATTR_W, fn_ndev_s_blk_attr);			//桩机黑名单属性
	fn_hash_register(g_fn_hash_ndev, NDEV_S_ATTR_W, fn_ndev_s_attr);					//桩机属性
	fn_hash_register(g_fn_hash_ndev, NDEV_YCT_STAT_W, fn_ndev_yct_stat);				//羊城通读卡器状态
	fn_hash_register(g_fn_hash_ndev, NDEV_BTHEART_RETURN_W, fn_ndev_btheart_return); 	//心跳回应
	fn_hash_register(g_fn_hash_ndev, NDEV_BTHEART_TIME_SET_W, fn_ndev_btheart_time_set);  			//心跳时间设置
	fn_hash_register(g_fn_hash_ndev, NDEV_S_LOCK_OP_W, fn_ndev_s_lock_op);  						//立刻开锁
	fn_hash_register(g_fn_hash_ndev, NDEV_LOAD_SERV_IP_SET_W, fn_ndev_load_serv_ip_set);  	 		//负载服务器IP
	fn_hash_register(g_fn_hash_ndev, NDEV_LOAD_SERV_PORT_SET_W, fn_ndev_load_serv_port_set);  	//负载服务器端口
	fn_hash_register(g_fn_hash_ndev, NDEV_LOAD_SERV2_IP_SET_W, fn_ndev_load_serv2_ip_set);  	 	//备份负载服务器IP
	fn_hash_register(g_fn_hash_ndev, NDEV_LOAD_SERV2_PORT_SET_W, fn_ndev_load_serv2_port_set);  	//备份负载服务器端口
	fn_hash_register(g_fn_hash_ndev, NDEV_DEVICE_REBOOT_W, fn_ndev_device_reboot); 				//重启设备 
	fn_hash_register(g_fn_hash_ndev, NDEV_FTP_SET_W, fn_ndev_ftp_set);					//FTP服务器设置
	fn_hash_register(g_fn_hash_ndev, NDEV_SITE_NAME_W, fn_ndev_site_name_ctrl);    		//站点名称
	fn_hash_register(g_fn_hash_ndev, NDEV_UNIV_UPDATE_W, fn_ndev_univ_update_ctrl);   	 //更新泛文件
	fn_hash_register(g_fn_hash_ndev, NDEV_EXCEPT_HANDLE_W, fn_ndev_except_handle);    //异常处理
	fn_hash_register(g_fn_hash_ndev, NDEV_FTP_DNL_W, fn_ndev_ftp_download);   	//FTP固件下载
	fn_hash_register(g_fn_hash_ndev, NDEV_UPDATE_CTRL_W, fn_ndev_update_ctrl);   	//固件更新控制
//	fn_hash_register(g_fn_hash_ndev, NDEV_RENT_INFO_W, fn_ndev_rent_info);    		//租还车信息
//	fn_hash_register(g_fn_hash_ndev, NDEV_GET_S_ATTR_W, fn_ndev_get_s_attr);		//获取桩机属性

 	fn_hash_register(g_fn_hash_ndev, NDEV_GET_ALL_SAE_PHY_INFO_W, fn_ndev_sae_all_phy_info);  //全部锁桩物理信息
 	fn_hash_register(g_fn_hash_ndev, NDEV_GET_GPS_INFO_W, fn_ndev_gps_info);   //获取GPS信息
	fn_hash_register(g_fn_hash_ndev, NDEV_SET_LIGHT_CTRL_TIME_W, fn_ndev_set_light_ctrl_time);   //设置灯箱控制时间
	fn_hash_register(g_fn_hash_ndev, NDEV_SITE_QR_CODE_W, fn_ndev_set_site_QR_code);  //网点二维码
	
}

static enum UPE_FIRMWARE_TYPE __firmware_find(const char *fname)
{
	if(strstr(fname, "kernel_") != NULL)
		return UPE_FIRMWARE_TYPE_KERNEL;
	else if(strstr(fname, "fw_") != NULL)
		return UPE_FIRMWARE_TYPE_FIRMWARE;
	else if(strstr(fname, "app_") != NULL)
		return UPE_FIRMWARE_TYPE_APPL;
	else if(strstr(fname, "LNT_ZM_") != NULL)
		return UPE_FIRMWARE_TYPE_LNT_ZM;
	return 255;
}

int ndev_init(void)
{
	int err = B_DEV_ERROR;
	int sockfd;
	ndev_status_t nstat;
	pthread_t ndev_reg_thr;
	pthread_t ndev_recv_thr;
	pthread_t ndev_explain_thr;
	pthread_t ndev_bheart_thr;
	pthread_t ndev_ufile_mng_thr;
	//pthread_t ndev_database_thr;
	pthread_t thr_ehndl;
		
	/* 接收缓冲区 */
	g_ndev_rb = lib_ringbuffer_create(CONFS_NDEV_RECV_BUF_SZ);
	if(g_ndev_rb == NULL)
	{
		SYS_LOG_ERR("%s: lib_ringbuffer_create failed!", __FUNCTION__);		
		goto ERR;
	}

	/* 哈希函数 */
	g_fn_hash_ndev = fn_hash_create(CONFS_NDEV_FN_HASH_NUM);
	if(g_fn_hash_ndev == NULL)
	{
		SYS_LOG_ERR("%s: fn_hash_create failed!", __FUNCTION__);		
		goto ERR;
	}
	
	__fn_ndev_register(); //哈希函数注册 

	/* 设置网络初始化连接状态 */
	__set_conn_stat(NDEV_NETSTAT_INIT);

	/* 均衡服务器连接状态 */
	__set_balance_stat(NDEV_BALSTAT_ERR_UNKNOW);

	/* 节点机状态 */
	memset(&nstat, 0, sizeof(ndev_status_t));
	ndev_info_stat_get(&nstat);
	nstat.power = 1;
	nstat.rtc = 1;
	nstat.keyboard = 1;
	nstat.ext_mcu = 1;
	ndev_info_stat_put(&nstat);

	/* 节点机升级 */
	g_upgrade = lib_upgrade_create(LIB_UPE_MTYPE_NDEV);
	if(g_upgrade == LIB_UPE_NULL)
	{
		SYS_LOG_ERR("%s: lib_upgrade_create LIB_UPE_MTYPE_NDEV failed!", __FUNCTION__);		
		goto ERR;
	}  
	
	/* 获取FTP下载控制信息 */
	ftp_download_ctrl_info_t fdnl_ctrl_info;
	memset(&fdnl_ctrl_info, 0, sizeof(ftp_download_ctrl_info_t));
	err = lib_upgrade_get_ftp_dnl_ctrl_info(g_upgrade, &fdnl_ctrl_info);
	if(err == LIB_UPE_EOK)
	{
		memcpy(&g_ftp_dnl_ctrl_info, &fdnl_ctrl_info, sizeof(ftp_download_ctrl_info_t));
		lib_printf_v2("FTP_DNL_CTRL_INFO", &g_ftp_dnl_ctrl_info, sizeof(ftp_download_ctrl_info_t), 16);
	}
   
	/* 创建数据库 */
	g_database = db_create(SQLITE3_DB_PATHNAME);
	if(g_database == DB_NULL)
	{
		SYS_LOG_ERR("%s: db create %s failed!", __FUNCTION__, SQLITE3_DB_PATHNAME);
		goto ERR;
	}
 
	/* 创建泛文件数据库 */
	g_unity_file_db = lib_unity_file_create(UNITY_FILE_DB_PATHNAME);
	if(g_unity_file_db == LIB_UF_NULL)
	{
		fprintf(stderr, "%s: lib_unity_file_create %s failed!", __FUNCTION__, UNITY_FILE_DB_PATHNAME);
		SYS_LOG_ERR("%s: lib_unity_file_create %s failed!", __FUNCTION__, UNITY_FILE_DB_PATHNAME);
		goto ERR;
	}

	/* 泛文件配置 */
	unity_file_config_t uconfig;
	memset(&uconfig, 0, sizeof(unity_file_config_t));
	int ret = lib_unity_file_config_select_data(g_unity_file_db, &uconfig);
	if(ret != LIB_UF_OK)
	{
		fprintf(stderr, "------------------Unity File Config Init-------------------\n");

		strncpy(uconfig.total_bl_last_ver, UNIV_BLK_TOTAL_DFAULT, 8);
		strncpy(uconfig.inc_bl_last_ver, UNIV_BLK_INC_DFAULT, 8);
		strncpy(uconfig.dec_bl_last_ver, UNIV_BLK_DEC_DFAULT, 8);
		strncpy(uconfig.temp_bl_last_ver, UNIV_BLK_TEMP_DFAULT, 8);
		strncpy(uconfig.stake_fw_last_ver, UNIV_SAE_FW_DEFAULT, 8);
		strncpy(uconfig.stake_para_last_ver, UNIV_SAE_PARA_DEFAULT, 8);
		strncpy(uconfig.unionpay_key_last_ver, UNIV_UNIONPAY_KEY_DEFAULT, 8);
		
		lib_unity_file_config_replace_data(g_unity_file_db, &uconfig);
	}
	
	unity_file_config_printf();

	/* 创建异步队列 */
	g_async_mutex_queue = lib_async_mutex_queue_create(3);
	if(g_async_mutex_queue == LIB_GE_NULL)
	{
		SYS_LOG_ERR("%s: lib_async_mutex_queue_create failed!", __FUNCTION__);
		goto ERR;
	}

	/* 负载均衡线程 */
	err = lib_normal_thread_create(&ndev_reg_thr, __ndev_reg_thread, NULL);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("%s: __ndev_reg_thread failed!", __FUNCTION__);
		goto ERR;
	}
 
	/* 数据接收线程 */
	err = lib_normal_thread_create(&ndev_recv_thr, __ndev_recv_thread, NULL);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("%s: __ndev_recv_thread failed!", __FUNCTION__);
		goto ERR;
	}    
	
	/* 网络数据解析线程 */
	err = lib_normal_thread_create(&ndev_explain_thr, __ndev_explain_thread, NULL);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("%s: __ndev_explain_thread failed!", __FUNCTION__);
		goto ERR;
	}
	
	/* 心跳处理线程 */
	err = lib_normal_thread_create(&ndev_bheart_thr, __ndev_bheart_thread, NULL);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("%s: __ndev_bheart_thread failed!", __FUNCTION__);
		goto ERR;
	}
	
	/* 泛文件管理线程 */
	err = lib_normal_thread_create(&ndev_ufile_mng_thr, __ndev_unity_file_mng_thread, NULL);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("%s: __ndev_univ_file_mng_thread failed!", __FUNCTION__);
		goto ERR;
	}

#if 0
	/* 数据库轮询线程 */
	err = lib_normal_thread_create(&ndev_database_thr, __database_loop_handle, NULL);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("%s: __database_loop_handle failed!", __FUNCTION__);
		goto ERR;
	}

	lib_msleep(500);
#endif

	lib_normal_thread_create(&thr_ehndl, __unity_file_except_handle, NULL);

	lib_msleep(500);
	
	SYS_LOG_INFO("nodev device init ok");
	
	return B_DEV_EOK;

ERR:
	sockfd = __get_sockfd();
	if(sockfd > 0)
	{
		fprintf(stderr, "main close sockfd: %d\n", sockfd);
		lib_close(sockfd);
	}

	if(g_upgrade != NULL)
		lib_upgrade_destroy(g_upgrade);
	
	if(g_database != NULL)
		db_destroy(g_database);

	if(g_unity_file_db != NULL)
		lib_unity_file_destroy(g_unity_file_db);

	if(g_async_mutex_queue != LIB_GE_NULL)
		lib_async_mutex_queue_destroy(g_async_mutex_queue);
	
	lib_ringbuffer_destroy(g_ndev_rb);
	fn_hash_destroy(g_fn_hash_ndev);
	
	fprintf(stderr, "nodev device init failed!\n");
	return B_DEV_ERROR;
}

int ndev_destroy(void)
{ 
	int sockfd = __get_sockfd();
	if(sockfd > 0)
	{
		fprintf(stderr, "ndev destroy close sockfd: %d\n", sockfd);
		lib_close(sockfd);
	}

	if(g_async_mutex_queue != LIB_GE_NULL)
		lib_async_mutex_queue_destroy(g_async_mutex_queue);
		
	lib_ringbuffer_destroy(g_ndev_rb);
	fn_hash_destroy(g_fn_hash_ndev);

	if(g_upgrade != NULL)
		lib_upgrade_destroy(g_upgrade);
	
	if(g_database != NULL)
		db_destroy(g_database);
	
	if(g_unity_file_db != NULL)
		lib_unity_file_destroy(g_unity_file_db);

	return B_DEV_EOK;
}

/*
 *  负载均衡服务器连接
 */
 #define G_LB_CNT_MAX			(30)
 #define G_LB_CNT_TIME			(300)
 static int g_lb_cnt = 0;

 static char g_bal_ipaddr[32] = {0};

static int __udp_net_recvform(int sockfd, const char *ipaddr, const unsigned short port, void *buff, const unsigned int len, const unsigned int sec)
{
	if(buff == NULL)
		return -1;

	int ret = -1;
	int s_sec = sec;
	int i;

	printf("-----------------__udp_net_recvform, sec:%d\n", s_sec);
	
	
	for(i = 0; i < s_sec; i++)
	{
		lib_msleep(400);

		printf("-----------after sleep 400ms\n");

		ret = lib_udp_recvfrom(sockfd, ipaddr, port, buff, len);
		printf("--------------------lib_udp_recvfrom, recv ret:%d\n", ret);
		if(ret > 0)
		{
			return ret;
		}
		
		lib_msleep(200);
	}
	
	return -2;  //超时
}

#if 0
static int __udp_net_sendto(const int sockfd, const char *ipaddr, const unsigned short port, const void *ptr, const unsigned int len)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in s_addr;
	memset(&s_addr, 0, sizeof(struct sockaddr_in));

	s_addr.sin_family  = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr(ipaddr);
	s_addr.sin_port = htons(port);

	return sendto(sockfd, ptr, len, 0, (struct sockaddr *)&s_addr, addrlen));		
}
#endif


/* 用于ping测试的DNS服务器,防止均衡负载服务器出问题 */
#define	PING_NDS1	"180.76.76.76" //百度
#define	PING_NDS2	"223.5.5.5"	   //阿里

#define UDP_ERROR_TIMES_TO_REBOOT	1 // 2->1 2017-01-09

#define	NET_INFO_BACKUP_PATH	"/opt/logpath/net_info.txt"


static int __load_balance(void)
{
	int n = 0;
	int err = -1;
	int sockfd = -1;
	unsigned char buf[512] = {0};
	device_config_t conf;
	ndev_rx_buf_t rxbuf;
	load_balance_ack_t load_ack;
	ndev_info_t info;
	char *ipaddr = NULL;
	unsigned short port = 0;
	unsigned char sotimeout = 0; 
	unsigned int link_loop = 1;
	int esc_len = 0;
	int d_len = 0;
	int b64_len = 0;
	unsigned short calc_crc16, crc16;
	
	static unsigned int try_times = 0, test_times = 0; //test_times:udp接收、发送失败次数
	static unsigned char wireless_flag = 0; //3g网络状态标记，0为异常，1为正常
	static int sys_reboots = 0; //系统重启次数 
	FILE *logfd = NULL;
	static int gpio_ctrl_fd = -1;
	static int reboot_flag = 0; //系统重启标志 0:不重启 1:重启
	static int deal_type = 0; //网络异常后的处理方式 0:重新拨号 1:重启系统
	
	char cmd_str[64] = {0};
	int ret1 = -1, ret2 = -1;

	while(1)
	{   
		memset(&conf, 0, sizeof(device_config_t));

		if(sockfd > 0)
		{
			lib_udp_close(sockfd);
			sockfd = -1;
		}

		device_config_get(&conf);  //获取设备配置
		if(link_loop == 1)
		{          
			ipaddr = lib_iaddr_to_saddr(conf.nconf.load_server1_ip);
			port = conf.nconf.load_server1_port;				//负载均衡端口
		}
		else if(link_loop == 2)
		{
			ipaddr = lib_iaddr_to_saddr(conf.nconf.load_server2_ip);
			port = conf.nconf.load_server2_port;				//负载均衡端口
		}

		if(conf.nconf.load_timeout < 20) //修改超时 add by zjc at 2016-11-28
			sotimeout = 20;
		else
			sotimeout = conf.nconf.load_timeout;					//超时等待时间

		if(ipaddr != NULL)  //拷贝地址
			memcpy(&g_bal_ipaddr, ipaddr, sizeof(g_bal_ipaddr));
		
		/* 使用网络类型 */
		fprintf(stderr, "conf.nconf.using_wireless = %d\n", conf.nconf.using_wireless);		
		fprintf(stderr, "ndev start connect to load balancers server\n");
		SYS_LOG_NOTICE("ndev start connect to load balancers server\n");
		SYS_LOG_NOTICE("UDP failed %d times, reboot flag:%d\n", test_times, reboot_flag);
	
		/* 重新连接负载均衡,要清理旧连接数据 */
		memset(&info, 0, sizeof(ndev_info_t));
		ndev_info_get(&info);
		memset(&(info.load), 0, sizeof(load_balance_ack_t));
		memset(&(info.center_ip), 0, sizeof(info.center_ip));
		info.center_port = 0;
		info.register_time = 0;

	    /* 连接均衡负载前检查网络状态 */
		/* 修改网络状态检查和节点机重启策略 add by zjc at 2016-12-14 */
		ret1 = lib_get_network_status(ipaddr); //1.测试均衡负载服务器
		SYS_LOG_NOTICE("WARNING: Check Network Status, ret1:%d\n", ret1);
		//fprintf(stderr, "NOTICE: Check Network Status, ret1:%d\n", ret1);
		if(ret1 != CHECK_RESULT_PACKET_OK) 
		{
			info.stat.wireless = 0; 
			wireless_flag = 0;
			
			if(ret1 == CHECK_RESULT_PACKET_ERROR) //CHECK_RESULT_PACKET_ERROR:100%丢包
			{
				ret2 = lib_get_network_status(PING_NDS1); //2.测试百度DNS服务器
				
				SYS_LOG_NOTICE("WARNING: Check Network Status, ret2:%d\n", ret2);
				
				if(ret2 == CHECK_RESULT_PACKET_ERROR) //一、网络确实异常
				{
					SYS_LOG_NOTICE("WARNING: Network Abnormal, deal_type:%d, test_times:%d\n", deal_type, test_times);

					if((deal_type == 0) && (test_times >= UDP_ERROR_TIMES_TO_REBOOT)) //重新拨号
					{
						SYS_LOG_NOTICE("WARNING: Network Abnormal and UDP failed %d times, redial!\n", test_times);

						system("killall -9 pppd");  
						system("killall -9 chat");

						deal_type = 1;

						lib_sleep(60); //50->60 2017-01-09
					}
					else if(deal_type == 1)
					{
						reboot_flag = (test_times >= (3*UDP_ERROR_TIMES_TO_REBOOT))?1:0;
					}
				}
				else //二、网络正常但均衡负载服务器异常
				{
					wireless_flag = 1;
					ndev_info_put(&info);  
					
					SYS_LOG_NOTICE("WARNING: Network OK but Balance Server Abnormal, deal_type:%d, test_times:%d\n", deal_type, test_times);

					if((deal_type == 0) && (test_times >= (2*UDP_ERROR_TIMES_TO_REBOOT))) //重新拨号
					{
						SYS_LOG_NOTICE("WARNING: Network OK but UDP failed %d times, redial!\n", test_times);
						
						system("killall -9 pppd");  
						system("killall -9 chat");

						deal_type = 1;
						
						lib_sleep(60); //50->60 2017-01-09
					}
					else if(deal_type == 1)
					{
						reboot_flag = (test_times >= (4*UDP_ERROR_TIMES_TO_REBOOT))?1:0;
					}
				}
			}
		}
		else //三、均衡负载服务器ping包正常
		{
			info.stat.wireless = 1;  //正常
			wireless_flag = 1;

			SYS_LOG_NOTICE("WARNING: Network OK, deal_type:%d, test_times:%d\n", deal_type, test_times);
			
			if((deal_type == 0) && (test_times >= (3*UDP_ERROR_TIMES_TO_REBOOT))) //重新拨号
			{
				SYS_LOG_NOTICE("WARNING: Network OK but UDP failed %d times, redial!\n", test_times);
				
				system("killall -9 pppd");  
				system("killall -9 chat");

				deal_type = 1;

				lib_sleep(60); //50->60 2017-01-09
			}
			else if(deal_type == 1)
			{
				reboot_flag = (test_times >= (5*UDP_ERROR_TIMES_TO_REBOOT))?1:0;
			}
		}
   
		ndev_info_put(&info);  
		
		#if 1
		if(reboot_flag == 1)
		{
			//将系统重启次数记录下来, 防止网络一直异常时节点机频繁重启
			logfd = fopen(SYS_REBOOT_PATH, "rb");
			if(NULL != logfd)
			{
				fread(&sys_reboots, sizeof(sys_reboots), 1, logfd);
				fclose(logfd);
			}
			
			SYS_LOG_DEBUG("Network Abnormal, udp faild %d times, sys_reboots:%d\n", test_times, sys_reboots);
			 
			if((sys_reboots <= 2) || ((test_times % 145) == 0)) //后面为大概6小时重启 2017-01-09
			{		
				logfd = fopen(SYS_REBOOT_PATH, "wb"); //满足重启条件才写入重启次数 2016-12-31
				if(NULL != logfd)
				{
					sys_reboots++;
					
					fwrite(&sys_reboots, sizeof(sys_reboots), 1, logfd);
					fclose(logfd); 
				}
			
				SYS_LOG_DEBUG("Network Abnormal, Reboot System %d times!!!\n", sys_reboots);
			
				system("/mnt/firmware/reboot_wdt");
			}
			else
			{
				deal_type = (test_times % (3*UDP_ERROR_TIMES_TO_REBOOT))?1:0; //重启次数超限时的处理方法 2017-01-04
			}
		}
		#endif
		
		sockfd = lib_udp_client_nonb_new();
		if(sockfd < 0)
		{
			lib_sleep(5);
			try_times++;
			fprintf(stderr, "-----------------------ndev connect to load balancers failed, Reconnect %d times Now!!!\n", try_times);
			SYS_LOG_NOTICE("ndev connect to load balancers failed, Reconnect %d times Now!!!\n", try_times);
			
			g_lb_cnt++;
			if(g_lb_cnt > G_LB_CNT_MAX) //最多30次
			{
				g_lb_cnt = 0;
				lib_sleep(G_LB_CNT_TIME); //休眠300秒
			}

			continue;
		}

		fprintf(stderr, "load balancers udp sockfd = %d\n", sockfd);

		n = ndev_protocol_load_req(buf);  //均衡协议封包
		if(n > 0)
		{
			/* 清除中心连接"位" */
			ndev_status_t nstatus;  
			memset(&nstatus, 0, sizeof(ndev_status_t));
			ndev_info_stat_get(&nstatus);
			nstatus.center = 0;
			ndev_info_stat_put(&nstatus);

			//lib_printf_v2("--------------UDP sendto------------", buf, n, 16);
			
			esc_len = __uplink_escape(N_RX_DA(rxbuf), buf, n); //上行转义

			err = lib_udp_sendto(sockfd, ipaddr, port, N_RX_DA(rxbuf), esc_len); //UDP返回0不一定发送成功
			//printf("-----------------udp send size:%d\n", err);
			if(err > 0)
			{	
				memset(&buf, 0, sizeof(buf));
				
				err = lib_udp_recvfrom_select(sockfd, ipaddr, port, buf, 512, sotimeout * 1000);
				//printf("-----------------udp recv size:%d\n", err);
				if(err < 0)	// 连接失败,重新连接另一个服务器
				{					
					lib_udp_close(sockfd);
					sockfd = -1;    
					
					lib_sleep(15);  //重连间隔

					if(link_loop == 1) 		//循环连接服务器
						link_loop = 2;
					else if(link_loop == 2)
						link_loop = 1;

					try_times++;

					test_times++; //udp发送、udp接收失败次数
					
					g_lb_cnt++;
					if(g_lb_cnt > G_LB_CNT_MAX)
					{
						g_lb_cnt = 0;
						lib_sleep(G_LB_CNT_TIME); 
					}
					
					//fprintf(stderr, "-----------------ndev try to connect to other load balancers server %d times!!!\n", try_times);
					SYS_LOG_NOTICE("ndev try to connect to other load balancers server %d times!!!\n", try_times);

					continue;
				}
				else  //成功接收报文
				{		
					test_times = 0; //均衡负载申请返回成功后清零! 2016-12-12
					deal_type = 0;  //网络异常处理方式初始化
					
					#if 1 //均衡申请返回成功后节点机重启次数清零 2016-12-28
					logfd = fopen(SYS_REBOOT_PATH, "wb");
					if(NULL != logfd)
					{
						sys_reboots = 0;
						fwrite(&sys_reboots, sizeof(sys_reboots), 1, logfd);
						fclose(logfd); 
					}		
					#endif

					//fprintf(stderr, "UDP recvfrom err = %d\n", err);

					lib_printf_v2("--------------UDP recvfrom------------", buf, err, 16);
					
					/* 读取数据完毕,马上断开套接口 */
					lib_udp_close(sockfd);
					sockfd = -1;
					
					memset(&rxbuf, 0, sizeof(ndev_rx_buf_t));
					esc_len = __downlink_escape(N_RX_DA(rxbuf), buf, err); //下行转义
					d_len = ntohs(N_RX_LEN(rxbuf));
					calc_crc16 = lib_crc16_with_table((char *)&(N_RX_DA_B(rxbuf, 1)), esc_len - 4); //CRC16校验
					crc16 = ntohs(calc_crc16);
					if(memcmp(&crc16, &(N_RX_DA_B(rxbuf, esc_len - 3)), 2) != 0)  
					{
						fprintf(stderr, "-------------------ndev load balancers server crc16 failed!!!\n");
						
						SYS_LOG_ERR("%s:ndev load balancers server crc16 failed", __FUNCTION__);
						
						__set_balance_stat(NDEV_BALSTAT_ERR_UNKNOW);

						if(link_loop == 1) 		//循环连接服务器
							link_loop = 2;
						else if(link_loop == 2)
							link_loop = 1;

						try_times++;
					
						lib_sleep(15);

						g_lb_cnt++;
						if(g_lb_cnt > G_LB_CNT_MAX)
						{
							g_lb_cnt = 0;
							lib_sleep(G_LB_CNT_TIME); 
						}
						
						continue;  //检验失败，重连
					}

					fprintf(stderr, "load balance crc16 OK\n");
		
					/* Base64解码*/
					memset(&load_ack, 0, sizeof(load_balance_ack_t));
					b64_len = lib_b64_decode_hex((char *)&load_ack, (char *)&(N_RX_DATA_B(rxbuf, 2)), d_len - 2);
		
					lib_printf_v2("LOAD RECV B64", &load_ack, b64_len, 16);
                     
					/* 错误状态 */
					fprintf(stderr, "load_ack.status=0x%02x\n", load_ack.status);
					if(load_ack.status != NDEV_BALSTAT_OK)
					{
						fprintf(stderr, "-------------------ndev load balancers server authentication failed!!!\n");

						SYS_LOG_ERR("%s:ndev load balancers server authentication failed, load_ack.status=0x%02x\n", __FUNCTION__, load_ack.status);
						
						__set_balance_stat(load_ack.status);

						if(link_loop == 1) 		//循环连接服务器
							link_loop = 2;
						else if(link_loop == 2)
							link_loop = 1;

						try_times++;
						
						lib_sleep(15);

						g_lb_cnt++;
						if(g_lb_cnt > G_LB_CNT_MAX)
						{
							g_lb_cnt = 0;
							lib_sleep(G_LB_CNT_TIME); 
						}
						
						continue;  //认证失败,重连
					}

					unsigned int ip = LIB_IPV4(load_ack.ip[4], load_ack.ip[5], load_ack.ip[6], load_ack.ip[7]);
					char *s_ip = lib_iaddr_to_saddr(ip);
					
					fprintf(stderr, "ndev get {ip:port} = %s:%d\n", s_ip, ntohs(load_ack.port));

					/* 保存负载均衡数据*/
					memset(&info, 0, sizeof(ndev_info_t));
					ndev_info_get(&info);
					memcpy(&info.load, &load_ack, sizeof(load_balance_ack_t));
					strcpy(info.center_ip, s_ip); //保存中心ip
					info.center_port = ntohs(load_ack.port); //保存中心端口
					ndev_info_put(&info);

					link_loop = 1;
					__set_balance_stat(NDEV_BALSTAT_OK);
					
					SYS_LOG_NOTICE("ndev connect to load balancers server success\n");

					try_times = 0; //add by zjc at 2016-11-18
										
					return B_DEV_EOK;
				}
			}
			else  //休眠，防止死循环
			{
				test_times++; //udp发送、udp接收失败次数
				
				SYS_LOG_DEBUG("udp sendto failed\n");
				
				lib_sleep(10);
				continue;
			}
		}
		else
		{
			SYS_LOG_DEBUG("ndev protocol load req failed\n");

			lib_sleep(15); //防止线程死循环	
			continue;
		}
	}
}

/*
 *  设备注册
 */
#define __NDEV_REGISTER_SOCKET_SLEEP			(15)
static int __ndev_register(void)
{
	int err = B_DEV_ERROR;
	int n = 0;
	int esc_len = 0;
	int sockfd = 0;
	int try_times = 0;
	unsigned char buf[512] = {0};
	unsigned char esc_buf[512] = {0};
	ndev_info_t info;
	device_config_t dev_conf;
	ndev_status_t nstat;
	unsigned char txbuf[512] = {0};
	unsigned int notify = NDEV_NOTIFY_INIT;
	
	while(1)
	{
		memset(&info, 0, sizeof(ndev_info_t));
		ndev_info_get(&info);

		fprintf(stderr, "ndev start connect to server\n");
		fprintf(stderr, "center_ip: %s\n", info.center_ip);
		fprintf(stderr, "center_port: %d\n", info.center_port);

		memset(&dev_conf, 0, sizeof(device_config_t));
		device_config_get(&dev_conf);
		
	
		sockfd = lib_tcp_client_nonb_new(info.center_ip, info.center_port, 20); //5->20 2016-11-25
		if(sockfd > 0)
		{
			fprintf(stderr, "ndev tcp client connect sockfd: %d\n", sockfd);
			
			__set_sockfd(sockfd);		
			__set_conn_stat(NDEV_NETSTAT_CONNECT);

			n = ndev_protocol_reg_req(buf);
			if(n > 0)
			{
				ndev_info_get(&info);
				info.register_time = 0;
				info.netstat = NDEV_NETSTAT_CONNECT;  
				info.stat.center = 0;  //与中心通讯异常
				memset(info.terminal_name, 0, sizeof(info.terminal_name)); 
				ndev_info_put(&info);

				lib_printf_v2("---------------register write -------------\n", buf, n, 16);
				
				esc_len = __uplink_escape(esc_buf, buf, n); //上行转义
				err = lib_tcp_write_select(sockfd, esc_buf, esc_len, 5000); // 1000->5000 2016-11-25
				//printf("-----------------register send size:%d\n", err);
				if(err > 0)
				{	
					///notify在收到中心签到响应后改变
					err = __register_notify_timedwait(CONFS_NDEV_WAIT_REG_TIMES * 1000, &notify); //成功返回0
					if(err == B_DEV_ETIMEOUT)
					{
						fprintf(stderr, "ndev register timeout!!!!\n");

						SYS_LOG_DEBUG("ndev register timeout");
						
						lib_close(__get_sockfd());//关闭套接字
						__set_sockfd(0);
						__set_conn_stat(NDEV_NETSTAT_INIT);

						try_times++;
						if(try_times >= CONFS_NDEV_TRY_REG_TIMES)
						{
							try_times = 0;
							return B_DEV_ETIMEOUT;
						}
						continue;  
					}
     
					/* 注册成功 */
					if((notify == NDEV_NOTIFY_REGISTERED) && (NDEV_NETSTAT_REGISTERED == __get_conn_stat()))  
					{
						__set_conn_stat(NDEV_NETSTAT_SESSION);

						memset(&info, 0, sizeof(ndev_info_t));
						ndev_info_get(&info);
						info.stat.center = 1; //与中心通讯正常
						info.register_time = time(NULL); //保存注册时间
						ndev_info_put(&info);
						
						fprintf(stderr, "ndev register success\n");
						return B_DEV_EOK;
					}
				}
			}
			else
			{
				fprintf(stderr, "ndev tcp client connect sockfd failed! sleep %d sec", __NDEV_REGISTER_SOCKET_SLEEP);

				SYS_LOG_DEBUG("ndev protocol register packet failed");
					
				lib_sleep(__NDEV_REGISTER_SOCKET_SLEEP); //防止线程死循环	
				continue;
			}
		}
		else  //连接失败
		{
			try_times++;
			
			fprintf(stderr, "ndev tcp client try connect to server fail times: %d\n", try_times);

			SYS_LOG_DEBUG("ndev tcp client try connect to server failed");
			
			if(try_times >= CONFS_NDEV_TRY_REG_TIMES)
			{
				try_times = 0;
				return B_DEV_ETIMEOUT;
			}

			lib_sleep(CONFS_NDEV_WAIT_REG_TIMES);
		}
	}
}

/*
 * 负载均衡-注册线程
 */
static void *__ndev_reg_thread(void *arg)
{
	int err = B_DEV_ERROR;
	unsigned int notify = NDEV_NOTIFY_INIT;	
	ndev_status_t nstat;
	unsigned char txbuf[512] = {0};
	
	while(1)
	{
		err = __load_balance(); //负载均衡请求
		if(err == B_DEV_EOK)
		{
			err = __ndev_register(); //设备注册
			if(err == B_DEV_ETIMEOUT) //返回B_DEV_ETIMEOUT: 说明尝试注册N次都失败
				continue;

			if(__get_conn_stat() >= NDEV_NETSTAT_SESSION)
			{
				/*
				 *  注册成功后,广播通知等待的发送线程,发送3 次
				 */
				__netstat_notify_broadcast_put(NDEV_NOTIFY_REGISTERED);
				lib_msleep(100);
				__netstat_notify_broadcast_put(NDEV_NOTIFY_REGISTERED);
				lib_msleep(100);
				__netstat_notify_broadcast_put(NDEV_NOTIFY_REGISTERED);

				/* 把节点机信息发送给锁桩 */
				memset(&nstat, 0, sizeof(ndev_status_t));
				ndev_info_stat_get(&nstat);
				nstat.center = 1;  //与中心连通
				nstat.wireless = 1;  //无线信息
				ndev_info_stat_put(&nstat);
				err = sae_protocol_ndev_stat(txbuf, SAE_CAN_BROADCAST_ID);
				if(err > 0)
					stake_ctos_put(SAE_CAN_BROADCAST_ID, txbuf, err);
					//stake_ctos_put_priority(SAE_CAN_BROADCAST_ID, txbuf, err, CAN_PRIORITY_LOW);

				__register_notify_wait(&notify);  //注册成功后,线程休眠
				if((notify == NDEV_NOTIFY_RE_REGISTER) || (NDEV_NETSTAT_INIT == __get_conn_stat()))
				{
					SYS_LOG_NOTICE("%s:register notify %d, netstat %d", __FUNCTION__, notify, __get_conn_stat());
		
					__set_conn_stat(NDEV_NETSTAT_INIT);
					lib_close(__get_sockfd());
					__set_sockfd(0);

					/* 把节点机信息发送给锁桩 */
					memset(&nstat, 0, sizeof(ndev_status_t));
					memset(txbuf, 0, sizeof(txbuf));
					ndev_info_stat_get(&nstat);
					nstat.center = 0;  //与中心断开

					continue;  //重新连接负载均衡服务器，以及重新注册
				}
			}
		}
		else
			lib_sleep(15); //防止死循环
	}

	return lib_thread_exit((void*)NULL);
}

/*
 * 心跳处理线程
 */
#define BH_LOOP_1            	(1)
#define BH_LOOP_2				(2)
#define BH_LOOP_3              	(3)
#define BH_LOOP_4              	(4)

struct bh_boop
{
	int loop1;
	int loop2;
	int loop3;
	int loop4;
};

static void *__ndev_bheart_thread(void *arg)
{
	int err = B_DEV_ERROR;
	int n, i = 0, esc_len;
	int sockfd;
	int bh_loop_1_sw, bh_loop_2_sw, bh_loop_3_sw, bh_loop_4_sw;
	unsigned int c_stat = NDEV_NETSTAT_INIT;
	unsigned int notify = NDEV_NOTIFY_INIT;	
	unsigned char buf[1024] = {0};
	unsigned char esc_buf[1024] = {0};
	device_config_t conf;
	ndev_bheart_limit_t limit;
	int f_bh = 1;
	struct bh_boop g_bh_loop;
	static int reboot_times = 0;  
	 
	memset(&g_bh_loop, 0, sizeof(struct bh_boop));
	
	  
	while(1)
	{
		c_stat = __get_conn_stat();
		if(c_stat <  NDEV_NETSTAT_SESSION)  //如果没有注册成功，休眠
		{
			//__btheart_notify_wait(&notify);

			SYS_LOG_NOTICE("%s: netstat  notify wait\n", __FUNCTION__);
			
			__netstat_notify_wait(&notify);
		}
		
		sockfd = __get_sockfd();
		if(sockfd > 0)
		{
			memset(&limit, 0, sizeof(ndev_bheart_limit_t));

			limit.ndev_stat = 1;
			limit.ndev_time = 1;
			limit.sae_quantity = 1;
			limit.sae_online = 1;
			limit.total_bl_ver = 1;
			limit.inc_bl_ver = 1;
			limit.dec_bl_ver = 1;
			limit.sae_attr = 1;  //桩机属性
			limit.all_sae_stat = 1;  //全部桩机状态
			limit.sae_online_mark = 1;
			
			/* 心跳发送策略 */
			g_bh_loop.loop1++;
			g_bh_loop.loop2++;
			g_bh_loop.loop3++;
			g_bh_loop.loop4++;

			//printf("------g_bh_loop, loop1:%d, loop2:%d, loop3:%d, loop4:%d\n", \
			//	g_bh_loop.loop1, g_bh_loop.loop2, g_bh_loop.loop3, g_bh_loop.loop4);
			if(g_bh_loop.loop1 >=  BH_LOOP_1)  
			{
				limit.ndev_stat = 1; //节点机状态
				limit.ndev_time = 1;  //节点机时间
				limit.sae_quantity = 1; //桩机总数量
				limit.sae_online = 1; //桩机在线数量
				limit.lnt_card_stat = 1; //羊城通读卡器状态
				limit.all_sae_stat = 1; //全部桩机状态
				limit.sae_online_mark = 1; //桩机在线标志
				limit.sae_attr = 1; //桩机属性
   
				if(f_bh == 1) //第一个心跳包
				{
					limit.total_bl_ver = 1;  //总量黑名单版本
					limit.inc_bl_ver = 1;  //增量黑名单版本
					limit.dec_bl_ver = 1;  //减量黑名单版本
					limit.temp_bl_ver = 1;  //临时黑名单版本
					limit.sae_bl_attr = 1; //桩机黑名单属性		
					
					limit.ndev_fw_ver = 1;  //节点机库版本
					limit.ndev_app_ver = 1;  //节点机应用版本
					limit.sae_fw_ver = 1; //桩机固件版本
					limit.sae_para_ver = 1; //桩机参数版本

					#if CONFS_USING_UPLOAD_READER_VER
					limit.lnt_reader_firmware_ver = 1; //岭南通读卡器固件版本
					#endif
					
					f_bh = 0;
				}
				
				g_bh_loop.loop1  = 0;
			}

			if(g_bh_loop.loop2 >=  BH_LOOP_2)
			{
				g_bh_loop.loop2  = 0;
			}

			if(g_bh_loop.loop3 >= BH_LOOP_3)
			{
				limit.total_bl_ver = 1;  //总量黑名单版本
				limit.inc_bl_ver = 1;  //增量黑名单版本
				limit.dec_bl_ver = 1;  //减量黑名单版本
				limit.temp_bl_ver = 1;  //临时黑名单版本
				limit.sae_bl_attr = 1; //桩机黑名单属性
				
				g_bh_loop.loop3  = 0;
			}

			if(g_bh_loop.loop4 >= BH_LOOP_4)
			{
				limit.ndev_fw_ver = 1;  //节点机库版本
				limit.ndev_app_ver = 1;  //节点机应用版本
				limit.sae_fw_ver = 1; //桩机固件版本

				#if CONFS_USING_UPLOAD_READER_VER
				limit.lnt_reader_firmware_ver = 1; //岭南通读卡器固件版本
				#endif
				
				g_bh_loop.loop4  = 0;
			}

			
			/* FTP升级信息确认 */
			struct ftp_download_ctrl_info fdnl_ctrl_info;
			memset(&fdnl_ctrl_info, 0, sizeof( struct ftp_download_ctrl_info));
			err = lib_upgrade_get_ftp_dnl_ctrl_info(g_upgrade, &fdnl_ctrl_info);
			if(err == LIB_UPE_EOK)
			{
				memcpy(&g_ftp_dnl_ctrl_info, &fdnl_ctrl_info, sizeof(g_ftp_dnl_ctrl_info));

				if(g_ftp_dnl_ctrl_info.isvaild == 1) //有效
				{
					limit.ndev_ftp_stat = 1;  //节点机FTP下载状态
				}
			}
			   
			n = ndev_protocol_bheart_level1_req(buf, &limit);
			if(n > 0)
			{
				esc_len = __uplink_escape(esc_buf, buf, n); //上行转义	

				//fprintf(stderr, "++++++++++++++++++bheart length %d\n", esc_len);
				
				while(1)
				{
					//lib_printf_v2("__ndev_bheart_thread:", esc_buf, esc_len, 16);
					
					i++;
					n = lib_tcp_writen(sockfd, esc_buf, esc_len);
					//printf("-----------------bheart send size:%d\n", n);
					if(n > 0)
					{
						i = 0; //发送成功之后发送次数清零 add by zjc at 2016-12-23
						break;
					}

					if(i >= 3) //发送3次失败,重新注册;防止3G重拨后,套接口失效情况,快速响应
					{
						i = 0;
						__set_bheart_cnt(0);
						__set_conn_stat(NDEV_NETSTAT_INIT);
						__register_notify_put(NDEV_NOTIFY_RE_REGISTER); 	//通知注册线程

						SYS_LOG_DEBUG("%s: bheart tcp write error\n", __FUNCTION__);
						
						break;
					}

					lib_sleep(1);
				}
			}
		}
		else
			lib_msleep(500);
		
		err = __btheart_notify_timedwait(CONFS_NDEV_BTHEART_WAITTIME * 1000, &notify); //等待心跳回应
		if(err == B_DEV_ETIMEOUT) //超时
		{
			fprintf(stderr, "ndev heartbeat recv timeout!!!\n");

			SYS_LOG_NOTICE("%s:ndev heartbeat recv timeout, wait time: %d sec", __FUNCTION__, CONFS_NDEV_BTHEART_WAITTIME);

			__set_bheart_inc();
		}
		else if(notify == NDEV_NOTIFY_BTHEART) //心跳正常返回
		{
			__set_bheart_cnt(0);
		}

		n = __get_bheart_cnt();  //读取心跳发送次数
		if(n >= CONFS_NDEV_TRY_BTHEART_TIMES) //重新注册
		{
			__set_bheart_cnt(0);
			__set_conn_stat(NDEV_NETSTAT_INIT);
			__register_notify_put(NDEV_NOTIFY_RE_REGISTER);

			fprintf(stderr, "ndev heartbeat timeout, re-register!!!");

			SYS_LOG_NOTICE("%s:ndev heartbeat timeout, re-register", __FUNCTION__);

			continue;
		}

		device_config_get(&conf);
		
		fprintf(stderr, "ndev heartbeat interval time:%d, count:%d\n", conf.nconf.heart_time, n);

		//SYS_LOG_NOTICE("%s:ndev heartbeat interval time:%d, count:%d\n", __FUNCTION__, conf.nconf.heart_time, n);

		
		lib_sleep(conf.nconf.heart_time);	
	}
	
	return lib_thread_exit((void*)NULL);
}

/*
 * 接收网络数据线程
 */
static void *__ndev_recv_thread(void *arg)
{
	int err = -1;
	int n = -1;
	unsigned char rxbuf[B_DEV_RXBUF_SZ] = {0};
	int sockfd;
	unsigned int cstat;

	while(1)
	{
		cstat = __get_conn_stat();
		if((cstat > NDEV_NETSTAT_INIT) && (cstat < NDEV_NETSTAT_UNKNOWN))
		{
			memset(rxbuf, 0, B_DEV_RXBUF_SZ);
			sockfd = __get_sockfd();
			if(sockfd > 0)
			{
				n = lib_tcp_read_select(sockfd, rxbuf, sizeof(rxbuf), 5 * 1000);
				//printf("---------------network read size:%d\n", n);
				if(n > 0)
				{
					do{ 
						err = lib_ringbuffer_put(g_ndev_rb, rxbuf, n);
						if(err <= 0)
							lib_msleep(500);
					}while(!err);
				}	
			}
			else
				lib_msleep(500);
		}
		else
			lib_sleep(2);
	}

	return lib_thread_exit((void*)NULL);
}

/*
 * 数据解释线程
 */
static void *__ndev_explain_thread(void *arg)
{
	int err = -1;
	unsigned char buf[B_DEV_RXBUF_SZ] = {0};
	ndev_rx_buf_t rxbuf;
	unsigned int data_len = 0;
	unsigned char ch = 0;
	int esc_len = 0;
	int idx = 0;
	int s_pos = 0;
	
	while(1)
	{		
		ch = 0;
		err = lib_ringbuffer_getchar(g_ndev_rb, &ch);
		if(err > 0)
		{
			switch(ch)
			{
				case NDEV_HD: //找到头部
				{
					idx = 0;
					memset(&buf, 0, sizeof(buf));
					memset(&rxbuf, 0, sizeof(ndev_rx_buf_t));
					
					buf[idx++] = ch;
					s_pos = 1; //找到头部标记
					
					continue;
				}
				break;

				case NDEV_TAIL:  //找到尾部
				{
					buf[idx++] = ch;
					if(s_pos == 1) //完整一帧数据
					{
						esc_len = __downlink_escape((unsigned char *)&(rxbuf.s_un.data), buf, idx);
						if(esc_len > 0)
						{
							data_len = ntohs(N_RX_LEN(rxbuf));
							if(data_len > NDEV_FRAME_MAX_SZ) //非法长度 
							{
								idx = 0;
								s_pos = 0;
								memset(&buf, 0, sizeof(buf));
								memset(&rxbuf, 0, sizeof(ndev_rx_buf_t));	
								continue;
							}
							
							__ndev_protocol_explain(&rxbuf, esc_len); //指令解释
						}

						idx = 0;
						s_pos = 0;
						memset(&buf, 0, sizeof(buf));
						memset(&rxbuf, 0, sizeof(ndev_rx_buf_t));
					}					
				}
				break;

				default: //既不是帧头也不是帧尾
				{
					buf[idx++] = ch;
					
					if(idx >= NDEV_DATA_SZ) //超过限定长度值,也没有收到尾部，直接掉数据
					{
						idx = 0;
						s_pos = 0;
						memset(&buf, 0, sizeof(buf));
						memset(&rxbuf, 0, sizeof(ndev_rx_buf_t));
						continue;
					}					
				}
			}
		}
		else
			lib_sleep(2);
	}

	return lib_thread_exit((void*)NULL);
}

static void __unity_file_printf(unity_file_hd_t *new_hd,  unity_file_hd_t *old_hd)
{
	if((new_hd == NULL ) || (old_hd == NULL))
		return;

	fprintf(stderr, "\n\n\n----------------------------------Unity File Printf Begin---------------------------------\n");

	fprintf(stderr, "请求->文件序号:%02d\n", ntohs(new_hd->file_seq));
	fprintf(stderr, "请求->分割总数:%02d\n", ntohs(new_hd->total));
	fprintf(stderr, "请求->分割序列:%02d\n", ntohs(new_hd->div_seq));
	fprintf(stderr, "请求->泛文件类别:0x%02X\n", new_hd->ufc);
	fprintf(stderr, "请求->特征定义:");
	lib_printf("", new_hd->attr, 9);

	fprintf(stderr, "旧->文件序列:%02d\n", ntohs(old_hd->file_seq));

	if(new_hd->file_seq == old_hd->file_seq)  
	{
		fprintf(stderr, "失败原因:请求的泛文件版本与已有泛文件版本相同,因此不执行升级\n");
	}

	fprintf(stderr, "\n----------------------------------Unity File Printf End---------------------------------\n\n\n");
}

/*
 * 泛文件管理线程
 */
static void *__ndev_unity_file_mng_thread(void *arg)
{
	int err = LIB_GE_ERROR;
	int cmp = 0;
	enum UPE_FIRMWARE_TYPE ftype;  
	unsigned char txbuf[1024] = {0};
	ftp_download_ctrl_info_t fdnl_ctrl_info;
	
	while(1)
	{
		async_mutex_queue_data_t *async_mqd = (async_mutex_queue_data_t *)lib_async_mutex_queue_get(g_async_mutex_queue);
		if(async_mqd != NULL)
		{
			//fprintf(stderr, "async_mqd->qid=%d\n", async_mqd->qid);
			
			switch(async_mqd->qid)
			{
				case NDEV_ASYNC_MUTEX_QDATA_UNIV:  //泛文件处理
				{
					SYS_LOG_INFO("%s: NDEV_ASYNC_MUTEX_QDATA_UNIV", __FUNCTION__);

					unity_file_hd_t last_hd;
					memset(&last_hd, 0, sizeof(unity_file_hd_t));
						
					unity_file_hd_t *t_hd = (unity_file_hd_t *)async_mqd->qptr;
					if(t_hd == NULL)
						break;

					unity_file_hd_get(&last_hd, t_hd->ufc);  //读取正在使用的泛文件版本 (拷贝泛文件头)

					__unity_file_printf(t_hd, &last_hd);  //打印输出 

					if(last_hd.file_seq == t_hd->file_seq)    //文件序列一样的请求，不处理  [如果请求的泛文件和正在下载的泛文件"文件序列"一样，就不申请下载]
					{
						SYS_LOG_INFO("%s: last_hd.file_seq(%d) == t_hd->file_seq(%d)", __FUNCTION__, ntohs(last_hd.file_seq), ntohs(t_hd->file_seq));
						fprintf(stderr, "%s: last_hd.file_seq(%d) == t_hd->file_seq(%d)\n", __FUNCTION__, ntohs(last_hd.file_seq), ntohs(t_hd->file_seq));

						lib_zfree(t_hd);		
						lib_msleep(200);
						break;
					}				
					else if(last_hd.file_seq != t_hd->file_seq) //如果泛文件不一样
					{
						SYS_LOG_INFO("%s: last_hd.file_seq(%d) != t_hd->file_seq(%d)", __FUNCTION__, ntohs(last_hd.file_seq), ntohs(t_hd->file_seq));
						fprintf(stderr, "%s: last_hd.file_seq(%d) != t_hd->file_seq(%d)\n", __FUNCTION__, ntohs(last_hd.file_seq), ntohs(t_hd->file_seq));

						#if 0
						cmp = univ_file_version_compare(&(t_hd->attr[0]), &(last_hd.attr[0])); //版本比较
						
						SYS_LOG_INFO("%s: file_seq:%d, total:%d, div_seq:%d, ufc:0x%02x, cmp: %d", 
										__FUNCTION__, ntohs(t_hd->file_seq), ntohs(t_hd->total), ntohs(t_hd->div_seq), t_hd->ufc, cmp);

						fprintf(stderr, "%s: file_seq:%d, total:%d, div_seq:%d, ufc:0x%02x, cmp: %d\n", 
										__FUNCTION__, ntohs(t_hd->file_seq), ntohs(t_hd->total), ntohs(t_hd->div_seq), t_hd->ufc, cmp);


						if(cmp == 1) //如果新的泛文件比旧的泛文件新,通知正在发送的请求的线程退出
						{
							unity_file_notify_put(NDEV_NOTIFY_UNIV_QUIT, t_hd, t_hd->ufc);   
							lib_sleep(1);
							unity_file_notify_put(NDEV_NOTIFY_UNIV_QUIT, t_hd, t_hd->ufc);   
							lib_sleep(1);
						}
						else 
						{
							lib_zfree(t_hd);
							lib_msleep(200);
							break;
						}
						#endif


						unity_file_notify_put(NDEV_NOTIFY_UNIV_QUIT, t_hd, t_hd->ufc); //?   
						lib_sleep(1);
						unity_file_notify_put(NDEV_NOTIFY_UNIV_QUIT, t_hd, t_hd->ufc);   
						lib_sleep(1);
					}

					struct unity_file_struct file_struct;
					file_struct.total = t_hd->total;
					file_struct.file_seq = t_hd->file_seq;
					file_struct.div_seq = t_hd->div_seq;
					file_struct.status = 1;
					time(&(file_struct.start_time));
				
					unity_file_info_struct_ndev_put(&file_struct, t_hd->ufc); 
					unity_file_request_handle(t_hd); //新版本泛文件处理程序
				}
				break;

				case NDEV_ASYNC_MUTEX_QDATA_FTP_DNL: //FTP固件下载
				{
					SYS_LOG_INFO("%s: NDEV_ASYNC_MUTEX_QDATA_FTP_DNL", __FUNCTION__);
				
					ndev_ftp_download_ctrl_req_t *ftp_dnl_req  = (ndev_ftp_download_ctrl_req_t *)async_mqd->qptr;
					if(ftp_dnl_req == NULL)
					{
						SYS_LOG_ERR("%s: ftp_dnl_req is NULL!\n", __FUNCTION__);
						break;
					}

					struct ftp_config ftp_conf;
					device_config_t dev_conf;
					struct ftp_upgrade_status fstatus;
					
					memset(&ftp_conf, 0, sizeof(struct ftp_config ));
					memset(&fstatus, 0, sizeof(struct ftp_upgrade_status));
					
					device_config_get(&dev_conf); //获取FTP配置信息

					ftype = __firmware_find(ftp_dnl_req->fw_name); //固件类型

					fprintf(stderr, "upsn=%d\n", ntohs(ftp_dnl_req->upsn));
					fprintf(stderr, "fw_name=%s\n", ftp_dnl_req->fw_name);
					fprintf(stderr, "ftype=%d\n", ftype);

					/* 检查是否有文件正在下载,如果有,则不执行下载 */
					memset(&fdnl_ctrl_info, 0, sizeof(ftp_download_ctrl_info_t));
					err = lib_upgrade_get_upstat(g_upgrade, ftype, &fstatus); //获取升级状态
					if(err != LIB_UPE_EOK)
					{
						SYS_LOG_DEBUG("lib_upgrade_get_upstat and break!");
						lib_zfree(ftp_dnl_req);
						break;
					}

					err = lib_upgrade_get_ftp_dnl_ctrl_info(g_upgrade, &fdnl_ctrl_info); //获取FTP下载控制信息
					if(err != LIB_UPE_EOK)
					{
						SYS_LOG_DEBUG("lib_upgrade_get_ftp_dnl_ctrl_info fail and break!");
						lib_zfree(ftp_dnl_req);
						break;
					}
					
#if 0
					if(fdnl_ctrl_info.upsn == ftp_dnl_req->upsn)  //请求报文序列一样,不执行FTP下载操作
					{
						SYS_LOG_NOTICE("fdnl_ctrl_info.upsn == ftp_dnl_req->upsn\n");
						fprintf(stderr, "fdnl_ctrl_info.upsn == ftp_dnl_req->upsn\n");
						lib_zfree(ftp_dnl_req);
						break;
					}
#endif
					printf("--------------download_status:0x%02x\n", fstatus.download_status);
					//if(fstatus.download_status == 0x01) //文件正在下载,不执行FTP下载操作
					if((fstatus.download_status == 0x01) || (fstatus.download_status == 0x03)) //文件正在下载或者下载完成,不执行FTP下载操作 2017-01-09
					{
						SYS_LOG_NOTICE( "fstatus.download_status = 0x%02x, Stop Download!", fstatus.download_status);
						fprintf(stderr, "--------------fstatus.download_status = 0x%02x, Stop Download!\n", fstatus.download_status);

						struct ndev_ftp_download_ctrl_ack fdnl_ctrl1;
						memcpy(&fdnl_ctrl1, &(fdnl_ctrl_info.upsn), sizeof(struct ndev_ftp_download_ctrl_ack));

						err = ndev_protocol_ftp_download_ctrl_ack(txbuf, &fdnl_ctrl1, sizeof(struct ndev_ftp_download_ctrl_ack), async_mqd->sn);
						if(err == B_DEV_ERROR)
						{
							lib_zfree(ftp_dnl_req);
							break;
						}

						lib_printf_v2("FTP DNL CTRL SEND[1]", txbuf, err, 16);
					
						ndev_tcp_send_to_server_with_escape(txbuf, err, 1000);  //对中心应答

						break; //退出下载流程
					}
					
					switch(ftype) //类型转换
					{
						case UPE_FIRMWARE_TYPE_KERNEL:
							ftp_conf.update_type = UPE_TYPE_KERNEL;
							break;

						case UPE_FIRMWARE_TYPE_FIRMWARE:
							ftp_conf.update_type = UPE_TYPE_FIRMWARE;
							break;

						case UPE_FIRMWARE_TYPE_APPL:
							ftp_conf.update_type = UPE_TYPE_APPL;
							break;

						case UPE_FIRMWARE_TYPE_LNT_ZM:
							ftp_conf.update_type = UPE_TYPE_LNT_ZM;
							break;
					}
					  
					strcpy(ftp_conf.username, dev_conf.nconf.ftp_username);
					strcpy(ftp_conf.passwd,  dev_conf.nconf.ftp_password);
					ftp_conf.port = dev_conf.nconf.ftp_port;
					ftp_conf.connect_timeout = 5;  //FTP连接超时时间 //3->5 2016-11-28
					ftp_conf.download_timeout = CONFS_FTP_DOWNLOAD_TIMEOUT; //FTP下载超时时间 //180-->300 by zjc at 2016-10-11
					strcpy(ftp_conf.firmware_name, ftp_dnl_req->fw_name);  //下载的固件名称

					#if CONFS_USING_FTP_DOWNLOAD_VERSION_CHECK
					struct ndev_config config;
					memset(&config, 0, sizeof(struct ndev_config));

					gui_get_ndev_device_config(&config); //获取本地软件版本

					printf("--------------Download ver:%s, Local FW ver:%s, Local APP ver:%s\n", ftp_conf.firmware_name, config.fw_ver, config.appl_ver);

					/* 增加下载前的版本判断，版本不一致才下载 2017-01-09 */
					if(strncmp((char *)ftp_conf.firmware_name, "fw", strlen("fw")) == 0) //fw_*
					{
						//printf("---------------fw ver:%s\n", &ftp_conf.firmware_name[strlen("fw_")]);
						if(strncmp((char *)&ftp_conf.firmware_name[strlen("fw_")], config.fw_ver, strlen(config.fw_ver)) == 0)
						{
							SYS_LOG_NOTICE("Download ver:%s == Local ver:%s, Stop Download!\n", ftp_conf.firmware_name, config.fw_ver);
							printf("Download ver:%s == Local ver:%s, Stop Download!\n", ftp_conf.firmware_name, config.fw_ver);
							break;
						}
					}
					else if(strncmp((char *)ftp_conf.firmware_name, "app", strlen("app")) == 0) //app_*
					{
						//printf("---------------app ver:%s\n", &ftp_conf.firmware_name[strlen("app_")]);
						if(strncmp((char *)&ftp_conf.firmware_name[strlen("app_")], config.appl_ver, strlen(config.appl_ver)) == 0)
						{
							SYS_LOG_NOTICE("Download ver:%s == Local ver:%s, Stop Download!\n", ftp_conf.firmware_name, config.appl_ver);
							printf("Download ver:%s == Local ver:%s, Stop Download!\n", ftp_conf.firmware_name, config.appl_ver);
							break;
						}
					}
					#endif

					char *s_ftp_ip = lib_inet_ntoa(dev_conf.nconf.ftp_ip); //FTP IP
					sprintf(ftp_conf.remote_path, "ftp://%s:%d/%s", s_ftp_ip, dev_conf.nconf.ftp_port, ftp_dnl_req->fw_name);
					sprintf(ftp_conf.local_path, "/opt/ftppath/%s", ftp_dnl_req->fw_name);

					fprintf(stderr, "remote_path:%s\n", ftp_conf.remote_path);
					fprintf(stderr, "local_path:%s\n", ftp_conf.local_path);

					SYS_LOG_NOTICE("remote_path:%s\n", ftp_conf.remote_path);
  
					err = lib_upgrade_set_ftp_config(g_upgrade, &ftp_conf);  //设置FTP
					if(err != LIB_UPE_EOK)
					{
						fprintf(stderr, "upgrade set ftp config failed!\n");
						lib_zfree(ftp_dnl_req);
						break;
					}

					err = lib_upgrade_set_ftp_download_switch(g_upgrade, UPE_FTP_DOWNLOAD_SW_ON); //FTP下载开启
					if(err != LIB_UPE_EOK)
					{
						fprintf(stderr, "upgrade set ftp download switch failed!\n");
						lib_zfree(ftp_dnl_req);
						break;
					}

					lib_sleep(3);  //休眠 

					/* FTP固件下载应答 */
					memset(&g_ftp_dnl_ctrl_info, 0, sizeof(ndev_ftp_download_ctrl_info_t));

					time(&(g_ftp_dnl_ctrl_info.ftime)); //时间
					g_ftp_dnl_ctrl_info.isvaild = 1; //有效
					g_ftp_dnl_ctrl_info.ftype = ftype;  //固件类型
					
					g_ftp_dnl_ctrl_info.fdnl_ctrl.upsn = ftp_dnl_req->upsn;  //升级序列号
					g_ftp_dnl_ctrl_info.fdnl_ctrl.supplier = ftp_dnl_req->supplier;  //供应商编码
					lib_get_systime_bcd(&(g_ftp_dnl_ctrl_info.fdnl_ctrl.time)); //时间

					
					/* 设置FTP下载控制信息 */
					memcpy(&fdnl_ctrl_info, &g_ftp_dnl_ctrl_info, sizeof(ftp_download_ctrl_info_t));
					err = lib_upgrade_set_ftp_dnl_ctrl_info(g_upgrade, &fdnl_ctrl_info);
					if(err != LIB_UPE_EOK)  
					{
						fprintf(stderr, "upgrade set ftp dnl ctrl info failed!\n");
						SYS_LOG_ERR("upgrade set ftp dnl ctrl info failed!\n");
					}

					err = lib_upgrade_get_upstat(g_upgrade, ftype, &fstatus); //FTP状态
					if(err == LIB_UPE_EOK)
					{
						fprintf(stderr, "[2]download_status:%02x\n", fstatus.download_status);
						fprintf(stderr, "[2]ftp_recode:%d\n", fstatus.ftp_recode);
						fprintf(stderr, "[2]ftp_errno:%d\n", fstatus.ftp_errno);

						fdnl_ctrl_info.download_status = fstatus.download_status;
						fdnl_ctrl_info.ftpcode = fstatus.ftp_recode;
						fdnl_ctrl_info.err = fstatus.ftp_errno;

						struct ndev_ftp_download_ctrl_ack fdnl_ctrl2;
						memcpy(&fdnl_ctrl2, &(fdnl_ctrl_info.upsn), sizeof(fdnl_ctrl2));

						lib_printf_v2("***********************************FDNL CTRL 2", &fdnl_ctrl2, sizeof(fdnl_ctrl2), 16);
						
						err = ndev_protocol_ftp_download_ctrl_ack(txbuf, &fdnl_ctrl2, sizeof(ndev_ftp_download_ctrl_ack_t), async_mqd->sn);
						if(err == B_DEV_ERROR)
						{
							lib_zfree(ftp_dnl_req);
							break;
						}

						lib_printf_v2("FTP DNL CTRL SEND[2]", txbuf, err, 16);
						
						ndev_tcp_send_to_server_with_escape(txbuf, err, 1000);
					}
					
					
					lib_zfree(ftp_dnl_req);
				}
				break;

				case NDEV_ASYNC_MUTEX_QDATA_FW_CTRL: //固件升级控制
				{
					SYS_LOG_INFO("%s: NDEV_ASYNC_MUTEX_QDATA_FW_CTRL", __FUNCTION__);
				
					ndev_fw_update_ctrl_req_t *fw_upctrl_req  = async_mqd->qptr;
					if(fw_upctrl_req == NULL)
					{
						SYS_LOG_ERR("%s: fw_upctrl_req is NULL!\n", __FUNCTION__);
						break;
					}

					ftype =  g_ftp_dnl_ctrl_info.ftype; //固件类型

					#if 1
					struct firmware_config firmware;
					err = lib_upgrade_get_firmware_config(g_upgrade, ftype, &firmware);  //查看信息
					if(err == LIB_UPE_EOK)
					{
						//fprintf(stderr, "ftp_username:%s\n", firmware.ftp_username);
						//fprintf(stderr, "ftp_passwd:%s\n", firmware.ftp_passwd);
						fprintf(stderr, "ftp_remote_path:%s\n", firmware.ftp_remote_path);
						fprintf(stderr, "ftp_local_path:%s\n", firmware.ftp_local_path);
						fprintf(stderr, "ftp_connect_timeout:%d\n", firmware.ftp_connect_timeout);
						fprintf(stderr, "ftp_download_timeout:%d\n", firmware.ftp_download_timeout);

						fprintf(stderr, "firmware_name:%s\n", firmware.firmware_name);
						fprintf(stderr, "total_len:%d\n", firmware.total_len);
						fprintf(stderr, "download_len:%d\n", firmware.download_len);

						lib_printf("last_datetime:", firmware.last_datetime, 7);
						lib_printf("md5:", firmware.md5, 16);

						fprintf(stderr, "is_download_complete:%d\n", firmware.is_download_complete);
						fprintf(stderr, "is_write_parameter:%d\n", firmware.is_write_parameter);
						fprintf(stderr, "is_write_flash:%d\n", firmware.is_write_flash);
					}
					#endif

					
					switch(fw_upctrl_req->upop)
					{
						case 0x01: //执行升级操作
						{
							fprintf(stderr, "upgrade start.....\n");
							
							unsigned char update_type = UPE_TYPE_INIT;
							
							if((firmware.is_download_complete == UP_DNL_STAT_COMPLETE) &&
								(firmware.is_write_flash == UP_WR_STAT_WR))
							{
								switch(ftype)
								{
									case UPE_FIRMWARE_TYPE_KERNEL:
										update_type = UPE_TYPE_KERNEL;
										break;

									case UPE_FIRMWARE_TYPE_FIRMWARE:
										update_type = UPE_TYPE_FIRMWARE;
										break;

									case UPE_FIRMWARE_TYPE_APPL:
										update_type = UPE_TYPE_APPL;
										break;
									case UPE_FIRMWARE_TYPE_LNT_ZM:
										update_type = UPE_TYPE_LNT_ZM;
										break;
								}
							 
								err = lib_upgrade_set_upgrade_start(g_upgrade, update_type);  //升级开启
								if(err == LIB_UPE_EOK)
								{
									fprintf(stderr, "upgrade type [%d] success\n", update_type);

									g_ftp_dnl_ctrl_info.isvaild = 0; //无效
									memset(&g_ftp_dnl_ctrl_info, 0, sizeof(g_ftp_dnl_ctrl_info));//清空
									   
									memcpy(&fdnl_ctrl_info, &g_ftp_dnl_ctrl_info, sizeof(ftp_download_ctrl_info_t));
									lib_upgrade_set_ftp_dnl_ctrl_info(g_upgrade, &fdnl_ctrl_info);
								}
								
							}
							else
							{
								fprintf(stderr, "upgrade type [%d]  failed!\n", update_type);
							}
						}
						break;
					}
					

					lib_zfree(fw_upctrl_req);
				}
				break;
			}
		}
		else
		{
			SYS_LOG_ERR("%s: async_mqd is NULL!\n", __FUNCTION__);
			lib_sleep(1); 
		}
		
		if(async_mqd != NULL)  //释放内存
			lib_zfree(async_mqd);
	}
	
	return lib_thread_exit((void*)NULL); 
}

static int __ndev_protocol_explain(ndev_rx_buf_t *rxbuf, const unsigned int len)
{
	lib_printf_v2("\n------------------------tcp recv----------------------", rxbuf->s_un.data, len, 16);

	int ret;
	unsigned char txbuf[B_DEV_RXBUF_SZ] = {0};
	n_data_attr_t attr;
	unsigned short s_len;
	unsigned char dev_attr;	
	char *p = NULL;
	unsigned char cmd = N_RX_CMD_PTR(rxbuf);
	if(!n_rx_buf_crc_check(rxbuf))  //CRC检验检测
		return B_DEV_ERROR;

	switch(cmd)
	{
		case NDEV_REGISTER_ACK: //注册 (设备签到回应)
		{
			__ndev_word_analysis(rxbuf, len);
		}
		break;

		case NDEV_CTRL_REQ: //控制请求
		{
			__ndev_word_analysis(rxbuf, len);
		}
		break;

		case NDEV_CTRL_ACK: //控制返回
		{
			/* 增加独立为租还车信息查询解释函数 */
			unsigned char s_key[8] = {0};
			memcpy(s_key, rxbuf->s_un.st.data, 2);
			s_key[2]  = '\0';

			if(strncmp(s_key, NDEV_RENT_INFO_W, 2) == 0)
			{
				fn_ndev_rent_info_explain(&(rxbuf->s_un.st.data[2]), ntohs(rxbuf->s_un.st.len) - 3); //-3:引导字和分隔符&

				break;
			}

			/* 增加独立为附近网点信息查询解释函数 add by zjc at 2016-11-03 */
			if(strncmp(s_key, NDEV_NEARBY_SITE_INFO_W, 2) == 0)
			{
				fn_ndev_nearby_site_info_explain(&(rxbuf->s_un.st.data[2]), ntohs(rxbuf->s_un.st.len) - 3); //-3:引导字和分隔符&

				break;
			}
			/* end of 增加独立为附近网点信息查询解释函数 */
			
			__ndev_word_analysis(rxbuf, len);
		}
		break;

		case NDEV_FILE_TRANS_ACK: //泛文件
		{
			unity_file_handle_to_ndev(rxbuf, len);
		}
		break;

		case NDEV_PASS_REQ:  //全透传  请求
		{
			fprintf(stderr, "NDEV_PASS_REQ\n");
		
			memset(&attr, 0, sizeof(n_data_attr_t));
			s_len = ntohs(N_RX_LEN_PTR(rxbuf));
			dev_attr = N_RX_DATA_ATTR_PTR(rxbuf);
			memcpy(&attr, &dev_attr, sizeof(dev_attr));
			if(attr.pass == 1)	//透传数据
			{
				ret = sae_protocol_whole_pass(SAE_PASS_REQ, N_RX_DEV_ADDR_PTR(rxbuf), txbuf, N_RX_DATA_PTR(rxbuf), s_len);
				if(ret > 0)
				{
					lib_printf_v2("-----------------whole pass req-------------\n", txbuf, ret, 16);
					
					//stake_ctos_put(N_RX_DEV_ADDR_PTR(rxbuf), txbuf, ret);
					stake_ctos_put_priority(N_RX_DEV_ADDR_PTR(rxbuf), txbuf, ret, CAN_PRIORITY_HIGH);
				}
			}				
		}
		break;

		case NDEV_PASS_ACK:  //全透传  确认
		{
			fprintf(stderr, "NDEV_PASS_ACK\n");

			memset(&attr, 0, sizeof(n_data_attr_t));
			s_len = ntohs(N_RX_LEN_PTR(rxbuf));
			dev_attr = N_RX_DATA_ATTR_PTR(rxbuf);
			memcpy(&attr, &dev_attr, sizeof(dev_attr));
			if(attr.pass == 1)	//透传数据
			{
				ret = sae_protocol_whole_pass(SAE_PASS_ACK, N_RX_DEV_ADDR_PTR(rxbuf), txbuf, N_RX_DATA_PTR(rxbuf), s_len);
				if(ret > 0)
				{
					lib_printf_v2("-----------------whole pass ack-------------\n", txbuf, ret, 16);
					
					//stake_ctos_put(N_RX_DEV_ADDR_PTR(rxbuf), txbuf, ret);
					stake_ctos_put_priority(N_RX_DEV_ADDR_PTR(rxbuf), txbuf, ret, CAN_PRIORITY_HIGH);
				}
			}				
		}
		break;
		
		default:
			return B_DEV_ERROR;
	}

	return B_DEV_EOK;
}

static void __ndev_word_analysis(ndev_rx_buf_t *buf, const unsigned int len)
{
	int i;
	int cnt_a = 0, cnt_r = 0;
	unsigned char ack_f = 0;
	fn_ndev_val_t fn_ndev_val_a[32];   	
	fn_ndev_val_t fn_ndev_val_r[32];
	n_var_data_t *var = NULL;
	char *p = NULL;
	char *p1 = NULL;

	memset(&fn_ndev_val_a, 0, sizeof(fn_ndev_val_t) * 32);
	memset(&fn_ndev_val_r, 0, sizeof(fn_ndev_val_t) * 32);
	
	//fprintf(stderr, "\n------------------------------ __ndev_word_analysis --------------------------\n%s\n", N_RX_DATA_PTR(buf));

	p =  N_RX_DATA_PTR(buf);
	while(p1 = strchr(p, '&'))  //相邻指令间以'&'分隔
	{
		*p1 = '\0';	//每条指令尾置0，便于后面计算指令长度strlen(p)
		if(*(p+2) == '?')  //请求参数  //+2是跳过引导字
		{
			strncpy(fn_ndev_val_r[cnt_r].key, p, 2);  //引导字
			fn_ndev_val_r[cnt_r].sel = FN_SEL_TRUE; //是否为请求指令
			fn_ndev_val_r[cnt_r].d_size = 0; //数据长度  为什么不用buf中自带的数据长度(本指令的数据长度)
			fn_ndev_val_r[cnt_r].sn = N_RX_SN_PTR(buf); //流水号
			cnt_r++; //指令条数
		}
		else //应答指令
		{
			strncpy(fn_ndev_val_a[cnt_a].key, p, 2);
			fn_ndev_val_a[cnt_a].sel = FN_SEL_FLASE;
			p += 2; //跳过引导字
			strcpy(fn_ndev_val_a[cnt_a].data, p); //本指令的数据而非通信数据帧中的数据!
			fn_ndev_val_a[cnt_a].d_size = strlen(p);
			fn_ndev_val_a[cnt_a].sn = N_RX_SN_PTR(buf); //流水号
			cnt_a++;
		}
		p = p1 + 1; //指向下一条指令
	}	
	
	if(cnt_a > 0)   //控制回复
	{
		var = n_var_data_alloc();
		if(var != NULL)
		{
			n_var_data_hd_ack(var, N_RX_CMD_PTR(buf), N_RX_SN_PTR(buf));  //数据组包

			for(i = 0; i < cnt_a; i++)
			{
				fn_ndev_val_a[i].var_ptr = var;  //分配内存给应答报文  //不同指令的应答报文的内存一样?
			}

			__ndev_word_run(&fn_ndev_val_a, cnt_a, &ack_f);  //运行哈希函数
		}
	}
	
	/* 控制指令回应 */
	if((var != NULL) && (ack_f == 1))
	{
		n_var_data_len(var);
		n_var_data_crc(var);

		unsigned char esc_txbuf[B_DEV_RXBUF_SZ] = {0};
		int esc_len = 0;

		esc_len = uplink_escape(esc_txbuf, var->data, var->total_len);
		if(esc_len > 0)
			__tcp_send_to_server(esc_txbuf, esc_len, 1000);
		
		ack_f = 0;
	}

	if(var != NULL)
		n_var_data_free(var);


	if(cnt_r > 0)  //带? 号的请求,需要回复中心
	{
		var = n_var_data_alloc();
		if(var != NULL)
		{
			n_var_data_hd_ack(var, N_RX_CMD_PTR(buf), N_RX_SN_PTR(buf));
			
			for(i = 0; i < cnt_r; i++)
			{
				fn_ndev_val_r[i].var_ptr = var;
			}
			
			__ndev_word_run(&fn_ndev_val_r, cnt_r, &ack_f);	
			n_var_data_len(var);
			n_var_data_crc(var);
			__tcp_send_to_server(var->data, var->total_len, 1000);
			n_var_data_free(var);
		}
	}

}

static void __ndev_word_run(fn_ndev_val_t *val, const unsigned int len, unsigned char *ack_f)
{
	int i;

	for(i = 0; i < len; i++)
	{
		fn_hash_run(g_fn_hash_ndev, &val[i]);
		if(val[i].ack == 1)  //判断是否需要发确认报文
			*ack_f = 1;
	}	
}

void *ndev_get_univ_async_mutex_queue(void)
{
	if(g_async_mutex_queue != NULL)
		return g_async_mutex_queue;

	return B_DEV_NULL;
}

static int __unity_file_ack_to_server(unity_file_st_t *univ, const unsigned char cmd, const unsigned char sn, const unsigned char result)
{
	int n, esc_len;
	unsigned char tx_buf[512] = {0};
	unsigned char esc_txbuf[512] = {0};
	univ_file_ack_t ack;
	ndev_rx_buf_t buf;

	memset(&buf, 0, sizeof(ndev_rx_buf_t));
	memset(&ack, 0, sizeof(univ_file_ack_t));
	buf.s_un.st.cmd = cmd;
	buf.s_un.st.sn = sn;
	memcpy(&(ack.hd), &(univ->hd), sizeof(univ_file_hd_t));
	ack.result = result;
	
	n = ndev_protocol_blk_ack(tx_buf, &ack, &buf);
	if(n > 0)
	{
		esc_len = __uplink_escape(esc_txbuf, tx_buf, n); //上行转义

		lib_printf_v2("--------------unity file ack-------------------", esc_txbuf, esc_len, 16);
		
		if(esc_len > 0)
			return __tcp_send_to_server(esc_txbuf, esc_len, 1000);
	}

	return B_DEV_ERROR;
}

static int __unity_file_req_to_server(univ_file_ack_t *ack)
{
	int n, esc_len;
	unsigned char tx_buf[512] = {0};
	unsigned char esc_txbuf[512] = {0};
	
	n = ndev_protocol_blk_req(tx_buf, ack);
	if(n > 0)
	{
		esc_len = __uplink_escape(esc_txbuf, tx_buf, n); //上行转义

		lib_printf_v2("-----------------unity file req-----------------", esc_txbuf, esc_len, 16);
		
		if(esc_len > 0)
			return __tcp_send_to_server(esc_txbuf, esc_len, 1000);
	}

	return B_DEV_ERROR;	
}

int ndev_trading_records_upload(const unsigned char dev_addr, void *ptr, const unsigned int len)
{
	unsigned char d_buf[512] = {0};
	unsigned char esc_buf[512] = {0};
	unsigned int sockfd;
	int nlen = 0;
	int esc_len = 0;
	int err = B_DEV_ERROR;

	nlen = ndev_protocol_univ_trading_records_upload(d_buf, ptr, dev_addr, len); //组包
	if(nlen < 0)
		return B_DEV_ERROR;
	
	sockfd = __get_sockfd();
	if(sockfd > 0)
	{
		esc_len = __uplink_escape(esc_buf, d_buf, nlen); //上行转义
		if(esc_len > 0)
		{
			return lib_tcp_write_select(sockfd, esc_buf, esc_len, 1000);
		}
	}
	
	return B_DEV_ERROR;
}

int ndev_whole_pass_upload(const unsigned char op, const unsigned char dev_addr, void *ptr, const unsigned int len)
{
	unsigned char d_buf[512] = {0};
	unsigned char esc_buf[512] = {0};
	unsigned int sockfd;
	int nlen = 0;
	int esc_len = 0;
	int err = B_DEV_ERROR;

	nlen = ndev_protocol_whole_pass_upload(op, d_buf, ptr, dev_addr, len);
	if(nlen < 0)
		return B_DEV_ERROR;
	
	sockfd = __get_sockfd();
	if(sockfd > 0)
	{
		esc_len = __uplink_escape(esc_buf, d_buf, nlen); //上行转义
		if(esc_len > 0)
		{
			return lib_tcp_write_select(sockfd, esc_buf, esc_len, 1000);
		}
	}
	
	return B_DEV_ERROR;
}

#include "lib_sn.h"
/*
 * 轮询数据库,把数据库信息上传到中心
 */
static void *__database_loop_handle(void *arg)
{
	while(1)
	{
		




		

		
		lib_sleep(180);
	}
}

static void *__unity_file_except_handle(void *arg)
{
	while(1)
	{
		if(__get_conn_stat() >= NDEV_NETSTAT_REGISTERED)  //设备注册后，才检查泛文件
			break;
		else
			lib_sleep(5);
	}

	//unity_file_except_handle(arg);
	unity_file_except_handle();
	
	return lib_thread_exit((void *)NULL);
}

