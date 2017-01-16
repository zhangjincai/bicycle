#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__


#define DEV_CONF_EOK					(0)		//正常
#define DEV_CONF_ERROR					(-1)		//错误
#define DEV_CONF_EMAGIC					(-2)       //魔数错误
#define DEV_CONF_ECRC					(-3)      //CRC错误



struct ndev_config
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
	
	unsigned char site_name[50]; //站点名称  252-50=202
	unsigned char site_QR_code[128];  //站点二维码  202-32=170 //170-96=74 2016-11-25

	unsigned char rfu[74];       //保留 {-4:light_ctrl_time_enable, light_ctrl_time_disable, site_name, site_QR_code}
}__attribute__((packed));
typedef  struct ndev_config ndev_config_t;

 struct stake_config
{
	unsigned short heart_time;       		//桩机心跳时间
	char firmware_uptime[7]; 			//固件更新时间
	unsigned char can_baud_rate;		//CAN波特率
	unsigned short quantity;   			//锁桩数量

	unsigned char rfu[128];       //保留
}__attribute__((packed));
 typedef struct stake_config stake_config_t;

struct device_config
{
	unsigned int magic_head;
	ndev_config_t nconf;
	stake_config_t sconf;
	unsigned short crc16;
	unsigned int magic_tail;
}__attribute__((packed));
typedef struct device_config device_config_t;


int device_config_init(void);
void device_config_destroy(void);
int device_config_put(device_config_t *conf);
int device_config_get(device_config_t *conf);
int device_config_check(device_config_t *conf);

void device_config_printf(void);



#endif


