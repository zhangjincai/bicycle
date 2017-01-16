#ifndef __DEFINES_H__
#define __DEFINES_H__

/*
 * 函数返回码定义
 */
#define B_DEV_NULL					(NULL) 
#define B_DEV_EOK					(0)  //正常
#define B_DEV_ERROR					(-1) //错误
#define B_DEV_ETIMEOUT				(-2) //超时
#define B_DEV_EFULL					(-3) //满
#define B_DEV_EEMPTY				(-4) //空
#define B_DEV_ENOMEM 				(-5) //内存不够
#define B_DEV_EXCMEM				(-6) //内存越界
#define B_DEV_EBUSY					(-7) //忙
#define B_DEV_NOT_SUPPORT			(-8) //不支持该指令

#define B_DEV_TRUE					(1)
#define B_DEV_FALSE					(0)

#define B_DEV_CHAR_MAX				0xff
#define B_DEV_SHORT_MAX			0xffff
#define B_DEV_INT_MAX				0xffffffff


#define FN_SEL_TRUE					(1)
#define FN_SEL_FLASE					(0)


/* 接收缓冲区大小 */
#define B_DEV_RXBUF_SZ				(2048)


/* 日志定义 */
#define BICYCLE_LOG_RUN

#ifdef BICYCLE_LOG_RUN
#include <syslog.h>
#define SYS_LOG_EMERG(fmt, args...) 		syslog(LOG_EMERG, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 		syslog(LOG_ALERT, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			syslog(LOG_CRIT, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			syslog(LOG_ERR, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		syslog(LOG_WARNING, fmt, ##args) 
#define SYS_LOG_NOTICE(fmt, args...)  		syslog(LOG_NOTICE, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			syslog(LOG_INFO, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		syslog(LOG_DEBUG, fmt, ##args)
#else
#define SYS_LOG_EMERG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_NOTICE(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#endif


/*
 * ***************************************  桩机部分 **************************************
 */
#define SAE_HD_H						0x55
#define SAE_HD_L						0xAA

#define SAE_CAN_LOCAL_ID			0x00	//CAN 本地ID
#define SAE_CAN_BROADCAST_ID		0x7F	//CAN 广播ID

#define SAE_CAN_BUS_0				0	//CAN 总线0
#define SAE_CAB_BUS_1				1	//CAN 总线1

#define SAE_DEV_REG_REQ				0x81	//设备签到请求
#define SAE_DEV_REG_ACK				0x91	//设备签到确定
#define SAE_DEV_CTRL_REQ			0x82	//设备控制请求
#define SAE_DEV_CTRL_ACK			0x92	//设备控制确认
#define SAE_FILE_TRANS_REQ			0x83	//泛文件传输请求
#define SAE_FILE_TRANS_ACK			0x93	//泛文件传输确认
#define SAE_PASS_REQ				0x84 //数据透传
#define SAE_PASS_ACK				0x94



/*
 *  锁桩协议引导字
 */
#define SAE_REG_W					"aa"		//锁桩主动签到和节点机应答
#define SAE_N_REG_W					"ab"		//节点机主动签到和桩机应答
#define SAE_N_BHEART_W				"ac"		//节点机主动心跳和桩机心跳应答
#define SAE_ID_W						"ad"		//锁桩编号
#define SAE_VER_W					"ae"		//锁桩版本号
#define SAE_LOCK_ID_W				"af"		//锁控编号
#define SAE_LOCK_VER_W				"ag"		//锁控版本号
#define SAE_PSAM_W					"ah"		//PSAM卡编号
#define SAE_SN_W					"ba"		//锁桩流水号
#define SAE_TOTAL_BLK_VER_W		"bb"		//总量黑名单版本
#define SAE_INC_BLK_VER_W			"bc"		//增量黑名单版本
#define SAE_DEC_BLK_VER_W			"bd"		//减量黑名单版本
#define SAE_TEMP_BLK_VER_W			"be"		//临时黑名单版本
#define SAE_TIME_W					"bf"		//桩机时间
#define SAE_STATUS_W				"AA"		//桩机状态
#define SAE_N_ID_W					"bg"		//节点机编号
#define SAE_N_STATUS_W				"AB"		//节点机状态
#define SAE_N_TIME_W				"bh"		//节点机时间
#define SAE_N_MAC_W					"bi"          //节点机MAC地址
#define SAE_PHY_SN_W				"bj"		//锁桩物理编号
#define SAE_CTRL_C					"AA"		//桩机控制
#define SAE_PARA_VER_W				"bk"         //锁桩配置版本
#define SAE_QUANTITY_W				"bl"		//锁桩总数

/*
 * 锁桩报文结构
 */
 #define SAE_FRAME_MAX_SZ			(272)	//应用数据帧的长度为256
 #define SAE_DATA_SZ					320
struct s_struct_buf
{
	unsigned char hd_h;
	unsigned char hd_l;
	unsigned char s_addr;
	unsigned char d_addr;
	unsigned char cmd;
	unsigned char sn;
	unsigned short len;
	unsigned char data[SAE_DATA_SZ - 8];
}__attribute__((packed));

typedef struct sae_rx_buf
{
	union{
		struct s_struct_buf st;
		unsigned char data[SAE_DATA_SZ];
	}s_un;
}sae_rx_buf_t;

#define S_RX_LEN(p)				((p).s_un.st.len)
#define S_RX_DATA(p)				((p).s_un.st.data)
#define S_RX_DATA_B(p,b)			((p).s_un.st.data[(b)])
#define S_RX_DATA_BV(p,b,v)		((p).s_un.st.data[(b)] = (v))
#define S_RX_CMD(p)				((p).s_un.st.cmd)
#define S_RX_SADDR(p)			((p).s_un.st.s_addr)
#define S_RX_DADDR(p)			((p).s_un.st.d_addr)
#define S_RX_SN(p)				((p).s_un.st.sn)
#define S_RX_HD_H(p)				((p).s_un.st.hd_h)
#define S_RX_HD_L(p)				((p).s_un.st.hd_l)
#define S_RX_DA(p)				((p).s_un.data)
#define S_RX_DA_B(p,b)			((p).s_un.data[(b)])
#define S_RX_DA_BV(p,b,v)			((p).s_un.data[(b)] = (v))

//指针方式
#define S_RX_LEN_PTR(p)			((p)->s_un.st.len)
#define S_RX_DATA_PTR(p)			((p)->s_un.st.data)
#define S_RX_DATA_B_PTR(p,b)		((p)->s_un.st.data[(b)])
#define S_RX_CMD_PTR(p)			((p)->s_un.st.cmd)
#define S_RX_SADDR_PTR(p)		((p)->s_un.st.s_addr)
#define S_RX_DADDR_PTR(p)		((p)->s_un.st.d_addr)
#define S_RX_SN_PTR(p)			((p)->s_un.st.sn)
#define S_RX_HD_H_PTR(p)			((p)->s_un.st.hd_h)
#define S_RX_HD_L_PTR(p)			((p)->s_un.st.hd_l)
#define S_RX_DA_PTR(p)			((p)->s_un.data)
#define S_RX_DA_B_PTR(p,b)		((p)->s_un.data[(b)])



/*
 * 锁桩状态
 */
struct sae_status
{
	unsigned short in:1;		//锁舌收回检测, 表示无车
	unsigned short out:1;		//锁舌伸出检测, 表示有车
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
typedef struct sae_status sae_status_t;

/*
 * 锁桩信息
 */
struct sae_info
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
typedef struct sae_info sae_info_t;

/*
 * 桩机黑名单属性
 */
struct sae_blk_attr
{
	unsigned char temp_co:1;  //一致性
	unsigned char dec_co:1;
	unsigned char inc_co:1;
	unsigned char total_co:1;
	
	unsigned char temp_va:1;  //有效性
	unsigned char dec_va:1;
	unsigned char inc_va:1;
	unsigned char total_va:1;
}__attribute__((packed));
typedef struct sae_blk_attr sae_blk_attr_t;

/*
 * 桩机属性
 */
struct sae_attribute
{
	unsigned char id;					//桩机编号
	unsigned char physn[4];				//桩机物理编号
	unsigned short sn;					//桩机最新流水号
	unsigned char psam[4];				//PSAM卡编号
	unsigned char version[4];			//桩机版本号
	unsigned char time[6];				//桩机时间
	unsigned char register_time[6];		//桩机注册时间
	unsigned char lock_id;				//锁控编号
	unsigned char lock_version[4];		//锁控版本号
	unsigned char total_blk_ver[4];		//总量黑名单版本号
	unsigned char inc_blk_ver[4];		//增量黑名单版本号
	unsigned char dec_blk_ver[4];		//减量黑名单版本号
	unsigned char temp_blk_ver[4];		//临时黑名单版本号
	struct sae_status stat;				//桩机状态
}__attribute__((packed));
typedef  struct sae_attribute sae_attribute_t;


/*
 * 桩机控制 [AA]
 */
 struct sae_control  
{
	/* 功能设置位 : 0禁止, 1允许 */
	unsigned int f_op:1;		//允许/禁止桩机借还车
	unsigned int f_reboot:1;       //桩机重启
	unsigned int f_cpu:1;		//CPU钱包
	unsigned int f_cpu_to_m1:1; //CPU模拟M1钱包
	unsigned int f_m1:1;		//M1钱包
	unsigned int f_lock:2;		//车锁控制
	unsigned int f_led:2;		//状态灯控制
	unsigned int f_rfu:7;		//待定

	/* 功能屏蔽位 : 0禁止, 1允许 */
	unsigned int m_op:1;		
	unsigned int m_reboot:1;       
	unsigned int m_cpu:1;
	unsigned int m_cpu_to_m1:1;
	unsigned int m_m1:1;
	unsigned int m_lock:2;
	unsigned int m_led:2;
	unsigned int m_rfu:7;
}__attribute__((packed));
typedef struct sae_control sae_control_t;



















/*
 * ***************************************  中心部分 **************************************
 */
#define NDEV_HD					0xF1
#define NDEV_TAIL				0xF2
#define NDEV_ESC_F0H			0xF0	//转义字符
#define NDEV_ESC_00H			0x00
#define NDEV_ESC_01H			0x01
#define NDEV_ESC_02H			0x02

#define UFC_NULL					0
#define REG_NO					0
#define REG_OK					1

#define NDEV_REGISTER_REQ		0xA1	//设备签到
#define NDEV_REGISTER_ACK		0xB1
#define NDEV_CTRL_REQ			0xA2	//设备控制
#define NDEV_CTRL_ACK			0xB2
#define NDEV_FILE_TRANS_REQ	0xA3	//泛文件传输
#define NDEV_FILE_TRANS_ACK	0xB3
#define NDEV_LOAD_REQ			0xA4	//负载均衡申请
#define NDEV_LOAD_ACK			0xB4
#define NDEV_PASS_REQ			0xA5  //数据透传
#define NDEV_PASS_ACK			0xB5 


#define KERNEL_KEY					"kernel_"
#define FIRMWARE_KEY				"fw_"
#define APP_KEY						"app_"

#define SQLITE3_DB_PATHNAME			"/opt/universal/database.db"
#define UNITY_FILE_DB_PATHNAME		"/opt/universal/unity_file.db"


/*
 *  协议引导字
 */
#define NDEV_LOAD_W					"DP"		//负载均衡
#define NDEV_REG_W						"AN"		//设备注册
#define NDEV_STAT_W						"AB"		//节点机状态
#define NDEV_N_TIME_W					"bh"		//节点机时间
#define NDEV_S_TOTAL_NUM_W			"ai"		//桩机总数量
#define NDEV_S_ONLINE_NUM_W			"aj"		//桩机在线数量
#define NDEV_TOTAL_BLK_VER_W			"bb"		//总量黑名单版本
#define NDEV_INC_BLK_VER_W				"bc"		//增量黑名单版本
#define NDEV_DEC_BLK_VER_W				"bd"		//减量黑名单版本
#define NDEV_TEMP_BLK_VER_W			"be"		//临时黑名单版本
#define NDEV_S_BLK_ATTR_W				"AL"		//桩机黑名单属性
#define NDEV_S_ATTR_W					"AM"		//桩机属性
#define NDEV_YCT_STAT_W				"AK"		//羊城通读卡器状态
#define NDEV_BTHEART_RETURN_W			"al"		//心跳回应
#define NDEV_S_ALL_STAT					"AA"         //全部锁桩状态
#define NDEV_S_ONLINE_F					"AC"		//桩机在线标志
#define NDEV_FW_VER_W					"ao"        //节点机库版本
#define NDEV_APPL_VER_W				"ap"        //节点机固件版本
#define NDEV_FTP_DNLS_W				"BR"		//节点机FTP下载状态
#define NDEV_SAE_VER_W					"aq"         //桩机固件版本


#define NDEV_LOAD_SERV_IP_SET_W		"BA"		//设置负载服务器IP
#define NDEV_LOAD_SERV_PORT_SET_W		"BB"		//设置负载服务器端口
#define NDEV_LOAD_SERV2_IP_SET_W		"BC"		//设置备份负载服务器IP
#define NDEV_LOAD_SERV2_PORT_SET_W	"BD"		//设置备份负载服务器端口
#define NDEV_FTP_SET_W					"BE"		//FTP设置
#define NDEV_DEVICE_REBOOT_W			"BM"		//设备重启	
#define NDEV_S_LOCK_OP_W				"BO"		//立即锁车/开锁
#define NDEV_BTHEART_TIME_SET_W		"BF"		//设置心跳时间
#define NDEV_SITE_NAME_W				"BQ"        //下发站点名称
#define NDEV_UNIV_UPDATE_W				"BP"		//更新泛文件
#define NDEV_EXCEPT_HANDLE_W			"BU"		//异常处理
#define NDEV_FTP_DNL_W					"BR"		//节点机FTP固件下载
#define NDEV_UPDATE_CTRL_W				"BS"          //节点机固件更新控制
#define NDEV_RENT_INFO_W				"BV"		//租还车信息
#define NDEV_GET_S_ATTR_W				"BT"		//获取桩属性
#define NDEV_GET_ALL_SAE_PHY_INFO_W	"BW"       //全部锁桩物理信息
#define NDEV_GET_GPS_INFO_W			"by"		//获取gps信息
#define NDEV_SET_LIGHT_CTRL_TIME_W		"BX"		//灯箱开关时间
#define NDEV_SAE_PARA_VER_W			"br"     	 //锁桩参数版本号
#define NDEV_SITE_QR_CODE_W			"ca"		//网点二维码

#define NDEV_LNT_FIRMWARE_VER_W		"cd"	//岭南通读卡器固件版本
#define NDEV_NEARBY_SITE_INFO_W		"BZ"		//附近网点信息 add by zjt at 2016-11-03




/* 网络连接状态 */
typedef enum NDEV_NETSTAT
{
	NDEV_NETSTAT_INIT = 0,				//初始化 
	NDEV_NETSTAT_CONNECT,				//成功连接网络
	NDEV_NETSTAT_REGISTERED,			//成功注册服务器
	NDEV_NETSTAT_SESSION,				//正在回话
	NDEV_NETSTAT_UNKNOWN				// 未知状态
}NDEV_NETSTAT_EU;

/* 事件通知 */
typedef enum NDEV_NOTIFY
{
	NDEV_NOTIFY_INIT = 0,	
	NDEV_NOTIFY_RE_REGISTER,		//重新注册
	NDEV_NOTIFY_REGISTERED,		//已经注册
	NDEV_NOTIFY_RE_BTHEART,         //重新发心跳
	NDEV_NOTIFY_BTHEART,			//心跳接收成功
	NDEV_NOTIFY_TOTAL_BLK,
	NDEV_NOTIFY_INC_BLK,
	NDEV_NOTIFY_DEC_BLK,
	NDEV_NOTIFY_TEMP_BLK,
	NDEV_NOTIFY_UNIV_UPDATE,      //泛文件更新
	NDEV_NOTIFY_UNIV_CONTINUE,  //泛文件继续接收
	NDEV_NOTIFY_UNIV_QUIT,         //泛文件接收退出
	NDEV_NOTIFY_UNIV_REMOVE, //泛文件移除
	NDEV_NOTIFY_UNKNOWN		
}NDEV_NOTIFY_EU;

/* 负载均衡连接状态 */
typedef enum NDEV_BALSTAT
{
/*
	NDEV_BALSTAT_INIT = 0,
	NDEV_BALSTAT_SUCCESS,
	NDEV_BALSTAT_FAIL,
	NDEV_BALSTAT_ERR_CRC,
	NDEV_BALSTAT_END
*/

	NDEV_BALSTAT_OK = 0x00,
	NDEV_BALSTAT_ERR_CRC = 0x01,
	NDEV_BALSTAT_ERR_AUTH = 0x02,
	NDEV_BALSTAT_ERR_UNKNOW = 0xFF
}NDEV_BALSTAT_EU;

/*
 * 节点机报文结构
 */
#define NDEV_DATA_SZ					B_DEV_RXBUF_SZ  //512->B_DEV_RXBUF_SZ
#define NDEV_FRAME_MAX_SZ				(B_DEV_RXBUF_SZ - 128)		//应用数据帧的长度为320->B_DEV_RXBUF_SZ
#define NDEV_ATTR_BIT0_PASS(c)			((c) | (1 << 0))
#define NDEV_ATTR_BIT0_NOT_PASS(c)		((c) &= ~(1 << 0))
#define NDEV_ATTR_BIT1_RECV(c)			((c) | (1 << 1))
#define NDEV_ATTR_BIT1_NOT_RECV(c)		((c) &= ~(1 << 1))
struct n_struct_buf
{
	unsigned char hd;
	unsigned char sn;
	unsigned short term_id;
	unsigned char dev_addr;  //设备地址
	unsigned char data_attr; //数据属性
	unsigned char cmd; 	//类别引导字
	unsigned short len;
	unsigned char data[NDEV_DATA_SZ - 7];
}__attribute__((packed));

struct ndev_rx_buf
{
	union{
		struct n_struct_buf st;
		unsigned char data[NDEV_DATA_SZ];
	}s_un;
};
typedef struct ndev_rx_buf ndev_rx_buf_t;

#define N_RX_LEN(p)					((p).s_un.st.len)
#define N_RX_DATA(p)					((p).s_un.st.data)
#define N_RX_DATA_B(p,b)				((p).s_un.st.data[(b)])
#define N_RX_CMD(p)					((p).s_un.st.cmd)
#define N_RX_ID(p)					((p).s_un.st.term_id)
#define N_RX_SN(p)					((p).s_un.st.sn)
#define N_RX_HD(p)					((p).s_un.st.hd)
#define N_RX_DA(p)					((p).s_un.data)
#define N_RX_DA_B(p,b)				((p).s_un.data[(b)])
#define N_RX_DEV_ADDR(p)				((p).s_un.st.dev_addr)
#define N_RX_DATA_ATTR(p)			((p).s_un.st.data_attr)

/* 指针方式 */
#define N_RX_LEN_PTR(p)				((p)->s_un.st.len)
#define N_RX_DATA_PTR(p)				((p)->s_un.st.data)
#define N_RX_DATA_B_PTR(p,b)			((p)->s_un.st.data[(b)])
#define N_RX_CMD_PTR(p)				((p)->s_un.st.cmd)
#define N_RX_ID_PTR(p)				((p)->s_un.st.term_id)
#define N_RX_SN_PTR(p)				((p)->s_un.st.sn)
#define N_RX_HD_PTR(p)				((p)->s_un.st.hd)
#define N_RX_DA_PTR(p)				((p)->s_un.data)
#define N_RX_DA_B_PTR(p,b)			((p)->s_un.data[(b)])
#define N_RX_DEV_ADDR_PTR(p)		((p)->s_un.st.dev_addr)
#define N_RX_DATA_ATTR_PTR(p)		((p)->s_un.st.data_attr)


/*
 * 负载均衡
 */

struct load_balance_req
{
	unsigned char mac_addr[6];		//MAC地址
	unsigned short term_id;			//终端编码
	unsigned short operators;     	//运营商
	unsigned short dev_ver;  		//设备类型
}__attribute__((packed));
typedef struct load_balance_req load_balance_req_t;

struct load_balance_ack
{
	unsigned char status;
	unsigned char ip[8];
	unsigned short port;
	unsigned char encry_key[16];
}__attribute__((packed));
typedef struct load_balance_ack load_balance_ack_t;

/*
 * 设备签到
 */
struct ndev_reg_req
{
	unsigned char mac_addr[6];
	unsigned char encry_key[16];
}__attribute__((packed));
typedef struct ndev_reg_req ndev_reg_req_t;

struct ndev_reg_ack
{
	unsigned char status;
	unsigned char serv_time[7];  
}__attribute__((packed));
typedef struct ndev_reg_ack ndev_reg_ack_t;

/*
 * FTP服务器设置
 */
 struct ndev_ftp_server
{
	unsigned int ipaddr;
	unsigned short port;
	unsigned char username[16];
	unsigned char passwd[16];
}__attribute__((packed));
typedef  struct ndev_ftp_server ndev_ftp_server_t;


/*
 * 异常处理
 */
struct ndev_except_handle_req
{
	unsigned short sn;
	unsigned char pid[8];
	unsigned char ticket[42];
	unsigned char rentinfo[36];
}__attribute__((packed));
typedef struct ndev_except_handle_req ndev_except_handle_req_t;

struct ndev_except_handle_ack
{
	unsigned short sn;
	unsigned char pid[8];
	unsigned char status;
	unsigned char info[48];
}__attribute__((packed));
typedef struct ndev_except_handle_ack ndev_except_handle_ack_t;


/*
 * 心跳发送限制 //是否要发送的标志
 */
struct ndev_bheart_limit
{
	unsigned int ndev_stat:1;    //节点机状态
	unsigned int ndev_time:1;       //节点机时间
	unsigned int sae_quantity:1;  //桩机总数量
	unsigned int sae_online:1;	//桩机在线数量
	unsigned int total_bl_ver:1;	//总量黑名单版本
	unsigned int inc_bl_ver:1;	//增量黑名单版本
	unsigned int dec_bl_ver:1;	//减量黑名单版本
	unsigned int temp_bl_ver:1;	//临时黑名单版本
	unsigned int sae_bl_attr:1;	//桩机黑名单属性
	unsigned int sae_attr:1;		//桩机属性
	unsigned int lnt_card_stat:1;	//羊城通读卡器状态
	unsigned int all_sae_stat:1;		//全部桩机状态
	unsigned int sae_online_mark:1;	//桩机在线标志
	unsigned int ndev_fw_ver:1;		//节点机库版本
	unsigned int ndev_app_ver:1;	//节点机应用版本
	unsigned int sae_fw_ver:1;		//桩机固件版本
	unsigned int ndev_ftp_stat:1;	//节点机FTP下载状态
	unsigned int sae_para_ver:1; 	//锁桩参数版本
	
	unsigned int lnt_reader_firmware_ver:1; //岭南通读卡器固件版本 add by zjc CONFS_USING_UPLOAD_READER_VER
	unsigned int rfu:13;		//保留 14-1
}__attribute__((packed));
typedef struct ndev_bheart_limit ndev_bheart_limit_t;





/*
 * 节点机状态
 */
struct ndev_status
{
	unsigned short power:1;				//电源电压状态
	unsigned short rtc:1;				//RTC状态
	unsigned short can:1;				//CAN总线状态
	unsigned short center:1;				//与中心通讯状态
	unsigned short wireless:1;			// 3G/4G网络状态
	unsigned short yct:1;				//羊城通读卡器
	unsigned short gps:1;				//GPS
	unsigned short keyboard:1;			//键盘板状态
	unsigned short ext_mcu:1;			//辅助CPU板状态
	unsigned short time:1;				//时间同步状态
	unsigned short rfu:6;				//保留
}__attribute__((packed));
typedef struct ndev_status ndev_status_t;


struct ndev_info
{
	NDEV_NETSTAT_EU netstat;
	struct ndev_status stat;
	struct load_balance_ack load;
	char center_ip[32];
	unsigned short center_port;
	char terminal_name[50];    //终端名称
	unsigned short terminal_no; //终端编号
	unsigned long register_time;  //注册时间
	unsigned char network_type; //网络类型
};
typedef struct ndev_info ndev_info_t;


/*
 * 管理卡使用记录
 */
struct admin_card_info_req
{
	unsigned int sn;			//流水号
	unsigned char pid[8];              //物理卡号
	unsigned char ticket[42];		//票卡信息
	unsigned char rentinfo[36];	//租还车记录
	unsigned char time[6]; 		 //YYMMDDHHmmSS
}__attribute__((packed));
typedef struct admin_card_info_req admin_card_info_req_t;

struct admin_card_info_ack
{
	unsigned int sn;
	unsigned char pid[8];
	unsigned char result;
}__attribute__((packed));
typedef struct admin_card_info_ack admin_card_info_ack_t;


/*
 * 异常处理扣费交易记录
 */
 struct exception_handle_record_req
{
	unsigned int sn;			//流水号
	unsigned char pid[8];              //物理卡号
	unsigned char record[90];      //交易记录
}__attribute__((packed));
typedef struct exception_handle_record_req exception_handle_record_req_t;

struct exception_handle_record_ack
{
	unsigned int sn;
	unsigned char pid[8];
	unsigned char result;
}__attribute__((packed));
typedef struct exception_handle_record_ack exception_handle_record_ack_t;

/*
 * 异常处理
 */
struct exception_handle_req
{
	unsigned int sn;
	unsigned char pid[8];              //物理卡号
	unsigned char ticket[42];		//票卡信息
	unsigned char rentinfo[36];	//租还车记录	
}__attribute__((packed));
typedef struct exception_handle_req exception_handle_req_t;

struct exception_handle_ack
{
	unsigned int sn;
	unsigned char pid[8];
	unsigned char result;	
	unsigned int fee; //扣费
	unsigned char info[48];    //显示信息
}__attribute__((packed));
typedef struct exception_handle_ack exception_handle_ack_t;


/*
 * FTP固件下载
 */ 
struct ndev_ftp_download_ctrl_req
{
	unsigned short upsn;
	unsigned char supplier;
	unsigned char fw_name[48];
}__attribute__((packed));
typedef struct ndev_ftp_download_ctrl_req ndev_ftp_download_ctrl_req_t;

struct ndev_ftp_download_ctrl_ack
{
	unsigned short upsn;
	unsigned char supplier;
	unsigned char time[7];
	unsigned char download_status;
	unsigned char ftpcode;
	unsigned char err;
}__attribute__((packed));
typedef struct ndev_ftp_download_ctrl_ack ndev_ftp_download_ctrl_ack_t;


struct ndev_ftp_download_ctrl_info
{
	int isvaild;  //是否有效
	int ftype;   //固件类型
	unsigned int ftime;
	
	struct ndev_ftp_download_ctrl_ack fdnl_ctrl;
}__attribute__((packed));
typedef struct ndev_ftp_download_ctrl_info ndev_ftp_download_ctrl_info_t;


/*
 * 固件更新控制
 */
 struct ndev_fw_update_ctrl_req
{
	unsigned short upsn;
	unsigned char supplier;
	unsigned char upop;
}__attribute__((packed));
typedef  struct ndev_fw_update_ctrl_req ndev_fw_update_ctrl_req_t;

 struct ndev_fw_update_ctrl_ack
{
	unsigned short upsn;
	unsigned char supplier;
	unsigned char upresult;
}__attribute__((packed));
typedef  struct ndev_fw_update_ctrl_ack ndev_fw_update_ctrl_ack_t;


/*
 * 会员申请
 */
 struct ndev_member_apply_req
{
	unsigned int sn;				//流水号
	unsigned char apply_type;		//申请类型
	unsigned char id_number[18];	//身份证
	unsigned char phone_number[11];	//手机号码
	unsigned char apply_member_type; //申请会员类型
	unsigned char pid[8];				//票卡物理卡号
	unsigned char ticket_info[42];		//票卡信息
	unsigned char rent_info[36];			//租还车信息
	unsigned char time[6];				//时间
}__attribute__((packed));
typedef  struct ndev_member_apply_req ndev_member_apply_req_t;

struct ndev_member_apply_ack
{
	unsigned int sn;				//流水号
	unsigned char apply_type;		//申请类型
	unsigned char pid[8];			//票卡物理卡号
	unsigned int deposit;			//应收押金
	unsigned char result;			//申请结果
	unsigned char time[6];			//时间
	unsigned char showinfo[48];  		//显示信息
}__attribute__((packed));
typedef  struct ndev_member_apply_ack ndev_member_apply_ack_t;

/*
 * 会员处理回报
 */
struct ndev_member_handle_req
{
	unsigned int sn;				//流水号
	unsigned char apply_type;		//申请类型
	unsigned char handle_type;		//回报类型
	unsigned char pid[8];			//票卡物理卡号
	unsigned char result;			//申请结果
	unsigned char time[6];			//时间	
	unsigned char errcode[12];		//错误代码
}__attribute__((packed));
typedef struct ndev_member_handle_req ndev_member_handle_req_t;

struct ndev_member_handle_ack
{
	unsigned int sn;				//流水号
	unsigned char apply_type;		//申请类型
	unsigned char handle_type;		//回报类型
	unsigned char pid[8];			//票卡物理卡号
	unsigned char result;			//申请结果
	unsigned char time[6];			//时间	
}__attribute__((packed));
typedef struct ndev_member_handle_ack ndev_member_handle_ack_t;

/*
 * 开卡消费交易记录
 */
struct ndev_apply_consume_req
{
	unsigned int sn;			//流水号
	unsigned char pid[8];		//票卡物理卡号
	unsigned char record[90];	//交易记录
}__attribute__((packed));
typedef struct ndev_apply_consume_req ndev_apply_consume_req_t;

struct ndev_apply_consume_ack
{
	unsigned int sn;		//流水号
	unsigned char pid[8];	//票卡物理卡号
	unsigned char result;	//结果
}__attribute__((packed));
typedef struct ndev_apply_consume_ack ndev_apply_consume_ack_t;


/*
 * 异步处理数据
 */
#define NDEV_ASYNC_MUTEX_QDATA_UNIV			1
#define NDEV_ASYNC_MUTEX_QDATA_FTP_DNL		2
#define NDEV_ASYNC_MUTEX_QDATA_FW_CTRL		3

 struct async_mutex_queue_data
{
	unsigned char qid;
	unsigned char sn;
	void *qptr;
}__attribute__((packed));
typedef struct async_mutex_queue_data async_mutex_queue_data_t;

/*
 * 租还车记录查询
 */
struct ndev_rent_info_qry_req
{
	unsigned char pid[8];		//票卡物理卡号
	unsigned char item;       //记录条数
}__attribute__((packed));
typedef struct ndev_rent_info_qry_req ndev_rent_info_qry_req_t;

struct ndev_rent_info_qry_ack
{
	unsigned char pid[8];		//票卡物理卡号
	unsigned char item;       		//记录条数
	unsigned short length;   //记录长度
	unsigned char record[2048];  //记录
}__attribute__((packed));
typedef struct ndev_rent_info_qry_ack ndev_rent_info_qry_ack_t;

struct rent_info_fmt		//租用记录格式
{ 
	unsigned char bike_NO[32];		//自行车编号
	unsigned char rent_name[64];	//租车点名称
	unsigned char rent_time[32];		//租车时间
	unsigned char return_name[64];	//还车点名称
	unsigned char return_time[32];	//还车时间
	unsigned char used_time[16];		//骑行时间
	unsigned char fee[16];			//扣费
}__attribute__((packed));
typedef struct rent_info_fmt rent_info_fmt_t;


/*
 * gps定位信息
 */
struct gps_all_info
{
	unsigned char vaild;                       //数据有效位
	unsigned char latitude[10];  		//纬度，格式为 ddmm.mmmmm（度分格式）
	unsigned char ns_indicator;  		//纬度半球，N 或 S（北纬或南纬）
	unsigned char longitude[10]; 		//经度，格式为 dddmm.mmmmm（度分格式）
	unsigned char ew_indicator; 		//经度半球，E 或 W（东经或西经）
	unsigned char status; 			// GPS 状态，0=未定位，1=非差分定位，2=差分定位  
	unsigned char satellite[2]; 			//正在使用的用于定位的卫星数量（00~12） 
	unsigned char times[7];                //GPS时间,  BCD, YYYYMMDDHHmmSS
}__attribute__((packed));
typedef struct gps_all_info gps_all_info_t;

struct gps_rxbuf
{
	int vaild;
	int size;
	unsigned char string[512];
}__attribute__((packed));


/*
 * 岭南通信息
 */
struct lnt_all_info
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
typedef struct lnt_all_info lnt_all_info_t;

/*
 * 灯箱控制时间
 */ 
 struct light_ctrl_time
{
	unsigned char enable[2];
	unsigned char disable[2];
}__attribute__((packed));
typedef  struct light_ctrl_time light_ctrl_time_t;





/*
 * 锁桩控制流水号 【保存】
 */
static unsigned char g_sae_ctrl_sn = 1;


/*
 * 附近网点信息查询 add by zjc at 2016-11-03
 */
struct ndev_nearby_site_info_qry_req
{
	unsigned char siteNo[2];		//网点终端编号
	unsigned char gpsInfo[32];       //网点经纬度信息
}__attribute__((packed));
typedef struct ndev_nearby_site_info_qry_req ndev_nearby_site_info_qry_req_t;

struct ndev_nearby_site_info_qry_ack
{
	unsigned char siteNo[2];		//网点终端编号
	unsigned char item;       		//网点信息条数
	unsigned short length;   //网点信息长度
	unsigned char info[2048];  //网点信息内容
}__attribute__((packed));
typedef struct ndev_nearby_site_info_qry_ack ndev_nearby_site_info_qry_ack_t;

struct nearby_site_info_fmt		//附近网点信息格式
{ 
	unsigned char siteName[64];		//网点名称
	unsigned char distance[16];	//距离
	unsigned char bikes[16];	//自行车数
	unsigned char stacks[16];	//总桩数
}__attribute__((packed));
typedef struct nearby_site_info_fmt nearby_site_info_fmt_t;
/* end of 附近网点信息查询 */

#endif


