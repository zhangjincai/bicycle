#ifndef __GUI_HEADER_H__
#define __GUI_HEADER_H__


struct gui_bit_status
{
	unsigned int except_ack:1;
	unsigned int rfu:31;
}__attribute__((packed));
typedef struct gui_bit_status gui_bit_status_t;


/*
 * 库版本
 */
 struct gui_version
{
	char ver[32];
}__attribute__((packed));
typedef  struct gui_version gui_version_t;

/*
 * 节点机软版本
 */
 struct gui_ndev_version
{
	char ver[32];
}__attribute__((packed));
typedef  struct gui_ndev_version gui_ndev_version_t;

/*
 * 节点机状态  1:正常, 0:异常
 */
struct gui_ndev_status
{
	unsigned short power:1;				//电源电压状态
	unsigned short rtc:1;				//RTC状态
	unsigned short can:1;				//CAN总线状态
	unsigned short center:1;				//与中心通讯状态
	unsigned short wireless:1;			// 3G/4G网络状态
	unsigned short yct:1;				//羊城通读卡器
	unsigned short gps:1;				//gps模块状态
	unsigned short keyboard:1;			//键盘板状态
	unsigned short ext_mcu:1;			//辅助CPU板状态
	unsigned short time:1;				//时间同步状态
	unsigned short rfu:6;				//保留
}__attribute__((packed));
typedef struct gui_ndev_status gui_ndev_status_t;

/*
 * 锁桩状态  1:正常, 0:异常
 */
struct gui_sae_status
{
	unsigned short in:1;		//锁舌收回检测
	unsigned short out:1;		//锁舌伸出检测
	unsigned short bike:1;		//是否有车
	unsigned short op:1;		//操作成功
	unsigned short rc531:1;		//RC531响应
	unsigned short lock:1;		//电子锁响应
	unsigned short time:1;		//时间同步状态
	unsigned short volt:1;		//电源电压状态
	unsigned short rtc:1;		//RTC状态
	unsigned short can:1;		//CAN总线状态
	unsigned short rfu:6;		//保留
}__attribute__((packed));
typedef struct gui_sae_status gui_sae_status_t;

/*
 * 节点机设备配置
 */
struct gui_ndev_config
{
	char boot_ver[32];				//UBOOT 版本
	char kernel_ver[32];			//kernel版本
	char rootfs_ver[32];			//rootfs版本
	char fw_ver[32]; 				//firmware版本
	char appl_ver[32];				//appl版本
	
	unsigned int local_ip;			//本地IP
	unsigned int local_submask;		//本地子网掩码
	unsigned int local_gateway;		//本地网关
	unsigned int local_dns;			//域名地址

	unsigned int wifi_ip;			//WIFI 接入IP
	unsigned int wifi_submask;		//WIFI 子网掩码
	unsigned int wifi_gateway;		//WIFI 网关
	unsigned int wifi_dns;			//WIFI 域名地址
	
	unsigned char using_wireless;   //使用无线网络   {1:无线 2:有线}
	
	unsigned int load_server1_ip;		//负载均衡服务器1 IP 地址
	unsigned int load_server2_ip;		//负载均衡服务器2 IP 地址
	unsigned short load_server1_port;	//负载均衡1端口
	unsigned short load_server2_port;	//负载均衡2端口
	unsigned short load_timeout;			//负载均衡超时时间(秒)
		
	unsigned int ftp_ip;
	unsigned short ftp_port;
	char ftp_username[32];
	char ftp_password[32];

	unsigned short heart_time;		//中心心跳时间
	unsigned short emerg_heart_time; //中心紧急心跳时间
	unsigned char timer_gate_value;  //时钟修正闸值
	unsigned short term_id; 		//终端编号
	unsigned short area_info;       	//分区信息
	
	unsigned int flow_3g;   		// 3G流量
	unsigned short first_level_3g;  // 3G流量第一闸值
	unsigned short second_level_3g; // 3G流量第二闸值

	unsigned int lnt_ipaddr;   //岭南通中心IP
	unsigned short lnt_port;  //岭南通中心端口
	unsigned char lnt_userid[16];  //岭南通用户ID
	unsigned char lnt_spno[2]; //服务商代码
	unsigned char lnt_conpa; //消费初始化参数
	unsigned char lnt_cvad; //卡离线有效期启动	

	unsigned char light_ctrl_time_enable[2];  //hhmm
	unsigned char light_ctrl_time_disable[2];  //hhmm
	

	unsigned char rfu[252];       //保留 {-4:light_ctrl_time_enable, light_ctrl_time_disable}
}__attribute__((packed));
typedef struct gui_ndev_config gui_ndev_config_t;

/*
 * 桩机设备配置
 */
struct gui_stake_config
{
	unsigned short heart_time;       		//桩机心跳时间
	char firmware_uptime[7]; 			//固件更新时间
	unsigned char can_baud_rate;		//CAN波特率
	unsigned short quantity;   			//锁桩数量


	unsigned char rfu[128];       //保留
}__attribute__((packed));
typedef struct gui_stake_config gui_stake_config_t;














/*
 * 基本信息页面配置
 */
struct gui_basic_info_page_config
{
	char ndev_firmware_ver[32];  //节点机版本
	char stake_firmware_ver[32];  //桩版本
	char yct_firmware_ver[32];	//羊城通版本
		
	char terminal_name[50];	//终端名称
	unsigned short terminal_no;	//终端编号
	unsigned int register_time;	//注册时间
	char conn_ipaddr[32];	//中心连接地址
	unsigned short conn_port; //连接端口
	unsigned int last_time_power_on; //最后一次上电时间

	unsigned int dial_stat;		//拨号状态
	unsigned char	system_mode;	//系统模式
	unsigned char	uim_card_stat;	//UIM卡状态
	unsigned char rssi;	//信号强度
	unsigned char fer;	//信道误帧率

	unsigned char center_conn_stat;	//自行车中心连接
	unsigned char load_bal_stat;       //负载均衡状态

	char total_blacklist_ver[9];	//总量黑名单
	char inc_blacklist_ver[9];	//增量黑名单
	char dec_blacklist_ver[9];	//减量黑名单
	char temp_blacklist_ver[9];  //临时黑名单
	char stake_para[9];   //锁桩配置
	char unionpay_key[9];  //银联密钥

	unsigned short use_total_seq;
	unsigned short use_inc_seq;
	unsigned short use_dec_seq;
	unsigned short use_temp_seq;
	unsigned short use_fw_seq;
	unsigned short use_stake_para_seq;
	unsigned short use_unionpay_key_seq;
	
	unsigned char use_total_db;			
	unsigned char use_inc_db;
	unsigned char use_dec_db;
	unsigned char use_temp_db;
	unsigned char use_fw_db;
	unsigned char use_stake_para_db;
	unsigned char use_unionpay_key_db;
	
	gui_ndev_status_t nstatus; //节点机状态信息
}__attribute__((packed));
typedef struct gui_basic_info_page_config gui_basic_info_page_config_t;


/*
 * 本机IP地址
 */
struct gui_ndev_local_ip_config
{
	char ipaddr[16];
	char subnet_mask[16];
	char default_gateway[16];
}__attribute__((packed));
typedef struct gui_ndev_local_ip_config gui_ndev_local_ip_config_t;

/*
 * WIFI-IP地址
 */
struct gui_ndev_wifi_ip_config
{
	char ipaddr[16];
	char subnet_mask[16];
	char default_gateway[16];
}__attribute__((packed));
typedef struct gui_ndev_wifi_ip_config gui_ndev_wifi_ip_config_t;

/*
 * 自行车中心地址
 */
struct gui_ndev_center_ip_config
{
	char load1_ipaddr[16];
	unsigned short load1_port;
	char load2_ipaddr[16];
	unsigned short load2_port;
}__attribute__((packed));
typedef struct gui_ndev_center_ip_config gui_ndev_center_ip_config_t;

/*
 * FTP地址
 */
struct gui_ndev_ftp_config
{
	char ipaddr[16];
	unsigned short port;
	char user_name[16];
	char user_password[16];
}__attribute__((packed));
typedef struct gui_ndev_ftp_config gui_ndev_ftp_config_t;

/*
 * 节点机参数设置
 */
struct gui_ndev_parameter_config
{
	unsigned short load_timeout;
	unsigned short heart_time;
	unsigned short emerg_heart_time;
	unsigned char timer_gate_value;  //时钟修正闸值
	unsigned short term_id;
}__attribute__((packed));
typedef struct gui_ndev_parameter_config gui_ndev_parameter_config_t;

/*
 * 锁桩参数设置
 */
struct gui_stake_parameter_config
{
	unsigned char can_baud_rate;
	unsigned short heart_time; 
	unsigned short quantity; 	
}__attribute__((packed));
typedef struct gui_stake_parameter_config gui_stake_parameter_config_t;

/*
 * 网络接入方式
 */
struct gui_ndev_access_pattern_config
{
	unsigned char using_wireless;   //使用无线网络
}__attribute__((packed));
typedef struct gui_ndev_access_pattern_config gui_ndev_access_pattern_config_t;

/*
 * 岭南通中心设置
 */
struct gui_lnt_page_config
{
	char ipaddr[16];   //岭南通中心IP
	unsigned short port;  //岭南通中心端口
	unsigned char userid[16];  //岭南通用户ID
	unsigned char spno[2]; //服务商代码
	unsigned char conpa; //消费初始化参数
	unsigned char cvad; //卡离线有效期启动	
}__attribute__((packed));
typedef struct gui_lnt_page_config gui_lnt_page_config_t;


/*
 * 获取节点机页面配置
 */
struct gui_ndev_page_config
{
	struct gui_ndev_local_ip_config ip_conf;
	struct gui_ndev_wifi_ip_config wifi_ip_conf;
	struct gui_ndev_center_ip_config center_ip_conf;
	struct gui_ndev_ftp_config ftp_conf;
	struct gui_ndev_parameter_config n_param_conf;
	struct gui_stake_parameter_config s_param_conf;
	struct gui_ndev_access_pattern_config access_conf;
	struct gui_lnt_page_config lnt_conf;
}__attribute__((packed));
typedef struct gui_ndev_page_config gui_ndev_page_config_t;

/*
 * 获取锁桩信息页面
 */
struct gui_stake_info_page_config
{
	unsigned char s_stat[17];	//锁桩全部状态
	unsigned char s_bicycle_exist_stat[17]; //车是否在桩状态
	unsigned char max;     	//支持最大锁桩数
	unsigned char online;    	//在线锁桩数
	unsigned char quantity;	//锁桩数量
	unsigned short status[64];  //存放锁桩状态
}__attribute__((packed));
typedef struct gui_stake_info_page_config gui_stake_info_page_config_t;


/* 节点机主页显示信息 */
struct gui_ndev_home_page_info
{
	char terminal_name[50];   		//站点名称
	unsigned short terminal_no;		//终端编号
	unsigned short network_type:2;	//网络类型
	unsigned short signal_level:3;	// 3/4G信号等级
	unsigned short wifi_status:1;         //wifi状态
	unsigned short gps_status:1;         //gps状态
	unsigned short center_status:1;    //连接中心状态
	unsigned short yct_status:1;         //连接羊城通中心状态
	unsigned short canbus_status:1;  //can总线状态
	unsigned char stake_online_num;  //锁桩在线数
	unsigned char stake_total_num; //锁桩总数
	
	unsigned char site_QR_code[128];   //站点二维码  //32->128 2016-11-25
	
	#if 1 //CONFS_USING_READER_UPDATE
	unsigned char lnt_firmware_update_flag:1;		 //岭南通固件升级标记,1:有新固件需要升级,0:没有新固件需要升级
	char lnt_firmware_path[96]; 	 //岭南通读卡器升级固件路径
	#endif
}__attribute__((packed));
typedef struct gui_ndev_home_page_info gui_ndev_home_page_info_t;



/* 锁桩信息统计 */
struct gui_sae_info
{
	unsigned char healthy_flag;			//标志 1:正常 0:无效
	unsigned char send_hb_times;		//心跳发送时加一,接收成功后减一	
	unsigned char id;					//锁桩编号-- 十进制
	unsigned short status;				//桩机状态
	unsigned char s_psam[7];			//PSAM卡编号
	unsigned char s_id[4];				//锁桩编号
	unsigned char s_ver[9];				//锁桩版本号
	unsigned char s_lock_id[4];			//锁控编号
	unsigned char s_lock_ver[9];			//锁控版本号
	unsigned char s_sn[7];				//锁桩流水号
	unsigned char s_total_bl_ver[9];		//总量黑名单版本
	unsigned char s_inc_bl_ver[9];		//增量黑名单版本
	unsigned char s_dec_bl_ver[9];		//减量黑名单版本
	unsigned char s_temp_bl_ver[9];		//临时黑名单版本
	unsigned char s_stake_para[9];           //锁桩配置
	unsigned char s_unionpay_key[9];     //银联配置
	unsigned char s_time[13]; 			//锁桩时间
	unsigned int s_reg_time;			//签到时间
	unsigned char s_phy_sn[9];			//锁桩物理编号
}__attribute__((packed));
typedef struct gui_sae_info gui_sae_info_t;

struct gui_stake_all_info_page_config
{
	unsigned char quantity;                              //锁桩总数
	struct gui_sae_info info[65];			//锁桩信息
}__attribute__((packed));
typedef struct gui_stake_all_info_page_config gui_stake_all_info_page_config_t;

/* 岭南通读卡器状态 */
struct gui_lnt_card_status
{
	unsigned char lnt_card_stat:1;
}__attribute__((packed));
typedef struct gui_lnt_card_status gui_lnt_card_status_t;

/*
 * 管理卡使用记录
 */
struct gui_admin_card_info
{
	unsigned int sn;			//流水号
	unsigned char pid[8];              //物理卡号
	unsigned char ticket[42];		//票卡信息
	unsigned char rentinfo[36];	//租还车记录
	unsigned char time[6]; 		 //YYMMDDHHmmSS
}__attribute__((packed));
typedef struct gui_admin_card_info gui_admin_card_info_t;

/*
 * 异常处理扣费交易记录
 */
 struct gui_exception_handle_record
{
	unsigned int sn;			//流水号
	unsigned char pid[8];              //物理卡号
	unsigned char record[90];      //交易记录
}__attribute__((packed));
typedef struct gui_exception_handle_record gui_exception_handle_record_t;


struct gui_except_handle_req
{
	unsigned int sn;
	unsigned char pid[8];
	unsigned char ticket[42];
	unsigned char rentinfo[36];
}__attribute__((packed));
typedef struct gui_except_handle_req gui_except_handle_req_t;



#define GUI_EX_HDL_NORMAL				0x00   //正常(无异常处理)
#define GUI_EX_HDL_NOT_CARD			0x01   //还车未刷卡
#define GUI_EX_HDL_NOT_COST			0x02  //欠费
#define GUI_EX_HDL_NOT_DEPOSIT			0x03  //未交押金
#define GUI_EX_HDL_NOT_BIKE				0x04 //刷卡未取车
#define GUI_EX_HDL_MANUAL				0xFF   //后台异常(人工点查询)



struct gui_except_handle_ack
{
	unsigned int sn;
	unsigned char pid[8];
	unsigned char status;
	unsigned int fee; //扣费
	unsigned char info[48];
}__attribute__((packed));
typedef struct gui_except_handle_ack gui_except_handle_ack_t;


/*
 * 锁桩比对状态
 */
 struct gui_sae_comparison_status
{
	unsigned char fw:1;  //固件
	unsigned char bl_total:1; //总量黑名单
	unsigned char bl_inc:1; //增量黑名单
	unsigned char bl_dec:1;  //减量黑名单
	unsigned char bl_temp:1;  //临时黑名单
	unsigned char stake_para:1;  //锁桩配置
	unsigned char unionpay_key:1;  //银联密钥
	unsigned char rfu:1;
}__attribute__((packed));
typedef struct gui_sae_comparison_status gui_sae_comparison_status_t;


/*
 * 接入方式、连接状态
 */
 struct gui_access_state
{
	unsigned char access;   //接入方式
	unsigned int connect_stat;  //连接状态
}__attribute__((packed));
typedef  struct gui_access_state gui_access_state_t;



/*
 * GPS
 */
 struct g_gps_attr
{
	unsigned char gga:1;
	unsigned char tm:1;
	unsigned char rfu:6;
}__attribute__((packed));
typedef struct g_gps_attr g_gps_attr_t;

struct g_gps_tm 
{
	int tm_sec;       /* 秒 – 取值区间为[0,59] */
	int tm_min;       /* 分 - 取值区间为[0,59] */
	int tm_hour;      /* 时 - 取值区间为[0,23] */
	int tm_mday;      /* 一个月中的日期 - 取值区间为[1,31] */
	int tm_mon;       /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
	int tm_year;      /* 年份，其值等于实际年份减去1900 */
	int tm_wday;      /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	int tm_yday;      /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
	
	int tm_isdst;   /*夏令时标识符，使用夏令时，tm_isdst为正，不使用夏令时，tm_isdst为0，不了解情况时，tm_isdst为负*/ 
};

struct g_gps_gga
{
	double latitude;  //纬度，格式为 ddmm.mmmmm（度分格式）
	unsigned char ns_indicator;  //纬度半球，N 或 S（北纬或南纬）
	double longitude; //经度，格式为 dddmm.mmmmm（度分格式）
	unsigned char ew_indicator; //经度半球，E 或 W（东经或西经）
	int status; // GPS 状态，0=未定位，1=非差分定位，2=差分定位  
	int satellite; //正在使用的用于定位的卫星数量（00~12） 
};

struct gui_gps_info
{
	struct g_gps_attr attr;
	
	struct g_gps_tm tm;
	struct g_gps_gga gga;
};
typedef struct gui_gps_info gui_gps_info_t;


/*
 * 租还车记录查询
 */
struct gui_rent_info_qry_req
{
	unsigned char pid[8];		//票卡物理卡号
	unsigned char item;       //记录条数
}__attribute__((packed));
typedef struct gui_rent_info_qry_req gui_rent_info_qry_req_t;

struct gui_rent_info_qry_ack
{
	unsigned char pid[8];		//票卡物理卡号
	unsigned char item;       		//记录条数
	unsigned short length;   //记录长度
	unsigned char record[2048];  //记录
}__attribute__((packed));
typedef struct gui_rent_info_qry_ack gui_rent_info_qry_ack_t;

struct gui_rent_info_fmt		//租用记录格式
{ 
	unsigned char bike_NO[32];		//自行车编号
	unsigned char rent_name[64];	//租车点名称
	unsigned char rent_time[32];		//租车时间
	unsigned char return_name[64];	//还车点名称
	unsigned char return_time[32];	//还车时间
	unsigned char used_time[16];		//骑行时间
	unsigned char fee[16];			//扣费
}__attribute__((packed));
typedef struct gui_rent_info_fmt gui_rent_info_fmt_t;


/*
 * 内存信息
 */
struct gui_proc_meminfo
{
	unsigned char mac0[6];
	unsigned char mac1[6];
	
	unsigned int MemTotal;
	unsigned int MemFree;
	unsigned int Buffers;
	unsigned int Cached;
	unsigned int SwapCached;
	unsigned int Active;
	unsigned int Inactive;
	unsigned int Active_anon;
	unsigned int Inactive_anon;
	unsigned int Active_file;
	unsigned int Inactive_file;
	unsigned int Unevictable;
	unsigned int Mlocked;
	unsigned int SwapTotal;
	unsigned int SwapFree;
	unsigned int Dirty;
	unsigned int Writeback;
	unsigned int AnonPages;
	unsigned int Mapped;
	unsigned int Shmem;
	unsigned int Slab;
	unsigned int SReclaimable;
	unsigned int SUnreclaim;
	unsigned int KernelStack;
	unsigned int PageTables;
	unsigned int NFS_Unstable;
	unsigned int Bounce;
	unsigned int WritebackTmp;
	unsigned int CommitLimit;
	unsigned int Committed_AS;
	unsigned int VmallocTotal;
	unsigned int VmallocUsed;
	unsigned int VmallocChunk;
};
typedef struct gui_proc_meminfo gui_proc_meminfo_t;

/*
 * 接口板版本信息
 */
struct gui_iboard_device_version
{
	unsigned short vendor_encoding; //供应商编码
	unsigned char device_type;  //设备类型
	unsigned char rfu[5];   //保留
	unsigned char device_sn[16];  //设备序列号
	unsigned short product_batch; //产品批次
	unsigned short hardware_version;  //硬件版本号
	unsigned char software_version[8]; //软件版本号	
}__attribute__((packed));
typedef struct gui_iboard_device_version gui_iboard_device_version_t;

/*
 * 灯箱控制
 */
struct gui_light_ctrl_time
{
	unsigned char light_ctrl_time_enable[2];  //hhmm
	unsigned char light_ctrl_time_disable[2];  //hhmm
}__attribute__((packed));
typedef struct gui_light_ctrl_time gui_light_ctrl_time_t;


/*
 * 泛文件信息
 */
struct gui_unity_file_struct
{
	unsigned short file_seq;		 //文件序号
	unsigned short total;		//分割总数
	unsigned short div_seq;		//分割序号

	int status;			//状态	1:正在下载, 2:正在广播
	time_t start_time;  		 //开始下载时间
	time_t end_time;		//结束下载时间
}__attribute__((packed));

struct gui_unity_file_info
{
	struct gui_unity_file_struct ndev_ufile_st[6];  //节点机下载
	struct gui_unity_file_struct broadcast_ufile_st[6];  //广播泛文件
	struct gui_unity_file_struct stake_ufile_st[6];  //锁桩下载  
}__attribute__((packed));
typedef struct gui_unity_file_info gui_unity_file_info_t;

/*
 * 岭南通信息
 */
struct gui_lnt_all_info
{
	unsigned char lib_version[32];   //库版本
	unsigned char hw_version[32];  //读卡器版本

	unsigned int register_stat;   //注册状态
	unsigned char pki_r[4];  //PSAM卡号
	unsigned char psam[4];   //PKI
	
	unsigned int pkt_sn_RO; //报文序列号
	
	unsigned char proc:4;     //执行步骤
	unsigned char fresult:4;  //执行结果
	unsigned char stat;      	//返回状态
	unsigned short sw;  	//返回码
	time_t time;  		//执行时间
	unsigned char agent_err; //代理错误码
}__attribute__((packed));
typedef struct gui_lnt_all_info gui_lnt_all_info_t;

/*
 * 延迟还车
 */
 struct gui_delay_return_bicycle
{
	int quantity;	 			//锁桩数量
	int register_count;		//注册锁桩数
	int bicycle_online_count;   //有车锁桩数
}__attribute__((packed));
typedef  struct gui_delay_return_bicycle gui_delay_return_bicycle_t;
 

struct lnt_firmware_update_result
{
	char type;
	char result;  //0:初始化，1:成功，2:失败
	char ftp_local_path[96];
}__attribute__((packed));
typedef struct lnt_firmware_update_result lnt_firmware_update_result_t;

/* 备份gps信息 */
struct gps_gga_info_backup
{
	double latitude;  //纬度，格式为 ddmm.mmmmm（度分格式）
	unsigned char ns_indicator;  //纬度半球，N 或 S（北纬或南纬）
	double longitude; //经度，格式为 dddmm.mmmmm（度分格式）
	unsigned char ew_indicator; //经度半球，E 或 W（东经或西经）
	int status; // GPS 状态，0=未定位，1=非差分定位，2=差分定位  
	int satellite; //正在使用的用于定位的卫星数量（00~12） 
};
typedef struct gps_gga_info_backup gps_gga_info_backup_t;

/*
 * 附近网点信息查询 add by zjc at 2016-11-03
 */
struct gui_nearby_site_info_qry_req
{
	unsigned short siteNo;		//网点终端编号
	unsigned char gpsInfo[32];       //网点经纬度信息
}__attribute__((packed));
typedef struct gui_nearby_site_info_qry_req gui_nearby_site_info_qry_req_t;

struct gui_nearby_site_info_qry_ack
{
	unsigned char siteNo[2];		//网点终端编号
	unsigned char item;       		//网点信息条数
	unsigned short length;   //网点信息长度
	unsigned char info[2048];  //网点信息内容
}__attribute__((packed));
typedef struct gui_nearby_site_info_qry_ack gui_nearby_site_info_qry_ack_t;

struct gui_nearby_site_info_fmt		//附近网点信息格式
{ 
	unsigned char siteName[64];		//网点名称
	unsigned char distance[16];	//距离
	unsigned char bikes[16];	//自行车数
	unsigned char stacks[16];	//总桩数
}__attribute__((packed));
typedef struct gui_nearby_site_info_fmt gui_nearby_site_info_fmt_t;

/* end of 附近网点信息查询 */

/* 网卡信息 2016-12-21 */
struct gui_net_info_backup
{
	long long rx_bytes;
	long long tx_bytes;
	long long total_bytes;
};
typedef struct gui_net_info_backup gui_net_info_backup_t;


#endif

