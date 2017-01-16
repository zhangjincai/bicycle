#ifndef __LNT_H__
#define __LNT_H__

#include <arpa/inet.h>

#define LNT_NULL					(NULL) 
#define LNT_EOK					(0)  //正常
#define LNT_ERROR				(-1) //错误
#define LNT_ETIMEOUT				(-2) //超时
#define LNT_EFULL				(-3) //满
#define LNT_EEMPTY				(-4) //空
#define LNT_ENOMEM 				(-5) //内存不够
#define LNT_EXCMEM				(-6) //内存越界
#define LNT_EBUSY				(-7) //忙
#define LNT_NOT_SUPPORT			(-8) //不支持该指令

#define LNT_ADMIN_SET_ENALBE	(2)
#define LNT_TRUE				(1)
#define LNT_FALSE				(0)

#define LNT_CHAR_MAX			0xff
#define LNT_SHORT_MAX			0xffff
#define LNT_INT_MAX				0xffffffff

#define LNT_NTOHS(n)				ntohs(n)	
#define LNT_HTONS(n)				htons(n)
#define LNT_NTOHL(n)				ntohl(n)
#define LNT_HTONL(n)				htonl(n)


#if 0
/* 日志定义 */
//#define LNT_LOG_RUN

#ifdef LNT_LOG_RUN
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
#endif

/*
* 岭南通模块错误码
*/
#define LNT_MOC_ERROR_NORMAL					0x00  //操作正常
#define LNT_MOC_ERROR_SAM_EMPTY				0x60 //没有安装SAM卡
#define LNT_MOC_ERROR_INIT						0x61 //SAM卡初始化错误或未初始化
#define LNT_MOC_ERROR_PIN						0x62 //SAM卡检验PIN错误
#define LNT_MOC_ERROR_READ						0x65 //SAM卡读错误
#define LNT_MOC_ERROR_WRITE					0x66 //SAM卡写错误
#define LNT_MOC_ERROR_CARD_NOTAG				0x80  //没有卡
#define LNT_MOC_ERROR_CARD_AUTH				0x83  //认证卡片错误
#define LNT_MOC_ERROR_CARD_OP_FAIL			0x89  //卡片操作失败
#define LNT_MOC_ERROR_CARD_UNKOWN			0x90  //非岭南通卡
#define LNT_MOC_ERROR_CARD_OUTOFDATE			0x91  //卡片超出有效期
#define LNT_MOC_ERROR_CARD_NAMELIST			0x94   //黑名单
#define LNT_MOC_ERROR_CARD_OWEPTY			0x95   //钱包余额不
#define LNT_MOC_ERROR_CARD_OVERLIMIT			0x96   //钱包余额超出上限
#define LNT_MOC_ERROR_CARD_PURSE_UNUSED		0x97   //钱包未启用
#define LNT_MOC_ERROR_TS_PENDING				0xB0   //交易操作中途中断
#define LNT_MOC_ERROR_LASTTIME_TRADE_FAIL		0xD4   //上次交易未成功
#define LNT_MOC_ERROR_INVALID_PARAM			0xE3	   //参数错误
#define LNT_MOC_ERROR_M1						0xD3   //M1卡

#define LNT_MOC_ERROR_LILIN_NOT_CPU_CARD		0xFE	   //非CPU卡
#define LNT_MOC_ERROR_LILIN_NOT_MEMBER_CARD	0xFF   //非会员卡

#define LNT_MOC_ERROR_UNKNOW	0xEE   	//未知错误


/*
 * 设备响应状态码
 */
#define LNT_SW_ERROR_NORMAL					0x9000 //成功执行,无错误
#define LNT_SW_ERROR_LENGTH					0x6700 //长度不对
#define LNT_SW_ERROR_INS						0x6D00 //INS错误
#define LNT_SW_ERROR_CLA						0x6E00 //CLA错误
#define LNT_SW_ERROR_DATA						0x6F00 //数据无效
#define LNT_SW_ERROR_AUTH						0x6300 //认证失败

/*
 * 信息码对照表
 */
 #define LNT_CINFO_ERROR_ORDER					0xA0    //当前操作次序不一致
 #define LNT_CINFO_ERROR_PKEY					0xA1    //网充终端找不到公钥
 #define LNT_CINFO_ERROR_CRC					0xA2    //报文校验码不正确
 #define LNT_CINFO_ERROR_TICKET_TYPE			0xA3	   //查询卡的钱包类型不确定
 #define LNT_CINFO_ERROR_ECORRECT				0xA4	   //冲正失败
 #define LNT_CINFO_ERROR_CORRECT				0xA5	   //冲正成功
 #define LNT_CINFO_ERROR_APP_TYPE				0xA6   //应用类型不确定
 #define LNT_CINFO_ERROR_ACC_BAL				0xA7   //充值账号余额为零
 #define LNT_CINFO_ERROR_CARD					0xAA   //读卡器报错
 #define LNT_CINFO_ERROR_NOT_REGISTER			0xBB   //网充终端未签到
 #define LNT_CINFO_ERROR_PKT_LEN				0xCC	  //读卡器返回报文长度异常
 #define LNT_CINFO_ERROR_SYS_BUSY				0xDD   //系统繁忙
 #define LNT_CINFO_ERROR_ESYS					0xEE   //代系统返回结果报错
 #define LNT_CINFO_ERROR_ORDER_NOT_EXIST		0x96   //对应订单不存在
 #define LNT_CINFO_ERROR_ORDER_BAL				0x97   //对应订单的金额不一致
 #define LNT_CINFO_ERROR_ORDER_NO				0x98   //对应订单的卡号不一致


/*
 * 岭南通卡充值上限
 */
#define LNT_CPU_RECHARGE_LIMIT					(100000)  //1000元



/*
 * 岭南通签到状态
 */
enum LNT_REGISTER_STAT
{
	LNT_REGISTER_STAT_INIT = 0,  //初始化
	LNT_REGISTER_STAT_ING,   //正在签到
	LNT_REGISTER_STAT_OK,  //已经签到
	LNT_REGISTER_STAT_RE, //重新签到
	LNT_REGISTER_STAT_FAIL //签到失败
};

struct lnt_register_proc_code
{
	unsigned char proc:4;     //执行步骤
	unsigned char fresult:4;  //执行结果
	unsigned char stat;      	//返回状态
	unsigned short sw;  	//返回码
	time_t time;  		//执行时间
	unsigned char agent_err; //代理错误码
}__attribute__((packed));
typedef struct lnt_register_proc_code lib_lnt_register_proc_code_t;

struct lnt_member_proc_code
{
	unsigned char proc;     //执行步骤
	unsigned char fresult;  //执行结果
	unsigned char stat;      	//返回状态
	unsigned short sw;  	//返回码
}__attribute__((packed));
typedef struct lnt_member_proc_code lnt_member_proc_code_t;

/*
 * 签到流程
 */
enum LNT_REGISTER_PROC
{
	LNT_REGISTER_PROC_QUERTICKET = 1, //查询票卡信息
	LNT_REGISTER_PROC_CONN,   //网络连接
	LNT_REGISTER_PROC_DD01,
	LNT_REGISTER_PROC_DD02,
	LNT_REGISTER_PROC_R_INIT,
	LNT_REGISTER_PROC_DD03,
	LNT_REGISTER_PROC_DD04,
	LNT_REGISTER_PROC_R_AC_LOGIN_1,
	LNT_REGISTER_PROC_DD05,
	LNT_REGISTER_PROC_DD06,
	LNT_REGISTER_PROC_R_AC_LOGIN_2,
	LNT_REGISTER_PROC_DD07,
	LNT_REGISTER_PROC_DD08,
	LNT_REGISTER_PROC_END //结束
};

/*
 * 开销卡流程
 */
enum LNT_MEMBER_PROC
{
	LNT_MEMBER_PROC_DEPOSIT_INIT = 1,
	LNT_MEMBER_PROC_CONN,   //网络连接
	LNT_MEMBER_PROC_DD73,
	LNT_MEMBER_PROC_DD74,
	LNT_MEMBER_PROC_QUERTICKET,
	LNT_MEMBER_PROC_DEPOSIT_PROCESS
};


/*
 * 开卡流程
 */
enum LNT_ON_CARD_PROC
{
	LNT_ON_CARD_PROC_QUERTICKET = 1,  //查询票卡信息
	
	
};




/*
 * 流程执行结果
 */
enum LNT_FRESULT
{
	LNT_FRESULT_E_OK =  0,  //全部成功
	LNT_FRESULT_E_CRC,  //校验失败
	LNT_FRESULT_E_NET,  //网络出错
	LNT_FRESULT_E_STEP, //步骤出错
	LNT_FRESULT_E_UART, //串口出错
	LNT_FRESULT_E_STAT, //状态出错
	LNT_FRESULT_E_PID, //物理卡号不符
	LNT_FRESULT_E_MEMBER, //已经是会员卡
	LNT_FRESULT_E_RETCODE  //返回码出错
};

/*
 * 开卡,销卡标志
 */
enum LNT_MEMBER_OP
{
	LNT_MEMBER_OP_REGISTER = 0x01,   //开卡
	LNT_MEMBER_OP_CANCEL = 0x02, //销卡
};






/******************************************************************
*						岭南通读卡器指令
********************************************************************/
/*
* 岭南通读卡器指令--广州公共自行车
*/
#define LNT_GETVERSION				0x60 	//获取程序版本
#define LNT_CORE_INIT				0xD2		//核心模块初始化
#define LNT_QRY_TICKET_INFO			0xD3        //查询卡信息
#define LNT_QRY_RENT_INFO			0xD9         //查询租还信息
#define LNT_SET_RENT_INFO			0xDA		//设置租还信息
#define LNT_CONSUME					0xD4         //消费
#define LNT_SET_BL_FLAG				0xD5         //黑名单设置
#define LNT_GET_HIS_RECORD			0xD6         //最近历史交易记录
#define LNT_SET_PARAM				0xD7         //参数设置
#define LNT_GET_TRANS				0xD8         //获取交易记录

#define LNT_DEPOSIT_INIT				0xE3         //押金初始化
#define LNT_DEPOSIT_PROCESS			0xE4        //押金处理

#define LNT_READER_APDU 			0x47       //APDU指令


/*
 * 岭南通核心模块充值协议
 */
#define LNT_R_CMD					0x40
#define LNT_R_CLA					0xBB
#define LNT_R_INIT_INS				0x10
#define LNT_R_AC_LOGIN_1_INS		0xC8




/*
 * 押金标志
 */
struct deposit_sign
{
	unsigned char rfu1:2;
	unsigned char type:2;
	unsigned char foshan:1;
	unsigned char guangzhou:1;
	unsigned char rfu2:2;
}__attribute__((packed));
typedef struct deposit_sign deposit_sign_t;

struct pmflag
{
	unsigned char rfu:7;
	unsigned char pflag:1;
}__attribute__((packed));
typedef struct pmflag pmflag_t;


/*
 * 票卡信息数据说明
 */
struct ticket_info
{
	unsigned int blance;         		//余额
	unsigned char pid[8];			//物理卡号(纯M1卡前补4个0)
	unsigned char lid[8];               		//逻辑卡号
	unsigned short ttype;			//卡片类型
	unsigned char ctim[4];			//充值时间(YYYYMMDD)
	unsigned char pursetype;		//钱包类型(0x01:M1钱包, 0x02:CPU钱包)
	unsigned char areaid;			//区域代码
	unsigned char areacardtype[2];	//区域卡代码
	unsigned char areacarddate[4];	//区域卡类型有效期
	unsigned char dflag;			//广州佛山押金标志
	unsigned char dflagother[4];		//广州以外的押金标志
	unsigned short pcode;			//运营商编码
	unsigned char pmflag;			//运营商会员标志
}__attribute__((packed));
typedef struct ticket_info ticket_info_t;

/*
 * 交易记录
 */
struct trade_record
{
	unsigned char rectype; 			//记录类型
	unsigned char areaid;			//卡片区域代码
	unsigned char ctype[2];			//卡类型
	unsigned char areacardtype[2];	//区域卡类型
	unsigned int opid:24;			//操作员编码,3字节
	unsigned char curtransdevid[6];	//本次交易设备编号
	unsigned int offlinetranssn; 		////脱机交易流水
	unsigned char curtransdtime[7];	//本次交易日期时间
	unsigned char lid[8];               		//逻辑卡号
	unsigned char pid[8];			//物理卡号(纯M1卡前补4个0)
	unsigned int amount;			//交易金额
	unsigned int priceofticket;		//票价
	unsigned int blance;         		//交易后卡内余额
	unsigned char ttype;	      		//交易类型
	unsigned char addittype;		//附加交易类型
	unsigned short rechargettimes;	//充值交易次数
	unsigned short consumettimes;      //消费交易次数
	unsigned char lasttdevid[6];		//上次交易设备编号
	unsigned char lasttdtime[7];		//上次交易日期时间
	unsigned int authcode;			//交易认证码
	unsigned char rfu[13];			//备用
}__attribute__((packed));
typedef struct trade_record trade_record_t;

/*
 * 黑名单记录
 */
struct blacklist_record
{
	unsigned char pid[8];			//物理卡号
	unsigned char lid[8];			//逻辑卡号
}__attribute__((packed));
typedef struct blacklist_record blacklist_record_t;

/*
 * M1历史记录
 */
struct m1_history_record
{
	unsigned short spid;          //服务商代码
	unsigned char ttype;	      //交易类型
	unsigned int amount:24;     //交易金额(高位在前)
	unsigned char samid[4]; 	      //SAM卡号
	unsigned char dtime[6];        //交易时间(YYMMDDHHMMSS)
}__attribute__((packed));
typedef struct m1_history_record m1_history_record_t; 

/*
 * CPU历史记录
 */
struct cpu_history_record
{
	unsigned short spid;          //交易序号(高位在前
	unsigned int ovd:24; 	    //透支限额(高位在前
	unsigned int amount;    		//交易金额(高位在前)
	unsigned char ttype;	      //交易类型
	unsigned char tid[6];	      //终端机编号
	unsigned char date[4]; 	      //交易日期(XXMMDD)
	unsigned char time[3];        //交易时间(HHMMSS)
}__attribute__((packed));
typedef struct cpu_history_record cpu_history_record_t; 

#if 0
/*
 * 租还车信息
 */
struct rent_info
{
	unsigned char app_locker;        	//应用锁定标志
	unsigned short provider;		//服务营运代码
	unsigned char version_no:4;		//版本序号--低4位
	unsigned char rent_flag:4;		//租还车标志--高4位
	unsigned char member_level:4;	//会员级别--低4位
	unsigned char member_type:4;	//会员类型--高4位
	unsigned char ticket_type;		//票种标记
	unsigned short accum;			//次数/积分/天数
	unsigned short cur_accum;		//当次消费次数/积分/天数
	unsigned short cur_deal_sn;		//当次交易流水
	unsigned char start_time[3];		//启用时间   			YYMMDD
	unsigned char  cur_deal_date[5];	//当次交易日期时间   YYMMDDHHMM
	unsigned char site_no[6]; 		//站点编号
	unsigned char lock_no[4];		//锁桩编号
	unsigned char bike_no[4];		//自行车编号
	unsigned short accu_time_of_day; //当日累计时长
}__attribute__((packed));
typedef struct rent_info rent_info_t;
#endif



/*
 * 租还车信息
 */
enum TICKET_FLAG
{
	TICKET_FLAG_ORDINARG = 0x00,  //普通卡
	TICKET_FLAG_TIME = 0x01,	//次卡
	TICKET_FLAG_DAY = 0x02, //天卡
	TICKET_FLAG_MON = 0x03, //月卡
	TICKET_FLAG_YEAR = 0x04, //年卡
	TICKET_FLAG_SPECIAL = 0x05, //专车
	
	TICKET_FLAG_OLD = 0xfd, //原客户
	TICKET_FLAG_ADMIN = 0xfe,  //管理卡
};

struct rent_flag
{
	unsigned char b0:1;
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
};


struct accum
{
	unsigned short accum;			//次数/天数/月数/年数/原客户优惠次数[A8]	
	unsigned short cur_accum;		//当次消费的积分/次数 [A9]
}__attribute__((packed));

struct rent_info
{
	unsigned char app_locker;        	//应用锁定标志  [A1]
	unsigned short provider;		//服务营运代码 [A2]

	unsigned char version_no:4;		//版本序号--低4位 [A4]
	unsigned char rent_flag:4;		//租还车标志--高4位 [A3]
	unsigned char area_flag:4;		//区域标记--低4位 [A6]
	unsigned char free_time:4;		//免费时长--高4位 [A5]
	unsigned char ticket_flag;		//票种标记 [A7]
	union{
		struct accum acc;
		unsigned char special_phy[4];  // A[8] A[9]
	}s_un;
	unsigned short cur_deal_sn;		//当次交易流水 [A10]
	unsigned char start_time[3];		//启用时间  YYMMDD [A11]
	unsigned char  cur_deal_date[5];	//当次交易日期时间   YYMMDDHHMM [A12]
	unsigned char site_no[6]; 		//站点编号 [A13]
	unsigned char lock_no[4];		//锁桩编号 [A14]
	unsigned char bike_no[4];		//自行车编号 [A15]
	unsigned short points;	 		//积分  [A16]
}__attribute__((packed));
typedef struct rent_info rent_info_t;





/*
 * APDU(发往读卡器的APDU指令)
 */
struct lnt_r_apdu_req
{
	unsigned char len;//指令长度
	unsigned char cmd[32];//APDU指令
}__attribute__((packed));
typedef struct lnt_r_apdu_req lnt_r_apdu_req_t;

/*
 * 读卡器对APDU指令返回的应答
 */
struct lnt_r_apdu_ack
{
	unsigned char snum;	//指令序号
	unsigned char len;	//数据长度
	unsigned char data[128];//数据
}__attribute__((packed));
typedef struct lnt_r_apdu_ack lnt_r_apdu_ack_t;


/*
 * 读卡器响应报文
 */
struct lnt_reader_ack
{
	unsigned char head;	
	unsigned char len;	
	unsigned char cmd;
	unsigned char stat;
	unsigned char data[128];//数据
	unsigned char checksum;
}__attribute__((packed));
typedef struct lnt_reader_ack lnt_reader_ack_t;







/*
* 获取程序版本
*/
struct lnt_getversion_ack
{
	unsigned char version[24]; 		//ASCII
}__attribute__((packed));
typedef struct lnt_getversion_ack lnt_getversion_ack_t;

/*
 * 核心模块初始化
 */
struct lnt_core_init_req
{
	unsigned char spno[2];
}__attribute__((packed));
typedef struct lnt_core_init_req lnt_core_init_req_t;

struct lnt_core_init_ack
{
	unsigned char psam[4];
}__attribute__((packed));
typedef struct lnt_core_init_ack lnt_core_init_ack_t;

/*
 * 查询票卡信息
 */
 struct lnt_qry_ticket_info_ack
{
	unsigned char length;
	struct ticket_info ticket;
}__attribute__((packed));
typedef  struct lnt_qry_ticket_info_ack lnt_qry_ticket_info_ack_t;

/*
 * 查询租还车信息
 */
struct lnt_rent_info_req
{
	unsigned char id;
}__attribute__((packed));
typedef struct lnt_rent_info_req lnt_rent_info_req_t;

struct lnt_rent_info_ack
{
	unsigned char length;
	struct rent_info rent;
}__attribute__((packed));
typedef struct lnt_rent_info_ack lnt_rent_info_ack_t;

struct lnt_rent_info
{
	unsigned char id;
	unsigned char length;
	struct rent_info rent;
}__attribute__((packed));
typedef struct lnt_rent_info lnt_rent_info_t;


/*
 * 消费
 */
#define LNT_ATY_RENT			0x11		//租车
#define LNT_ATY_RETURN		0x17		//还车
struct lnt_consume_req
{
	unsigned int amount;		//交易金额(低位在前)
	unsigned int fee;			//应收金额(低位在前)
	unsigned char time[6];		//YYMMDDHHMMSS
	unsigned char phyiscalid[8];	//物理卡号
	unsigned char aty;			//附加交易类型
}__attribute__((packed));
typedef struct lnt_consume_req lnt_consume_req_t;

/*
 * 参数设置
 */
struct lnt_parameter_set_req
{
	unsigned char conpa;
	unsigned char cvad;
}__attribute__((packed));
typedef struct lnt_parameter_set_req lnt_parameter_set_req_t;

/*
 * 黑名单设置
 */
struct lnt_blacklist_record_ack
{
	struct blacklist_record blacklist;
}__attribute__((packed));
typedef struct lnt_blacklist_record_ack lnt_blacklist_record_ack_t;

/*
 * 最近历史交易记录
 */
struct lnt_history_record_ack
{
	unsigned char ticket_type;
	union{
		struct m1_history_record m1_his;
		struct cpu_history_record cpu_his;
	}s_un;
}__attribute__((packed));
typedef struct lnt_history_record_ack lnt_history_record_ack_t;


/*
 * 获取交易数据
 */
struct lnt_trade_record_ack
{
	struct trade_record record;
}__attribute__((packed));
typedef struct lnt_trade_record_ack lnt_trade_record_ack_t;








/*
 * *******************  岭南通核心模块充值协议 *************************
 */
/*
 * R_INIT(读卡初始化)
 */
struct lnt_r_init_ack
{
	unsigned char pki[4];   	 //PKI管理卡号
	unsigned char fw_ver[8];  //读卡器固件版本
	unsigned short sw;    	 //状态码
}__attribute__((packed));
typedef struct lnt_r_init_ack lnt_r_init_ack_t;

/*
 * R_AC_LOGIN_1(签到步骤1)
 */
struct lnt_r_ac_login_1_ack
{
	unsigned char login1_data[8+128];
	unsigned short sw;
}__attribute__((packed));
typedef struct lnt_r_ac_login_1_ack lnt_r_ac_login_1_ack_t;

/*
 * R_AC_LOGIN_2(签到步骤2)
 */
struct lnt_r_ac_login_2_req
{
	unsigned char login2_data[8+128];
}__attribute__((packed));
typedef struct lnt_r_ac_login_2_req lnt_r_ac_login_2_req_t;

struct lnt_r_ac_login_2_ack
{
	unsigned char rfu[46];
	unsigned char login2_data[8+128];
	unsigned short sw;
}__attribute__((packed));
typedef struct lnt_r_ac_login_2_ack lnt_r_ac_login_2_ack_t;

/*
 * R_PUB_QRY_HIS(查询卡交易信息)
 */
struct card_status_16bytes 
{
	unsigned short spid;          //服务商代码
	unsigned char ttype;	      //交易类型			//0x17:普通消费,0x14:充值,0x13:地铁消费
	unsigned int amount:24;     //交易金额(高位在前)
	unsigned char samid[4]; 	      //SAM卡号
	unsigned char dtime[6];        //交易时间(YYMMDDHHMMSS)
}__attribute__((packed));

struct card_status_18bytes 
{
	unsigned int amount;    		//交易金额(高位在前)
	unsigned char ttype_ident;  //交易类型标识
	unsigned char tid[6];	      //终端机编号
	unsigned char rfu[2];	      //备用
	unsigned char date[4]; 	      //交易日期(XXMMDD)
	unsigned char ttype;	      //交易类型     //0x17:普通消费,0x14:充值,0x13:地铁消费
}__attribute__((packed));

struct lnt_r_pub_qry_his_ack
{
	unsigned char sak;
	unsigned char card_status;
	union{
		struct card_status_16bytes b16;
		struct card_status_18bytes b18;
	}s_un;
}__attribute__((packed));
typedef struct lnt_r_pub_qry_his_ack lnt_r_pub_qry_his_ack_t;

/*
 * R_PUB_QRY_PHYSICS_INFO(查询卡物理信息)
 */
struct lnt_r_pub_qry_physics_info_NoEncry_ack  //不加密
{
	unsigned char data[51];
	unsigned short sw;
}__attribute__((packed));
typedef struct lnt_r_pub_qry_physics_info_NoEncry_ack lnt_r_pub_qry_physics_info_NoEncry_ack_t;

struct lnt_r_pub_qry_physics_info_Encry_ack  //加密
{
	unsigned char data[80];
	unsigned short sw;
}__attribute__((packed));
typedef struct lnt_r_pub_qry_physics_info_Encry_ack lnt_r_pub_qry_physics_info_Encry_ack_t;

/*
 * R_PUB_SET_READCRADINFO(设置读卡器+卡信息)
 */
 struct lnt_r_pub_set_readcardinfo_req
{
	unsigned char data[8 + 72];
}__attribute__((packed));
typedef  struct lnt_r_pub_set_readcardinfo_req lnt_r_pub_set_readcardinfo_req_t;

 struct lnt_r_pub_set_readcardinfo_ack
{
	unsigned char data[72];   //各种类型的票卡返回不同
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_pub_set_readcardinfo_ack lnt_r_pub_set_readcardinfo_ack_t;

/*
 * R_CPU_LOAD_INIT(CPU钱包圈存初始化)
 */
struct lnt_r_cpu_load_init_req
{
	unsigned char data[93];///
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_init_req lnt_r_cpu_load_init_req_t;

struct lnt_r_cpu_load_init_ack
{
	unsigned char data[104];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_init_ack lnt_r_cpu_load_init_ack_t;

/*
 * R_CPU_LOAD(CPU 钱包圈存)
 */
struct lnt_r_cpu_load_req
{
	unsigned char data[61];///
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_req lnt_r_cpu_load_req_t;

struct lnt_r_cpu_load_ack
{
	unsigned char data[80];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_ack lnt_r_cpu_load_ack_t;

/*
 * R_M1_LOAD(M1 充值)
 */
struct lnt_r_m1_load_req
{
	unsigned char data[128];///
}__attribute__((packed));
typedef  struct lnt_r_m1_load_req lnt_r_m1_load_req_t;

struct lnt_r_m1_load_ack
{
	unsigned char data[32];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_m1_load_ack lnt_r_m1_load_ack_t;

/*
 * R_M1_CPU_QRY(M1_CPU 钱包查询)
 */
struct lnt_r_m1_cpu_qry_req
{
	unsigned char data[128]; 
}__attribute__((packed));
typedef  struct lnt_r_m1_cpu_qry_req lnt_r_m1_cpu_qry_req_t;

struct lnt_r_m1_cpu_qry_ack
{
	unsigned char data[72];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_m1_cpu_qry_ack lnt_r_m1_cpu_qry_ack_t;




































/*
 * R_DEPOSIT_INIT(押金初始化)
 */
struct lnt_r_deposit_init_req
{
	unsigned char area;          		//区域代码
	unsigned char dtype;         		//押金类型
	unsigned char member_op;  	  	//开卡销卡标志
	unsigned short spno;      	 		//运营商编码
	unsigned char member_mark;	   //会员标志
}__attribute__((packed));
typedef struct lnt_r_deposit_init_req lnt_r_deposit_init_req_t;

struct lnt_r_deposit_init_ack
{
	unsigned char data[176];
}__attribute__((packed));
typedef struct lnt_r_deposit_init_ack lnt_r_deposit_init_ack_t;

/*
 * R_DEPOSIT_PROCESS(押金处理)
 */
struct lnt_r_deposit_process_req
{
	unsigned char data[152];
}__attribute__((packed));
typedef struct lnt_r_deposit_process_req lnt_r_deposit_process_req_t;

struct lnt_r_deposit_process_ack
{
	unsigned char sw;        			//卡片返回码
}__attribute__((packed));
typedef struct lnt_r_deposit_process_ack lnt_r_deposit_process_ack_t;











/******************************************************************
*						岭南通网络指令
********************************************************************/
/*
 * 签到流程指令
 */
#define LNT_REGISTER_STAGE1				0xDD01
#define LNT_REGISTER_STAGE2				0xDD02
#define LNT_REGISTER_STAGE3				0xDD03
#define LNT_REGISTER_STAGE4				0xDD04
#define LNT_REGISTER_STAGE5				0xDD05
#define LNT_REGISTER_STAGE6				0xDD06
#define LNT_REGISTER_STAGE7				0xDD07
#define LNT_REGISTER_STAGE8				0xDD08

/*
 * 开卡,销卡
 */
#define LNT_MEMBER_REGISTER				0xDD73
#define LNT_MEMBET_CANCEL				0xDD74

/*
 * 充值流程指令
 */
#define LNT_RECHARGE_STAGE1				0xDD11
#define LNT_RECHARGE_STAGE2				0xDD12
#define LNT_RECHARGE_STAGE3				0xDD13
#define LNT_RECHARGE_STAGE4				0xDD14
#define LNT_RECHARGE_STAGE5				0xDD15
#define LNT_RECHARGE_STAGE6				0xDD16
#define LNT_RECHARGE_STAGE7				0xDD17
#define LNT_RECHARGE_STAGE8				0xDD18
#define LNT_RECHARGE_STAGE9				0xDD19
#define LNT_RECHARGE_STAGE10			0xDD20
#define LNT_RECHARGE_STAGE11			0xDD21
#define LNT_RECHARGE_STAGE12			0xDD22


struct lnt_packet_head
{
	unsigned short head;
	unsigned char version;
	unsigned short length;
}__attribute__((packed));
typedef struct lnt_packet_head lnt_packet_head_t;


/*
 * 签到阶段1
 */
 struct lnt_packet_register_stage1_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	unsigned char INFO[0];      //信息
	unsigned int crc32;         //校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage1_CTS lnt_packet_register_stage1_CTS_t;

/*
 * 签到阶段2
 */
 struct lnt_packet_register_stage2_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[5];  	//信息  [R_INIT REQ]
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage2_STC lnt_packet_register_stage2_STC_t;

/*
 * 签到阶段3: R-INIT报文
 */
 struct lnt_packet_register_stage3_CTS    
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	unsigned char INFO[14];      //信息 [R-INIT ACK]
	unsigned int crc32;         //校验码	
}__attribute__((packed));
typedef  struct lnt_packet_register_stage3_CTS lnt_packet_register_stage3_CTS_t;

/*
 * 签到阶段4
 */
 struct lnt_packet_register_stage4_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[5];  	//信息 [R_AC_LOGIN_1 REQ]
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage4_STC lnt_packet_register_stage4_STC_t;


/*
 * 签到阶段5
 */
 struct lnt_packet_register_stage5_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[8+128+2];  	//信息  [R_AC_LOGIN_1 ACK]
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage5_CTS lnt_packet_register_stage5_CTS_t;

/*
 * 签到阶段6
 */
 struct lnt_packet_register_stage6_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[5+8+128];  	//信息[R_AC_Login2 REQ]
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage6_STC lnt_packet_register_stage6_STC_t;

/*
 * 签到阶段7
 */
 struct lnt_packet_register_stage7_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[46+8+128+2];  	//信息 [R_AC_Login2 ACK]
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage7_CTS lnt_packet_register_stage7_CTS_t;

/*
 * 签到阶段8
 */
 struct lnt_packet_register_stage8_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[0];  	//信息 
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage8_STC lnt_packet_register_stage8_STC_t;




 struct lnt_member_process_stage1_CTS
 {
 	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[176]; //信息  [R_DEPOSIT_INIT]
	unsigned int crc32;  	//校验码
 }__attribute__((packed));
typedef  struct lnt_member_process_stage1_CTS lnt_member_process_stage1_CTS_t;

 struct lnt_member_process_stage2_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[152];  	//信息  [R_DEPPSIT_PROCESS]
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_member_process_stage2_STC lnt_member_process_stage2_STC_t;




/**************************充值****************************/

struct recharge_info
{
	unsigned char apply_type;//应用类型
	unsigned char recharge_amount[4];//充值金额
	unsigned char order_num[10];//订单号
}__attribute__((packed));
typedef struct recharge_info recharge_info_t;

struct lnt_packet_recharge_stage1_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	struct recharge_info INFO;      //信息
	unsigned int crc32;         //校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage1_CTS lnt_packet_recharge_stage1_CTS_t;

/*
 * 充值阶段2
 */
struct lnt_packet_recharge_stage2_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[29];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage2_STC lnt_packet_recharge_stage2_STC_t;

/*
 * 充值阶段3 
 */
struct lnt_packet_recharge_stage3_CTS     //113byte
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	//union info{
	//	unsigned char Encry_INFO[80+2];//加密
	//	unsigned char NoEncry_INFO[51+2];//不加密
	//}INFO;

	union info{
		unsigned char Encry_INFO[80+2];//加密
	}INFO;

	unsigned int crc32;         //校验码	
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage3_CTS  lnt_packet_recharge_stage3_CTS_t;

/*
 * 充值阶段4
 */
 struct lnt_packet_recharge_stage4_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[8+72+5];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage4_STC lnt_packet_recharge_stage4_STC_t;


/*
 * 充值阶段5
 */
 struct lnt_packet_recharge_stage5_CTS 
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[72+2];  	//信息 ///
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage5_CTS  lnt_packet_recharge_stage5_CTS_t;

/*
 * 充值阶段6
 */
 struct lnt_packet_recharge_stage6_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[93];  	/// 
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage6_STC lnt_packet_recharge_stage6_STC_t;

/*
 * 充值阶段7
 */
 struct lnt_packet_recharge_stage7_CTS 
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[104+2];   
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage7_CTS  lnt_packet_recharge_stage7_CTS_t;
  

/*
 * 充值阶段8
 */
 struct lnt_packet_recharge_stage8_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[61];   //信息///
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage8_STC lnt_packet_recharge_stage8_STC_t;


/*
 * 充值阶段9
 */
 struct lnt_packet_recharge_stage9_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[80+2];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage9_CTS lnt_packet_recharge_stage9_CTS_t;
  

/*
 * 充值阶段10
 */
 struct lnt_packet_recharge_stage10_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[128];   //信息///
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage10_STC lnt_packet_recharge_stage10_STC_t;

/*
 * 充值阶段11
 */
 struct lnt_packet_recharge_stage11_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	//unsigned char INFO[128];  	//信息
	unsigned char INFO[74];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage11_CTS lnt_packet_recharge_stage11_CTS_t;
  
/*
 * 充值阶段12
 */
struct recharge_msg
{
	unsigned char recharge_record_id[10];//充值记录ID
	unsigned char pki_num[4];//PKI 卡号
	unsigned char logic_num[8];//逻辑卡号
	unsigned char recharge_amount[4];//充值金额
	unsigned char balance[4];//交易后余额
}__attribute__((packed));

struct lnt_packet_recharge_stage12_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	struct recharge_msg INFO;
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage12_STC lnt_packet_recharge_stage12_STC_t;





/******************************************************************
*						代理服务器指令
********************************************************************/
/*
 * 代理签到命令
 */
#define AGENT_REGISTER_STAGE1				0xDD01
#define AGENT_REGISTER_STAGE2				0xDD02
#define AGENT_REGISTER_STAGE3				0xDD03
#define AGENT_REGISTER_STAGE4				0xDD04
#define AGENT_REGISTER_STAGE5				0xDD05
#define AGENT_REGISTER_STAGE6				0xDD06
#define AGENT_REGISTER_STAGE7				0xDD07
#define AGENT_REGISTER_STAGE8				0xDD08

/*
 * 开卡命令
 */
#define AGENT_ON_CARD_STAGE1				0xBB01  //[开卡会员申请]
#define AGENT_ON_CARD_STAGE2				0xBB02

#define AGENT_ON_CARD_STAGE3				0xBB10 //[消费记录上报]
#define AGENT_ON_CARD_STAGE4				0xBB11

#define AGENT_ON_CARD_STAGE5				0xDD73 //[修改押金标志]
#define AGENT_ON_CARD_STAGE6				0xDD74

#define AGENT_ON_CARD_STAGE7				0xBB20  //[开卡结果上报]
#define AGENT_ON_CARD_STAGE8				0xBB21

/*
 * 销卡命令
 */
#define AGENT_OFF_CARD_STAGE1				0xBB03    //[销卡会员申请]
#define AGENT_OFF_CARD_STAGE2				0xBB04    //信息长度2字节

#define AGENT_OFF_CARD_STAGE3				0xBB30    //信息长度2字节  [账户查询]
#define AGENT_OFF_CARD_STAGE4				0xBB31   //信息长度2字节

#define AGENT_OFF_CARD_STAGE5				0xBB34  //信息长度2字节 [获取订单号]
#define AGENT_OFF_CARD_STAGE6				0xBB35

#define AGENT_OFF_CARD_STAGE7				0xDD73  //[修改押金标志]
#define AGENT_OFF_CARD_STAGE8				0xDD74

#define AGENT_OFF_CARD_STAGE9				0xBB32  //[销卡结果上报]
#define AGENT_OFF_CARD_STAGE10				0xBB33

#define AGENT_OFF_CARD_STAGE11				0xDD11  //[充值流程]
#define AGENT_OFF_CARD_STAGE12				0xDD12
#define AGENT_OFF_CARD_STAGE13				0xDD13
#define AGENT_OFF_CARD_STAGE14				0xDD14
#define AGENT_OFF_CARD_STAGE15				0xDD15
#define AGENT_OFF_CARD_STAGE16				0xDD16
#define AGENT_OFF_CARD_STAGE17				0xDD17
#define AGENT_OFF_CARD_STAGE18				0xDD18 //(该过程执行完毕后，岭南通后台就会认为充值成功了,失败的话，需要人工处理)
#define AGENT_OFF_CARD_STAGE19				0xDD19
#define AGENT_OFF_CARD_STAGE20				0xDD20
#define AGENT_OFF_CARD_STAGE21				0xDD21
#define AGENT_OFF_CARD_STAGE22				0xDD22

#define AGENT_OFF_CARD_STAGE23				0xBB40 //[充值结果上报]
#define AGENT_OFF_CARD_STAGE24				0xBB41



/*
 * 充值命令
 */
#define AGENT_RECHARGE_STAGE1				0xBB05  //[充值申请]
#define AGENT_RECHARGE_STAGE2				0xBB06   

#define AGENT_RECHARGE_STAGE3				0xBB34  //[获取订单号]  
#define AGENT_RECHARGE_STAGE4				0xBB35 

#define AGENT_RECHARGE_STAGE5				0xDD11  //[充值流程]
#define AGENT_RECHARGE_STAGE6				0xDD12
#define AGENT_RECHARGE_STAGE7				0xDD13
#define AGENT_RECHARGE_STAGE8				0xDD14
#define AGENT_RECHARGE_STAGE9				0xDD15
#define AGENT_RECHARGE_STAGE10				0xDD16
#define AGENT_RECHARGE_STAGE11				0xDD17
#define AGENT_RECHARGE_STAGE12				0xDD18  //(该过程执行完毕后，岭南通后台就会认为充值成功了,失败的话，需要人工处理)
#define AGENT_RECHARGE_STAGE13				0xDD19
#define AGENT_RECHARGE_STAGE14				0xDD20
#define AGENT_RECHARGE_STAGE15				0xDD21
#define AGENT_RECHARGE_STAGE16				0xDD22

#define AGENT_RECHARGE_STAGE17				0xBB40   //[充值结果上报]
#define AGENT_RECHARGE_STAGE18				0xBB41 











/*
 * 会员申请结果
 */
enum AGENT_APPLY_RESULT
{
	AGENT_APPLY_RESULT_BLACKLIST = 0x01,					//黑名单
	AGENT_APPLY_RESULT_ID_CARD_INVALID = 0x02,				//身份证格式无效
	AGENT_APPLY_RESULT_PHONE_NUMBER_INVALID = 0x03,		//手机号码格式无效
	AGENT_APPLY_RESULT_ALLOW_ON_CARD = 0x10,				//允许开卡
	AGENT_APPLY_RESULT_CONSUME_NOT_ON_CARD = 0x011,		//已消费，未开卡
	AGENT_APPLY_RESULT_ID_CRAD_AMOUNT_TO_MUCH = 0x12,	//身份证开卡数量超过允许
	AGENT_APPLY_RESULT_ON_CARD = 0x13,						//已开卡
	AGENT_APPLY_RESULT_FIND_MANUAL = 0x14,					//找人工处理
	AGENT_APPLY_RESULT_NOT_BALANCE = 0x15					//余额不足
};




#define AGENT_ATTR_BIT0_PASS(c)			((c) | (1 << 0))
#define AGENT_ATTR_BIT0_NOT_PASS(c)		((c) &= ~(1 << 0))
#define AGENT_ATTR_BIT1_SUP(c)			((c) | (1 << 1))
#define AGENT_ATTR_BIT1_NOT_SUP(c)		((c) &= ~(1 << 1))




#define AGENT_ATTR_NOT_PASS			0x00   //不透传
#define AGENT_ATTR_PASS					0x01   //透传
#define AGENT_ATTR_SUP					0x02   //续传


/*
 * 代理服务器响应
 */
enum AGENT_ERR
{
	AGENT_ERR_OK = 0x00,   				//成功
	AGENT_ERR_LNT_OFFLINE = 0xE0,	   	//岭南通网关不通
	AGENT_ERR_BICYCLE_OFFLINE	= 0xE1,	  //自行车网关不通
	AGENT_ERR_LNT_TIMEOUT	= 0xE2,	   //岭南通网关超时
	AGENT_ERR_BICYCLE_TIMEOUT = 0xE3,   //自行车网关超时
	AGENT_ERR_PKT_CRC = 0xE4,	  //报文校验码超时
	AGENT_ERR_SYS_BUSY = 0xE5,   //系统繁忙
	AGENT_ERR_NOT_BALANCE	= 0xE6,   //账号余额不足	 


	AGENT_ERR_CINFO_ERROR_ORDER=0xA0,    //当前操作次序不一致
	AGENT_ERR_CINFO_ERROR_PKEY=0xA1,    //网充终端找不到公钥
	AGENT_ERR_CINFO_ERROR_TICKET_TYPE=0xA3,	   //查询卡的钱包类型不确定
	AGENT_ERR_CINFO_ERROR_ECORRECT=0xA4,	   //冲正失败
	AGENT_ERR_CINFO_ERROR_CORRECT=0xA5,	   //冲正成功
	AGENT_ERR_CINFO_ERROR_APP_TYPE=0xA6,   //应用类型不确定
	AGENT_ERR_CINFO_ERROR_ACC_BAL=0xA7,   //充值账号余额为零
	AGENT_ERR_CINFO_ERROR_CARD=0xAA,   //读卡器报错
	AGENT_ERR_CINFO_ERROR_NOT_REGISTER=0xBB,   //网充终端未签到
	AGENT_ERR_CINFO_ERROR_PKT_LEN=0xCC,	  //读卡器返回报文长度异常
	AGENT_ERR_CINFO_ERROR_SYS_BUSY=0xDD,   //系统繁忙
	AGENT_ERR_CINFO_ERROR_ESYS=0xEE,   //代系统返回结果报错
 	AGENT_ERR_CINFO_ERROR_ORDER_NOT_EXIST=0x96,   //对应订单不存在
	AGENT_ERR_CINFO_ERROR_ORDER_BAL=0x97,   //对应订单的金额不一致
	AGENT_ERR_CINFO_ERROR_ORDER_NO=0x98,   //对应订单的卡号不一致



	AGENT_ERR_CRC = 0xFD, //CRC错误
	AGENT_ERR_NOT_DATA = 0xFE, //接收不到数据
	AGENT_ERR_UNKNOWN = 0xFF  //未知错误
};





#define AGENT_HEADER			0x55aa 
#define AGENT_ONCR_ILEN			255
#define AGENT_OFFCR_ILEN		512


enum AGENT_TYPE
{
	AGENT_LNT = 0x01,        //岭南通
	AGENT_UNIONPAY = 0x02, //银联
	AGENT_ALIPAY = 0x03,  //支付宝
	AGENT_WECHAT = 0x04,  //微信

	AGENT_RECHARGE = 0x05  //充值
};

/*
 * 重传数据 : 用于插入到数据库
 */
struct agent_retransmission_data
{
	unsigned short cmd;  			//命令
	unsigned char pid[8];  			//票卡物理卡号
	unsigned int process_pkt_sn; 	//流程报文流水号
	unsigned int local_pkt_sn;  		//本地报文流水号

	int pkt_len;  //报文长度
	unsigned char pkt_data[512];  //报文数据
}__attribute__((packed));
typedef struct agent_retransmission_data agent_retransmission_data_t;


/*
 * ADPU数据
 */
struct adpu_str
{
	unsigned int s_len;
	unsigned char s_str[512];
}__attribute__((packed));
typedef struct adpu_str adpu_str_t;


/*
 * 额外数据
 */
 struct agent_extra_data
{
	unsigned int process_pkt_sn; 	//流程报文流水号
	unsigned char pid[8];			//票卡物理卡号
	unsigned char lid[8];                     //票卡逻辑卡号
	unsigned char macaddr[6];		//物理地址
	unsigned char pki[4];			//PKI
	unsigned int local_pkt_sn;  		//本地报文流水号
	unsigned char indcode[8];    		//流程指示码

	unsigned char order[10]; 		//订单号   //add by zjc 2016-03-24
	unsigned int recharge;  		//充值金额	 //add by zjc 2016-03-24
	
	unsigned char result;			//结果

	unsigned char toreader_step;    //step
	unsigned char toreader_pki[4];     //PKI
	unsigned char toreader_serial[8];	//握手流水号	
	adpu_str_t str; //ADPU


	unsigned char r_stat;		//返回读卡器状态字
}__attribute__((packed));
typedef  struct agent_extra_data agent_extra_data_t;




/*
 * 账户查询数据
 */ 
 struct agent_toreader_array
{
	unsigned char pki[4];				//PKI卡号
	unsigned char serial[8];				//握手流水号	
	unsigned char toreader_array[512];	//发往读卡器指令
}__attribute__((packed));
typedef  struct agent_toreader_array agent_toreader_array_t;

 struct agent_reqinfo_array
{
	unsigned char pki[4];			//PKI卡号
	unsigned char serial[8];			//握手流水号	
	unsigned char step; 			//读卡器指令执行到哪一步
	unsigned char reqinfo_array[512];	//读卡器返回的应答
}__attribute__((packed));
typedef  struct agent_reqinfo_array agent_reqinfo_array_t;






/*
 * 代理报文头部
 */
struct lnt_agent_packet_head
{
	unsigned short head;   //头部
	unsigned char type;	     //类型
	unsigned short cmd;    //命令
	unsigned int sn;        //报文序列号
	unsigned char attr;      //属性
	unsigned short length; //报文长度
}__attribute__((packed));
typedef struct lnt_agent_packet_head lnt_agent_packet_head_t;

/*
 * 代理属性
 */
struct agent_attribute   
{
	unsigned char pass:1;			//代理透传
	unsigned char supplement:1;		//数据补传
	unsigned char rfu[6];			//保留
}__attribute__((packed));
typedef struct agent_attribute agent_attribute_t;

/*
 * 代理注册
 */
 struct agent_register
{
	unsigned char pki[4];
	unsigned char macaddr[6];
	unsigned char infolen;
}__attribute__((packed));
typedef  struct agent_register agent_register_t;



/*
 * 开卡流程阶段1(会员申请请求)
 */
  struct lnt_agent_on_card_stage1_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char id_card[36];		//身份证号码
	unsigned char phone_number[6];  //手机号码
	
	unsigned char member_type;	        //会员类型
	unsigned short member_count;		//次数、天数、月数、年数 2016-04-14 add by zjc
	
	struct ticket_info ticket_info;         //票卡信息
	struct rent_info rent_info;             //租还车信息
	unsigned char time[6];                    //时间 
}__attribute__((packed)); 
typedef  struct lnt_agent_on_card_stage1_CTS lnt_agent_on_card_stage1_CTS_t;

/*
 * 开卡流程阶段2(会员申请应答)
 */
 struct lnt_agent_on_card_stage2_STC
{
	unsigned char pid[8];		//票卡物理卡号
	unsigned char retinfo; 		//信息返回码
	
	unsigned char member_type;	        //会员类型 2016-04-14 add by zjc
	unsigned short member_count;		//次数、天数、月数、年数
	
	unsigned int fee;	   		//应收金额
	unsigned char apply_result;  //申请结果
}__attribute__((packed)); 
typedef struct lnt_agent_on_card_stage2_STC lnt_agent_on_card_stage2_STC_t;

/*
 * 开卡流程阶段3(交易记录上报)
 */
struct lnt_agent_on_card_stage3_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int trade_sn;			//交易流水号
	struct trade_record trecord;         //交易记录
	unsigned char time[6];                    //时间 
}__attribute__((packed)); 
typedef  struct lnt_agent_on_card_stage3_CTS lnt_agent_on_card_stage3_CTS_t;


/*
 * 开卡流程阶段4(交易记录上报应答)
 */
 struct lnt_agent_on_card_stage4_STC
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int trade_sn;			//交易流水号
}__attribute__((packed)); 
typedef struct lnt_agent_on_card_stage4_STC lnt_agent_on_card_stage4_STC_t;

/*
 * 开卡流程阶段5(开卡请求)
 */
struct lnt_agent_on_card_stage5_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int trade_sn;			//交易流水号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_ONCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_on_card_stage5_CTS lnt_agent_on_card_stage5_CTS_t;

/*
 * 开卡流程阶段6(开卡应答)
 */
struct lnt_agent_on_card_stage6_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_ONCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_on_card_stage6_STC lnt_agent_on_card_stage6_STC_t;


/*
 * 开卡流程阶段7(开卡结果上报)
 */
struct lnt_agent_on_card_stage7_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int pkt_sn;			//报文流水号
	unsigned char indcode[8];    		//流程指示码
	unsigned char result;			//结果
	unsigned char time[6];                    //时间 
}__attribute__((packed)); 
typedef  struct lnt_agent_on_card_stage7_CTS lnt_agent_on_card_stage7_CTS_t;

/*
 * 开卡流程阶段8(开卡结果上报应答)
 */
struct lnt_agent_on_card_stage8_STC
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int pkt_sn;			//报文流水号
	unsigned char result;			//结果
}__attribute__((packed)); 
typedef  struct lnt_agent_on_card_stage8_STC lnt_agent_on_card_stage8_STC_t;


/////////////////////////////////////////销卡流程/////////////////////////////

/*
 * 销卡流程阶段1(会员申请请求)
 */
struct lnt_agent_off_card_stage1_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char id_card[36];		//身份证号码
	unsigned char phone_number[6];  //手机号码
	struct ticket_info ticket_info;         //票卡信息
	struct rent_info rent_info;             //租还车信息
	unsigned char time[6];                    //时间 	
}__attribute__((packed)); 
typedef struct lnt_agent_off_card_stage1_CTS lnt_agent_off_card_stage1_CTS_t;


/*
 * 销卡流程阶段2(会员申请应答)
 */
struct lnt_agent_off_card_stage2_STC
{
	unsigned char pid[8];	//票卡物理卡号
	unsigned char retinfo; //信息返回码
	unsigned char apply_result;  //申请结果	
	unsigned short agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef struct lnt_agent_off_card_stage2_STC lnt_agent_off_card_stage2_STC_t;


/*
 * 销卡流程阶段3(账户查询业务请求)
 */
struct lnt_agent_off_card_stage3_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned short agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 	
}__attribute__((packed)); 
typedef struct lnt_agent_off_card_stage3_CTS lnt_agent_off_card_stage3_CTS_t;


/*
 * 销卡流程阶段4(账户查询业务回报)
 */
struct lnt_agent_off_card_stage4_STC
{
	unsigned char pid[8];	//票卡物理卡号
	unsigned char retinfo; //信息返回码
	unsigned short agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 	
}__attribute__((packed)); 
typedef struct lnt_agent_off_card_stage4_STC lnt_agent_off_card_stage4_STC_t;

/*
 * 销卡流程阶段5(充值订单号请求)
 */
struct lnt_agent_off_card_stage5_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned short agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 	
}__attribute__((packed)); 
typedef struct lnt_agent_off_card_stage5_CTS lnt_agent_off_card_stage5_CTS_t;


/*
 * 销卡流程阶段6(账户查询业务回报)
 */
struct lnt_agent_off_card_stage6_STC
{
	unsigned char pid[8];	//票卡物理卡号
	unsigned char retinfo; //信息返回码
	unsigned char order[10]; //订单号
	unsigned int recharge;  //充值金额
	unsigned char apply_result;  //申请结果	
}__attribute__((packed)); 
typedef struct lnt_agent_off_card_stage6_STC lnt_agent_off_card_stage6_STC_t;


/*
 * 销卡流程阶段7(销卡请求)
 */
struct lnt_agent_off_card_stage7_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
		}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage7_CTS lnt_agent_off_card_stage7_CTS_t;

/*
 * 销卡流程阶段8(销卡应答)
 */
struct lnt_agent_off_card_stage8_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage8_STC lnt_agent_off_card_stage8_STC_t;

/*
 * 销卡流程阶段9(销卡结果上报)
 */
struct lnt_agent_off_card_stage9_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int pkt_sn;			//报文流水号
	unsigned char indcode[8];    		//流程指示码
	unsigned char result;			//结果
	unsigned char time[6];                    //时间 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage9_CTS lnt_agent_off_card_stage9_CTS_t;

/*
 * 销卡流程阶段10(销卡结果上报应答)
 */
struct lnt_agent_off_card_stage10_STC
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int pkt_sn;			//报文流水号
	unsigned char result;			//结果
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage10_STC lnt_agent_off_card_stage10_STC_t;


/*
 * 销卡流程阶段11
 */
struct lnt_agent_off_card_stage11_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage11_CTS lnt_agent_off_card_stage11_CTS_t;

/*
 * 销卡流程阶段12
 */
struct lnt_agent_off_card_stage12_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage12_STC lnt_agent_off_card_stage12_STC_t;

/*
 * 销卡流程阶段13
 */
struct lnt_agent_off_card_stage13_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage13_CTS lnt_agent_off_card_stage13_CTS_t;

/*
 * 销卡流程阶段14
 */
struct lnt_agent_off_card_stage14_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage14_STC lnt_agent_off_card_stage14_STC_t;


/*
 * 销卡流程阶段15
 */
struct lnt_agent_off_card_stage15_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage15_CTS lnt_agent_off_card_stage15_CTS_t;

/*
 * 销卡流程阶段16
 */
struct lnt_agent_off_card_stage16_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage16_STC lnt_agent_off_card_stage16_STC_t;


/*
 * 销卡流程阶段17
 */
struct lnt_agent_off_card_stage17_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage17_CTS lnt_agent_off_card_stage17_CTS_t;

/*
 * 销卡流程阶段18
 */
struct lnt_agent_off_card_stage18_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage18_STC lnt_agent_off_card_stage18_STC_t;


/*
 * 销卡流程阶段19
 */
struct lnt_agent_off_card_stage19_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage19_CTS lnt_agent_off_card_stage19_CTS_t;

/*
 * 销卡流程阶段20
 */
struct lnt_agent_off_card_stage20_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage20_STC lnt_agent_off_card_stage20_STC_t;

/*
 * 销卡流程阶段21
 */
struct lnt_agent_off_card_stage21_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage21_CTS lnt_agent_off_card_stage21_CTS_t;

/*
 * 销卡流程阶段22
 */
struct lnt_agent_off_card_stage22_STC
{
	unsigned char pid[8];				//票卡物理卡号
	unsigned char retinfo; 				//信息返回码
	unsigned char agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage22_STC lnt_agent_off_card_stage22_STC_t;


/*
 * 销卡流程阶段23(充值结果上报) 
 */
struct lnt_agent_off_card_stage23_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int pkt_sn;			//报文流水号
	unsigned char indcode[8];    		//流程指示码

	unsigned char order[10];		//订单号   //add by zjc 2016-03-24
	unsigned int recharge;			//充值金额	 //add by zjc 2016-03-24
	
	unsigned char result;			//结果
	unsigned char time[6];                    //时间 
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage23_CTS lnt_agent_off_card_stage23_CTS_t;

/*
 * 销卡流程阶段24(充值结果上报应答)
 */
struct lnt_agent_off_card_stage24_STC
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned int pkt_sn;			//报文流水号
	unsigned char result;			//结果
}__attribute__((packed)); 
typedef  struct lnt_agent_off_card_stage24_STC lnt_agent_off_card_stage24_STC_t;


/*
 * 管理卡
 * Key(16):羊城通物理卡号（8）+羊城通逻辑号（8）明文(8)：A6(1)+A7(2)+A9(3)+A12(2) TAC=3DES(明文，key)
 */
struct lnt_admin_36bytes
{
	unsigned char A1;   		 //应用锁定标志
	unsigned short A2;  		 //服务营运代码
	unsigned char A3:4;		//租还车标志
	unsigned char A4:4;		//版本序号
	unsigned char A5;			//待定
	unsigned char A6;			//票种标记  0XFE：员工卡（管理卡）
	unsigned short A7;			//天数
	unsigned char A8[4];		//待定
	unsigned int A9:24;			//启用时间
	unsigned char A10_TAC_KEY[8]; //Tac
	unsigned char A11[11];			//待定
	unsigned short A12;			//片区
}__attribute__((packed)); 
typedef struct lnt_admin_36bytes lnt_admin_36bytes_t;

struct lnt_admin_card_info_req
{
	unsigned char id;
}__attribute__((packed));
typedef struct lnt_admin_card_info_req lnt_admin_card_info_req_t;

struct lnt_admin_card_info_ack
{
	unsigned char length;
	struct lnt_admin_36bytes admin_36bytes;
}__attribute__((packed));
typedef struct lnt_admin_card_info_ack lnt_admin_card_info_ack_t;

struct lnt_admin_card_info
{
	unsigned char id;
	unsigned char length;
	struct lnt_admin_36bytes admin_36bytes;
}__attribute__((packed));
typedef struct lnt_admin_card_info lnt_admin_card_info_t;

/*
 * 管理卡加密数据
 */
struct admin_card_encry_data
{
	unsigned char A6;
	unsigned short A7;
	unsigned int A9:24;
	unsigned short A12;
}__attribute__((packed));
typedef struct admin_card_encry_data admin_card_encry_data_t;



/*
 * 管理卡类型
 */
 enum LNT_ADMIN_CARD_TYPE
{
	LNT_ADMIN_CARD_TYPE_NOT = 1,
	LNT_ADMIN_CARD_TYPE_RD,
	LNT_ADMIN_CARD_TYPE_RW,
};




unsigned int lnt_crc32(unsigned char *buffer, unsigned int size);



/////////////////////////////////////////充值流程///////////////////////////////////////
/*
 * 充值流程阶段1(充值申请请求)
 */
struct lnt_agent_recharge_stage1_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	struct ticket_info ticket_info; //票卡信息
	unsigned char order[10]; 		//订单号
	unsigned char time[6];          //时间 	
}__attribute__((packed)); 
typedef struct lnt_agent_recharge_stage1_CTS lnt_agent_recharge_stage1_CTS_t;


/*
 * 充值流程阶段2(充值申请应答)
 */
struct lnt_agent_recharge_stage2_STC
{
	unsigned char pid[8];						//票卡物理卡号
	unsigned char retinfo; 						//信息返回码
	unsigned char apply_result;  				//申请结果	
	unsigned short agent_ilen;     				//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 
}__attribute__((packed)); 
typedef struct lnt_agent_recharge_stage2_STC lnt_agent_recharge_stage2_STC_t;

/*
 * 充值流程阶段3(充值订单号请求)
 */
struct lnt_agent_recharge_stage3_CTS
{
	unsigned char pid[8];			//票卡物理卡号
	unsigned char macaddr[6];		//MAC地址
	unsigned char pki[4];			//PKI卡号
	unsigned short agent_ilen;     			//信息长度
	unsigned char agent_info[AGENT_OFFCR_ILEN]; //信息 	
}__attribute__((packed)); 
typedef struct lnt_agent_recharge_stage3_CTS lnt_agent_recharge_stage3_CTS_t;


/*
 * 充值流程阶段4(充值订单号回报)
 */
struct lnt_agent_recharge_stage4_STC
{
	unsigned char pid[8];	//票卡物理卡号
	unsigned char retinfo; //信息返回码
	unsigned char order[10]; //订单号
	unsigned int recharge;  //充值金额
	unsigned char apply_result;  //申请结果	
}__attribute__((packed)); 
typedef struct lnt_agent_recharge_stage4_STC lnt_agent_recharge_stage4_STC_t;



#endif


