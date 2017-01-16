#ifndef __LIB_UPGRADE_H__
#define __LIB_UPGRADE_H__


#define LIB_UPE_NULL				(NULL) 
#define LIB_UPE_EOK				(0)  //正常
#define LIB_UPE_ERROR			(-1) //错误
#define LIB_UPE_ETIMEOUT			(-2) //超时
#define LIB_UPE_EFULL			(-3) //满
#define LIB_UPE_EEMPTY			(-4) //空
#define LIB_UPE_ENOMEM 			(-5) //内存不够
#define LIB_UPE_EXCMEM			(-6) //内存越界
#define LIB_UPE_EBUSY			(-7) //忙
#define LIB_UPE_ERR_COMMAND     	(-8) //不支持的命令

#define LIB_UPE_TRUE				(1)
#define LIB_UPE_FALSE			(0)

#define LIB_UPE_MTYPE_NDEV		0x11
#define LIB_UPE_MTYPE_UP		0x12


enum UPE_TYPE			//更新类型
{
	UPE_TYPE_INIT = 0,
	UPE_TYPE_KERNEL,
	UPE_TYPE_FIRMWARE,
	UPE_TYPE_APPL,
	UPE_TYPE_LNT_ZM,
	UPE_TYPE_END
};

enum UPE_NETWORK_STAT	//网络状态
{
	UPE_NETWORK_STAT_INIT = 0,
	UPE_NETWORK_STAT_CONNECT,
	UPE_NETWORK_STAT_DISCONNECT,
	UPE_NETWORK_STAT_END
};

enum UPE_FIRMWARE_TYPE   
{
	UPE_FIRMWARE_TYPE_NO,
	UPE_FIRMWARE_TYPE_KERNEL,
	UPE_FIRMWARE_TYPE_FIRMWARE,
	UPE_FIRMWARE_TYPE_APPL,
	UPE_FIRMWARE_TYPE_LNT_ZM
};

enum UPE_FTP_DOWNLOAD_SWITCH	//下载开关
{
	UPE_FTP_DOWNLOAD_SW_ON = 1,
	UPE_FTP_DOWNLOAD_SW_OFF
};

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

struct ftp_config
{
	unsigned char update_type;	//更新类型
	char username[32];			//FTP用户名
	char passwd[32];			//FTP密码
	unsigned short port;		//FTP端口
	char firmware_name[32];	//固件名
	char remote_path[96];		//FTP远程下载路径文件
	char local_path[96];		//FTP本地保存路径文件
	long  connect_timeout;		//FTP连接超时
	long download_timeout;		//FTP下载超时
}__attribute__((packed));
typedef struct ftp_config ftp_config_t;

struct firmware_config
{
	char ftp_username[32];					//FTP用户名
	char ftp_passwd[32];					//FTP密码
	char ftp_remote_path[96];				//FTP远程下载路径文件
	char ftp_local_path[96];					//FTP本地保存路径文件
	long ftp_connect_timeout;				//FTP连接超时
	long ftp_download_timeout;				//FTP下载超时

	char firmware_name[32];                      	 	//名称
	unsigned int total_len;					//总长度
	unsigned int download_len;		  		//已下载长度
	unsigned char last_datetime[7];			//YY MM DD HH MM SS,最后执行文件时间
	unsigned char md5[16];		 			//MD5 校验
	unsigned char is_download_complete;      	//是否下载完成? 2:完成 1:没有完成 0:没有下载操作
	unsigned char is_write_parameter;             //是否把参数写入EEPROM	1:已写 0:未写
	unsigned char is_write_flash;			//是否把文件写入flash	1:已写 0:未写
}__attribute__((packed));

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

/*
 * FTP 下载控制信息
 */
struct ftp_download_ctrl_info
{
	int isvaild;  //是否有效
	int ftype;   //固件类型
	unsigned int ftime;

	unsigned short upsn;
	unsigned char supplier;
	unsigned char time[7];
	unsigned char download_status;
	unsigned char ftpcode;
	unsigned char err;
}__attribute__((packed));
typedef struct ftp_download_ctrl_info ftp_download_ctrl_info_t;

typedef struct lib_upgrade lib_upgrade_t;
lib_upgrade_t *lib_upgrade_create(const unsigned int msg_type);
void lib_upgrade_destroy(lib_upgrade_t *up);
int lib_upgrade_set_ftp_download_timeout(lib_upgrade_t *up, const int timeout);
int lib_upgrade_get_ftp_download_timeout(lib_upgrade_t *up, int *timeout);
int lib_upgrade_set_ftp_download_switch(lib_upgrade_t *up, enum UPE_FTP_DOWNLOAD_SWITCH sw);
int lib_upgrade_set_upgrade_start(lib_upgrade_t *up, enum UPE_TYPE type);
int lib_upgrade_set_ftp_config(lib_upgrade_t *up, struct ftp_config *ftp);
int lib_upgrade_get_ftp_config(lib_upgrade_t *up, struct ftp_config *ftp);
int lib_upgrade_get_firmware_config(lib_upgrade_t *up, enum UPE_FIRMWARE_TYPE type, struct firmware_config *firmware);
int lib_upgrade_get_upstat(lib_upgrade_t *up, enum UPE_FIRMWARE_TYPE type, struct ftp_upgrade_status *status);
int lib_upgrade_set_ftp_dnl_ctrl_info(lib_upgrade_t *up, struct ftp_download_ctrl_info *fdnl_ctrl_info);
int lib_upgrade_get_ftp_dnl_ctrl_info(lib_upgrade_t *up, struct ftp_download_ctrl_info *fdnl_ctrl_info);



#endif


