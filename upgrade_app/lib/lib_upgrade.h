#ifndef __LIB_UPGRADE_H__
#define __LIB_UPGRADE_H__


#define LIB_UPE_NULL				(NULL) 
#define LIB_UPE_EOK				(0)  //����
#define LIB_UPE_ERROR			(-1) //����
#define LIB_UPE_ETIMEOUT			(-2) //��ʱ
#define LIB_UPE_EFULL			(-3) //��
#define LIB_UPE_EEMPTY			(-4) //��
#define LIB_UPE_ENOMEM 			(-5) //�ڴ治��
#define LIB_UPE_EXCMEM			(-6) //�ڴ�Խ��
#define LIB_UPE_EBUSY			(-7) //æ
#define LIB_UPE_ERR_COMMAND     	(-8) //��֧�ֵ�����

#define LIB_UPE_TRUE				(1)
#define LIB_UPE_FALSE			(0)

#define LIB_UPE_MTYPE_NDEV		0x11
#define LIB_UPE_MTYPE_UP		0x12


enum UPE_TYPE			//��������
{
	UPE_TYPE_INIT = 0,
	UPE_TYPE_KERNEL,
	UPE_TYPE_FIRMWARE,
	UPE_TYPE_APPL,
	UPE_TYPE_LNT_ZM,
	UPE_TYPE_END
};

enum UPE_NETWORK_STAT	//����״̬
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

enum UPE_FTP_DOWNLOAD_SWITCH	//���ؿ���
{
	UPE_FTP_DOWNLOAD_SW_ON = 1,
	UPE_FTP_DOWNLOAD_SW_OFF
};

enum UP_DNL_STAT
{
	UP_DNL_STAT_NOT_OP = 0,              //û�����ز���
	UP_DNL_STAT_NOT_COMPLETE = 1,  //û�����
	UP_DNL_STAT_COMPLETE = 2  //���
};

enum UP_WR_STAT
{
	UP_WR_STAT_NOT_WR = 0,   //δд
	UP_WR_STAT_WR = 1,   //��д
	UP_WR_STAT_NOT_WR_LNT = 2,	 //����ͨ����������ר�� 2016-07-28
};

struct ftp_config
{
	unsigned char update_type;	//��������
	char username[32];			//FTP�û���
	char passwd[32];			//FTP����
	unsigned short port;		//FTP�˿�
	char firmware_name[32];	//�̼���
	char remote_path[96];		//FTPԶ������·���ļ�
	char local_path[96];		//FTP���ر���·���ļ�
	long  connect_timeout;		//FTP���ӳ�ʱ
	long download_timeout;		//FTP���س�ʱ
}__attribute__((packed));
typedef struct ftp_config ftp_config_t;

struct firmware_config
{
	char ftp_username[32];					//FTP�û���
	char ftp_passwd[32];					//FTP����
	char ftp_remote_path[96];				//FTPԶ������·���ļ�
	char ftp_local_path[96];					//FTP���ر���·���ļ�
	long ftp_connect_timeout;				//FTP���ӳ�ʱ
	long ftp_download_timeout;				//FTP���س�ʱ

	char firmware_name[32];                      	 	//����
	unsigned int total_len;					//�ܳ���
	unsigned int download_len;		  		//�����س���
	unsigned char last_datetime[7];			//YY MM DD HH MM SS,���ִ���ļ�ʱ��
	unsigned char md5[16];		 			//MD5 У��
	unsigned char is_download_complete;      	//�Ƿ��������? 2:��� 1:û����� 0:û�����ز���
	unsigned char is_write_parameter;             //�Ƿ�Ѳ���д��EEPROM	1:��д 0:δд
	unsigned char is_write_flash;			//�Ƿ���ļ�д��flash	1:��д 0:δд
}__attribute__((packed));

/*
 * FTP����״̬
 */
struct ftp_upgrade_status
{
	unsigned char download_status;
	unsigned char ftp_recode;
	unsigned char ftp_errno;
}__attribute__((packed));
typedef struct ftp_upgrade_status ftp_upgrade_status_t;

/*
 * FTP ���ؿ�����Ϣ
 */
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


