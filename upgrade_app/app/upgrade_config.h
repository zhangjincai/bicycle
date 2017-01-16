#ifndef __UPGRADE_CONFIG_H__
#define __UPGRADE_CONFIG_H__


struct ftp_download_ctrl_info
{
	int isvaild;  //�Ƿ���Ч
	int ftype;   //�̼�����
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
	char ftp_username[32];			//FTP�û���
	char ftp_passwd[32];			//FTP����
	char ftp_remote_path[96];		//FTPԶ������·���ļ�
	char ftp_local_path[96];			//FTP���ر���·���ļ�
	long ftp_connect_timeout;		//FTP���ӳ�ʱ
	long ftp_download_timeout;		//FTP���س�ʱ
	
	char firmware_name[32];                      	 	//����
	unsigned int total_len;					//�ܳ���
	unsigned int download_len;		  		//�����س���
	unsigned char last_datetime[7];			//YY MM DD HH MM SS,���ִ���ļ�ʱ��
	unsigned char md5[16];		 			//MD5 У��
	unsigned char is_download_complete;      	//�Ƿ��������? 2:��� 1:û����� 0:û�����ز���
	unsigned char is_write_parameter;             //�Ƿ�Ѳ���д��EEPROM	1:��д 0:δд
	unsigned char is_write_flash;			//�Ƿ���ļ�д��flash	1:��д 0:δд
}__attribute__((packed));

struct upgrade_config
{
	char used_kernel_name[32];			//�Ѿ�д��flash�Ĺ̼�����
	char used_firmware_name[32];
	char used_appl_name[32];
	char used_lnt_zm_name[32];
	
	struct firmware_config kernel;
	struct firmware_config firmware;
	struct firmware_config appl;
	struct firmware_config lnt_zm; //����ͨ�̼�����

	struct ftp_download_ctrl_info fdnl_ctrl_info;
		
	unsigned char rfu[128];
}__attribute__((packed));
typedef struct upgrade_config upgrade_config_t;


int upgrade_config_init(void);
int upgrade_config_put(struct upgrade_config *config);
void upgrade_config_get(struct upgrade_config *config);



#endif


