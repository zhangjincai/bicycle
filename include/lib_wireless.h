#ifndef __LIB_WIRELESS_H__
#define __LIB_WIRELESS_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include <time.h>

/*
 * 函数返回码定义
 */
#define LIB_WL_NULL					(NULL) 
#define LIB_WL_EOK					(0)  //正常
#define LIB_WL_ERROR				(-1) //错误
#define LIB_WL_ETIMEOUT				(-2) //超时
#define LIB_WL_EFULL					(-3) //满
#define LIB_WL_EEMPTY				(-4) //空
#define LIB_WL_ENOMEM 				(-5) //内存不够
#define LIB_WL_EXCMEM				(-6) //内存越界
#define LIB_WL_EBUSY				(-7) //忙
#define LIB_WL_ERR_COMMAND			(-8) //不支持该指令

#define LIB_WL_TRUE					(1)
#define LIB_WL_FALSE					(0)


/*
 * 拨号状态
 */
enum WL_DIAL_STAT
{
	WL_DIAL_STAT_UNDIAL = 0,
	WL_DIAL_STAT_DIALING,
	WL_DIAL_STAT_DIALOK,
	WL_DIAL_STAT_SOFT_RESET,
	WL_DIAL_STAT_HW_RESET,
	WL_DIAL_STAT_HW_RESTART,
	WL_DIAL_STAT_NOT_SIM,
};

/*
 * 网络信号等级
 */
enum WL_SIGNAL_LEVEL
{
	WL_SIGNAL_LEVEL0 = 0,
	WL_SIGNAL_LEVEL1,
	WL_SIGNAL_LEVEL2,
	WL_SIGNAL_LEVEL3,
	WL_SIGNAL_LEVEL4,
	WL_SIGNAL_LEVEL5,
};

/*
 * 网络模式
 */
enum WL_NETWORK_MODEL
{
	WL_NETWORK_MODEL_WIRED = 1,
	WL_NETWORK_MODEL_WIRELESS,
};

/*
 * 操作指令
 */
#define LIB_WL_CMD_RESET_SET				0x01
#define LIB_WL_CMD_DIAL_STAT_GET			0x02
#define LIB_WL_CMD_HWVER_GET				0x03
#define LIB_WL_CMD_SYSINFO_GET			0x04
#define LIB_WL_CMD_CGMR_GET				0x05
#define LIB_WL_CMD_CGMI_GET				0x06
#define LIB_WL_CMD_CGMM_GET				0x07
#define LIB_WL_CMD_CSQ_GET				0x08
#define LIB_WL_CMD_PPPD_INFO_GET			0x09
#define LIB_WL_CMD_PPPD_ONLINE			0x10
#define LIB_WL_CMD_CSQ_SYSINFO			0x11
#define LIB_WL_CMD_CUR_FLOW_INFO			0x12
#define LIB_WL_CMD_DIAL_TIME				0x13
#define LIB_WL_CMD_FN_CB					0x14
#define LIB_WL_CMD_MODEL_SET				0x15
#define LIB_WL_CMD_GPS_ZGPSR				0x16
#define LIB_WL_CMD_GPS_INFO				0x17
#define LIB_WL_CMD_3G_HW_RESET				0x18
#define LIB_WL_CMD_3G_SOFT_RESET				0x19



typedef void *(*fn_wl)(void *);

/*
 * PPPD拨号信息
 */
struct lib_wl_pppd_info
{
	unsigned int using_channel;
	char using_interface[16];
	char local_ip_address[32];
	char remote_ip_address[32];
	char primary_dns_address[32];
	char secondary_dns_address[32];
}__attribute__((packed));
typedef struct lib_wl_pppd_info lib_wl_pppd_info_t;

/*
  * 查询模块软件版本 
  */
struct lib_wl_cgmr
{
	char softversion[32];
}__attribute__((packed));
typedef struct lib_wl_cgmr lib_wl_cgmr_t;

/*
 * 厂商信息查询 
 */
struct lib_wl_cgmi
{
	char manufacturer[32];
}__attribute__((packed));
typedef struct lib_wl_cgmi lib_wl_cgmi_t;

/*
 * 产品名称查询命令 
 */
 struct lib_wl_cgmm
{
	char production_name[32];
}__attribute__((packed));
typedef struct lib_wl_cgmm lib_wl_cgmm_t;

/*
 * 查询硬件版本
 */
struct lib_wl_hwver
{
	char firmversion[32];
}__attribute__((packed));
typedef struct lib_wl_hwver lib_wl_hwver_t;

/*
 * 信号强度查询
 */
struct lib_wl_csq
{
	unsigned char rssi;   //信号强度
	unsigned char fer;    //信道误帧率
}__attribute__((packed));
typedef struct lib_wl_csq lib_wl_csq_t;

/*
 * 系统的信息查询命令 
 */
struct lib_wl_sysinfo
{
	unsigned char srv_status;  	//系统服务状态
	unsigned char srv_domain;	//系统服务域
	unsigned char roam_status; //漫游状态
	unsigned char sys_mode;    //系统模式
	unsigned char sim_state;   //UIM卡状态
}__attribute__((packed));
typedef struct lib_wl_sysinfo lib_wl_sysinfo_t;


struct lib_wl_csq_sysinfo
{
	unsigned char rssi;   //信号强度
	unsigned char fer;    //信道误帧率
	unsigned char srv_status;  	//系统服务状态
	unsigned char srv_domain;	//系统服务域
	unsigned char roam_status; //漫游状态
	unsigned char sys_mode;    //系统模式
	unsigned char sim_state;   //UIM卡状态
}__attribute__((packed));
typedef struct lib_wl_csq_sysinfo lib_wl_csq_sysinfo_t;

struct lib_wl_flow_info
{
	long long rx_bytes;
	long long tx_bytes;
}__attribute__((packed));
typedef struct lib_wl_flow_info lib_wl_flow_info_t;

struct lib_wl_dial_time
{
	long last_discon_time;
	long last_dial_time;
}__attribute__((packed));
typedef struct lib_wl_dial_time lib_wl_dial_time_t;

struct wl_zgpsr
{
	double UTC;
	double latitude;
	double longitude;
	double hdop;
	double altitude;
	int fix;
	double cog;
	double spkm;
	double spkn;
	int date;
	int nsat;
}__attribute__((packed));
typedef struct wl_zgpsr wl_zgpsr_t;

struct wl_gga
{
	double latitude;  				//纬度，格式为 ddmm.mmmmm（度分格式）
	unsigned char ns_indicator;  		//纬度半球，N 或 S（北纬或南纬）
	double longitude; 				//经度，格式为 dddmm.mmmmm（度分格式）
	unsigned char ew_indicator; 		//经度半球，E 或 W（东经或西经）
	int status; 					// GPS 状态，0=未定位，1=定位 
	int satellite; 					//正在使用的用于定位的卫星数量（00~12） 
};
typedef struct wl_gga wl_gga_t;

struct wl_gps_attr
{
	unsigned char gga:1;
	unsigned char tm:1;
	unsigned char rfu:6;
}__attribute__((packed));
typedef struct wl_gps_attr wl_gps_attr_t;

struct wl_gps_info
{
	struct tm ptm;
	struct wl_gga gga;
	struct wl_gps_attr attr;
}__attribute__((packed));
typedef struct wl_gps_info wl_gps_info_t;




typedef struct wl lib_wl_t;
char *lib_wl_version(void);
lib_wl_t *lib_wl_new(void);
void lib_wl_destroy(lib_wl_t *wl);
int lib_wl_reconnect(lib_wl_t *wl);
enum WL_DIAL_STAT lib_wl_dial_stat_get(lib_wl_t *wl, const unsigned int msec);	
int lib_wl_pppd_is_online(lib_wl_t *wl, const unsigned int msec);
int lib_wl_sysinfo_get(lib_wl_t *wl, lib_wl_sysinfo_t *info, const unsigned int msec);
int lib_wl_csq_sysinfo_get(lib_wl_t *wl, lib_wl_csq_sysinfo_t *info, const unsigned int msec);
int lib_wl_cgmr_get(lib_wl_t *wl, lib_wl_cgmr_t *cgmr, const unsigned int msec);
int lib_wl_hwver_get(lib_wl_t *wl, lib_wl_hwver_t *hwver, const unsigned int msec);
int lib_wl_pppd_info_get(lib_wl_t *wl, lib_wl_pppd_info_t *pppd, const unsigned int msec);
int lib_wl_current_flow_info_get(lib_wl_t *wl, lib_wl_flow_info_t *flow_info, const unsigned int msec);
int lib_wl_dial_time_get(lib_wl_t *wl, lib_wl_dial_time_t *dial_time, const unsigned int msec);
int lib_wl_csq_get(lib_wl_t *wl, lib_wl_csq_t *csq, const unsigned int msec);
enum WL_SIGNAL_LEVEL lib_wl_signal_level_get(lib_wl_t *wl, const unsigned int msec);	
int lib_wl_set_model(lib_wl_t *wl, enum WL_NETWORK_MODEL model, const unsigned int msec);
int lib_wl_get_gps_info(lib_wl_t *wl, wl_gps_info_t *info, const unsigned int msec);
int lib_wl_get_gps_zgpsr(lib_wl_t *wl, wl_zgpsr_t *zgpsr, const unsigned int msec);
int lib_wl_dial_stat_change_cb(lib_wl_t *wl, fn_wl *fn);
int lib_wl_3g_hw_reset(lib_wl_t *wl, const unsigned int msec); /* msec建议为2000ms以上 */
int lib_wl_3g_soft_reset(lib_wl_t *wl, const unsigned int msec);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

