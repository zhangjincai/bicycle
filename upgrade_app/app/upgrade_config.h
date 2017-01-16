#ifndef __UPGRADE_CONFIG_H__
#define __UPGRADE_CONFIG_H__


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


struct firmware_config
{
	char ftp_username[32];			//FTP用户名
	char ftp_passwd[32];			//FTP密码
	char ftp_remote_path[96];		//FTP远程下载路径文件
	char ftp_local_path[96];			//FTP本地保存路径文件
	long ftp_connect_timeout;		//FTP连接超时
	long ftp_download_timeout;		//FTP下载超时
	
	char firmware_name[32];                      	 	//名称
	unsigned int total_len;					//总长度
	unsigned int download_len;		  		//已下载长度
	unsigned char last_datetime[7];			//YY MM DD HH MM SS,最后执行文件时间
	unsigned char md5[16];		 			//MD5 校验
	unsigned char is_download_complete;      	//是否下载完成? 2:完成 1:没有完成 0:没有下载操作
	unsigned char is_write_parameter;             //是否把参数写入EEPROM	1:已写 0:未写
	unsigned char is_write_flash;			//是否把文件写入flash	1:已写 0:未写
}__attribute__((packed));

struct upgrade_config
{
	char used_kernel_name[32];			//已经写入flash的固件名称
	char used_firmware_name[32];
	char used_appl_name[32];
	char used_lnt_zm_name[32];
	
	struct firmware_config kernel;
	struct firmware_config firmware;
	struct firmware_config appl;
	struct firmware_config lnt_zm; //岭南通固件升级

	struct ftp_download_ctrl_info fdnl_ctrl_info;
		
	unsigned char rfu[128];
}__attribute__((packed));
typedef struct upgrade_config upgrade_config_t;


int upgrade_config_init(void);
int upgrade_config_put(struct upgrade_config *config);
void upgrade_config_get(struct upgrade_config *config);



#endif


