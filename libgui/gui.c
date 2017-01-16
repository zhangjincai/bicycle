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

#include "lib_general.h"
#include "lib_eventloop.h"
#include "lib_gui.h"


/*
 * 支持指令
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
#define GUI_CMD_SET_LIGHT_CTRL_TIME				0x8016  //灯箱控制
#define GUI_CMD_SET_LNT_ALL_INFO					0x8017  //岭南通信息
#define GUI_CMD_SET_DELAY_BICYCLE_REQ			0x8018   //延迟还车请求
#define GUI_CMD_SET_DELAY_BICYCLE_ACK			0x8019   //延迟还车确认

/* 附近网点信息查询指令 add by zjc at 2016-11-03 */
#define GUI_CMD_NEARBY_SITE_INFO_REQ					0x8020  //附近网点信息请求
#define GUI_CMD_NEARBY_SITE_INFO_ACK					0x8021  //附近网点信息确认




#define GUI_REBOOT									(99)		//设备需要重启
#define GUI_STAT1									(100)       //状态1


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
 * 报文结构
 */
#define GUI_PKG_LEN			8192
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



#define GUI_ID_H			0x55
#define GUI_ID_L			0xaa


#define GUI_UNIX_DOMAIN					"/tmp/lib_gui_unix.domain"
#define GUI_PERM							(0777)
#define GUI_TCP_SERV_PORT				10086
#define GUI_SERIAL_TTY					"/dev/ttyO4"


#define GUI_SERIAL			0x1
#define GUI_LOCAL			0x2
#define GUI_NETWORK			0x4


struct gui
{
	int sockfd;
	lib_serial_t serial;
	struct gui_setup setup;
}__attribute__((packed));


static struct gui *g_gui_ptr = NULL;

static int __gui_connect(struct gui *gui);
static int __is_gui_package(gui_package_t *pkg, const unsigned short cmd);
static void __gui_package_create(gui_package_t *pkg, const unsigned short cmd, const unsigned short length, const char result);
static void __gui_package_create_with_data(gui_package_t *pkg, const unsigned short cmd, void *ptr, const unsigned short length, const char result);
static int __gui_package_writen(int sockfd, const void *buff, const unsigned int len);
static int __gui_package_writen_with_data(int sockfd, const void *buff, const unsigned int len);
static int __gui_package_readn_select(int sockfd, void *buff, const unsigned int len, const unsigned int msec);
static int __gui_package_readn_select_without_data(int sockfd, void *buff, const unsigned int msec);



static int __is_gui_package(gui_package_t *pkg, const unsigned short cmd)
{
	if((pkg->id_h == GUI_ID_H) && (pkg->id_l == GUI_ID_L) && (pkg->cmd == cmd))
		return LIB_GUI_TRUE;

	fprintf(stderr, "LIB GUI ERROR id_h:0x%02x, id_l:0x%02x, cmd:0x%02x\n", pkg->id_h, pkg->id_l, pkg->cmd);
	
	return LIB_GUI_FALSE;
}

static int __gui_pkg(gui_package_t *pkg, const unsigned int len)
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

			return LIB_GUI_EOK;
		}
	}

	return LIB_GUI_ERROR;
	
}

static void __gui_package_create(gui_package_t *pkg, const unsigned short cmd, const unsigned short length, const char result)
{
	memset(pkg, 0, sizeof(gui_package_t));
	
	pkg->id_h = GUI_ID_H;
	pkg->id_l = GUI_ID_L;
	pkg->cmd = cmd;
	pkg->result = result;
	pkg->length = length;
}

static void __gui_package_create_with_data(gui_package_t *pkg, const unsigned short cmd, void *ptr, const unsigned short length, char result)
{
	memset(pkg, 0, sizeof(gui_package_t));
	
	pkg->id_h = GUI_ID_H;
	pkg->id_l = GUI_ID_L;
	pkg->cmd = cmd;
	pkg->result = result;
	pkg->length = length;
	
	memcpy(&(pkg->data), ptr, length);
}

static int __gui_package_writen(int sockfd, const void *buff, const unsigned int len)
{
	int ret = -1;

#if 1
	if(g_gui_ptr != NULL){
		ret = __gui_connect(g_gui_ptr);
		//printf("__gui_connect, ret:%d\n", ret);
	}
#endif		
	
	//ret = write(sockfd, buff, len);
	ret = write(g_gui_ptr->sockfd, buff, len);

	//printf("-----------__gui_package_writen, write ret:%d\n", ret);  
	if(ret <= 0)
	{
		if(sockfd > 0)
			lib_close(sockfd);

		if(g_gui_ptr != NULL)
			return __gui_connect(g_gui_ptr);
	}

	return ret;
}

static int __gui_package_writen_with_data(int sockfd, const void *buff, const unsigned int len)
{
#if 1
	if(g_gui_ptr != NULL)
		__gui_connect(g_gui_ptr);
#endif	
	//int ret = write(sockfd, buff, len + GUI_HEAD_LEN);
	int ret = write(g_gui_ptr->sockfd, buff, len + GUI_HEAD_LEN);
	if(ret <= 0)
	{
		if(sockfd > 0)
			lib_close(sockfd);

		if(g_gui_ptr != NULL)
			return __gui_connect(g_gui_ptr);
	}

	return ret;
}

static int __gui_package_readn_select(int sockfd, void *buff, const unsigned int len, const unsigned int msec)
{
#if 0
	return lib_tcp_read_select(sockfd, buff, GUI_PKG_LEN, msec);
	//return lib_tcp_readn_select(sockfd, buff, len + GUI_HEAD_LEN, msec);
#else
	int retval = lib_tcp_read_select(sockfd, buff, GUI_PKG_LEN, msec);

	if(g_gui_ptr != NULL)
	{
		if(sockfd > 0)
		{
			lib_close(sockfd);
			sockfd = -1;
		}
	}
	return retval;
#endif
}

static int __gui_package_readn_select_without_data(int sockfd, void *buff, const unsigned int msec)
{
#if 0
	return lib_tcp_read_select(sockfd, buff, GUI_PKG_LEN, msec);
#else
	int retval = lib_tcp_read_select(sockfd, buff, GUI_PKG_LEN, msec);
	if(g_gui_ptr != NULL)
	{
		if(sockfd > 0)
		{
			lib_close(sockfd);
			sockfd = -1;
		}
	}
	return retval;
#endif
}











/*
 * 获取GUI库版本
 */
int lib_gui_version(lib_gui_t *gui, char *version, const unsigned int msec)
{
	if((gui == NULL) || (version == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_version);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_VER, GUI_HEAD_LEN, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_VER) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		strcpy(version, (char *)pkg.data);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;
}

static int __gui_connect(struct gui *gui)
{
	switch(gui->setup.connect_mode)
	{
		case GUI_CONNECT_MODE_UNIX:  //UNIX域协议模式
		{
			printf("------------GUI_CONNECT_MODE_UNIX\n");
			gui->sockfd = lib_unix_connect(GUI_UNIX_DOMAIN);
			if(gui->sockfd == LIB_GUI_ERROR)
				return LIB_GUI_ERROR;
		}
		break;
		 
		case GUI_CONNECT_MODE_TCP:  //TCP模式
		{
			gui->sockfd = lib_tcp_connect_nonb(gui->setup.s_un.tcp.ipaddr, gui->setup.s_un.tcp.port, gui->setup.s_un.tcp.conn_nsec);
			if(gui->sockfd == LIB_GUI_ERROR)
				return LIB_GUI_ERROR;
		}
		break;

		case GUI_CONNECT_MODE_SERIAL: //串口模式
		{
			int err = LIB_GE_ERROR;
		
			strcpy(gui->serial.pathname, gui->setup.s_un.serial.pathname);
			gui->serial.flags = O_RDWR;
			gui->serial.speed = 115200;  //波特率
			gui->serial.databits = 8;
			gui->serial.stopbits = 1;
			
			err = lib_serial_init(&(gui->serial));
			if(err == LIB_GUI_ERROR)
				return LIB_GUI_ERROR;

			gui->sockfd = gui->serial.sfd;  //串口fd
		}
		break;
	}
	
	lib_setfd_noblock(gui->sockfd);	

	return LIB_GUI_EOK;	
}


lib_gui_t *lib_gui_new(struct gui_setup *setup)
{
	if(setup == NULL)
		return LIB_GUI_NULL;

	struct gui *gui = (struct gui *)malloc(sizeof(struct gui));
	if(gui == NULL)
		return LIB_GUI_NULL;

	memset(gui, 0, sizeof(struct gui));
	memcpy(&(gui->setup), setup, sizeof(struct gui_setup));

#if 0
	if(__gui_connect(gui) == LIB_GUI_ERROR)
	{
		free(gui);
		gui = NULL;
		return LIB_GUI_NULL;
	}
#endif

	g_gui_ptr = gui;

	return gui;
}

void lib_gui_destroy(lib_gui_t *gui)
{
	if(gui != NULL)
	{
		lib_close(gui->sockfd);
		free(gui);
		gui = NULL;
	}
}

int lib_gui_reconnect(lib_gui_t *gui)
{
	if(gui == NULL)
		return LIB_GUI_ERROR;
	
	if(gui->sockfd > 0)
		lib_close(gui->sockfd);
	return __gui_connect(gui);
}



/*
 * 获取节点机状态
 */
int lib_gui_get_ndev_stat(lib_gui_t *gui, gui_ndev_status_t *stat, const unsigned int msec)
{
	if((gui == NULL) || (stat == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_ndev_status);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_STAT, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
		
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_STAT) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(stat, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	      
	return ret;	
}

/*
 * 获取桩状态
 */   
int lib_gui_get_sae_stat(lib_gui_t *gui, const unsigned char id, gui_sae_status_t *stat, const unsigned int msec)
{
	if((gui == NULL) || (stat == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length =  sizeof(struct gui_sae_status);
	gui_package_t pkg;

	__gui_package_create(&pkg, GUI_CMD_GET_SAE_STAT, length, GUI_RESULT_OK);
	
	pkg.data[0] = id;
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN + 1);
	memset(&pkg, 0, sizeof(gui_package_t));
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_SAE_STAT) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(stat, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

/*
 * 获取节点机配置
 */
int lib_gui_get_ndev_config(lib_gui_t *gui, struct gui_ndev_config *config,  const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_ndev_config);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_CONFIG, length, GUI_RESULT_OK);
	
	ret = __gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	//printf("-----------lib_gui_get_ndev_config, write ret:%d\n", ret);  
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	//printf("-----------lib_gui_get_ndev_config, read ret:%d\n", ret);  
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;		
}

/*
 * 获取桩配置
 */
int lib_gui_get_stake_config(lib_gui_t *gui, struct gui_stake_config *config,  const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_stake_config);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_STAKE_CONFIG, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_STAKE_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;		
}

/*
 * 获取节点机固件版本号
 */
int lib_gui_ndev_version(lib_gui_t *gui, char version[32], const unsigned int msec)
{
	if((gui == NULL) || (version == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_ndev_version);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_VERSION, length, GUI_RESULT_OK);
		
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_VERSION) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		strcpy(version, (char *)pkg.data);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

/*
 * 获取基本信息配置
 */
 int lib_gui_get_basic_page_config(lib_gui_t *gui, struct gui_basic_info_page_config *config, const unsigned int msec)
 {
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_basic_info_page_config);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_BASIC_INFO_PAGE_CONFIG, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_BASIC_INFO_PAGE_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
 }

/*
 * 获取节点机页面配置
 */
int lib_gui_get_ndev_page_config(lib_gui_t *gui, struct gui_ndev_page_config *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_ndev_page_config);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_PAGE_CONFIG, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	
	memset(&pkg, 0, pkg_len);
	
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_PAGE_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

/*
 * 获取网络接入方式
 */
int lib_gui_get_ndev_access_pattern_config(lib_gui_t *gui, struct gui_ndev_access_pattern_config *config, const unsigned int msec)  
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_ndev_access_pattern_config);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_ACCESS_CONFIG, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_ACCESS_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

/*
 *设置FTP地址
 */
int lib_gui_set_ndev_ftp_config(lib_gui_t *gui, struct gui_ndev_ftp_config *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_ftp_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_FTP_CONFIG, config, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_FTP_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;	
	}
	
	return ret;	
}

int lib_gui_set_ndev_center_config(lib_gui_t *gui, struct gui_ndev_center_ip_config *config, const unsigned int msec)  //设置中心地址--负载均衡服务器地址
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_center_ip_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_CENTER_IP_CONFIG, config, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_CENTER_IP_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_set_ndev_wifi_config(lib_gui_t *gui, struct gui_ndev_wifi_ip_config *config, const unsigned int msec) //设置WIFI
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_wifi_ip_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_WIFI_IP_CONFIG, config, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_WIFI_IP_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;
}

int lib_gui_set_ndev_local_config_reboot(lib_gui_t *gui, struct gui_ndev_local_ip_config *config, const unsigned int msec) //设置本地地址
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_local_ip_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_IP_CONFIG, config, length, GUI_RESULT_REBOOT);  //设备需要重启
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_IP_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_set_ndev_parameter_config(lib_gui_t *gui, struct gui_ndev_parameter_config *config, const unsigned int msec) //节点机参数设置
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_parameter_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_PARAM_CONFIG, config, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_PARAM_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_set_ndev_parameter_config_reboot(lib_gui_t *gui, struct gui_ndev_parameter_config *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_parameter_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_PARAM_CONFIG, config, length, GUI_RESULT_REBOOT);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_PARAM_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;	
	}
	
	return ret;	
}

int lib_gui_set_stake_parameter_config(lib_gui_t *gui, struct gui_stake_parameter_config *config, const unsigned int msec) //锁桩参数设置
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_stake_parameter_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_STAKE_PARAM_CONFIG, config, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_STAKE_PARAM_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_set_ndev_access_pattern_config_reboot(lib_gui_t *gui, struct gui_ndev_access_pattern_config *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_ndev_access_pattern_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_NDEV_ACCESS_CONFIG, config, length, GUI_RESULT_REBOOT);  //设备需要重启
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_NDEV_ACCESS_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;	
	}
	
	return ret;		
}

int lib_gui_set_lnt_config_reboot(lib_gui_t *gui, struct gui_lnt_page_config *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_lnt_page_config);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_LNT_CONFIG, config, length, GUI_RESULT_REBOOT);  //设备需要重启
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_LNT_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

/*
 * 获取锁桩信息
 */
 int lib_gui_get_stake_info_page_config(lib_gui_t *gui, struct gui_stake_info_page_config *config, const unsigned int msec)
 {
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_stake_info_page_config);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_STAKE_INFO_PAGE_CONFIG, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_STAKE_INFO_PAGE_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
 }


/*
 * 获取基本信息页面
 */
int lib_gui_get_ndev_info_page_config_stage1(lib_gui_t *gui, const unsigned int msec)
{
	if(gui == NULL)
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = 0;
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE1) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_get_ndev_info_page_config_stage2(lib_gui_t *gui, struct gui_ndev_home_page_info *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_ndev_home_page_info);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE2, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_NDEV_HOME_PAGE_INFO_SATGE2) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);
		
		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}


int lib_gui_get_stake_all_info_page_config(lib_gui_t *gui, struct gui_stake_all_info_page_config *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_stake_all_info_page_config);
	gui_package_t pkg;

	fprintf(stderr, "lib stake_all_info length:%d\n", length);

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_STAKE_ALL_INFO_CONFIG, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_STAKE_ALL_INFO_CONFIG) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		memcpy(config, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;		
}


int lib_gui_set_lnt_card_status(lib_gui_t *gui, struct gui_lnt_card_status *config, const unsigned int msec)
{
	if((gui == NULL) || (config == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_lnt_card_status);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_LNT_CARD_STAT, config, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_LNT_CARD_STAT) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_set_admin_card_info(lib_gui_t *gui, gui_admin_card_info_t *info, const unsigned int msec)
{
	if((gui == NULL) || (info == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_admin_card_info);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_ADMIN_CARD_INFO, info, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_ADMIN_CARD_INFO) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_set_exception_handle_record(lib_gui_t *gui, gui_exception_handle_record_t *record, const unsigned int msec)
{
	if((gui == NULL) || (record == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(struct gui_exception_handle_record);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_EXCEPT_HANDLE_REC, record, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_EXCEPT_HANDLE_REC) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;	

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;	
	}
	
	return ret;	
}

int lib_gui_except_handle_req(lib_gui_t *gui, gui_except_handle_req_t *req, const unsigned int msec)
{
	if((gui == NULL) || (req == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	int length = sizeof(gui_except_handle_req_t);
	memset(&pkg, 0, pkg_len);

	__gui_package_create_with_data(&pkg, GUI_CMD_SET_EXCEPT_HANDLE_REQ, req, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_EXCEPT_HANDLE_REQ) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;
}

int lib_gui_except_handle_ack(lib_gui_t *gui, gui_except_handle_ack_t *ack, const unsigned int msec)
{
	if((gui == NULL) || (ack == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_except_handle_ack_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_SET_EXCEPT_HANDLE_ACK, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_EXCEPT_HANDLE_ACK) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(ack, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EFULL;  //有数据
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_EEMPTY; //无数据
	}
	
	return ret;		
}


int lib_gui_get_sae_comparison_status(lib_gui_t *gui, gui_sae_comparison_status_t comp[65], const unsigned int msec)
{
	if((gui == NULL) || (comp == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_sae_comparison_status_t) * 65;
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_CMP_INFO, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_CMP_INFO) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(comp, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;			
}

int lib_gui_access_state(lib_gui_t *gui, gui_access_state_t *access, const unsigned int msec)
{
	if((gui == NULL) || (access == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_access_state_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_ACCESS_STATE, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_ACCESS_STATE) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(access, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_get_gps_info(lib_gui_t *gui, struct gui_gps_info *info, const unsigned int msec)
{
	if((gui == NULL) || (info == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(struct gui_gps_info);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_GPS_INFO, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_GPS_INFO) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(info, pkg.data, length);
		
		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_rent_info_qry_req(lib_gui_t *gui, gui_rent_info_qry_req_t *req, const unsigned int msec)
{
	if((gui == NULL) || (req == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	int length = sizeof(gui_rent_info_qry_req_t);
	memset(&pkg, 0, pkg_len);

	__gui_package_create_with_data(&pkg, GUI_CMD_SET_RENT_INFO_REQ, req, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_RENT_INFO_REQ) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}


int lib_gui_rent_info_qry_ack(lib_gui_t *gui, gui_rent_info_qry_ack_t *ack, const unsigned int msec)
{
	if((gui == NULL) || (ack == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_rent_info_qry_ack_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_SET_RENT_INFO_ACK, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_RENT_INFO_ACK) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(ack, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EFULL;  //有数据
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_EEMPTY; //无数据
	}
	
	return ret;	
}


int lib_gui_iboard_device_version(lib_gui_t *gui, gui_iboard_device_version_t *version, const unsigned int msec)
{
	if((gui == NULL) || (version == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_iboard_device_version_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_IBOARD_DEVICE_VERSION, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_IBOARD_DEVICE_VERSION) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(version, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;  //有数据
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_ERROR; //无数据
	}
	
	return ret;	
}

int lib_gui_light_ctrl_time_set(lib_gui_t *gui, gui_light_ctrl_time_t *time, const unsigned int msec)
{
	if((gui == NULL) || (time == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(gui_light_ctrl_time_t);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_LIGHT_CTRL_TIME, time, length, GUI_RESULT_OK);  
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_LIGHT_CTRL_TIME) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}

int lib_gui_light_ctrl_time_get(lib_gui_t *gui, gui_light_ctrl_time_t *time, const unsigned int msec)
{
	if((gui == NULL) || (time == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_light_ctrl_time_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_LIGHT_CTRL_TIME, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_LIGHT_CTRL_TIME) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(time, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;  
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_ERROR; 
	}
	
	return ret;	
}

int lib_gui_unity_file_info_get(lib_gui_t *gui, gui_unity_file_info_t *file, const unsigned int msec)
{
	if((gui == NULL) || (file == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_unity_file_info_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_GET_UNITY_FILE_INFO, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_GET_UNITY_FILE_INFO) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(file, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;  
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_ERROR; 
	}
	
	return ret;	
}

int lib_gui_lnt_all_info_set(lib_gui_t *gui, gui_lnt_all_info_t *info, const unsigned int msec)
{
	if((gui == NULL) || (info == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = sizeof(gui_lnt_all_info_t);
	gui_package_t pkg;
	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);
	
	__gui_package_create_with_data(&pkg, GUI_CMD_SET_LNT_ALL_INFO, info, length, GUI_RESULT_OK);  
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_LNT_ALL_INFO) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}


int lib_gui_delay_return_bicyle_req(lib_gui_t *gui, const unsigned int msec)
{
	if(gui == NULL)
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	int length = 0;
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_SET_DELAY_BICYCLE_REQ, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_DELAY_BICYCLE_REQ) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;
		
		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;		
}

int lib_gui_delay_return_bicyle_ack(lib_gui_t *gui, gui_delay_return_bicycle_t *bicycle, const unsigned int msec)
{
	if((gui == NULL) || (bicycle  == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_delay_return_bicycle_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_SET_DELAY_BICYCLE_ACK, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_SET_DELAY_BICYCLE_ACK) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(bicycle, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;  
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_ERROR; 
	}
	
	return ret;
}


/* 附近网点信息查询 add by zjc at 2016-11-03 */
int lib_gui_nearby_site_info_qry_req(lib_gui_t *gui, gui_nearby_site_info_qry_req_t *req, const unsigned int msec)
{
	//printf("-----------lib_gui_nearby_site_info_qry_req\n");
	if((gui == NULL) || (req == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	int length = sizeof(gui_nearby_site_info_qry_req_t);
	memset(&pkg, 0, pkg_len);

	__gui_package_create_with_data(&pkg, GUI_CMD_NEARBY_SITE_INFO_REQ, req, length, GUI_RESULT_OK);
	
	__gui_package_writen_with_data(gui->sockfd, &pkg, length);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select_without_data(gui->sockfd, &pkg, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_NEARBY_SITE_INFO_REQ) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EOK;
		else
			return LIB_GUI_ERROR;
	}
	
	return ret;	
}


int lib_gui_nearby_site_info_qry_ack(lib_gui_t *gui, gui_nearby_site_info_qry_ack_t *ack, const unsigned int msec)
{
	if((gui == NULL) || (ack == NULL))
		return LIB_GUI_ERROR;
	
	int ret = LIB_GUI_ERROR;
	unsigned short length = sizeof(gui_nearby_site_info_qry_ack_t);
	gui_package_t pkg;

	int pkg_len = sizeof(struct gui_package);
	memset(&pkg, 0, pkg_len);

	__gui_package_create(&pkg, GUI_CMD_NEARBY_SITE_INFO_ACK, length, GUI_RESULT_OK);
	
	__gui_package_writen(gui->sockfd, &pkg, GUI_HEAD_LEN);
	memset(&pkg, 0, pkg_len);
	ret = __gui_package_readn_select(gui->sockfd, &pkg, length, msec);
	if(ret > 0)
	{
		if(__gui_pkg(&pkg, ret) != LIB_GUI_EOK)
			return LIB_GUI_ERROR;
			
		if(__is_gui_package(&pkg, GUI_CMD_NEARBY_SITE_INFO_ACK) != LIB_GUI_TRUE)
			return LIB_GUI_ERROR;

		memcpy(ack, pkg.data, length);

		if(pkg.result == GUI_RESULT_OK)
			return LIB_GUI_EFULL;  //有数据
		else if(pkg.result == GUI_RESULT_ERR)
			return LIB_GUI_EEMPTY; //无数据
	}
	
	return ret;	
}
/* end of 附近网点信息查询 */



