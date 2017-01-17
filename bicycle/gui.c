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
#include <features.h>
#include <linux/reboot.h>

#include "defines.h"
#include "lib_general.h"
#include "lib_eventloop.h"
#include "gui_info.h"
#include "lib_watchdog.h"
#include "database.h"
#include "gps.h"
#include "ndev_info.h"
#include "external_interface_board.h"
#include "unity_file_handle.h"
#include "sae_protocol.h"
#include "configs.h"

#include "gui.h"
#include "lib_upgrade.h"



#define GUI_LIB_VER				"gui library version 1.0.3 2015/10/09/01"

extern lib_wl_t *g_bicycle_wl;

extern lib_upgrade_t *g_upgrade;
extern ndev_ftp_download_ctrl_info_t g_ftp_dnl_ctrl_info;  //FTP固件下载信息


/*
 * 报文结构
 */
#define GUI_PKG_LEN			8192      //MTU最大为1500
#define GUI_HEAD_LEN		7
struct gui_package
{
	unsigned char id_h;
	unsigned char id_l;
	unsigned short cmd;
	unsigned char result;
	unsigned short length;	//数据长度
	unsigned char data[GUI_PKG_LEN - GUI_HEAD_LEN];
}__attribute__((packed));
typedef struct gui_package gui_package_t;


#define CLI_INFO_NUM	8
struct client_info
{
	int sockfd;
};



#define GUI_ID_H			0x55
#define GUI_ID_L			0xaa


#define GUI_UNIX_DOMAIN					"/tmp/lib_gui_unix.domain"
#define GUI_PERM							(0777)
#define GUI_TCP_SERV_PORT				10086
#define GUI_SERIAL_TTY					"/dev/ttyO4"


#define GUI_ENABLE_KEEPALIVE				1    //开启keepalie属性
#define GUI_KEEPIDLE						45  //如果在keepidle(秒)时间内没有任何数据传输,则进行探测
#define GUI_KEEPINTERVAL					15  //探测发包时间间隔为keepinterval(秒)
#define GUI_KEEPCOUNT					3    //探测次数(次)


/*
 * 操作指令
 */
#define GUI_CMD_GET_VER								0x1001	  //获取库版本
#define GUI_CMD_GET_NDEV_STAT						0x1002        //获取节点机状态
#define GUI_CMD_GET_SAE_STAT						0x1003        //获取桩状态

#define GUI_CMD_GET_NDEV_VERSION					0x1004      //节点机软件版本
#define GUI_CMD_GET_NDEV_CONFIG					0x1005      //节点机设备配置
#define GUI_CMD_GET_STAKE_CONFIG					0x1006     //桩机设备配置


#define GUI_CMD_GET_NDEV_PAGE_CONFIG				0x1007      //节点机配置
#define GUI_CMD_GET_BASIC_INFO_PAGE_CONFIG		0x1008      //基本信息
#define GUI_CMD_GET_STAKE_INFO_PAGE_CONFIG		0x1009      //锁桩信息
#define GUI_CMD_GET_NDEV_ACCESS_CONFIG			0x100a   //获取网络接入配置
#define GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1	0x100b    //获取节点首页信息,阶段一
#define GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE2	0x100c   //获取节点首页信息,阶段二
#define GUI_CMD_GET_STAKE_ALL_INFO_CONFIG			0x100d  //获取锁桩全部信息
#define GUI_CMD_GET_CMP_INFO						0x100e  //对比信息
#define GUI_CMD_GET_ACCESS_STATE					0x100f  //接入方式,状态
#define GUI_CMD_GET_GPS_INFO						0x1010  //GPS信息 
#define GUI_CMD_GET_MEMORY_INFO					0x1011  //内存信息
#define GUI_CMD_GET_IBOARD_DEVICE_VERSION			0x1012  //接口板版本信息
#define GUI_CMD_GET_LIGHT_CTRL_TIME				0x1013 //灯箱控制
#define GUI_CMD_GET_UNITY_FILE_INFO				0x1014 //泛文件信息
#define GUI_CMD_GET_LNT_ALL_INFO					0x1015  //岭南通信息


#define GUI_CMD_SET_NDEV_IP_CONFIG					0x8007	//节点机IP地址
#define GUI_CMD_SET_NDEV_WIFI_IP_CONFIG			0x8008	//WIFI IP地址
#define GUI_CMD_SET_NDEV_CENTER_IP_CONFIG			0x8009     //自行车中心地址
#define GUI_CMD_SET_NDEV_FTP_CONFIG				0x800a    //FTP地址
#define GUI_CMD_SET_NDEV_PARAM_CONFIG				0x800b	//节点机参数配置
#define GUI_CMD_SET_STAKE_PARAM_CONFIG			0x800c    //锁桩参数配置
#define GUI_CMD_SET_NDEV_ACCESS_CONFIG			0x800d    //节点机网络接入配置
#define GUI_CMD_SET_LNT_CARD_STAT					0x800e    //岭南通读卡器状态
#define GUI_CMD_SET_ADMIN_CARD_INFO				0x800f    //管理卡信息
#define GUI_CMD_SET_EXCEPT_HANDLE_REC				0x8010    //异常处理扣费交易记录
#define GUI_CMD_SET_EXCEPT_HANDLE_REQ				0x8011    //异常处理请求
#define GUI_CMD_SET_EXCEPT_HANDLE_ACK				0x8012    //异常处理确认
#define GUI_CMD_SET_LNT_CONFIG						0x8013  //岭南通配置
#define GUI_CMD_SET_RENT_INFO_REQ					0x8014  //租还车记录请求
#define GUI_CMD_SET_RENT_INFO_ACK					0x8015  //租还车记录确认
#define GUI_CMD_SET_LIGHT_BOX						0x8016  //控制灯箱 
#define GUI_CMD_SET_LNT_ALL_INFO					0x8017  //岭南通信息
#define GUI_CMD_SET_DELAY_BICYCLE_REQ			0x8018   //延迟还车请求
#define GUI_CMD_SET_DELAY_BICYCLE_ACK			0x8019   //延迟还车确认

/* 附近网点信息查询指令 add by zjc at 2016-11-03 */
#define GUI_CMD_NEARBY_SITE_INFO_REQ					0x8020  //附近网点信息请求
#define GUI_CMD_NEARBY_SITE_INFO_ACK					0x8021  //附近网点信息确认

/* 周期性备份可用内存等系统信息指令 add by zjc at 2016-11-16 */
#define GUI_CMD_SYS_INFO_BACKUP		0x9010


typedef struct async_notify  
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;
}async_notify_t;




/*
 * 指令操作返回结果
 */
#define GUI_NULL						(NULL) 
#define GUI_EOK						(0)  //正常
#define GUI_ERROR					(-1) //错误
#define GUI_ETIMEOUT					(-2) //超时
#define GUI_EFULL					(-3) //满
#define GUI_EEMPTY					(-4) //空
#define GUI_ENOMEM 					(-5) //内存不够
#define GUI_EXCMEM					(-6) //内存越界
#define GUI_EBUSY					(-7) //忙
#define GUI_NOT_CMD     				(-8) //不支持的命令


#define GUI_REBOOT					(99) //设备需要重启
#define GUI_STAT1					(100)  //状态1

/* 协议返回结果 */
enum GUI_RESULT
{
	GUI_RESULT_OK = 1,
	GUI_RESULT_ERR = 2,
	GUI_RESULT_REBOOT = 3,
	GUI_RESULT_STAT1 = 4,
	GUI_RESULT_PKG_ERR = 5,
	GUI_RESULT_NOT_CMD = 6
};


/*
 * 定时器
 */
 #define IBOARD_TIME_MS			(120000)  //120秒
 #define GPS_TIMER_MS			(5000) //5秒
#define SYS_INFO_TIMER_MS		   (10000) //10秒

#if 0
struct gps_rxbuf
{
	int vaild;
	int size;
	unsigned char string[512];
}__attribute__((packed));
#endif

static struct gui_gps_info g_gps_info;  //gps信息
static struct gps_rxbuf g_gps_rxbuf;  //GPS数据
struct gps_rxbuf g_gps_rxbuf2;  //GPS数据
static gps_handle_t *g_gps_hndl = NULL;
static struct lnt_all_info g_lnt_info;

static lib_event_loop_t *g_eventloop = NULL;
static int g_unix_sockfd = -1;
static int g_tcp_sockfd = -1;
//static lib_serial_t g_serial;
extern database_t *g_database;
static async_notify_t g_sync_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NDEV_NOTIFY_INIT};
static unsigned int g_sockfd_type  = 0;

static long long g_iboard_id = 0;
static long long g_gps_id = 0;
static long long g_sys_info_id = 0;

static gui_ndev_home_page_info_t g_ndev_home_page_info;  //节点机主页信息

static void __unix_accept_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);
static void __unix_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);
static void __gps_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);

static int __unix_pkg(gui_package_t *pkg, int len);
static int __is_unix_package(gui_package_t *pkg);
static void __unix_package_create(gui_package_t *pkg, const unsigned short cmd, const char result, const unsigned short length);

static int __unix_package_writen(int sockfd, const void *buff, const unsigned int len);
static int __unix_package_readn_select(int sockfd, void *buff, const unsigned int len, const unsigned int msec);


static void __tcp_accept_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);
static void __tcp_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);

//static void __serial_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);

static int __gui_package_explain(const int sockfd, void *ptr, const int len);
static void *__async_handle_thread(void *arg);  //异步处理线程

static int __iboard_time_proc(lib_event_loop_t *ep, long long id, void *client_data);
static int __gps_time_proc(lib_event_loop_t *ep, long long id, void *client_data);
static int __sys_info_time_proc(lib_event_loop_t *ep, long long id, void *client_data);



static int __sync_notify_put(const unsigned int notify)
{
	pthread_mutex_lock(&g_sync_notify.mutex);

	g_sync_notify.notify = notify;

	pthread_mutex_unlock(&g_sync_notify.mutex);

	return pthread_cond_signal(&g_sync_notify.cond);
}

static int __sync_notify_wait(unsigned int *notify)
{
	int err = B_DEV_ERROR;

	pthread_mutex_lock(&g_sync_notify.mutex);

	err = pthread_cond_wait(&g_sync_notify.cond, &g_sync_notify.mutex);
	*notify = g_sync_notify.notify;
	
	pthread_mutex_unlock(&g_sync_notify.mutex);

	return err;
}









int gui_init(void)
{
	int err = B_DEV_ERROR;
	pthread_t async_thr;
	struct gps_config gconf;
	int i;
	
	memset(&g_ndev_home_page_info, 0, sizeof(gui_ndev_home_page_info_t));
	memset(&gconf, 0, sizeof(struct gps_config));
		
	g_eventloop = lib_event_loop_create(8);
	if(g_eventloop == LIB_EP_NULL)
		goto ERR;

	/* 定时器 */
	g_iboard_id = lib_event_loop_time_create(g_eventloop, IBOARD_TIME_MS, __iboard_time_proc, NULL, NULL); 
	fprintf(stderr, "g_iboard_id = %u\n", g_iboard_id);

	g_gps_id = lib_event_loop_time_create(g_eventloop, GPS_TIMER_MS, __gps_time_proc, NULL, NULL); 
	fprintf(stderr, "g_gps_id = %u\n", g_gps_id);

	/* 周期性备份可用内存等系统信息 add by zjc at 2017-01-16 */
	g_sys_info_id = lib_event_loop_time_create(g_eventloop, SYS_INFO_TIMER_MS, __sys_info_time_proc, NULL, NULL); 
	fprintf(stderr, "g_sys_info_id = %u\n", g_sys_info_id);
	

	/* UNIX域协议 */
	g_unix_sockfd = lib_unix_server_new(GUI_UNIX_DOMAIN, GUI_PERM, 8); 
	if(g_unix_sockfd == LIB_GE_ERROR)
	{
		fprintf(stderr, "gui unix server new failed!\n");
		goto ERR;
	}

	/* TCP server */
	g_tcp_sockfd = lib_tcp_server_nonb_new(GUI_TCP_SERV_PORT, 8);
	if(g_tcp_sockfd == LIB_GE_ERROR)
	{
		fprintf(stderr, "gui tcp server new failed!\n");
		goto ERR;		
	}

	/* serial */
#if 0
	memset(&g_serial, 0, sizeof(lib_serial_t));
	strcpy(g_serial.pathname, GUI_SERIAL_TTY);
	g_serial.flags = O_RDWR;
	g_serial.speed = 115200;  //波特率
	g_serial.databits = 8;
	g_serial.stopbits = 1;

	err = lib_serial_init(&g_serial);
	if(err == LIB_GE_ERROR)
	{
		fprintf(stderr, "gui serial server new failed!\n");
		goto ERR;			
	}
#endif

	gconf.ms_rate = GPS_MS_RATE;
	gconf.baudrate = GPS_BAUDRATE;
	strcpy(gconf.pathname, GPS_PATHNAME);
	g_gps_hndl = gps_create(&gconf);
	if(g_gps_hndl == NULL)
	{
		fprintf(stderr, "gps create failed!!!!!\n");
	}

	memset(&g_gps_info, 0, sizeof(g_gps_info));
	memset(&g_gps_rxbuf, 0, sizeof(struct gps_rxbuf));

	/* unix accept */
	err = lib_event_loop_add(g_eventloop, g_unix_sockfd, LIB_EP_READ, __unix_accept_proc, NULL, 0);
	if(err == LIB_EP_ERR)
	{
		fprintf(stderr, "gui event loop add UNIX sockfd failed!\n");
		goto ERR;
	}
              
	/* tcp accept */
	err = lib_event_loop_add(g_eventloop, g_tcp_sockfd, LIB_EP_READ, __tcp_accept_proc, NULL, 0);
	if(err == LIB_EP_ERR)
	{
		fprintf(stderr, "gui event loop add TCP sockfd failed!\n");
		goto ERR;
	}

	/* serial read */
#if 0
	err = lib_event_loop_add(g_eventloop, g_serial.sfd, LIB_EP_READ, __serial_read_proc, NULL, 0);
	if(err == LIB_EP_ERR)
	{
		fprintf(stderr, "gui event loop add serial read fd failed!\n");
		goto ERR;
	}
#endif

	/* gps */
	int fd = gps_get_fd(g_gps_hndl);
	if(fd > 0)
	{
		lib_setfd_noblock(fd);  //设置非阻塞
		err = lib_event_loop_add(g_eventloop, fd,  LIB_EP_READ, __gps_read_proc, NULL, 0);
		if(err == LIB_EP_ERR)
		{
			fprintf(stderr, "gui event loop add gps sockfd failed!\n");
			goto ERR;
		}
	}

	lib_normal_thread_create(&async_thr, __async_handle_thread, NULL);
	
	fprintf(stderr, "gui init success\n");
	
	return B_DEV_EOK;

ERR:
	//lib_serial_close(&g_serial);
	lib_disconnect(g_tcp_sockfd);
	lib_unix_close(g_unix_sockfd);
	lib_event_loop_destroy(g_eventloop);
	
	fprintf(stderr, "gui quit!\n");
	
	return B_DEV_ERROR;
}

void gui_loop(void)
{
	fprintf(stderr, "gui loop\n");
	
	lib_event_loop(g_eventloop);
}

int gui_destroy(void)
{
	//lib_serial_close(&g_serial);
	gps_destroy(g_gps_hndl);
	lib_disconnect(g_tcp_sockfd);
	lib_unix_close(g_unix_sockfd);
 	lib_event_time_del(g_eventloop, g_iboard_id);
	lib_event_loop_destroy(g_eventloop);

	fprintf(stderr, "gui destroy!\n");
	
	return B_DEV_EOK;
}

void  gui_stop(void)
{
	lib_event_loop_stop(g_eventloop);
}

static void __unix_accept_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask)
{
	int cli_sockfd = -1;
	struct sockaddr_un cli_addr;
	
	memset(&cli_addr, 0, sizeof(struct sockaddr_un));

	while((cli_sockfd = lib_unix_accept(fd, &cli_addr) ) < 0)
	{
		if((errno == EAGAIN) || (errno == EINTR))
		{
			SYS_LOG_ERR("GUI UNIX accept continue failed!\n");
			continue;
		}
		else
		{
			fprintf(stderr, "GUI UNIX  gui accept: %s\n", strerror(errno));
			return;
		}
	}

	SYS_LOG_ERR("GUI UNIX accept client sockfd %d\n", cli_sockfd);

	fprintf(stderr, "GUI UNIX client sockfd: %d\n", cli_sockfd);
	
	lib_setsock_noblock(cli_sockfd);
	lib_event_loop_add(g_eventloop, cli_sockfd, LIB_EP_READ, __unix_read_proc, NULL, 0);	
}

static void __tcp_accept_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask)
{
	int cli_sockfd = -1;
	struct sockaddr_in cli_addr;
	
	memset(&cli_addr, 0, sizeof(struct sockaddr_in));

	while((cli_sockfd = lib_tcp_accept(fd, &cli_addr) )< 0)
	{
		if((errno == EAGAIN) || (errno == EINTR))
		{
			//SYS_LOG_ERR("GUI TCP accept continue failed!!!!\n");
			continue;
		}
		else
		{
			fprintf(stderr, "GUI TCP accept: %s\n", strerror(errno));
			return;
		}
	}

	//SYS_LOG_ERR("GUI TCP accept client sockfd %d\n", cli_sockfd);

	fprintf(stderr, "GUI TCP client sockfd: %d\n", cli_sockfd);

	lib_setsock_noblock(cli_sockfd);
	lib_tcp_keepalive_set(cli_sockfd, GUI_ENABLE_KEEPALIVE, GUI_KEEPIDLE, GUI_KEEPINTERVAL, GUI_KEEPCOUNT); //设置客户超时
	lib_event_loop_add(g_eventloop, cli_sockfd, LIB_EP_READ, __tcp_read_proc, NULL, 0);	
}

static void __unix_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask)
{
	int n = -1;
	unsigned char rxbuf[GUI_PKG_LEN] = {0};
	int i;
	
	while(1)
	{
		memset(rxbuf, 0, GUI_PKG_LEN);
		n = lib_tcp_read(fd, rxbuf, GUI_PKG_LEN);
		if(n > 0)
		{
			/* 指令解释执行 */
			__gui_package_explain(fd, rxbuf, n);
			
			break;
		}
		else if(n == 0)
		{
			fprintf(stderr, "gui unix read proc Quit!\n");
			
			SYS_LOG_ERR("GUI UNIX client sockfd %d QUIT\n", fd);

			lib_unix_close(fd);
			lib_event_loop_del(g_eventloop, fd, LIB_EP_READ);
			return;
		}
		else if(n < 0)
		{			
			if(errno == ETIMEDOUT) 
			{
				fprintf(stderr, "gui unix read proc error: ETIMEDOUT!\n");
				
				SYS_LOG_ERR("GUI UNIX client sockfd %d ETIMEDOUT\n", fd);

				lib_unix_close(fd);
				lib_event_loop_del(g_eventloop, fd, LIB_EP_READ);
				return;
			}

			fprintf(stderr, "gui unix read proc error!\n");

			SYS_LOG_ERR("GUI UNIX client sockfd %d CLOSE\n", fd);
	
			lib_unix_close(fd);
			lib_event_loop_del(g_eventloop, fd, LIB_EP_READ);
			return;
		}
	}
}

static void __tcp_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask)
{
	int n = -1;
	unsigned char rxbuf[GUI_PKG_LEN] = {0};
		
	while(1)
	{
		memset(rxbuf, 0, GUI_PKG_LEN);
		
		n = lib_tcp_read(fd, rxbuf, GUI_PKG_LEN);
		if(n > 0)
		{
			/* 读取数据后,关闭读端sockfd */
			lib_shutdown(fd, SHUT_RD);

			/* 标识TCP类型 */
			g_sockfd_type = 1;
			
			/* 指令解释执行 */
			__gui_package_explain(fd, rxbuf, n);

			break;
		}
		else if(n == 0)
		{
			fprintf(stderr, "gui tcp read proc Quit!\n");

			//SYS_LOG_ERR("GUI TCP client sockfd %d QUIT\n", fd);

			lib_disconnect(fd);
			lib_event_loop_del(g_eventloop, fd, LIB_EP_READ);  //删除
			return;
		}
		else if(n < 0)
		{
			if(errno == ETIMEDOUT) 
			{
				fprintf(stderr, "gui tcp read proc error: ETIMEDOUT!\n");

				//SYS_LOG_ERR("GUI TCP client sockfd %d ETIMEDOUT\n", fd);

				lib_disconnect(fd);
				lib_event_loop_del(g_eventloop, fd, LIB_EP_READ);
				return;
			}

			fprintf(stderr, "gui tcp read proc error!\n");

			//SYS_LOG_ERR("GUI TCP client sockfd %d CLOSE\n", fd);

			lib_disconnect(fd);
			lib_event_loop_del(g_eventloop, fd, LIB_EP_READ);
			return;
		}
	}
}

#if 0
static void __serial_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask)
{
	int n = -1;
	unsigned char rxbuf[GUI_PKG_LEN] = {0};
		
	while(1)
	{
		memset(rxbuf, 0, GUI_PKG_LEN);
		
		n = lib_serial_recv(fd, rxbuf, GUI_PKG_LEN);
		if(n > 0)
		{
			/* 读取串口数据 */

			//lib_printf_v2("-----------__serial_read_proc------------\n", rxbuf, n, 16);


			
			
			break;
		}

		fprintf(stderr, "-------__serial_read_proc----\n");
	}	
}
#endif


static int __unix_pkg(gui_package_t *pkg, int len)
{
	unsigned char tbuf[GUI_PKG_LEN] = {0};
	unsigned short length = 0;
	int i, s_pos = 1;

	memcpy(&tbuf, pkg, len);
	
	for(i = 0; i < len; i++)
	{
		if((tbuf[i] == GUI_ID_H) && (s_pos == 1))
		{
			s_pos = 2;
			continue;
		}

		if((tbuf[i] == GUI_ID_L) && (s_pos == 2) && (tbuf[i - 1] == GUI_ID_H))
		{
			memset(pkg, 0, sizeof(gui_package_t));
			memcpy(&length, &tbuf[i + 4], 2);

			memcpy(pkg, &tbuf[i-1], length + GUI_HEAD_LEN);

			return B_DEV_EOK;
		}
	}

	return B_DEV_ERROR;
}

static int __is_unix_package(gui_package_t *pkg)
{
	if((pkg->id_h == GUI_ID_H) && (pkg->id_l == GUI_ID_L))
		return B_DEV_TRUE;
	
	return B_DEV_FALSE;
}

static void __unix_package_create(gui_package_t *pkg, const unsigned short cmd, char result, const unsigned short length)
{
	memset(pkg, 0, sizeof(gui_package_t));
	
	pkg->id_h = GUI_ID_H;
	pkg->id_l = GUI_ID_L;
	pkg->cmd = cmd;
	pkg->result = result;
	pkg->length = length;
}


static int __unix_package_writen(int sockfd, const void *buff, const unsigned int len)
{
	return lib_tcp_writen(sockfd, buff, len + GUI_HEAD_LEN);
}

static int __unix_package_readn_select(int sockfd, void *buff, const unsigned int len, const unsigned int msec)
{
	return lib_tcp_read_select(sockfd, buff, GUI_PKG_LEN, msec);
}

/*
 * 协议解释 
 */ 
static int __gui_package_explain(const int sockfd, void *ptr, const int len)
{
	unsigned char is_device_reboot = 0;
	char result = GUI_RESULT_PKG_ERR;
	int ret = B_DEV_ERROR;
	int cpy_len = 0;
	unsigned short data_len = 0;
	gui_package_t pkg;
	unsigned short cmd = 0;
	int pkg_sz = sizeof(struct gui_package);
	
	if(len > pkg_sz)
		cpy_len = pkg_sz;
	else
		cpy_len = len;

	//lib_printf_v2("------------__unix_package_explain recv--------------", ptr, len, 16);

	memset(&pkg, 0, sizeof(gui_package_t));
	memcpy(&pkg, ptr, cpy_len);  //拷贝数据
	
	if(__unix_pkg(&pkg, len) != B_DEV_EOK)
	{
		fprintf(stderr, "GUI Unix PKG error!!!\n");

		SYS_LOG_ERR("GUI UNIX PKG errror!\n");

		pkg.result = GUI_RESULT_PKG_ERR;
		data_len = 0;
		goto Done;
	}

	if(__is_unix_package(&pkg) != B_DEV_TRUE)
	{
		fprintf(stderr, "GUI IS UNIX package error!!!\n");

		SYS_LOG_ERR("GUI IS UNIX package errror!\n");
	
		pkg.result = GUI_RESULT_PKG_ERR;
		data_len = 0;
		goto Done;
	}

	fprintf(stderr, "pkg.result = %d\n", pkg.result);

	if(pkg.result == GUI_RESULT_REBOOT)  //设备重启
		is_device_reboot = 1;

	cmd = pkg.cmd;  //命令
	
	switch(cmd)
	{
		case GUI_CMD_GET_VER: //获取版本号
		{
			fprintf(stderr, "GUI_CMD_GET_VER:0x%02x\n", GUI_CMD_GET_VER);

			data_len = 32;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			strcpy(pkg.data, GUI_LIB_VER);
			
		}
		break;

		case GUI_CMD_GET_NDEV_STAT: //获取节点机状态
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_STAT:0x%02x\n", GUI_CMD_GET_NDEV_STAT);

			ndev_status_t stat;
			
			data_len = sizeof(ndev_status_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			gui_get_ndev_stat(&stat);
			pkg.result = GUI_RESULT_OK;
			memcpy(&(pkg.data), &stat, data_len);
		}
		break;
		
		case GUI_CMD_GET_SAE_STAT:  //获取锁桩状态
		{
			fprintf(stderr, "GUI_CMD_GET_SAE_STAT:0x%02x\n", GUI_CMD_GET_SAE_STAT);

			unsigned char id = pkg.data[0]; //锁桩ID
			sae_status_t stat;
			
			data_len = sizeof(sae_status_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			gui_get_sae_stat(&stat, id);
			pkg.result = GUI_RESULT_OK;
			memcpy(&(pkg.data), &stat, data_len);
		}
		break;
		
		case GUI_CMD_GET_NDEV_VERSION:  //获取节点机固件版本
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_VERSION:0x%02x\n", GUI_CMD_GET_NDEV_VERSION);

			char ver[32] = {0};
		
			data_len = sizeof(ver);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			gui_get_ndev_version(&ver);
			strcpy(pkg.data, ver);
		}
		break;
	
		case GUI_CMD_GET_NDEV_CONFIG:  //获取节点机配置
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_CONFIG:0x%02x\n", GUI_CMD_GET_NDEV_CONFIG);

			struct ndev_config config;
			memset(&config, 0, sizeof(struct ndev_config));

			data_len = sizeof(struct ndev_config);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			gui_get_ndev_device_config(&config);
		
			pkg.result = GUI_RESULT_OK;
			memcpy(&(pkg.data), &config, data_len);
		}
		break;

		case GUI_CMD_GET_STAKE_CONFIG:  //获取桩配置
		{
			fprintf(stderr, "GUI_CMD_GET_STAKE_CONFIG:0x%02x\n", GUI_CMD_GET_STAKE_CONFIG);

			struct stake_config config;
			memset(&config, 0, sizeof(struct stake_config));

			    
			data_len = sizeof(struct stake_config);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			gui_get_stake_device_config(&config);
			memcpy(&(pkg.data), &config, data_len);
			pkg.result = GUI_RESULT_OK;
		}
		break;


/***************************************节点机主页面begin*****************************************************/

		case GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1:
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1:0x%02x\n", GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1);
			
			__sync_notify_put(GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1);
			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
		}
		break;

		case GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE2:
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE2:0x%02x\n", GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE2);

			data_len = sizeof(struct gui_ndev_home_page_info);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			memcpy(&(pkg.data), &g_ndev_home_page_info, data_len);
		}
		break;

/***************************************节点机主页面end*****************************************************/


/***************************************节点机配置页面 begin*****************************************************/

		case GUI_CMD_GET_NDEV_PAGE_CONFIG:  //节点机配置
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_PAGE_CONFIG:0x%02x\n", GUI_CMD_GET_NDEV_PAGE_CONFIG);

			gui_ndev_page_config_t config;
			memset(&config, 0, sizeof(gui_ndev_page_config_t));

			data_len = sizeof(gui_ndev_page_config_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_get_ndev_page_config(&config);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &config, data_len);	
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		case GUI_CMD_GET_NDEV_ACCESS_CONFIG:  //网络接入方式
		{
			fprintf(stderr, "GUI_CMD_GET_NDEV_ACCESS_CONFIG:0x%02x\n", GUI_CMD_GET_NDEV_ACCESS_CONFIG);

			gui_ndev_access_pattern_config_t config;
			memset(&config, 0, sizeof(gui_ndev_access_pattern_config_t));

			data_len = sizeof(gui_ndev_access_pattern_config_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_get_ndev_access_pattern_config(&config);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &config, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;	
		}
		break;
	
		case GUI_CMD_SET_NDEV_FTP_CONFIG:  //FTP地址设置
		{
			fprintf(stderr, "GUI_CMD_SET_NDEV_FTP_CONFIG:0x%02x\n", GUI_CMD_SET_NDEV_FTP_CONFIG);

			gui_ndev_ftp_config_t ftp_conf;
			memset(&ftp_conf, 0, sizeof(gui_ndev_ftp_config_t));
			memcpy(&ftp_conf, &(pkg.data), sizeof(gui_ndev_ftp_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_set_ndev_ftp_config(&ftp_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

	
		case GUI_CMD_SET_NDEV_CENTER_IP_CONFIG:  //负载均衡服务器配置
		{
			fprintf(stderr, "GUI_CMD_SET_NDEV_CENTER_IP_CONFIG:0x%02x\n", GUI_CMD_SET_NDEV_CENTER_IP_CONFIG);

			gui_ndev_center_ip_config_t center_conf;
			memset(&center_conf, 0, sizeof(gui_ndev_center_ip_config_t));
			memcpy(&center_conf, &(pkg.data), sizeof(gui_ndev_center_ip_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_set_ndev_center_config(&center_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

	
		case GUI_CMD_SET_NDEV_WIFI_IP_CONFIG:  //WIFI地址设置
		{
			fprintf(stderr, "GUI_CMD_SET_NDEV_WIFI_IP_CONFIG:0x%02x\n", GUI_CMD_SET_NDEV_WIFI_IP_CONFIG);

			gui_ndev_wifi_ip_config_t wifi_conf;
			memset(&wifi_conf, 0, sizeof(gui_ndev_wifi_ip_config_t));
			memcpy(&wifi_conf, &(pkg.data), sizeof(gui_ndev_wifi_ip_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_set_ndev_wifi_config(&wifi_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

	
		case GUI_CMD_SET_NDEV_IP_CONFIG:  //本地地址设置
		{
			fprintf(stderr, "GUI_CMD_SET_NDEV_IP_CONFIG:0x%02x\n", GUI_CMD_SET_NDEV_IP_CONFIG);

			gui_ndev_local_ip_config_t local_conf;
			memset(&local_conf, 0, sizeof(gui_ndev_local_ip_config_t));
			memcpy(&local_conf, &(pkg.data), sizeof(gui_ndev_local_ip_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_set_ndev_local_config(&local_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		case GUI_CMD_SET_NDEV_PARAM_CONFIG:  //节点机参数配置
		{
			fprintf(stderr, "GUI_CMD_SET_NDEV_PARAM_CONFIG:0x%02x\n", GUI_CMD_SET_NDEV_PARAM_CONFIG);

			gui_ndev_parameter_config_t param_conf;
			memset(&param_conf, 0, sizeof(gui_ndev_parameter_config_t));
			memcpy(&param_conf, &(pkg.data), sizeof(gui_ndev_parameter_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_set_ndev_parameter_config(&param_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;	
		}
		break;
		
		case GUI_CMD_SET_STAKE_PARAM_CONFIG:  //锁桩参数配置
		{
			fprintf(stderr, "GUI_CMD_SET_STAKE_PARAM_CONFIG:0x%02x\n", GUI_CMD_SET_STAKE_PARAM_CONFIG);

			gui_stake_parameter_config_t param_conf;
			memset(&param_conf, 0, sizeof(gui_stake_parameter_config_t));
			memcpy(&param_conf, &(pkg.data), sizeof(gui_stake_parameter_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_set_stake_parameter_config(&param_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;	
		}
		break;		

		case GUI_CMD_SET_NDEV_ACCESS_CONFIG:  //网络接入方式
		{
			fprintf(stderr, "GUI_CMD_SET_NDEV_ACCESS_CONFIG:0x%02x\n", GUI_CMD_SET_NDEV_ACCESS_CONFIG);
			
			gui_ndev_access_pattern_config_t access_conf;
			memset(&access_conf, 0, sizeof(gui_ndev_access_pattern_config_t));
			memcpy(&access_conf, &(pkg.data), sizeof(gui_ndev_access_pattern_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_set_ndev_access_pattern_config(&access_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		case GUI_CMD_SET_LNT_CONFIG: //岭南通配置
		{
			fprintf(stderr, "GUI_CMD_SET_LNT_CONFIG:0x%02x\n", GUI_CMD_SET_LNT_CONFIG);

			gui_lnt_page_config_t lnt_conf;
			memset(&lnt_conf, 0, sizeof(gui_lnt_page_config_t));
			memcpy(&lnt_conf, &(pkg.data), sizeof(gui_lnt_page_config_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_set_lnt_config(&lnt_conf);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;		
		}
		break;

		
/***************************************节点机配置页面 end*****************************************************/

/***************************************基本配置配置页面 begin*****************************************************/

		case GUI_CMD_GET_BASIC_INFO_PAGE_CONFIG:
		{
			fprintf(stderr, "GUI_CMD_GET_BASIC_INFO_PAGE_CONFIG:0x%02x\n", GUI_CMD_GET_BASIC_INFO_PAGE_CONFIG);

			gui_basic_info_page_config_t config;
			memset(&config, 0, sizeof(gui_basic_info_page_config_t));

			data_len = sizeof(gui_basic_info_page_config_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_get_basic_info_page_config(&config);
			if(ret == B_DEV_EOK)
			{
				pkg.result = GUI_RESULT_OK;
				memcpy(&(pkg.data), &config, data_len);	
			}
			else
				pkg.result = GUI_RESULT_ERR;	
			
						
		}
		break;
/***************************************基本配置配置页面 end*****************************************************/


/***************************************锁桩信息 begin*****************************************************/

		case GUI_CMD_GET_STAKE_INFO_PAGE_CONFIG:
		{
			fprintf(stderr, "GUI_CMD_GET_STAKE_INFO_PAGE_CONFIG:0x%02x\n", GUI_CMD_GET_STAKE_INFO_PAGE_CONFIG);

			gui_stake_info_page_config_t config;
			memset(&config, 0, sizeof(gui_stake_info_page_config_t));

			data_len = sizeof(gui_stake_info_page_config_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_get_stake_info_page_config(&config);
			if(ret == B_DEV_EOK)
			{
				pkg.result = GUI_RESULT_OK;
				memcpy(&(pkg.data), &config, data_len);			
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

/***************************************锁桩信息 end*****************************************************/

		case GUI_CMD_GET_STAKE_ALL_INFO_CONFIG:
		{
			fprintf(stderr, "GUI_CMD_GET_STAKE_ALL_INFO_CONFIG:0x%02x\n", GUI_CMD_GET_STAKE_ALL_INFO_CONFIG);

			gui_stake_all_info_page_config_t config;
			memset(&config, 0, sizeof(gui_stake_all_info_page_config_t));

			data_len = sizeof(gui_stake_all_info_page_config_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);
			ret = gui_get_all_stake_info_config(&config);
			if(ret == B_DEV_EOK)
			{
				pkg.result = GUI_RESULT_OK;
				memcpy(&(pkg.data), &config, data_len);	
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 设置岭南通读卡器状态 */
		case GUI_CMD_SET_LNT_CARD_STAT:
		{
			fprintf(stderr, "GUI_CMD_SET_LNT_CARD_STAT:0x%02x\n", GUI_CMD_SET_LNT_CARD_STAT);
			
			gui_lnt_card_status_t lnt_card_stat;
			memset(&lnt_card_stat, 0, sizeof(gui_lnt_card_status_t));
			memcpy(&lnt_card_stat, &(pkg.data), sizeof(gui_lnt_card_status_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			ret = gui_set_lnt_card_status(&lnt_card_stat);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;		
		}
		break;

		/* 管理卡使用记录 */
		case GUI_CMD_SET_ADMIN_CARD_INFO:
		{
			fprintf(stderr, "GUI_CMD_SET_ADMIN_CARD_INFO:0x%02x\n", GUI_CMD_SET_ADMIN_CARD_INFO);

			int esc_len = 0;
			unsigned char txbuf[1024] = {0};
			unsigned char esc_buf[1024] = {0};
			
			gui_admin_card_info_t card_info;
			memset(&card_info, 0, sizeof(gui_admin_card_info_t));
			memcpy(&card_info, &(pkg.data), sizeof(gui_admin_card_info_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			
			db_insert_admin_card_info(g_database, card_info.sn, &card_info);  //把管理卡使用记录保存到数据库
			
			ret = ndev_protocol_univ_admin_card_record(txbuf, &card_info, sizeof(gui_admin_card_info_t));  //组包发送到中心
			if(ret > 0)
			{
				esc_len = uplink_escape(esc_buf, txbuf, ret); //上行转义	
				if(esc_len > 0)
				{
					unsigned int sockfd = ndev_get_sockfd();
					if(sockfd > 0)
						 lib_tcp_writen(sockfd, esc_buf, esc_len);
				}
			}
			
			pkg.result = GUI_RESULT_OK;	
		}
		break;

		/* 异常处理扣费交易记录 */
		case GUI_CMD_SET_EXCEPT_HANDLE_REC:
		{
			fprintf(stderr, "GUI_CMD_SET_EXCEPT_HANDLE_REC:0x%02x\n", GUI_CMD_SET_EXCEPT_HANDLE_REC);

			unsigned char txbuf[1024] = {0};
			gui_exception_handle_record_t excep_record;
			memset(&excep_record, 0, sizeof(gui_exception_handle_record_t));
			memcpy(&excep_record, &(pkg.data), sizeof(gui_exception_handle_record_t));

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			
			//db_insert_exception_handle_record(g_database, excep_record.sn, &excep_record);  

			ret = ndev_protocol_univ_exception_handle_record(txbuf, &excep_record, sizeof(gui_exception_handle_record_t));  //组包发送到中心
			if(ret > 0)
			{
				ndev_tcp_send_to_server_with_escape(txbuf, ret, 1000);
			}
		
			pkg.result = GUI_RESULT_OK;		
		}
		break;


		/* 异常处理请求 */
		case GUI_CMD_SET_EXCEPT_HANDLE_REQ:
		{
			fprintf(stderr, "GUI_CMD_SET_EXCEPT_HANDLE_REQ:0x%02x\n", GUI_CMD_SET_EXCEPT_HANDLE_REQ);

			struct gui_except_handle_req req;
			memset(&req, 0, sizeof(struct gui_except_handle_req));
			memcpy(&req,  &(pkg.data), sizeof(struct gui_except_handle_req));

			//lib_printf_v2("EXC REQ:", &req, sizeof(req), 16);
			
			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_get_exception_handle_req(&req);
			if(ret > 0)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 异常处理确认 */
		case GUI_CMD_SET_EXCEPT_HANDLE_ACK:
		{
			fprintf(stderr, "GUI_CMD_SET_EXCEPT_HANDLE_ACK:0x%02x\n", GUI_CMD_SET_EXCEPT_HANDLE_ACK);
			
			
			struct gui_except_handle_ack ack;
			memset(&ack, 0, sizeof(struct gui_except_handle_ack));

			data_len = sizeof(struct gui_except_handle_ack);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			ret = gui_get_exception_handle_ack(&ack);
			if(ret == B_DEV_EOK)
			{
				/*
				if(ack.sn == 0)  // ???
				{
					pkg.result = GUI_RESULT_OK;
				}
				else
				{
					memcpy(&(pkg.data), &ack, data_len);
					pkg.result = GUI_RESULT_OK;
				}
				*/

				memcpy(&(pkg.data), &ack, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 对比信息 */
		case GUI_CMD_GET_CMP_INFO:
		{
			fprintf(stderr, "GUI_CMD_GET_CMP_INFO:0x%02x\n", GUI_CMD_GET_CMP_INFO);
			
			unsigned char comp[65]= {0};

			data_len = sizeof(comp);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			ret = gui_get_sae_comparison_status(&comp);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &comp, 65);
				pkg.result = GUI_RESULT_OK;
			}
			else	
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 接入方式 连接状态 */
		case GUI_CMD_GET_ACCESS_STATE:
		{
			fprintf(stderr, "GUI_CMD_GET_ACCESS_STATE:0x%02x\n", GUI_CMD_GET_ACCESS_STATE);

			struct gui_access_state state;
			memset(&state, 0, sizeof(struct gui_access_state));

			data_len = sizeof(struct gui_access_state);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			ret = gui_get_access_state(&state);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &state, sizeof(struct gui_access_state));
				pkg.result = GUI_RESULT_OK;
			}
			else	
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* GPS信息 */
		case GUI_CMD_GET_GPS_INFO:
		{
			fprintf(stderr, "GUI_CMD_GET_GPS_INFO:0x%02x\n", GUI_CMD_GET_GPS_INFO);

			data_len = sizeof(struct gui_gps_info );
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	

			g_gps_info.tm.tm_year += 1900;
			g_gps_info.tm.tm_mon += 1;
			g_gps_info.tm.tm_yday += 1;
			
			memcpy(&(pkg.data), &g_gps_info, sizeof(struct gui_gps_info));
			pkg.result = GUI_RESULT_OK;
			g_gps_info.attr.tm = 0;
		}
		break;

		/* 租还车记录请求 */
		case GUI_CMD_SET_RENT_INFO_REQ:
		{
			fprintf(stderr, "GUI_CMD_SET_RENT_INFO_REQ:0x%02x\n", GUI_CMD_SET_RENT_INFO_REQ);

			struct gui_rent_info_qry_req req;
			memset(&req, 0, sizeof(struct gui_rent_info_qry_req));
			memcpy(&req,  &(pkg.data), sizeof(struct gui_rent_info_qry_req));

			//lib_printf_v2("RENT REQ:", &req, sizeof(req), 16);

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_get_rent_info_qry_req(&req);
			if(ret > 0)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 租还车记录确认 */ 
		case GUI_CMD_SET_RENT_INFO_ACK:
		{
			fprintf(stderr, "GUI_CMD_SET_RENT_INFO_ACK:0x%02x\n", GUI_CMD_SET_RENT_INFO_ACK);

			struct gui_rent_info_qry_ack ack;
			memset(&ack, 0, sizeof(struct gui_rent_info_qry_ack));

			data_len = sizeof(struct gui_rent_info_qry_ack);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			ret = gui_get_rent_info_qry_ack(&ack);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &ack, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 内存使用信息 */
		case GUI_CMD_GET_MEMORY_INFO:
		{
			fprintf(stderr, "GUI_CMD_GET_MEMORY_INFO:0x%02x\n", GUI_CMD_GET_MEMORY_INFO);

			struct gui_proc_meminfo gmeminfo;
			struct proc_meminfo pminfo;
			memset(&gmeminfo, 0, sizeof(struct gui_proc_meminfo));
			memset(&pminfo, 0, sizeof(struct proc_meminfo));
			
			data_len = sizeof(struct gui_proc_meminfo);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);

			lib_get_macaddr("eth0", (char *)&(gmeminfo.mac0));
			lib_get_macaddr("eth1", (char *)&(gmeminfo.mac1));
			ret = lib_get_proc_meminfo(&pminfo);
			memcpy(&(gmeminfo.MemTotal), &pminfo, sizeof(struct proc_meminfo));	
			if(ret == LIB_GE_EOK)
			{
				memcpy(&(pkg.data), &gmeminfo, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;	
		}
		break;

		/* 接口板版本信息 */
		case GUI_CMD_GET_IBOARD_DEVICE_VERSION: 
		{
			fprintf(stderr, "GUI_CMD_GET_IBOARD_DEVICE_VERSION:0x%02x\n", GUI_CMD_GET_IBOARD_DEVICE_VERSION);

			struct iboard_device_version version;
			memset(&version, 0, sizeof(struct iboard_device_version));

			data_len = sizeof(struct iboard_device_version);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	

			ret = ext_iboard_device_version(&version, 1500);
			if(ret == EXT_IB_EOK)
			{
				memcpy(&(pkg.data), &version, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
			
		}
		break;

		case GUI_CMD_SET_LIGHT_BOX:  //控制灯箱, set 
		{
			fprintf(stderr, "GUI_CMD_SET_LIGHT_BOX:0x%02x\n", GUI_CMD_SET_LIGHT_BOX);

			struct gui_light_ctrl_time ctrl_time;
			memset(&ctrl_time, 0, sizeof(struct gui_light_ctrl_time));
			memcpy(&ctrl_time,  &(pkg.data), sizeof(struct gui_light_ctrl_time));
			
			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_light_ctrl_time_set(&ctrl_time);
			if(ret == B_DEV_EOK)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		case GUI_CMD_GET_LIGHT_CTRL_TIME:   //控制灯箱, get 
		{
			fprintf(stderr, "GUI_CMD_GET_LIGHT_CTRL_TIME:0x%02x\n", GUI_CMD_GET_LIGHT_CTRL_TIME);

			struct gui_light_ctrl_time ctrl_time;
			memset(&ctrl_time, 0, sizeof(struct gui_light_ctrl_time));

			data_len = sizeof(struct gui_light_ctrl_time);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	

			ret = gui_light_ctrl_time_get(&ctrl_time);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &ctrl_time, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
			
		}
		break;

		case GUI_CMD_GET_UNITY_FILE_INFO: //泛文件信息
		{
			fprintf(stderr, "GUI_CMD_GET_UNITY_FILE_INFO:0x%02x\n", GUI_CMD_GET_UNITY_FILE_INFO);

			unity_file_info_t info;

			data_len = sizeof(unity_file_info_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			
			ret = unity_file_info_get(&info);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &info, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		case GUI_CMD_SET_LNT_ALL_INFO:  //设置岭南通信息
		{
			fprintf(stderr, "GUI_CMD_SET_LNT_ALL_INFO:0x%02x\n", GUI_CMD_SET_LNT_ALL_INFO);
			
			memcpy(&g_lnt_info,  &(pkg.data), sizeof(lnt_all_info_t));

			#if 0
			fprintf(stderr, "%s\n", g_lnt_info.lib_version);
			fprintf(stderr, "%s\n", g_lnt_info.hw_version);
			lib_printf_v2("", &g_lnt_info, sizeof(g_lnt_info), 16);
			#endif
			
			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			pkg.result = GUI_RESULT_OK;
		}
		break;

		case GUI_CMD_GET_LNT_ALL_INFO:  //读取岭南通信息
		{
			fprintf(stderr, "GUI_CMD_GET_LNT_ALL_INFO:0x%02x\n", GUI_CMD_GET_LNT_ALL_INFO);

			data_len = sizeof(lnt_all_info_t);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			memcpy(&(pkg.data), &g_lnt_info,  sizeof(lnt_all_info_t));
			pkg.result = GUI_RESULT_OK;
		}
		break;

		case GUI_CMD_SET_DELAY_BICYCLE_REQ: //延迟还车请求
		{
			fprintf(stderr, "GUI_CMD_SET_DELAY_BICYCLE_REQ:0x%02x\n", GUI_CMD_SET_DELAY_BICYCLE_REQ);
			
			__sync_notify_put(GUI_CMD_SET_DELAY_BICYCLE_REQ);
		}
		break;

		case GUI_CMD_SET_DELAY_BICYCLE_ACK:  //延迟还车确认
		{
			fprintf(stderr, "GUI_CMD_SET_DELAY_BICYCLE_ACK:0x%02x\n", GUI_CMD_SET_DELAY_BICYCLE_ACK);

			struct gui_delay_return_bicycle bicycle;
			memset(&bicycle, 0, sizeof(struct gui_delay_return_bicycle));

			data_len = sizeof(struct gui_delay_return_bicycle);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	

			ret = gui_delay_return_bicycle_ack(&bicycle);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &bicycle, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 附近网点信息查询 add by zjc at 2016-11-03 */
		/* 附近网点信息查询请求 */
		case GUI_CMD_NEARBY_SITE_INFO_REQ:
		{
			fprintf(stderr, "GUI_CMD_NEARBY_SITE_INFO_REQ:0x%02x\n", GUI_CMD_NEARBY_SITE_INFO_REQ);

			struct gui_nearby_site_info_qry_req req;
			memset(&req, 0, sizeof(struct gui_nearby_site_info_qry_req));
			memcpy(&req,  &(pkg.data), sizeof(struct gui_nearby_site_info_qry_req));

			lib_printf_v2("----------NEARBY_SITE REQ:", &req, sizeof(req), 16);

			data_len = 0;
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);		
			ret = gui_get_nearby_site_info_qry_req(&req);
			if(ret > 0)
				pkg.result = GUI_RESULT_OK;
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;

		/* 附近网点信息查询确认 */ 
		case GUI_CMD_NEARBY_SITE_INFO_ACK:
		{
			fprintf(stderr, "GUI_CMD_NEARBY_SITE_INFO_ACK:0x%02x\n", GUI_CMD_NEARBY_SITE_INFO_ACK);

			struct gui_nearby_site_info_qry_ack ack;
			memset(&ack, 0, sizeof(struct gui_nearby_site_info_qry_ack));

			data_len = sizeof(struct gui_nearby_site_info_qry_ack);
			__unix_package_create(&pkg, cmd, GUI_RESULT_OK, data_len);	
			ret = gui_get_nearby_siteinfo_qry_ack(&ack);
			if(ret == B_DEV_EOK)
			{
				memcpy(&(pkg.data), &ack, data_len);
				pkg.result = GUI_RESULT_OK;
			}
			else
				pkg.result = GUI_RESULT_ERR;
		}
		break;
		/* end of 附近网点信息查询 */

		
		
		default:
		{
			fprintf(stderr, "NOT SUPPORT COMMAND\n");
			
			__unix_package_create(&pkg, cmd, GUI_RESULT_NOT_CMD, data_len);
		}
	}


Done:

	//lib_printf_v2("------------__unix_package_writen--------------", &pkg, data_len + GUI_HEAD_LEN, 16);

	__unix_package_writen(sockfd, &pkg, data_len);

	/* 标识TCP类型 */
	if(g_sockfd_type == 1)
	{
		/* 写入数据后,关闭写端sockfd */
		lib_shutdown(sockfd, SHUT_WR);	

		g_sockfd_type = 0;
	}
	
#if 0
	unsigned char txbuf[GUI_PKG_LEN] = {0};
	memcpy(&txbuf[10], &pkg, data_len);
	__unix_package_writen(sockfd, &txbuf, data_len + 10);

	lib_printf_v2("------------txbuf--------------", &txbuf, data_len + 10, 16);
#endif

	if(is_device_reboot == 1)  //设备需要重启
	{
		fprintf(stderr, "device system reboot\n");
		
		lib_wdt_system_reboot();  //使用软件看门狗复位重启设备
	}

	return ret;
}

static void *__async_handle_thread(void *arg)
{
	unsigned int notify = NDEV_NOTIFY_INIT;

	device_config_t config;
	memset(&config, 0, sizeof(device_config_t));
	
	struct firmware_config firmware;
	//ftp_download_ctrl_info_t fdnl_ctrl_info;

	device_config_get(&config);
	
	while(1)
	{
		__sync_notify_wait(&notify);

		switch(notify)
		{  
			case GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1:
			{
				memset(&g_ndev_home_page_info, 0, sizeof(gui_ndev_home_page_info_t));
				
				gui_get_ndev_home_page_info(&g_ndev_home_page_info);
  
				#if CONFS_USING_READER_UPDATE  
				//增加读卡器升级固件信息 add by zjc at 2016-07-25
				memset(&firmware, 0, sizeof(firmware));
				lib_upgrade_get_firmware_config(g_upgrade, UPE_TYPE_LNT_ZM, &firmware); //获取固件配置
				printf("---------Bicycle, is_download_complete:%d, is_write_parameter:%d, is_write_flash:%d\n", \   
					firmware.is_download_complete, firmware.is_write_parameter, firmware.is_write_flash);
				if((firmware.is_download_complete == UP_DNL_STAT_COMPLETE) && (firmware.is_write_flash == UP_WR_STAT_WR) && (firmware.is_write_parameter == UP_WR_STAT_NOT_WR_LNT))
				{
					g_ndev_home_page_info.lnt_firmware_update_flag = 1; //读卡器固件升级标志
					memcpy(&g_ndev_home_page_info.lnt_firmware_path, &firmware.ftp_local_path, sizeof(g_ndev_home_page_info.lnt_firmware_path)); //固件路径
				}
				else
				{  
					g_ndev_home_page_info.lnt_firmware_update_flag = 0;
					memset(&g_ndev_home_page_info.lnt_firmware_path, 0, sizeof(g_ndev_home_page_info.lnt_firmware_path));
				}
				#endif
			}
			break;

			case GUI_CMD_GET_GPS_INFO: //解析GPS信息
			{
				ndev_status_t nstat;	
				
#ifdef CONFS_USING_GPS_MODEL			
				if(g_gps_rxbuf.vaild == 1)
				{
					memset(&g_gps_info, 0, sizeof(struct gui_gps_info));
					gps_get_info(g_gps_rxbuf.string, &(g_gps_info.gga), &(g_gps_info.tm), &(g_gps_info.attr));
					
				#if 0
					if(g_gps_info.attr.gga == 1)
						gps_show_gga( &(g_gps_info.gga));

					if(g_gps_info.attr.tm == 1)
						gps_show_tm(&(g_gps_info.tm));
				#endif

					if(g_gps_info.attr.tm == 1)
					{
						//gps_show_tm(&(g_gps_info.tm));
						
						time_modify_run(config.nconf.timer_gate_value, &(g_gps_info.tm));
						
						ndev_info_stat_get(&nstat);
						nstat.gps = 1;
						ndev_info_stat_put(&nstat);
					}
					else
					{
						ndev_info_stat_get(&nstat);
						nstat.gps = 0;
						ndev_info_stat_put(&nstat);
					}
				
					g_gps_rxbuf.vaild = 0;
				}
#else
				wl_gps_info_t gps_info;
				memset(&gps_info, 0, sizeof(wl_gps_info_t));
				int ret = lib_wl_get_gps_info(g_bicycle_wl, &gps_info, 1000);

				fprintf(stderr, "-------->4G get gps: %d\n", ret);
				
				if(ret == LIB_WL_EOK)
				{
					memcpy(&(g_gps_info.attr), &(gps_info.attr), sizeof(struct g_gps_attr)); //attr
					
					if(gps_info.attr.tm == 1)  //tm
					{
						memcpy(&(g_gps_info.tm), &(gps_info.ptm), sizeof(struct g_gps_tm)); //tm

						//gps_show_tm(&(g_gps_info.tm));
						
						time_modify_run(config.nconf.timer_gate_value, &(g_gps_info.tm));
						
						ndev_info_stat_get(&nstat);
						nstat.gps = 1;
						ndev_info_stat_put(&nstat);
					}
					else
					{
						ndev_info_stat_get(&nstat);
						nstat.gps = 0;
						ndev_info_stat_put(&nstat);
					}
				
					if(gps_info.attr.gga == 1) //gga
					{
						memcpy(&(g_gps_info.gga), &(gps_info.gga), sizeof(struct g_gps_gga)); //gga
					}	
				}
#endif			
			}
			break;

			case GUI_CMD_SET_LIGHT_BOX:  //控制灯箱
			{
				fprintf(stderr, "GUI_CMD_SET_LIGHT_BOX\n");

				unsigned char tm[7] = {0};
				memset(&config, 0, sizeof(device_config_t));
				device_config_get(&config);
				lib_get_systime(&tm); ////YYYY MM DD HH mm SS

				#if 0
				fprintf(stderr, "HH:mm=%02d:%02d\n", tm[4], tm[5]);  
				
				fprintf(stderr, "enable[0]=%d, enable[1]=%d, disenable[0]=%d, disenable[1]=%d", \
					config.nconf.light_ctrl_time_enable[0], config.nconf.light_ctrl_time_enable[1], \
					config.nconf.light_ctrl_time_disable[0], config.nconf.light_ctrl_time_disable[1]);
				#endif
				
				unsigned char t_ctrl_time_hh, t_ctrl_time_mm;

				t_ctrl_time_hh = config.nconf.light_ctrl_time_enable[1]; //HH
				t_ctrl_time_mm =  config.nconf.light_ctrl_time_enable[0]; //MM
			
				if((tm[4] == t_ctrl_time_hh) && ( (tm[5] >= t_ctrl_time_mm) && (tm[5] <= t_ctrl_time_mm + 5))) //开启
				{
					ext_iboard_LED4(IBOARD_CTRL_ON);
					ext_iboard_light_box(IBOARD_CTRL_ON);

					system("echo 70 > /sys/class/backlight/pwm-backlight/brightness");

					fprintf(stderr, "--------Enable Light: HH=%d,MM=%d----------\n", t_ctrl_time_hh, t_ctrl_time_mm);
				}

				t_ctrl_time_hh = config.nconf.light_ctrl_time_disable[1];
				t_ctrl_time_mm =  config.nconf.light_ctrl_time_disable[0];
				if((tm[4] == t_ctrl_time_hh) && ( (tm[5] >= t_ctrl_time_mm) && (tm[5] <= t_ctrl_time_mm + 5)))  //关闭
				{
					ext_iboard_LED4(IBOARD_CTRL_OFF);
					ext_iboard_light_box(IBOARD_CTRL_OFF);

					system("echo 100 > /sys/class/backlight/pwm-backlight/brightness");

					fprintf(stderr, "--------Disable Light: HH=%d,MM=%d----------\n", t_ctrl_time_hh, t_ctrl_time_mm);
				}
			}
			break;

			case GUI_CMD_SET_DELAY_BICYCLE_REQ:  //延迟还车请求
			{
				//fprintf(stderr, "GUI_CMD_SET_DELAY_BICYCLE_REQ\n");

				int n_ret = 0;
				unsigned char temp_buf[512] = {0};
				/* 发送心跳包 */
				memset(temp_buf, 0, sizeof(temp_buf));
				n_ret = sae_protocol_bheart_req(temp_buf, SAE_CAN_BROADCAST_ID);  //广播心跳
				if(n_ret != B_DEV_ERROR)
				{
					//lib_printf_v2("CAN", temp_buf, n_ret, 16);
					
					stake_ctos_put(SAE_CAN_BROADCAST_ID, temp_buf, n_ret);

					sae_info_send_hb_times_inc_all(CONFS_STAKE_NUM_MAX + 1); // 增加发送次数		
				}
			}
			break;

			/* 系统信息备份 */ 
			case GUI_CMD_SYS_INFO_BACKUP:
			{
				fprintf(stderr, "GUI_CMD_SYS_INFO_BACKUP:0x%02x\n", GUI_CMD_SYS_INFO_BACKUP);

				#if 0
				struct proc_meminfo pminfo;
				memset(&pminfo, 0, sizeof(struct proc_meminfo));
		
				lib_get_proc_meminfo(&pminfo);
				fprintf(stderr, "\n------------MemFree:%02d KB-----------\n", pminfo.MemFree);
				#endif

				system("cat /proc/meminfo > /opt/logpath/meminfo.bak");
				system("ps > /opt/logpath/progress.bak");
			}
			break;
		}
	}

	return lib_thread_exit((void *)NULL);
}

/*
 * 读取GPS信息
 */
static void __gps_read_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask)
{
#ifdef CONFS_USING_GPS_MODEL
	g_gps_rxbuf.size = lib_serial_readn_select(fd, g_gps_rxbuf.string, sizeof(g_gps_rxbuf.string), 200); //大学城版本 100

	//g_gps_rxbuf.size = read(fd, g_gps_rxbuf.string, sizeof(g_gps_rxbuf.string));  
	if(g_gps_rxbuf.size > 0)
	{
		memcpy(&g_gps_rxbuf2, &g_gps_rxbuf, sizeof(struct gps_rxbuf));
		
		g_gps_rxbuf.vaild  = 1;
		g_gps_rxbuf2.vaild = 1;
		__sync_notify_put(GUI_CMD_GET_GPS_INFO);
	}
#endif
}

static int __iboard_time_proc(lib_event_loop_t *ep, long long id, void *client_data)
{
	fprintf(stderr, "iboard id: %u\n", id);

#if 0
	device_config_t conf;
	memset(&conf, 0, sizeof(device_config_t));
	device_config_get(&conf);

	fprintf(stderr, "E:%02d:%02d\n", conf.nconf.light_ctrl_time_enable[1], conf.nconf.light_ctrl_time_enable[0]);
	fprintf(stderr, "D:%02d:%02d\n", conf.nconf.light_ctrl_time_disable[1], conf.nconf.light_ctrl_time_disable[0]);
#endif

	__sync_notify_put(GUI_CMD_SET_LIGHT_BOX);

	return IBOARD_TIME_MS;
}

static int __gps_time_proc(lib_event_loop_t *ep, long long id, void *client_data)
{
	fprintf(stderr, "gps id: %u\n", id);

#ifndef CONFS_USING_GPS_MODEL
	__sync_notify_put(GUI_CMD_GET_GPS_INFO);
#endif

	return GPS_TIMER_MS;
}


/* add by zjc at 2017-01-16 */
static int __sys_info_time_proc(lib_event_loop_t *ep, long long id, void *client_data)
{
	fprintf(stderr, "----------sysinfo id: %u\n", id);

#if CONFS_USING_SYS_INFO_BACKUP
	__sync_notify_put(GUI_CMD_SYS_INFO_BACKUP);
#endif

	return SYS_INFO_TIMER_MS;
}



