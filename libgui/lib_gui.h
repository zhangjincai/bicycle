#ifndef __LIB_GUI_H__
#define __LIB_GUI_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include "gui_header.h"

/*
 * LIB GUI静态库版本号
 */
#define LIB_GUI_VERSION		"1.0.3"


/*
 * 函数返回码定义
 */
#define LIB_GUI_NULL				(NULL) 
#define LIB_GUI_EOK				(0)  //正常
#define LIB_GUI_ERROR			(-1) //错误
#define LIB_GUI_ETIMEOUT			(-2) //超时
#define LIB_GUI_EFULL			(-3) //满
#define LIB_GUI_EEMPTY			(-4) //空
#define LIB_GUI_ENOMEM 			(-5) //内存不够
#define LIB_GUI_EXCMEM			(-6) //内存越界
#define LIB_GUI_EBUSY			(-7) //忙
#define LIB_GUI_ERR_COMMAND     	(-8) //不支持的命令

#define LIB_GUI_TRUE				(1)
#define LIB_GUI_FALSE			(0)



enum GUI_CONNECT_MODE
{
	GUI_CONNECT_MODE_UNIX = 0,
	GUI_CONNECT_MODE_TCP,
	GUI_CONNECT_MODE_SERIAL
};

struct tcp_connect_mode
{
	char ipaddr[32];
	unsigned short port;
	unsigned int conn_nsec;
};

struct serial_connect_mode
{
	char pathname[32];
};

struct gui_setup
{
	int connect_mode;
	union{
		struct tcp_connect_mode tcp;
		struct serial_connect_mode serial;
	}s_un;
};



typedef struct gui lib_gui_t;


/*
 * 成功返回:LIB_GUI_EOK
 */

int lib_gui_version(lib_gui_t *gui, char *version, const unsigned int msec);  //获取GUI库版本

lib_gui_t *lib_gui_new(struct gui_setup *setup);
void lib_gui_destroy(lib_gui_t *gui);
int lib_gui_reconnect(lib_gui_t *gui);


int lib_gui_get_ndev_stat(lib_gui_t *gui, gui_ndev_status_t *stat, const unsigned int msec);
int lib_gui_get_sae_stat(lib_gui_t *gui, const unsigned char id, gui_sae_status_t *stat, const unsigned int msec);
int lib_gui_get_ndev_config(lib_gui_t *gui, struct gui_ndev_config *config, const unsigned int msec);
int lib_gui_get_stake_config(lib_gui_t *gui, struct gui_stake_config *config, const unsigned int msec);
int lib_gui_ndev_version(lib_gui_t *gui, char version[32], const unsigned int msec);  //获取节点机固件版本


/*
 * 基本信息页面
 */
  int lib_gui_get_basic_page_config(lib_gui_t *gui, struct gui_basic_info_page_config *config, const unsigned int msec);

/*
 * 节点机配置页面
 */
int lib_gui_get_ndev_page_config(lib_gui_t *gui, struct gui_ndev_page_config *config, const unsigned int msec);  //获取节点机页面配置
int lib_gui_get_ndev_access_pattern_config(lib_gui_t *gui, struct gui_ndev_access_pattern_config *config, const unsigned int msec);  //获取网络接入方式

int lib_gui_set_ndev_ftp_config(lib_gui_t *gui, struct gui_ndev_ftp_config *config, const unsigned int msec);   //设置FTP
int lib_gui_set_ndev_center_config(lib_gui_t *gui, struct gui_ndev_center_ip_config *config, const unsigned int msec);   //设置中心地址--负载均衡服务器地址
int lib_gui_set_ndev_wifi_config(lib_gui_t *gui, struct gui_ndev_wifi_ip_config *config, const unsigned int msec);   //设置WIFI
int lib_gui_set_ndev_local_config_reboot(lib_gui_t *gui, struct gui_ndev_local_ip_config *config, const unsigned int msec);   //设置本地地址
int lib_gui_set_ndev_parameter_config(lib_gui_t *gui, struct gui_ndev_parameter_config *config, const unsigned int msec); //节点机参数设置
int lib_gui_set_ndev_parameter_config_reboot(lib_gui_t *gui, struct gui_ndev_parameter_config *config, const unsigned int msec);  
int lib_gui_set_stake_parameter_config(lib_gui_t *gui, struct gui_stake_parameter_config *config, const unsigned int msec); //锁桩参数设置
int lib_gui_set_ndev_access_pattern_config_reboot(lib_gui_t *gui, struct gui_ndev_access_pattern_config *config, const unsigned int msec); //节点机网络接入方式
int lib_gui_set_lnt_config_reboot(lib_gui_t *gui, struct gui_lnt_page_config *config, const unsigned int msec);  //岭南通配置


/*
 * 锁桩信息页面
 */
  int lib_gui_get_stake_info_page_config(lib_gui_t *gui, struct gui_stake_info_page_config *config, const unsigned int msec);

/*
 * 获取基本信息页面
 */
 int lib_gui_get_ndev_info_page_config_stage1(lib_gui_t *gui, const unsigned int msec);
 int lib_gui_get_ndev_info_page_config_stage2(lib_gui_t *gui, struct gui_ndev_home_page_info *config, const unsigned int msec); //获取基本信息页面

/* 锁桩全部信息 */
int lib_gui_get_stake_all_info_page_config(lib_gui_t *gui, struct gui_stake_all_info_page_config *config, const unsigned int msec);

/* 设置岭南通读卡器状态 */
int lib_gui_set_lnt_card_status(lib_gui_t *gui, struct gui_lnt_card_status *config, const unsigned int msec);

/* 管理卡使用记录 */
int lib_gui_set_admin_card_info(lib_gui_t *gui, gui_admin_card_info_t *info, const unsigned int msec);

/* 异常处理扣费交易记录 */
int lib_gui_set_exception_handle_record(lib_gui_t *gui, gui_exception_handle_record_t *record, const unsigned int msec);

/* 异常处理 */
int lib_gui_except_handle_req(lib_gui_t *gui, gui_except_handle_req_t *req, const unsigned int msec);
int lib_gui_except_handle_ack(lib_gui_t *gui, gui_except_handle_ack_t *ack, const unsigned int msec);


/* 对比信息 */
int lib_gui_get_sae_comparison_status(lib_gui_t *gui, gui_sae_comparison_status_t comp[65], const unsigned int msec);



int lib_gui_access_state(lib_gui_t *gui, gui_access_state_t *access, const unsigned int msec);
int lib_gui_get_gps_info(lib_gui_t *gui, struct gui_gps_info *info, const unsigned int msec);

/* 租还车记录查询 */
int lib_gui_rent_info_qry_req(lib_gui_t *gui, gui_rent_info_qry_req_t *req, const unsigned int msec);
int lib_gui_rent_info_qry_ack(lib_gui_t *gui, gui_rent_info_qry_ack_t *ack, const unsigned int msec);

int lib_gui_iboard_device_version(lib_gui_t *gui, gui_iboard_device_version_t *version, const unsigned int msec);
int lib_gui_light_ctrl_time_set(lib_gui_t *gui, gui_light_ctrl_time_t *time, const unsigned int msec);
int lib_gui_light_ctrl_time_get(lib_gui_t *gui, gui_light_ctrl_time_t *time, const unsigned int msec);

int lib_gui_unity_file_info_get(lib_gui_t *gui, gui_unity_file_info_t *file, const unsigned int msec);

int lib_gui_lnt_all_info_set(lib_gui_t *gui, gui_lnt_all_info_t *info, const unsigned int msec);

int lib_gui_delay_return_bicyle_req(lib_gui_t *gui, const unsigned int msec);
int lib_gui_delay_return_bicyle_ack(lib_gui_t *gui, gui_delay_return_bicycle_t *bicycle, const unsigned int msec);

/* 附近网点信息查询 */
int lib_gui_nearby_site_info_qry_req(lib_gui_t *gui, gui_nearby_site_info_qry_req_t *req, const unsigned int msec);
int lib_gui_nearby_site_info_qry_ack(lib_gui_t *gui, gui_nearby_site_info_qry_ack_t *ack, const unsigned int msec);




/*@*/
#ifdef __cplusplus
}
#endif
#endif

