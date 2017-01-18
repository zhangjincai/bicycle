#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <curl/curl.h>  
#include <sys/stat.h>
#include <dirent.h>

#include "lib_general.h"
#include "lib_update.h"
#include "upgrade_config.h"
//#include "lib_reader_update.h"

#include "firmware_check.h"

/* 开启MD5检验 */
#define UPE_USING_CHECK			

/* 日志定义 */
#define UPE_LOG_RUN

#ifdef UPE_LOG_RUN
#include <syslog.h>
#define SYS_LOG_EMERG(fmt, args...) 		syslog(LOG_EMERG, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 			syslog(LOG_ALERT, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			syslog(LOG_CRIT, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			syslog(LOG_ERR, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		syslog(LOG_WARNING, fmt, ##args) 
#define SYS_LOG_NOTICE(fmt, args...)  		syslog(LOG_NOTICE, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			syslog(LOG_INFO, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		syslog(LOG_DEBUG, fmt, ##args)
#else
#define SYS_LOG_EMERG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_NOTICE(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#endif



#define UPE_NULL					(NULL) 
#define UPE_EOK					(0)  //正常
#define UPE_ERROR				(-1) //错误
#define UPE_ETIMEOUT				(-2) //超时
#define UPE_EFULL				(-3) //满
#define UPE_EEMPTY				(-4) //空
#define UPE_ENOMEM 				(-5) //内存不够
#define UPE_EXCMEM				(-6) //内存越界
#define UPE_EBUSY				(-7) //忙
#define UPE_ERR_COMMAND     		(-8) //不支持的命令

#define UPE_TRUE					(1)
#define UPE_FALSE				(0)

#define UPE_DATA_SZ				512   //消息队列数据最大为8192字节
#define UPE_FTP_DL_TIMEOUT		60
#define UPE_MSG_KEY				0x2015
#define UPE_MSG_APP				0x01

#define MTD_DEV_KERNEL			"/dev/mtd8"
#define MTD_DEV_FIRMWARE		"/dev/mtd9"
#define MTD_DEV_APPL			"/dev/mtd10"

#define KERNEL_VERSION_PATH		"/mnt/firmware/kernel_ver.txt"
#define FW_VERSION_PATH			"/mnt/firmware/fw_ver.txt"
#define APPL_VERSION_PATH		"/mnt/app/appl_ver.txt"
#define TERMINAL_NO_PATH		"/opt/config/terminal_no_file.txt"

#define KERNEL_PREFIX			"kernel_"
#define FW_PREFIX				"fw_"
#define APP_PREFIX				"app_"
#define LNT_PREFIX				"LNT_ZM_"


/*
 * 2016-07-26 增加一个共享内存用于与bicycle_gui通信
 * 
 */

#define UPE_SHART_MEM_KEY		20160726	

struct lnt_firmware_update_result
{
	char type;
	char result;  //0:初始化，1:成功，2:失败
	char ftp_local_path[96];
}__attribute__((packed));
typedef struct lnt_firmware_update_result lnt_firmware_update_result_t;






enum UP_DNL_STAT
{
	UP_DNL_STAT_NOT_OP = 0,              //没有下载操作
	UP_DNL_STAT_NOT_COMPLETE = 1,  //没有完成
	UP_DNL_STAT_COMPLETE = 2  //完成
};

enum UP_WR_STAT
{
	UP_WR_STAT_NOT_WR = 0,   //未写
	UP_WR_STAT_WR = 1,   //已写
	UP_WR_STAT_NOT_WR_LNT = 2,	 //岭南通读卡器升级专用 2016-07-28
};

enum UPE_MSG
{
	UPE_CMD_SET_FTP_DOWNLOAD_ENABLE = 1,
	UPE_CMD_SET_FTP_DOWNLOAD_DISABLE,
	UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT,
	UPE_CMD_SET_UPGRADE_START,
	UPE_CMD_SET_FTP_CONFIG,
	UPE_CMD_SET_NETWORK_STAT,
	UPE_CMD_SET_FDNL_CTRL_INFO,

	
	UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT,
	UPE_CMD_GET_FTP_CONFIG,
	UPE_CMD_GET_FIRMWARE_CONFIG,
	UPE_CMD_GET_FTP_UPSTAT,
	UPE_CMD_GET_FDNL_CTRL_INFO,
	
	UPE_CMD_END
};

enum UPE_NOTIFY
{
	UPE_NOTIFY_INIT = 0,
	UPE_NOTIFY_WAIT,
	UPE_NOTIFY_RUN,
	UPE_NOTIFY_UPGRADE,
	UPE_NOTIFY_END
};

enum UPE_NETWORK_STAT
{
	UPE_NETWORK_STAT_INIT = 0,
	UPE_NETWORK_STAT_CONNECT,
	UPE_NETWORK_STAT_DISCONNECT,
	UPE_NETWORK_STAT_END
};

enum UPE_TYPE
{
	UPE_TYPE_INIT = 0,
	UPE_TYPE_KERNEL,
	UPE_TYPE_FIRMWARE,
	UPE_TYPE_APPL,
	UPE_TYPE_LNT_ZM,
	UPE_TYPE_END
};

enum FIRMWARE_TYPE
{
	FIRMWARE_TYPE_NO,
	FIRMWARE_TYPE_KERNEL,
	FIRMWARE_TYPE_FIRMWARE,
	FIRMWARE_TYPE_APPL,
	FIRMWARE_TYPE_LNT_ZM
};

enum FTP_ERRNO
{
	FTP_ERRNO_ERR = -1,
	FTP_ERRNO_OK = 0,  					 //正常(0)
	FTP_ERRNO_OPERATION_TIMEDOUT = 1,	  //操作超时(28)
	FTP_ERRNO_COULDNT_CONNECT = 2,		//connect（）的主机或代理失败(7)
	FTP_ERRNO_LOGIN_DENIED = 3,			 //远程服务器拒绝卷曲登录(67)
	FTP_ERRNO_REMOTE_FILE_NOT_FOUND = 4,   //不存在的URL引用的资源(78)
	FTP_ERRNO_CRC = 5,				//固件校验错误
	FTP_ERRNO_WR = 6,				//写入固件错误
	FTP_ERRNO_UNKNOWN = 255   //未知错误
};

#define F_NAME_CNT		6
struct update_file
{
	int cnt;
	char filename[F_NAME_CNT][32];
};


struct upgrade_message
{
	long d_addr;
	unsigned char cmd;
	int s_addr;
	int result;
	unsigned char data[UPE_DATA_SZ];
}__attribute__((packed));
typedef struct upgrade_message upgrade_message_t;

typedef struct upgrade_notify
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;		
}upgrade_notify_t;

struct ftp_config
{
	unsigned char update_type;
	char username[32];
	char passwd[32];
	unsigned short port;
	char firmware_name[32];
	char remote_path[96];
	char local_path[96];
	long  connect_timeout;
	long download_timeout;
}__attribute__((packed));
typedef struct ftp_config ftp_config_t;

/*
 * FTP更新状态
 */
struct ftp_upgrade_status
{
	unsigned char download_status;
	unsigned char ftp_recode;
	unsigned char ftp_errno;
}__attribute__((packed));
typedef struct ftp_upgrade_status ftp_upgrade_status_t;









static int g_upgrade_qid = -1;
static int g_ftp_dl_timeout = UPE_FTP_DL_TIMEOUT;
static int g_upgrade_type = UPE_TYPE_INIT;
static struct ftp_config g_ftp_config;
static lib_update_t *g_kernel_update, *g_firmware_update, *g_appl_update;

static upgrade_notify_t g_ftp_dl_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, UPE_NOTIFY_WAIT};
static upgrade_notify_t g_upgrade_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, UPE_NOTIFY_WAIT};

static ftp_upgrade_status_t g_ftp_upstat[4];

static void *__ftp_download_thread(void *arg);
static void *__upgrade_thread(void *arg);
static enum FTP_ERRNO __ftp_download(struct ftp_config *config, int *curl_code);
static int __update_file_find(const char *pathname, struct update_file file[4]);

static void __sigint(int sig)
{
	fprintf(stderr, "Upgrade App Signal: %d\n", sig);

#if 0
	curl_global_cleanup();  
	lib_msg_kill(g_upgrade_qid);
	
	lib_update_destroy(g_kernel_update);
	lib_update_destroy(g_firmware_update);
	lib_update_destroy(g_appl_update);
	
	exit(-1);
#endif

}

static void __signals_init(void)
{
	struct sigaction sa;
	
	sa.sa_flags = 0;
	sigaddset(&sa.sa_mask, SIGPIPE);
    	sigaddset(&sa.sa_mask, SIGCHLD); 
   	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM); 
	sigaddset(&sa.sa_mask, SIGTTOU);
	sigaddset(&sa.sa_mask, SIGTTIN);
	sigaddset(&sa.sa_mask, SIGTSTP);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sa, NULL);

	sa.sa_handler = __sigint;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGTTIN, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &sa, NULL);
}

static int __ftp_download_notify_wait(unsigned int *notify)
{
	int err = UPE_ERROR;

	pthread_mutex_lock(&g_ftp_dl_notify.mutex);

	if(g_ftp_dl_notify.notify == UPE_NOTIFY_WAIT)
	{
		err = pthread_cond_wait(&g_ftp_dl_notify.cond, &g_ftp_dl_notify.mutex);
		*notify = g_ftp_dl_notify.notify;
	}
	
	pthread_mutex_unlock(&g_ftp_dl_notify.mutex);

	return err;
}

static int __ftp_download_notify_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_ftp_dl_notify.mutex);

	g_ftp_dl_notify.notify = notify;

	pthread_mutex_unlock(&g_ftp_dl_notify.mutex);

	return pthread_cond_signal(&g_ftp_dl_notify.cond);
}

static void __ftp_download_notify_set(const unsigned int notify)
{
	pthread_mutex_lock(&g_ftp_dl_notify.mutex);

	g_ftp_dl_notify.notify = notify;

	pthread_mutex_unlock(&g_ftp_dl_notify.mutex);
}

static int __upgrade_notify_wait(unsigned int *notify)
{
	int err = UPE_ERROR;

	pthread_mutex_lock(&g_upgrade_notify.mutex);

	if(g_upgrade_notify.notify == UPE_NOTIFY_WAIT)
	{
		err = pthread_cond_wait(&g_upgrade_notify.cond, &g_upgrade_notify.mutex);
		*notify = g_upgrade_notify.notify;
	}
	
	pthread_mutex_unlock(&g_upgrade_notify.mutex);

	return err;
}

static int __upgrade_notify_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_upgrade_notify.mutex);

	g_upgrade_notify.notify = notify;

	pthread_mutex_unlock(&g_upgrade_notify.mutex);

	return pthread_cond_signal(&g_upgrade_notify.cond);
}

static void __upgrade_notify_set(const unsigned int notify)
{
	pthread_mutex_lock(&g_upgrade_notify.mutex);

	g_upgrade_notify.notify = notify;

	pthread_mutex_unlock(&g_upgrade_notify.mutex);
}


static int __kernel_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;

	fp = fopen(KERNEL_VERSION_PATH, "rb");
	if(fp == NULL)
		return UPE_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		strcpy(version, KERNEL_PREFIX);
		strcat(version, s_version);
		fprintf(stderr, "kernel: %s\n", s_version);
	}

	fclose(fp);
	return UPE_EOK;
}

static int __fw_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;

	fp = fopen(FW_VERSION_PATH, "rb");
	if(fp == NULL)
		return UPE_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		strcpy(version, FW_PREFIX);
		strcat(version, s_version);
		fprintf(stderr, "fw: %s\n", s_version);
	}

	fclose(fp);
	
	return UPE_EOK;
}

static int __app_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;

	fp = fopen(APPL_VERSION_PATH, "rb");
	if(fp == NULL)
		return UPE_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		strcpy(version, APP_PREFIX);
		strcat(version, s_version);
		fprintf(stderr, "app: %s\n", s_version);
	}

	fclose(fp);
	
	return UPE_EOK;
}

static void __terminal_no_file_get(char *terminal_no)
{
	FILE *fp = NULL;
	int s_len = 0;
	char s_terminal_no[64] = {0};

	fp = fopen(TERMINAL_NO_PATH, "rt");  /* 读文本 */
	if(fp != NULL)
	{
		if(fgets(s_terminal_no, sizeof(s_terminal_no), fp) != NULL)
		{
			fprintf(stderr, "Upgrade App Terminal NO get: %s\n", s_terminal_no);
			
			s_len = strlen(s_terminal_no);
			strcpy(terminal_no, s_terminal_no);
			terminal_no[s_len] = '\0';
		}
		else
			strcpy(terminal_no, "65535");
		
		fclose(fp);
	}
}

static void __ftp_upgrade_cleanup(const unsigned char update_type)
{
	switch(update_type)
	{
		case UPE_TYPE_KERNEL:
		{
			memset(&g_ftp_upstat[0], 0, sizeof(ftp_upgrade_status_t));
		}
		break;

		case UPE_TYPE_FIRMWARE:
		{
			memset(&g_ftp_upstat[1], 0, sizeof(ftp_upgrade_status_t));
		}
		break;

		case UPE_TYPE_APPL:
		{
			memset(&g_ftp_upstat[2], 0, sizeof(ftp_upgrade_status_t));
		}
		break;

		case UPE_TYPE_LNT_ZM:
		{
			memset(&g_ftp_upstat[3], 0, sizeof(ftp_upgrade_status_t));
		}
		break;
	}
}

static void __ftp_upgrade_download_status_set(const unsigned char update_type, const unsigned char status)
{
	switch(update_type)
	{
		case UPE_TYPE_KERNEL:
		{
			g_ftp_upstat[0].download_status = status; 
		}
		break;

		case UPE_TYPE_FIRMWARE:
		{
			g_ftp_upstat[1].download_status = status; 
		}
		break;

		case UPE_TYPE_APPL:
		{
			g_ftp_upstat[2].download_status = status; 
		}
		break;

		case UPE_TYPE_LNT_ZM:
		{
			g_ftp_upstat[3].download_status = status; 
		}
		break;
	}
}

static void __ftp_upgrade_recode_set(const unsigned char update_type, const unsigned char recode)
{
	switch(update_type)
	{
		case UPE_TYPE_KERNEL:
		{
			g_ftp_upstat[0].ftp_recode = recode; 
		}
		break;

		case UPE_TYPE_FIRMWARE:
		{
			g_ftp_upstat[1].ftp_recode = recode;
		}
		break;

		case UPE_TYPE_APPL:
		{
			g_ftp_upstat[2].ftp_recode = recode; 
		}
		break;

		case UPE_TYPE_LNT_ZM:
		{
			g_ftp_upstat[3].ftp_recode = recode; 
		}
		break;
	}
}

static void __ftp_upgrade_errno_set(const unsigned char update_type,  const unsigned char ferrno)
{
	switch(update_type)
	{
		case UPE_TYPE_KERNEL:
		{
			g_ftp_upstat[0].ftp_errno = ferrno; 
		}
		break;

		case UPE_TYPE_FIRMWARE:
		{
			g_ftp_upstat[1].ftp_errno = ferrno;
		}
		break;

		case UPE_TYPE_APPL:
		{
			g_ftp_upstat[2].ftp_errno = ferrno; 
		}
		break;

		case UPE_TYPE_LNT_ZM:
		{
			g_ftp_upstat[3].ftp_errno = ferrno; 
		}
		break;
	}
}

static void __ftp_to_firmware_config(struct ftp_config *ftp, struct firmware_config *firmware)
{
	strcpy(firmware->ftp_username, ftp->username);
	strcpy(firmware->ftp_passwd, ftp->passwd);
	strcpy(firmware->ftp_remote_path, ftp->remote_path);
	strcpy(firmware->ftp_local_path, ftp->local_path);
	strcpy(firmware->firmware_name, ftp->firmware_name);
	
	firmware->ftp_connect_timeout =  ftp->connect_timeout;
	firmware->ftp_download_timeout = ftp->download_timeout;
}

static void __firmware_config_to_ftp(struct firmware_config *firmware, struct ftp_config *ftp, const unsigned char update_type)
{
	strcpy(ftp->username, firmware->ftp_username);
	strcpy(ftp->passwd, firmware->ftp_passwd);
	strcpy(ftp->remote_path, firmware->ftp_remote_path);
	strcpy(ftp->local_path, firmware->ftp_local_path);
	strcpy(ftp->firmware_name, firmware->firmware_name);

	ftp->connect_timeout = firmware->ftp_connect_timeout;
	ftp->download_timeout = firmware->ftp_download_timeout;

	ftp->update_type = update_type;
}

#if 0
static void __upgrade_to_dnl_ctrl_info(void)
{
	struct upgrade_config up_config;
	
	memset(&up_config, 0, sizeof(struct upgrade_config));
	upgrade_config_get(&up_config);
	
	switch(up_config.fdnl_ctrl_info.ftype)  //读取之前FTP下载控制信息
	{
		case FIRMWARE_TYPE_KERNEL:
		{
			if(up_config.kernel.is_write_flash == UP_WR_STAT_WR) //升级准备就绪
				up_config.fdnl_ctrl_info.download_status = 0x03;
			else if(up_config.kernel.is_download_complete == UP_DNL_STAT_NOT_COMPLETE)
				up_config.fdnl_ctrl_info.download_status = 0x01;  //正在下载
			else if(up_config.kernel.is_download_complete == UP_DNL_STAT_COMPLETE)
				up_config.fdnl_ctrl_info.download_status= 0x02; //下载完成
		}
		break;

		case FIRMWARE_TYPE_FIRMWARE:
		{
			g_ftp_upstat[1].download_status = up_config.fdnl_ctrl_info.download_status;
			g_ftp_upstat[1].ftp_recode = up_config.fdnl_ctrl_info.ftpcode;
			g_ftp_upstat[1].ftp_errno = up_config.fdnl_ctrl_info.err;
		}
		break;

		case FIRMWARE_TYPE_APPL:
		{
			g_ftp_upstat[2].download_status = up_config.fdnl_ctrl_info.download_status;
			g_ftp_upstat[2].ftp_recode = up_config.fdnl_ctrl_info.ftpcode;
			g_ftp_upstat[2].ftp_errno = up_config.fdnl_ctrl_info.err;
		}
		break;
	}
	
}
#endif

static int __update_file_find(const char *pathname, struct update_file file[4])
{
	if((pathname == NULL) || (file == NULL))
		return -1;
	
	DIR * dp = NULL;
	struct dirent *filename = NULL;

	dp = opendir(pathname);
	if(dp == NULL)
		return -1;

	while ((filename = readdir(dp)) != NULL)
	{
		if(strstr(filename->d_name, KERNEL_PREFIX) != NULL)
		{
			if(file[0].cnt < F_NAME_CNT)
			{
				strcpy(file[0].filename[file[0].cnt++], filename->d_name);
			}
		}

		if(strstr(filename->d_name, FW_PREFIX) != NULL)
		{
			if(file[1].cnt < F_NAME_CNT)
			{
				strcpy(file[1].filename[file[1].cnt++], filename->d_name);
			}
		}

		if(strstr(filename->d_name, APP_PREFIX) != NULL)
		{
			if(file[2].cnt < F_NAME_CNT)
			{
				strcpy(file[2].filename[file[2].cnt++], filename->d_name);
			}
		}

		if(strstr(filename->d_name, LNT_PREFIX) != NULL)
		{
			if(file[3].cnt < F_NAME_CNT)
			{
				strcpy(file[3].filename[file[3].cnt++], filename->d_name);
			}
		}

		filename = NULL;
	}
	
	closedir(dp);
	
	return 0;	
}

int main(int argc, char *argv[])
{
	int err = LIB_GE_ERROR;
	int i;
	int f_continue_update = 0;
	int f_write_para = 0;
	pthread_t ftp_thr, up_thr;
	upgrade_message_t msg;
	struct update_config uconfig;
	struct upgrade_config up_config;

	__signals_init();

	/* 初始化syslog日志*/
#ifdef UPE_LOG_RUN
	char log_ident[64] = {0};
	char terminal_no[32] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};
	
	lib_get_macaddr("eth1", macaddr);
	lib_hex_to_str((unsigned char *)macaddr, 6, (unsigned char *)s_macaddr);
	
	__terminal_no_file_get(terminal_no);
	
	sprintf(log_ident, "------[UPGRADE]-[%s]:[%s]", s_macaddr, terminal_no);
	
	fprintf(stderr, "UPGRADE log ident: %s\n", log_ident);
	
	openlog(log_ident, LOG_NDELAY, LOG_DAEMON);
#endif	

	SYS_LOG_NOTICE("Upgrade Software compiled time: %s, %s.\r\n",__DATE__, __TIME__);

	upgrade_config_init();
	
	curl_global_init(CURL_GLOBAL_ALL); 

	err = lib_msg_init(&g_upgrade_qid, UPE_MSG_KEY);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("Upgrade Message init failed!\n");
		return 0;
	}

	fprintf(stderr, "upgrade qid: %d\n", g_upgrade_qid);

	g_ftp_config.download_timeout = g_ftp_dl_timeout; //下载超时时间

	/* kernel*/
	memset(&uconfig, 0, sizeof(struct update_config));
	strcpy(uconfig.mtd_device, MTD_DEV_KERNEL);
	g_kernel_update = lib_update_create(&uconfig);
	if(g_kernel_update == LIB_UP_NULL)
	{
		SYS_LOG_ERR("Kernel update create failed!\n");
		goto err;
	}
	
	/* firmware */
	memset(&uconfig, 0, sizeof(struct update_config));
	strcpy(uconfig.mtd_device, MTD_DEV_FIRMWARE);
	g_firmware_update = lib_update_create(&uconfig);
	if(g_firmware_update == LIB_UP_NULL)
	{
		SYS_LOG_ERR("Firmware update create failed!\n");
		goto err;
	}
	
	/* appl */
	memset(&uconfig, 0, sizeof(struct update_config));
	strcpy(uconfig.mtd_device, MTD_DEV_APPL);
	g_appl_update = lib_update_create(&uconfig);
	if(g_appl_update == LIB_UP_NULL)
	{
		SYS_LOG_ERR("Appl update create failed!\n");
		goto err;
	}

	/* 共享内存初始化(用于和bicycle_gui进程通信获取读卡器升级结果) */
	int upe_mid = 0;
	lib_share_mem_init(&upe_mid, UPE_SHART_MEM_KEY, sizeof(lnt_firmware_update_result_t) + 16);
	
	lib_normal_thread_create(&ftp_thr, __ftp_download_thread, NULL);	//FTP下载线程
	lib_normal_thread_create(&up_thr, __upgrade_thread, &upe_mid);  	//升级线程

	lib_msleep(500); 

	memset(&up_config, 0, sizeof(struct upgrade_config));
	upgrade_config_get(&up_config);

	__kernel_version_fgets(up_config.used_kernel_name);
	__fw_version_fgets(up_config.used_firmware_name);
	__app_version_fgets(up_config.used_appl_name);

	/* 继续下载kernel */
	if((up_config.kernel.is_download_complete == UP_DNL_STAT_NOT_COMPLETE) 
		&& (up_config.kernel.is_write_flash == UP_WR_STAT_NOT_WR) 
		&& (up_config.kernel.is_write_parameter == UP_WR_STAT_NOT_WR))
	{
		SYS_LOG_NOTICE( "kernel continue download\n");
		fprintf(stderr, "kernel continue download\n");

		f_continue_update = 1;
		__firmware_config_to_ftp(&(up_config.kernel), &g_ftp_config, UPE_TYPE_KERNEL);
		__ftp_download_notify_put(UPE_NOTIFY_RUN);
	}
	
	/* 继续下载firmware */
	if((up_config.firmware.is_download_complete == UP_DNL_STAT_NOT_COMPLETE) 
		&& (up_config.firmware.is_write_flash == UP_WR_STAT_NOT_WR) 
		&& (up_config.firmware.is_write_parameter == UP_WR_STAT_NOT_WR))
	{
		SYS_LOG_NOTICE( "firmware continue download\n");
		fprintf(stderr, "firmware continue download\n");

		f_continue_update = 1;
		__firmware_config_to_ftp(&(up_config.firmware), &g_ftp_config, UPE_TYPE_FIRMWARE);
		__ftp_download_notify_put(UPE_NOTIFY_RUN);
	}

	/* 继续下载appl */
	if((up_config.appl.is_download_complete == UP_DNL_STAT_NOT_COMPLETE) 
		&& (up_config.appl.is_write_flash == UP_WR_STAT_NOT_WR) 
		&& (up_config.appl.is_write_parameter == UP_WR_STAT_NOT_WR))
	{
		SYS_LOG_NOTICE( "appl continue download\n");
		fprintf(stderr, "appl continue download\n");

		f_continue_update = 1;
		__firmware_config_to_ftp(&(up_config.appl), &g_ftp_config, UPE_TYPE_APPL);
		__ftp_download_notify_put(UPE_NOTIFY_RUN);
	}

	/* 继续下载LNT_ZM */
	if((up_config.lnt_zm.is_download_complete == UP_DNL_STAT_NOT_COMPLETE) 
		&& (up_config.lnt_zm.is_write_flash == UP_WR_STAT_NOT_WR) 
		&& (up_config.lnt_zm.is_write_parameter == UP_WR_STAT_NOT_WR))
	{
		SYS_LOG_NOTICE( "lnt zm continue download\n");
		fprintf(stderr, "lnt zm continue download\n");

		f_continue_update = 1;
		__firmware_config_to_ftp(&(up_config.lnt_zm), &g_ftp_config, UPE_TYPE_LNT_ZM);
		__ftp_download_notify_put(UPE_NOTIFY_RUN);
	}


	/* 检查是否有未删除升级文件 */
	struct update_file file[4];
	char f_name[128] = {0};
	memset(&file, 0, sizeof(struct update_file) * 4);
	err = __update_file_find("/opt/ftppath", file);
	if(!err)
	{
		if(file[0].cnt > 0) //kernel 如果升级已经成功,升级文件还存在，删除文件
		{
			if((up_config.kernel.is_download_complete == UP_DNL_STAT_COMPLETE) 
				&& (up_config.kernel.is_write_flash == UP_WR_STAT_WR) 
				&& (up_config.kernel.is_write_parameter == UP_WR_STAT_WR))
			{
				for(i = 0; i < file[0].cnt; i++)
				{
					memset(&f_name, 0, sizeof(f_name));
					sprintf(f_name, "/opt/ftppath/%s", file[0].filename[i]);

					fprintf(stderr, "remove %s\n", f_name);
					SYS_LOG_NOTICE("remove %s\n", f_name);
					
					remove(f_name); 
				}	
			}
		}

		if(file[1].cnt > 0) //firmware
		{
			if((up_config.firmware.is_download_complete == UP_DNL_STAT_COMPLETE) 
				&& (up_config.firmware.is_write_flash == UP_WR_STAT_WR) 
				&& (up_config.firmware.is_write_parameter == UP_WR_STAT_WR))
			{
				for(i = 0; i < file[1].cnt; i++)
				{
					memset(&f_name, 0, sizeof(f_name));
					sprintf(f_name, "/opt/ftppath/%s", file[1].filename[i]);

					fprintf(stderr, "remove %s\n", f_name);
					SYS_LOG_NOTICE("remove %s\n", f_name);
					
					remove(f_name); 
				}	
			}
		}
		
		if(file[2].cnt > 0) //appl
		{
			if((up_config.appl.is_download_complete == UP_DNL_STAT_COMPLETE) 
				&& (up_config.appl.is_write_flash == UP_WR_STAT_WR) 
				&& (up_config.appl.is_write_parameter == UP_WR_STAT_WR))
			{
				for(i = 0; i < file[2].cnt; i++)
				{
					memset(&f_name, 0, sizeof(f_name));
					sprintf(f_name, "/opt/ftppath/%s", file[2].filename[i]);

					fprintf(stderr, "remove %s\n", f_name);
					SYS_LOG_NOTICE("remove %s\n", f_name);
					
					remove(f_name); 
				}	
			}
		}

		if(file[3].cnt > 0) //LNT_ZM
		{
			if((up_config.lnt_zm.is_download_complete == UP_DNL_STAT_COMPLETE) 
				&& (up_config.lnt_zm.is_write_flash == UP_WR_STAT_WR) 
				&& (up_config.lnt_zm.is_write_parameter == UP_WR_STAT_WR))
			{
				for(i = 0; i < file[3].cnt; i++)
				{
					memset(&f_name, 0, sizeof(f_name));
					sprintf(f_name, "/opt/ftppath/%s", file[3].filename[i]);

					fprintf(stderr, "remove %s\n", f_name);
					SYS_LOG_NOTICE("remove %s\n", f_name);
					
					remove(f_name); 
				}	
			}
		}
	}

	
	/* 读取参数配置 */
	if(f_continue_update == 0)  //上电不需要重新下载
	{
		struct parameter para;
		memset(&para, 0, sizeof(struct parameter));
		err = lib_update_read_parameter(g_appl_update, &para);
		if(err == LIB_UP_EOK)
		{
			if(para.partition[UP_PARTITION_KERNEL].is_need_update == UP_NEED_UPDATE_OK)  //kernel 更新完毕
			{
				fprintf(stderr, "clear UP_PARTITION_KERNEL upgrade config\n");
				SYS_LOG_NOTICE("clear UP_PARTITION_KERNEL upgrade config\n");
				
				memset(&(up_config.kernel), 0, sizeof(struct firmware_config));  //清空配置
				memset(&(up_config.fdnl_ctrl_info), 0, sizeof(ftp_download_ctrl_info_t));  

				para.partition[UP_PARTITION_KERNEL].is_need_update = UP_NEED_UPDATE_INIT; //设置初始化参数

				f_write_para = 1;
			}

			if(para.partition[UP_PARTITION_FIRMWARE].is_need_update == UP_NEED_UPDATE_OK)  //firmware
			{
				fprintf(stderr, "clear UP_PARTITION_FIRMWARE upgrade config\n");
				SYS_LOG_NOTICE("clear UP_PARTITION_FIRMWARE upgrade config\n");
				
				memset(&(up_config.firmware), 0, sizeof(struct firmware_config));  
				memset(&(up_config.fdnl_ctrl_info), 0, sizeof(ftp_download_ctrl_info_t));  
				
				para.partition[UP_PARTITION_FIRMWARE].is_need_update = UP_NEED_UPDATE_INIT;

				f_write_para = 1;
			}

			if(para.partition[UP_PARTITION_APP].is_need_update == UP_NEED_UPDATE_OK)  //appl
			{
				fprintf(stderr, "clear UP_PARTITION_APP upgrade config\n");
				SYS_LOG_NOTICE("clear UP_PARTITION_APP upgrade config\n");
				
				memset(&(up_config.appl), 0, sizeof(struct firmware_config));  
				memset(&(up_config.fdnl_ctrl_info), 0, sizeof(ftp_download_ctrl_info_t));  
				
				para.partition[UP_PARTITION_APP].is_need_update = UP_NEED_UPDATE_INIT;

				f_write_para = 1;
			}

			if(f_write_para == 1)  //避免不断写EEROM
			{
				memset(&(up_config.fdnl_ctrl_info), 0, sizeof(struct ftp_download_ctrl_info)); //升级成功,清除之前下载的控制信息
				lib_update_write_parameter(g_appl_update, &para);  //写入参数
			}

			/* LNT ZM下载成功,要清空配置信息 */
			if((up_config.lnt_zm.is_download_complete == UP_DNL_STAT_COMPLETE) && \
				(up_config.lnt_zm.is_write_flash == UP_WR_STAT_WR) && \
				(up_config.lnt_zm.is_write_parameter == UP_WR_STAT_WR))
			{
				fprintf(stderr, "clear LNT ZM upgrade config\n");
				SYS_LOG_NOTICE("clear LNT ZM upgrade config\n");
				
				memset(&(up_config.lnt_zm), 0, sizeof(struct firmware_config));  
				memset(&(up_config.fdnl_ctrl_info), 0, sizeof(ftp_download_ctrl_info_t));  
			}
		}

		fprintf(stderr, "f_write_para = %d\n", f_write_para);
	}
	
	upgrade_config_put(&up_config);  //保存配置文件
	
	while(1)
	{
		memset(&msg, 0, sizeof(upgrade_message_t));
		
		err = lib_msg_recv(g_upgrade_qid, &msg, sizeof(upgrade_message_t), UPE_MSG_APP);  //接收消息
		if(err > 0)
		{
			switch(msg.cmd)
			{
				case UPE_CMD_SET_FTP_DOWNLOAD_ENABLE:  //FTP下载启动
				{
					fprintf(stderr, "UPE_CMD_SET_FTP_DOWNLOAD_ENABLE: %d\n", UPE_CMD_SET_FTP_DOWNLOAD_ENABLE);

					__ftp_upgrade_cleanup(g_ftp_config.update_type);  //初始化
					__ftp_download_notify_put(UPE_NOTIFY_RUN);
					
					msg.cmd = UPE_CMD_SET_FTP_DOWNLOAD_ENABLE;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
					memset(&(msg.data), 0, sizeof(msg.data));
				}
				break;

				case UPE_CMD_SET_FTP_DOWNLOAD_DISABLE: //FTP下载关闭
				{
					fprintf(stderr, "UPE_CMD_SET_FTP_DOWNLOAD_DISABLE: %d\n", UPE_CMD_SET_FTP_DOWNLOAD_DISABLE);

					__ftp_download_notify_put(UPE_NOTIFY_WAIT);   

					msg.cmd = UPE_CMD_SET_FTP_DOWNLOAD_DISABLE;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
					memset(&(msg.data), 0, sizeof(msg.data));
				}
				break;

				case UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT: //FTP下载超时
				{
					fprintf(stderr, "UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT: %d\n", UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT);

					memcpy(&g_ftp_dl_timeout, msg.data, sizeof(g_ftp_dl_timeout));
					g_ftp_config.download_timeout = g_ftp_dl_timeout;

					fprintf(stderr, "ftp download timeout: %d\n", g_ftp_dl_timeout);
					
					msg.cmd = UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
					memset(&(msg.data), 0, sizeof(msg.data));
				}
				break;

				case UPE_CMD_SET_UPGRADE_START:  //升级启动
				{
					fprintf(stderr, "UPE_CMD_SET_UPGRADE_START: %d\n", UPE_CMD_SET_UPGRADE_START);

					upgrade_config_get(&up_config);
					memset(&(up_config.fdnl_ctrl_info), 0, sizeof(struct ftp_download_ctrl_info));   //要清除之前的FTP控制信息
					upgrade_config_put(&up_config);
					
					g_upgrade_type = msg.data[0];
					fprintf(stderr, "UPE_CMD_SET_UPGRADE_START g_upgrade_type = %d\n", g_upgrade_type);
					
					__upgrade_notify_put(UPE_NOTIFY_RUN);

					msg.cmd = UPE_CMD_SET_UPGRADE_START;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
					memset(&(msg.data), 0, sizeof(msg.data));
				}
				break;

				case UPE_CMD_SET_FTP_CONFIG:  //设置FTP配置
				{
					fprintf(stderr, "UPE_CMD_SET_FTP_CONFIG: %d\n", UPE_CMD_SET_FTP_CONFIG);
					
					memcpy(&g_ftp_config, &(msg.data), sizeof(struct ftp_config));

					upgrade_config_get(&up_config);
					
					switch(g_ftp_config.update_type)
					{
						case UPE_TYPE_KERNEL:
							__ftp_to_firmware_config(&g_ftp_config, &(up_config.kernel));
							break;
							
						case UPE_TYPE_FIRMWARE:
							__ftp_to_firmware_config(&g_ftp_config, &(up_config.firmware));
							break;

						case UPE_TYPE_APPL:
							__ftp_to_firmware_config(&g_ftp_config, &(up_config.appl));
							break;

						case UPE_TYPE_LNT_ZM:
							__ftp_to_firmware_config(&g_ftp_config, &(up_config.lnt_zm));
							break;
					}
					
					upgrade_config_put(&up_config);
		
					msg.cmd = UPE_CMD_SET_FTP_CONFIG;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
					memset(&(msg.data), 0, sizeof(msg.data));
				}
				break;
 
				case UPE_CMD_SET_FDNL_CTRL_INFO:  //FTP下载控制信息
				{
					fprintf(stderr, "UPE_CMD_SET_FDNL_CTRL_INFO: %d\n", UPE_CMD_SET_FDNL_CTRL_INFO);

					upgrade_config_get(&up_config);
					
					ftp_download_ctrl_info_t fdnl_ctrl_info;
					memset(&fdnl_ctrl_info, 0, sizeof(ftp_download_ctrl_info_t));
					memcpy(&fdnl_ctrl_info, &(msg.data), sizeof(ftp_download_ctrl_info_t));

					/* 不要设置下载状态 */
					up_config.fdnl_ctrl_info.upsn = fdnl_ctrl_info.upsn;
					up_config.fdnl_ctrl_info.supplier = fdnl_ctrl_info.supplier;

					memcpy(&(up_config.fdnl_ctrl_info.time), &(fdnl_ctrl_info.time), sizeof(up_config.fdnl_ctrl_info.time));  //时间
					
					up_config.fdnl_ctrl_info.isvaild = fdnl_ctrl_info.isvaild;
					up_config.fdnl_ctrl_info.ftype = fdnl_ctrl_info.ftype; 	//类型
					up_config.fdnl_ctrl_info.ftime = fdnl_ctrl_info.ftime;
					
					upgrade_config_put(&up_config);

					msg.cmd = UPE_CMD_SET_FDNL_CTRL_INFO;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
					memset(&(msg.data), 0, sizeof(msg.data));
				}
				break;








/**************************** 获取 ***********************************************/

				case UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT: //获取FTP下载超时
				{
					fprintf(stderr, "UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT: %d\n", UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT);

					memset(&(msg.data), 0, sizeof(msg.data));
					memcpy(&(msg.data), &g_ftp_dl_timeout, sizeof(g_ftp_dl_timeout));

					msg.cmd = UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;	
				}
				break;

				case UPE_CMD_GET_FTP_CONFIG: //获取FTP配置
				{
					fprintf(stderr, "UPE_CMD_GET_FTP_CONFIG: %d\n", UPE_CMD_GET_FTP_CONFIG);
				
					memset(&(msg.data), 0, sizeof(msg.data));
					memcpy(&(msg.data),  &g_ftp_config, sizeof(struct ftp_config));

					msg.cmd = UPE_CMD_GET_FTP_CONFIG;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;
				}
				break;

				case UPE_CMD_GET_FIRMWARE_CONFIG: //获取固件配置
				{
					fprintf(stderr, "UPE_CMD_GET_FIRMWARE_CONFIG: %d\n", UPE_CMD_GET_FIRMWARE_CONFIG);
					
					struct upgrade_config upgrade_conf;
					struct firmware_config firmware_conf;
					memset(&upgrade_conf, 0, sizeof(struct upgrade_config));
					memset(&firmware_conf, 0, sizeof(struct firmware_config));
					
					upgrade_config_get(&upgrade_conf);	
					enum FIRMWARE_TYPE type = msg.data[0];
					switch(type)
					{
						case FIRMWARE_TYPE_KERNEL:
						{
							memset(&(msg.data), 0, sizeof(msg.data));
							memcpy(&(msg.data), &(upgrade_conf.kernel), sizeof(struct firmware_config));
							
							msg.result = UPE_EOK;	
						}
						break;

						case FIRMWARE_TYPE_FIRMWARE:
						{
							memset(&(msg.data), 0, sizeof(msg.data));
							memcpy(&(msg.data), &(upgrade_conf.firmware), sizeof(struct firmware_config));
							
							msg.result = UPE_EOK;						
						}
						break;

						case FIRMWARE_TYPE_APPL:
						{
							fprintf(stderr, "is_download_complete:%d, is_write_parameter:%d, is_write_flash:%d\n", 
								up_config.appl.is_download_complete, up_config.appl.is_write_parameter, up_config.appl.is_write_flash);
							
							memset(&(msg.data), 0, sizeof(msg.data));
							memcpy(&(msg.data), &(upgrade_conf.appl), sizeof(struct firmware_config));
							
							msg.result = UPE_EOK;							
						}
						break;		

						case FIRMWARE_TYPE_LNT_ZM: //岭南通
						{
							fprintf(stderr, "LNT ZM is_download_complete:%d, is_write_parameter:%d, is_write_flash:%d\n", 
								up_config.lnt_zm.is_download_complete, up_config.lnt_zm.is_write_parameter, up_config.lnt_zm.is_write_flash);
							
							memset(&(msg.data), 0, sizeof(msg.data));
							memcpy(&(msg.data), &(upgrade_conf.lnt_zm), sizeof(struct firmware_config));
							
							msg.result = UPE_EOK;						
						}
						break;


						default:  //不返回会报错
						{
							msg.result = UPE_ERROR;	
						}
					}

					msg.cmd = UPE_CMD_GET_FIRMWARE_CONFIG;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;	
				}
				break;

				case UPE_CMD_GET_FTP_UPSTAT:  //FTP固件更新状态
				{
					fprintf(stderr, "UPE_CMD_GET_FTP_UPSTAT: %d\n", UPE_CMD_GET_FTP_UPSTAT);

					upgrade_config_get(&up_config);
					
					enum FIRMWARE_TYPE type = msg.data[0];//类型

					fprintf(stderr, "UPE_CMD_GET_FTP_UPSTAT ftype = %d\n", type);
					
					switch(type)
					{
						case FIRMWARE_TYPE_KERNEL:
						{
							memset(&(msg.data), 0, sizeof(msg.data));

							if(up_config.kernel.is_write_flash == UP_WR_STAT_WR)
								g_ftp_upstat[0].download_status = 0x03; //升级准备就绪
							else if(up_config.kernel.is_download_complete == UP_DNL_STAT_NOT_COMPLETE)
								g_ftp_upstat[0].download_status = 0x01;  //正在下载
							else if(up_config.kernel.is_download_complete == UP_DNL_STAT_COMPLETE)
								g_ftp_upstat[0].download_status = 0x02; //下载完成
								
							memcpy(&(msg.data), &(g_ftp_upstat[0]), sizeof(ftp_upgrade_status_t));

							msg.result = UPE_EOK;	
						}
						break;
						
						case FIRMWARE_TYPE_FIRMWARE:
						{
							memset(&(msg.data), 0, sizeof(msg.data));

							if(up_config.firmware.is_write_flash == UP_WR_STAT_WR)
								g_ftp_upstat[1].download_status = 0x03; //升级准备就绪
							else if(up_config.firmware.is_download_complete == UP_DNL_STAT_NOT_COMPLETE)
								g_ftp_upstat[1].download_status = 0x01;  //正在下载
							else if(up_config.firmware.is_download_complete == UP_DNL_STAT_COMPLETE)
								g_ftp_upstat[1].download_status = 0x02; //下载完成
								
							memcpy(&(msg.data), &(g_ftp_upstat[1]), sizeof(ftp_upgrade_status_t));

							msg.result = UPE_EOK;	
						}
						break;
						
						case FIRMWARE_TYPE_APPL:
						{
							memset(&(msg.data), 0, sizeof(msg.data));

							if(up_config.appl.is_write_flash == UP_WR_STAT_WR)
								g_ftp_upstat[2].download_status = 0x03; //升级准备就绪
							else if(up_config.appl.is_download_complete == UP_DNL_STAT_NOT_COMPLETE)
								g_ftp_upstat[2].download_status = 0x01;  //正在下载
							else if(up_config.appl.is_download_complete == UP_DNL_STAT_COMPLETE)
								g_ftp_upstat[2].download_status = 0x02; //下载完成
								
							memcpy(&(msg.data), &(g_ftp_upstat[2]), sizeof(ftp_upgrade_status_t));

							msg.result = UPE_EOK;	
						}
						break;

						case FIRMWARE_TYPE_LNT_ZM:  //岭南通 
						{
							memset(&(msg.data), 0, sizeof(msg.data));

							if(up_config.lnt_zm.is_write_flash == UP_WR_STAT_WR)
								g_ftp_upstat[3].download_status = 0x03; //升级准备就绪
							else if(up_config.lnt_zm.is_download_complete == UP_DNL_STAT_NOT_COMPLETE)
								g_ftp_upstat[3].download_status = 0x01;  //正在下载
							else if(up_config.lnt_zm.is_download_complete == UP_DNL_STAT_COMPLETE)
								g_ftp_upstat[3].download_status = 0x02; //下载完成
								
							memcpy(&(msg.data), &(g_ftp_upstat[3]), sizeof(ftp_upgrade_status_t));

							msg.result = UPE_EOK;	
						}
						break;
						
						default:
						{
							msg.result = UPE_ERROR;	
						}
					}
					
					msg.cmd = UPE_CMD_GET_FTP_UPSTAT;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
				}
				break;

				case UPE_CMD_GET_FDNL_CTRL_INFO:  //FTP下载控制信息
				{
					fprintf(stderr, "UPE_CMD_GET_FDNL_CTRL_INFO: %d\n", UPE_CMD_GET_FDNL_CTRL_INFO);

					upgrade_config_get(&up_config);
					memcpy(&(msg.data), &(up_config.fdnl_ctrl_info),  sizeof(ftp_download_ctrl_info_t));
				
					msg.cmd = UPE_CMD_GET_FDNL_CTRL_INFO;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_EOK;	
				}
				break;

				default:  //默认
				{
					memset(&(msg.data), 0, sizeof(msg.data));
					msg.cmd = UPE_CMD_END;
					msg.d_addr = msg.s_addr;
					msg.s_addr = UPE_MSG_APP;
					msg.result = UPE_ERR_COMMAND;		 //不支持命令
				}
			}

			lib_msg_send(g_upgrade_qid, &msg, sizeof(upgrade_message_t));
		}
	}

err:
	curl_global_cleanup();  
	lib_msg_kill(g_upgrade_qid);

	lib_share_mem_destroy(upe_mid);  //释放共享内存
	
	lib_update_destroy(g_kernel_update);
	lib_update_destroy(g_firmware_update);
	lib_update_destroy(g_appl_update);

	fprintf(stderr, "Upgrade Quit!\n");


#if 0
	enum FC_RET f_ret = FC_check(argv[1]);
#endif

	return 0;
}

static size_t __fn_write(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
	return fwrite(ptr, size, nmemb, (FILE*)stream);  
}  

static int __fn_progress(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow)
{	
#if 0
	fprintf(stderr, "dltotal = %f\n", dltotal);
	fprintf(stderr, "dlnow = %f\n", dlnow);
	fprintf(stderr, "ultotal = %f\n", ultotal);
	fprintf(stderr, "ulnow = %f\n", ulnow);
	
	fprintf(stderr, "%f /%f (%g %%)\n", dlnow, dltotal, dlnow * 100.0 / dltotal);
#endif

	return UPE_EOK;
}

/*
 * FTP下载 
 */
static enum FTP_ERRNO __ftp_download(struct ftp_config *config, int *curl_code)
{
	char userpass[128] = {0};
	FILE *fp = NULL;
	curl_off_t local_file_len = 0;
	CURLcode curl_r = CURLE_GOT_NOTHING;
	struct stat f_stat;
	int use_resume = 0; //断点续传 
	CURL *curl_download = NULL;
	struct upgrade_config up_config;
	
	memset(&up_config, 0, sizeof(struct upgrade_config));
	memset(&f_stat, 0, sizeof(struct stat));
	sprintf(userpass, "%s:%s", config->username, config->passwd);

	fprintf(stderr, "%s: update type:%d, username:%s, passwd:%s, port: %d, firmware_name:%s, remote_path:%s, local_path:%s, connect_timeout:%ld, download_timeout:%ld\n", __FUNCTION__, 
						config->update_type, config->username, config->passwd, config->port, 
						config->firmware_name, config->remote_path, config->local_path, config->connect_timeout, config->download_timeout);

	SYS_LOG_DEBUG( "%s: update type:%d, firmware_name:%s, remote_path:%s\n", __FUNCTION__, 
						config->update_type,  config->firmware_name, config->remote_path);
	
	SYS_LOG_DEBUG( "%s:local_path:%s, connect_timeout:%ld, download_timeout:%ld\n", __FUNCTION__, config->local_path, config->connect_timeout, config->download_timeout);
	
	/* 获取本地文件大小 */
	if(stat(config->local_path, &f_stat) == 0)
	{
		local_file_len = f_stat.st_size;
		use_resume = 1;   //续传
	}

	/* 保存下载信息 */
	upgrade_config_get(&up_config);
	switch(config->update_type)
	{
		case UPE_TYPE_KERNEL:
		{
			strcpy(up_config.kernel.firmware_name, config->firmware_name);
			up_config.kernel.download_len = local_file_len;
			up_config.kernel.is_download_complete = UP_DNL_STAT_NOT_COMPLETE;
			up_config.kernel.is_write_flash = UP_WR_STAT_NOT_WR;
			up_config.kernel.is_write_parameter = UP_WR_STAT_NOT_WR;
		}
		break;

		case UPE_TYPE_FIRMWARE:
		{
			strcpy(up_config.firmware.firmware_name, config->firmware_name);
			up_config.firmware.download_len = local_file_len;
			up_config.firmware.is_download_complete = UP_DNL_STAT_NOT_COMPLETE;
			up_config.firmware.is_write_flash = UP_WR_STAT_NOT_WR;
			up_config.firmware.is_write_parameter = UP_WR_STAT_NOT_WR;
		}
		break;

		case UPE_TYPE_APPL:
		{
			strcpy(up_config.appl.firmware_name, config->firmware_name);
			up_config.appl.download_len = local_file_len;
			up_config.appl.is_download_complete = UP_DNL_STAT_NOT_COMPLETE;
			up_config.appl.is_write_flash = UP_WR_STAT_NOT_WR;
			up_config.appl.is_write_parameter = UP_WR_STAT_NOT_WR;
		}
		break;

		case UPE_TYPE_LNT_ZM:
		{
			strcpy(up_config.lnt_zm.firmware_name, config->firmware_name);
			up_config.lnt_zm.download_len = local_file_len;
			up_config.lnt_zm.is_download_complete = UP_DNL_STAT_NOT_COMPLETE;
			up_config.lnt_zm.is_write_flash = UP_WR_STAT_NOT_WR;
			up_config.lnt_zm.is_write_parameter = UP_WR_STAT_NOT_WR;
		}
		break;
	}

	up_config.fdnl_ctrl_info.download_status = 0x01;  //正在下载
	
	upgrade_config_put(&up_config);
	
	/* 断点续传 */
	fp = fopen(config->local_path, "ab+");
	if(fp == NULL)
	{
		SYS_LOG_ERR("%s:ftp download fopen %s failed!\n", __FUNCTION__, config->local_path);
		fprintf(stderr, "%s:ftp download fopen %s failed!\n", __FUNCTION__, config->local_path);
		
		return FTP_ERRNO_ERR;
	}

	curl_download = curl_easy_init();
	if(curl_download == NULL)
	{
		SYS_LOG_ERR("%s: curl_easy_init failed!\n", __FUNCTION__);
		fprintf(stderr, "%s: curl_easy_init failed!\n", __FUNCTION__);

		return FTP_ERRNO_ERR;
	}

	curl_easy_setopt(curl_download, CURLOPT_URL, config->remote_path);  
	curl_easy_setopt(curl_download, CURLOPT_USERPWD, userpass);  //设置用户名与密码。参数是格式如 “user:password ”的字符串
	curl_easy_setopt(curl_download, CURLOPT_CONNECTTIMEOUT, config->connect_timeout);   //连接超时
	curl_easy_setopt(curl_download, CURLOPT_TIMEOUT, config->download_timeout);   //下载超时

	fprintf(stderr, "FTP download timeout: %ld\n", config->download_timeout); 

	/* 设置断点续传 */
	curl_easy_setopt(curl_download, CURLOPT_RESUME_FROM_LARGE, use_resume ? local_file_len : 0);   //断点续传
	curl_easy_setopt(curl_download, CURLOPT_WRITEFUNCTION, __fn_write);   //写回调函数
	curl_easy_setopt(curl_download, CURLOPT_WRITEDATA, fp);    	//流指针

	/* 设置当传输速度<2900字节/秒持续10秒时,重新开始下载 add by zjc at 2016-11-28 */
	//curl_easy_setopt(curl_download, CURLOPT_LOW_SPEED_LIMIT, 2900L); //set low speed limit in bytes per second
	//curl_easy_setopt(curl_download, CURLOPT_LOW_SPEED_TIME, 10L); 

	/* 传输进度 */
	curl_easy_setopt(curl_download, CURLOPT_NOPROGRESS, 0L);		//0L开启进度输出
	curl_easy_setopt(curl_download, CURLOPT_PROGRESSFUNCTION, __fn_progress);
	curl_easy_setopt(curl_download, CURLOPT_PROGRESSDATA, NULL);
	curl_easy_setopt(curl_download, CURLOPT_VERBOSE, 1L);   		//将CURLOPT_VERBOSE属性设置为1，libcurl会输出通信过程中的一些细节

	__ftp_upgrade_download_status_set(config->update_type, 0x01); //正在下载

	curl_r = curl_easy_perform(curl_download);

	if(curl_code != NULL)
		*curl_code = curl_r;

	fprintf(stderr, "curl easy perform code:%d\n", curl_r);
	SYS_LOG_DEBUG("%s: curl easy perform code:%d\n", __FUNCTION__, curl_r);
	
	curl_easy_cleanup(curl_download);
	
	fclose(fp);

	SYS_LOG_NOTICE("%s: %s\n", __FUNCTION__, curl_easy_strerror(curl_r));

	if(curl_r != 0)
	{
		fprintf(stderr, "Remove Local Path File\n");
		remove(config->local_path);//删除文件
	}

	//if(curl_r == FTP_ERRNO_OPERATION_TIMEDOUT)
		//printf("-----------------time out!\n");
	
	switch(curl_r)
	{
		case 0:
			return FTP_ERRNO_OK;  //正常(0)
			
		case 7:  //CURLE_COULDNT_CONNECT (7)
			return FTP_ERRNO_COULDNT_CONNECT;   //connect失败

		case 28:  //CURLE_OPERATION_TIMEDOUT (28) //超时
			return FTP_ERRNO_OPERATION_TIMEDOUT;  

		case 67: //CURLE_LOGIN_DENIED（67）  //用户或密码出错
			return FTP_ERRNO_LOGIN_DENIED; 

		case 78: //CURLE_REMOTE_FILE_NOT_FOUND（78）  //下载文件不存在
			return FTP_ERRNO_REMOTE_FILE_NOT_FOUND;  

		default:
			return FTP_ERRNO_UNKNOWN;   //未知错误
	}

	return FTP_ERRNO_UNKNOWN; //未知错误
}

/*
 * 升级线程
 */
static void *__upgrade_thread(void *arg)
{
	int ret = UPE_ERROR;
	unsigned int notify = UPE_NOTIFY_INIT;
	lib_parameter_t parameter;
	upgrade_config_t up_config;
	int f_reboot = 0; //重启标志

	int upe_mid = *(int *)arg;  //共享内存mid
		
	while(1)
	{
		f_reboot = 0;
		notify = UPE_NOTIFY_INIT;
		__upgrade_notify_wait(&notify);
		if(notify == UPE_NOTIFY_RUN)
		{
			memset(&parameter, 0, sizeof(lib_parameter_t));
			memset(&up_config, 0, sizeof(struct upgrade_config));

			upgrade_config_get(&up_config);

			fprintf(stderr, "%s: upgrade type = %d\n", __FUNCTION__, g_upgrade_type);

			switch(g_upgrade_type)
			{
				case UPE_TYPE_KERNEL:
				{
					fprintf(stderr, "UPE_TYPE_KERNEL: total_len = %d\n", up_config.kernel.total_len);

					if((up_config.kernel.is_download_complete != UP_DNL_STAT_COMPLETE)  //固件没有下载成功或者固件没有写入flash，不执行升级
						|| (up_config.kernel.is_write_flash != UP_WR_STAT_WR)) 
					{
						SYS_LOG_DEBUG("UPE_TYPE_KERNEL Not write parameter\n");
						break;
					}
					
					parameter.magic = LIB_UP_PARA_MAGIC;
					strcpy(parameter.partition[UP_PARTITION_KERNEL].firmware_name, up_config.kernel.firmware_name);
					parameter.partition[UP_PARTITION_KERNEL].len = up_config.kernel.total_len;
					lib_get_systime_bcd(parameter.partition[UP_PARTITION_KERNEL].datetime);
					parameter.partition[UP_PARTITION_KERNEL].is_need_update = LIB_UP_PARA_UPDATE;
					parameter.crc16 = lib_crc16_with_byte((unsigned char *)&parameter, sizeof(lib_parameter_t) - 2);

					ret = lib_update_write_parameter(g_appl_update, &parameter);
					if(ret == LIB_UP_EOK)
					{
						SYS_LOG_NOTICE("%s: update write [kernel] parameter ok, file size:%d\n", __FUNCTION__, up_config.kernel.total_len);
						up_config.kernel.is_write_parameter = UP_WR_STAT_WR;
						f_reboot = 1;  //需要重启设备
					}
					else
					{
						SYS_LOG_ERR("%s: update write [kernel] parameter failed!", __FUNCTION__);
						up_config.kernel.is_write_parameter = UP_WR_STAT_NOT_WR;
						f_reboot = 0;
					}
				
					g_upgrade_type = UPE_TYPE_INIT;
					upgrade_config_put(&up_config);

					/* 重启设备 */
					if(f_reboot == 1)
						system("/mnt/firmware/reboot_wdt");
				}
				break;

				case UPE_TYPE_FIRMWARE:
				{
					fprintf(stderr, "UPE_TYPE_FIRMWARE: total_len = %d\n", up_config.firmware.total_len);

					if((up_config.firmware.is_download_complete != UP_DNL_STAT_COMPLETE) 
						|| (up_config.firmware.is_write_flash != UP_WR_STAT_WR))
					{
						SYS_LOG_DEBUG("UPE_TYPE_FIRMWARE Not write parameter\n");
						break;
					}

					parameter.magic = LIB_UP_PARA_MAGIC;
					strcpy(parameter.partition[UP_PARTITION_FIRMWARE].firmware_name, up_config.appl.firmware_name);
					parameter.partition[UP_PARTITION_FIRMWARE].len = up_config.firmware.total_len;
					lib_get_systime_bcd(parameter.partition[UP_PARTITION_FIRMWARE].datetime);
					parameter.partition[UP_PARTITION_FIRMWARE].is_need_update = LIB_UP_PARA_UPDATE;
					parameter.crc16 = lib_crc16_with_byte((unsigned char *)&parameter, sizeof(lib_parameter_t) - 2);

					ret = lib_update_write_parameter(g_firmware_update, &parameter);
					if(ret == LIB_UP_EOK)
					{
						SYS_LOG_NOTICE("%s: update write [firmware] parameter ok, file size:%d\n", __FUNCTION__, up_config.firmware.total_len);
						up_config.firmware.is_write_parameter = UP_WR_STAT_WR;
						f_reboot = 1;
					}
					else
					{
						SYS_LOG_ERR("%s: update write [firmware] parameter failed!", __FUNCTION__);
						up_config.firmware.is_write_parameter = UP_WR_STAT_NOT_WR;
						f_reboot = 0;
					}
					
					g_upgrade_type = UPE_TYPE_INIT;
					upgrade_config_put(&up_config);

					/* 重启设备 */
					if(f_reboot == 1)
						system("/mnt/firmware/reboot_wdt");
				}
				break;

				case UPE_TYPE_APPL:
				{
					fprintf(stderr, "UPE_TYPE_APPL: total_len = %d\n", up_config.appl.total_len);

					if((up_config.appl.is_download_complete != UP_DNL_STAT_COMPLETE) 
						|| (up_config.appl.is_write_flash != UP_WR_STAT_WR)) 
					{
						SYS_LOG_DEBUG("UPE_TYPE_APPL Not write parameter\n");
						break;
					}
					
					parameter.magic = LIB_UP_PARA_MAGIC;
					strcpy(parameter.partition[UP_PARTITION_APP].firmware_name, up_config.appl.firmware_name);
					parameter.partition[UP_PARTITION_APP].len = up_config.appl.total_len;
					lib_get_systime_bcd(parameter.partition[UP_PARTITION_APP].datetime);
					parameter.partition[UP_PARTITION_APP].is_need_update = LIB_UP_PARA_UPDATE;
					parameter.crc16 = lib_crc16_with_byte((unsigned char *)&parameter, sizeof(lib_parameter_t) - 2);

					ret = lib_update_write_parameter(g_appl_update, &parameter);
					if(ret == LIB_UP_EOK)
					{
						SYS_LOG_NOTICE("%s: update write [appl] parameter ok, file size:%d\n", __FUNCTION__, up_config.appl.total_len);
						up_config.appl.is_write_parameter = UP_WR_STAT_WR;
						f_reboot = 1;
					}
					else
					{
						SYS_LOG_ERR("%s: update write [appl] parameter failed!", __FUNCTION__);
						up_config.appl.is_write_parameter = UP_WR_STAT_NOT_WR;
						f_reboot = 0;
					}
					
					g_upgrade_type = UPE_TYPE_INIT;
					upgrade_config_put(&up_config);

					/* 重启设备 */
					if(f_reboot == 1)
						system("/mnt/firmware/reboot_wdt");
				}
				break;

				case UPE_TYPE_LNT_ZM:  //执行岭南通升级
				{
					fprintf(stderr, "UPE_TYPE_LNT_ZM: total_len = %d\n", up_config.lnt_zm.total_len);

					if((up_config.lnt_zm.is_download_complete != UP_DNL_STAT_COMPLETE) 
						|| (up_config.lnt_zm.is_write_flash != UP_WR_STAT_WR)) 
					{
						SYS_LOG_DEBUG("UPE_TYPE_LNT_ZM Not write parameter\n");
						break;
					}

					#if 0
					ret = lnt_reader_update(up_config.lnt_zm.ftp_local_path);

					fprintf(stderr, "lnt_reader_update ret = %d\n", ret);
					
					if(ret == LIB_UPDATE_OK)
					{
						SYS_LOG_NOTICE("%s: update write [LNT ZM] parameter ok, file size:%d\n", __FUNCTION__, up_config.lnt_zm.total_len);
						up_config.lnt_zm.is_write_parameter = UP_WR_STAT_WR;
						
						remove(up_config.lnt_zm.ftp_local_path);//删除文件
						
						f_reboot = 1;
					}
					else
					{
						SYS_LOG_ERR("%s: update write [LNT ZM] parameter failed!", __FUNCTION__);
						up_config.lnt_zm.is_write_parameter = UP_WR_STAT_NOT_WR;
						f_reboot = 0;
					}
					#endif

					static int upe_cnt = 0;
					lnt_firmware_update_result_t update_result;

					#if 1
					g_upgrade_type = UPE_TYPE_LNT_ZM;
					up_config.lnt_zm.is_write_parameter = UP_WR_STAT_NOT_WR_LNT; //表示要执行读卡器升级操作
					upgrade_config_put(&up_config);
					#endif
					
					while(1)
					{
						memset(&update_result, 0, sizeof(lnt_firmware_update_result_t));

						//读取读卡器升级结果
						lib_share_mem_read(upe_mid, 0, (char *)&update_result, sizeof(lnt_firmware_update_result_t));
						printf("--------------upgrade_app, update_result:%d, upe_cnt:%d\n", update_result.result, upe_cnt);
						
						if(update_result.result == 0) //初始化状态
						{
							up_config.lnt_zm.is_write_parameter = UP_WR_STAT_NOT_WR;
							
							upe_cnt++;
						
							if(upe_cnt > 60) //要确保时间足够升级完成!
							{
								upe_cnt = 0; //跳出循环前要清零，防止下次升级状态为0时该值>60直接跳出!
								break;
							}
					
							lib_sleep(2);

							continue;
						}
						else if(update_result.result == 1) //成功
						{
							f_reboot = 1;
							up_config.lnt_zm.is_write_parameter = UP_WR_STAT_WR;
						
							remove(up_config.lnt_zm.ftp_local_path);//删除文件

							break;
						}
						else  //失败
						{
							f_reboot = 0;
							upe_cnt = 0; //跳出循环前要清零，防止下次进入升级时不为0
							
							up_config.lnt_zm.is_write_parameter = UP_WR_STAT_NOT_WR;

							remove(up_config.lnt_zm.ftp_local_path);//删除文件 

							//升级失败后将升级结果初始化才能正常触发下一次升级操作!
							memset(&update_result, 0, sizeof(update_result));
							lib_share_mem_write(upe_mid, 0, (char *)&update_result, sizeof(lnt_firmware_update_result_t));

							#if 1
							//清空升级配置信息
							up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_LNT_ZM;
							g_upgrade_type = UPE_TYPE_LNT_ZM;
							memset(&(up_config.lnt_zm), 0, sizeof(struct firmware_config));
							upgrade_config_put(&up_config);	
							#endif
							
							break;
						}
					}

					#if 1
					g_upgrade_type = UPE_TYPE_INIT;
					upgrade_config_put(&up_config);
					
					/* 重启设备 */
					if(f_reboot == 1)
						system("/mnt/firmware/reboot_wdt");
					#endif
				}
				break;
			}
			__upgrade_notify_set(UPE_NOTIFY_WAIT);   
		}

		lib_sleep(1);	
	}

	return lib_thread_exit((void *)NULL);
}

/*
 * FTP下载线程
 */
#define	FTP_DOWNLOAD_TIMEOUT_LIMITS	1 //ftp下载超时次数限制 2->1 2016-12-26

static void *__ftp_download_thread(void *arg)
{
	unsigned int notify = UPE_NOTIFY_INIT;
	int ret = UPE_ERROR;
	enum FTP_ERRNO ftp_errno = FTP_ERRNO_UNKNOWN;
	int curl_code = CURLE_OK;
	struct upgrade_config up_config;    
	int fd = -1;
	struct stat f_stat;
	enum FC_RET fc_ret = FC_RET_ERR;
	struct FC_parameter fc_para;
	static int ftp_timeout_cnt; //ftp超时次数
	
	while(1)
	{
		__ftp_download_notify_wait(&notify);
		if(notify == UPE_NOTIFY_RUN) //执行
		{
			upgrade_config_get(&up_config);
			
			ftp_errno = __ftp_download(&g_ftp_config, &curl_code); //FTP下载
			
			fprintf(stderr, "-------------------FTP errno=%d, CURL code=%d\n", ftp_errno, curl_code);
			SYS_LOG_DEBUG("FTP errno=%d, CURL code=%d\n", ftp_errno, curl_code);

			//if(ftp_errno == FTP_ERRNO_OPERATION_TIMEDOUT)  //继续下载,超时
			if(ftp_errno != FTP_ERRNO_OK)
			{
				/* ftp下载超时控制 add by zjc at 2016-12-16 */
				ftp_timeout_cnt++; //下载超时计数
				
				if(ftp_timeout_cnt >= FTP_DOWNLOAD_TIMEOUT_LIMITS)
				{					
					SYS_LOG_DEBUG("FTP timeout %d times, stop it!!!\n", ftp_timeout_cnt);
					printf("-------------FTP timeout %d times!\n", ftp_timeout_cnt);

					#if 1
					__ftp_upgrade_errno_set(g_ftp_config.update_type, ftp_errno);
					__ftp_upgrade_download_status_set(g_ftp_config.update_type, 0xFF); //下载失败
					
					up_config.fdnl_ctrl_info.download_status = 0xFF; 
					upgrade_config_put(&up_config);	
					#endif
					
					//清空升级配置信息以及删除文件
					if(g_ftp_config.update_type == UPE_TYPE_FIRMWARE)
					{
						up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_FIRMWARE;
						g_upgrade_type = UPE_TYPE_FIRMWARE;
						
						memset(&(up_config.firmware), 0, sizeof(struct firmware_config));  

						remove(up_config.firmware.ftp_local_path);
					}
					else if(g_ftp_config.update_type == UPE_TYPE_APPL)
					{
						up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_APPL;
						g_upgrade_type = UPE_TYPE_APPL;

						memset(&(up_config.appl), 0, sizeof(struct firmware_config));  

						remove(up_config.appl.ftp_local_path);
					}
					
					upgrade_config_put(&up_config);	
					
					ftp_timeout_cnt = 0;
					
					__ftp_download_notify_set(UPE_NOTIFY_WAIT);
					continue;
				}
				else
				{
					__ftp_upgrade_recode_set(g_ftp_config.update_type, curl_code);
					__ftp_upgrade_errno_set(g_ftp_config.update_type, ftp_errno);
					__ftp_download_notify_set(UPE_NOTIFY_RUN);

					printf("================FTP timeout %d times!\n", ftp_timeout_cnt);

					lib_msleep(15000); //200-->15000 超时后没那么快重新下载
					continue;
				}
			}
			else
				ftp_timeout_cnt = 0;

			__ftp_upgrade_recode_set(g_ftp_config.update_type, curl_code);
			__ftp_upgrade_errno_set(g_ftp_config.update_type, ftp_errno);

			if(ftp_errno == FTP_ERRNO_COULDNT_CONNECT)  //conenct失败
			{
				__ftp_upgrade_recode_set(g_ftp_config.update_type, curl_code);
				__ftp_upgrade_errno_set(g_ftp_config.update_type, ftp_errno);
				__ftp_download_notify_set(UPE_NOTIFY_RUN);

				lib_sleep(10);
				continue;
			}

			__ftp_upgrade_recode_set(g_ftp_config.update_type, curl_code);
			__ftp_upgrade_errno_set(g_ftp_config.update_type, ftp_errno);
			
			if(ftp_errno == FTP_ERRNO_OK) //下载完成
			{
				__ftp_upgrade_download_status_set(g_ftp_config.update_type, 0x02); //下载完成
				
				fprintf(stderr, "ftp download finish, update type: %d\n", g_ftp_config.update_type);
				
				memset(&up_config, 0, sizeof(struct upgrade_config));	
				upgrade_config_get(&up_config);
				
				switch(g_ftp_config.update_type)
				{
					case UPE_TYPE_KERNEL:  //下载内核
					{
						//#ifdef UPE_USING_CHECK
						#if 1 //内核文件没有校验 add by zjc at 2016-11-15
						memset(&fc_para, 0, sizeof(struct FC_parameter));
						fc_ret = FC_check(up_config.kernel.ftp_local_path, &fc_para);
						fprintf(stderr, "UPE_TYPE_KERNEL FC check %d\n", fc_ret);
						SYS_LOG_NOTICE("%s: UPE_TYPE_KERNEL FC check %d\n", __FUNCTION__, fc_ret);
						if(fc_ret != FC_RET_SUCCESS) //校验失败
						{
							up_config.fdnl_ctrl_info.err = FTP_ERRNO_CRC;  //校验失败
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_CRC);  //校验失败
							remove(up_config.kernel.ftp_local_path);//删除文件
							break;
						}
						
						fd = open(up_config.kernel.ftp_local_path, O_RDONLY);
						fstat(fd, &f_stat);
						close(fd);

						truncate(up_config.kernel.ftp_local_path, f_stat.st_size - 256);  //裁剪文件大小(除去校验信息)

						#endif
						
						ret = lib_update_write_flash_skip(g_kernel_update, up_config.kernel.ftp_local_path); 
						if(ret == LIB_UP_EOK)
						{
							SYS_LOG_NOTICE("%s: update write [kernel] flash ok\n", __FUNCTION__);

							fd = open(up_config.kernel.ftp_local_path, O_RDONLY);
							fstat(fd, &f_stat);
							close(fd);

							up_config.kernel.download_len= f_stat.st_size;  //已下载长度
							up_config.kernel.total_len = f_stat.st_size;   //固件总长度
							up_config.kernel.is_download_complete = UP_DNL_STAT_COMPLETE;  //下载完成
							up_config.kernel.is_write_flash = UP_WR_STAT_WR;  //写入flash完成
							up_config.kernel.is_write_parameter = UP_WR_STAT_NOT_WR;
							lib_get_systime_bcd( up_config.kernel.last_datetime);

							__ftp_upgrade_download_status_set(g_ftp_config.update_type, 0x03);  //升级准备就绪
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_OK);  //正常

							up_config.fdnl_ctrl_info.download_status = 0x03;   //升级准备就绪
							up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_KERNEL;
								
							remove(up_config.kernel.ftp_remote_path); //删除文件
						}
						else
						{
							SYS_LOG_ERR("%s: update write [kernel] flash failed!\n", __FUNCTION__);
							
							up_config.kernel.is_write_flash = UP_WR_STAT_NOT_WR;
							up_config.kernel.is_write_parameter = UP_WR_STAT_NOT_WR;
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_WR);  //写入固件错误
							
							remove(up_config.kernel.ftp_remote_path); //删除文件
						}
						
						upgrade_config_put(&up_config);
						g_upgrade_type = UPE_TYPE_KERNEL;
					}
					break;
			
					case UPE_TYPE_FIRMWARE: //下载固件
					{
						#ifdef UPE_USING_CHECK
						memset(&fc_para, 0, sizeof(struct FC_parameter));
						fc_ret = FC_check(up_config.firmware.ftp_local_path, &fc_para);
						fprintf(stderr, "UPE_TYPE_FIRMWARE FC check %d\n", fc_ret);
						SYS_LOG_NOTICE("%s: UPE_TYPE_FIRMWARE FC check %d\n", __FUNCTION__, fc_ret);
						if(fc_ret != FC_RET_SUCCESS) //校验失败
						{
							up_config.fdnl_ctrl_info.err = FTP_ERRNO_CRC;  //校验失败
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_CRC);  //校验失败
							remove(up_config.firmware.ftp_local_path);//删除文件
							break;
						}
						
						fd = open(up_config.firmware.ftp_local_path, O_RDONLY);
						fstat(fd, &f_stat);
						close(fd);

						truncate(up_config.firmware.ftp_local_path, f_stat.st_size - 256);  //裁剪文件大小
						#endif

						
						ret = lib_update_write_flash_skip(g_firmware_update, up_config.firmware.ftp_local_path);
						if(ret == LIB_UP_EOK)
						{
							SYS_LOG_NOTICE("%s: update write [firmware] flash ok\n", __FUNCTION__);
							fprintf(stderr, "%s: update write [firmware] flash ok\n", __FUNCTION__);

							fd = open(up_config.firmware.ftp_local_path, O_RDONLY);
							fstat(fd, &f_stat);
							close(fd);
							
							up_config.firmware.download_len= f_stat.st_size;  //已下载长度
							up_config.firmware.total_len = f_stat.st_size;   //固件总长度
							up_config.firmware.is_download_complete = UP_DNL_STAT_COMPLETE;
							up_config.firmware.is_write_flash = UP_WR_STAT_WR;
							up_config.firmware.is_write_parameter = UP_WR_STAT_NOT_WR;
							lib_get_systime_bcd( up_config.firmware.last_datetime);

							__ftp_upgrade_download_status_set(g_ftp_config.update_type, 0x03);  //升级准备就绪
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_OK);  //正常

							up_config.fdnl_ctrl_info.download_status = 0x03;   //升级准备就绪
							up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_FIRMWARE;
							
							remove(up_config.firmware.ftp_local_path);  //删除文件
						}
						else
						{
							SYS_LOG_ERR("%s: update write [firmware] flash failed!\n", __FUNCTION__);
							fprintf(stderr, "%s: update write [firmware] flash failed!\n", __FUNCTION__);
							
							up_config.firmware.is_write_flash = UP_WR_STAT_NOT_WR;
							up_config.firmware.is_write_parameter = UP_WR_STAT_NOT_WR;
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_WR);  //写入固件错误
							
							remove(up_config.firmware.ftp_local_path);  //删除文件
						}
						
						upgrade_config_put(&up_config);	
						g_upgrade_type = UPE_TYPE_FIRMWARE;
					}
					break;

  					case UPE_TYPE_APPL: //下载APP
					{
						/* 校验固件,如果检验失败,写检验失败标志,并删除固件 */
						#ifdef UPE_USING_CHECK
						memset(&fc_para, 0, sizeof(struct FC_parameter));
						fc_ret = FC_check(up_config.appl.ftp_local_path, &fc_para);
						fprintf(stderr, "UPE_TYPE_APPL FC check %d\n", fc_ret);
						SYS_LOG_NOTICE("%s: UPE_TYPE_APPL FC check %d\n", __FUNCTION__, fc_ret);
						if(fc_ret != FC_RET_SUCCESS) //校验失败
						{
							up_config.fdnl_ctrl_info.err = FTP_ERRNO_CRC;  //校验失败
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_CRC);  //校验失败
							remove(up_config.appl.ftp_local_path);//删除文件
							break;
						}
						
						fd = open(up_config.appl.ftp_local_path, O_RDONLY);
						fstat(fd, &f_stat);
						close(fd);

						truncate(up_config.appl.ftp_local_path, f_stat.st_size - 256);  //裁剪文件大小
						#endif
						
						ret = lib_update_write_flash_skip(g_appl_update, up_config.appl.ftp_local_path);  //写文件到flash
						if(ret == LIB_UP_EOK)  //写成功
						{
							SYS_LOG_NOTICE("%s: update write [appl] flash ok\n", __FUNCTION__);
							fprintf(stderr, "%s: update write [appl] flash ok\n", __FUNCTION__);

							fd = open(up_config.appl.ftp_local_path, O_RDONLY);
							fstat(fd, &f_stat);
							close(fd);

							up_config.appl.download_len= f_stat.st_size;  //已下载长度
							up_config.appl.total_len = f_stat.st_size;   //固件总长度
							up_config.appl.is_download_complete = UP_DNL_STAT_COMPLETE;
							up_config.appl.is_write_flash = UP_WR_STAT_WR;
							up_config.appl.is_write_parameter = UP_WR_STAT_NOT_WR;
							lib_get_systime_bcd( up_config.appl.last_datetime);

							__ftp_upgrade_download_status_set(g_ftp_config.update_type, 0x03);  //升级准备就绪
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_OK);  //正常

							up_config.fdnl_ctrl_info.download_status = 0x03;  //升级准备就绪
							up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_APPL;
							
							remove(up_config.appl.ftp_local_path);//删除文件
						}
						else
						{
							SYS_LOG_ERR("%s: update write [appl] flash failed!\n", __FUNCTION__);
							fprintf(stderr, "%s: update write [appl] flash failed!\n", __FUNCTION__);
							
							up_config.appl.is_write_flash = UP_WR_STAT_NOT_WR;
							up_config.appl.is_write_parameter = UP_WR_STAT_NOT_WR;
							__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_WR);  //写入固件错误
							
							remove(up_config.appl.ftp_local_path);//删除文件
						}
						
						upgrade_config_put(&up_config);	
						g_upgrade_type = UPE_TYPE_APPL;
					}
					break;

					case UPE_TYPE_LNT_ZM: //下载LNT
					{
						fd = open(up_config.lnt_zm.ftp_local_path, O_RDONLY);
						fstat(fd, &f_stat);
						close(fd);

						up_config.lnt_zm.download_len= f_stat.st_size;  //已下载长度
						up_config.lnt_zm.total_len = f_stat.st_size;   //固件总长度
						up_config.lnt_zm.is_download_complete = UP_DNL_STAT_COMPLETE;
						up_config.lnt_zm.is_write_flash = UP_WR_STAT_WR;
						up_config.lnt_zm.is_write_parameter = UP_WR_STAT_NOT_WR;
						lib_get_systime_bcd( up_config.lnt_zm.last_datetime);

						__ftp_upgrade_download_status_set(g_ftp_config.update_type, 0x03);  //升级准备就绪
						__ftp_upgrade_errno_set(g_ftp_config.update_type, FTP_ERRNO_OK);  //正常

						up_config.fdnl_ctrl_info.download_status = 0x03;  //升级准备就绪
						up_config.fdnl_ctrl_info.ftype = FIRMWARE_TYPE_LNT_ZM;

						upgrade_config_put(&up_config);	
						g_upgrade_type = UPE_TYPE_LNT_ZM;
					}
					break;
					
				}	
				__ftp_download_notify_set(UPE_NOTIFY_WAIT);
			}
			
			lib_sleep(1);
		}
		
		lib_sleep(1);
		__ftp_download_notify_set(UPE_NOTIFY_WAIT);
	}

	return lib_thread_exit((void *)NULL);
}



























