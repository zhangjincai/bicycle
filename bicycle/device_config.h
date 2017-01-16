#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__


#define DEV_CONF_EOK					(0)		//����
#define DEV_CONF_ERROR					(-1)		//����
#define DEV_CONF_EMAGIC					(-2)       //ħ������
#define DEV_CONF_ECRC					(-3)      //CRC����



struct ndev_config
{
	char boot_ver[32];				//UBOOT �汾
	char kernel_ver[32];			//kernel�汾
	char rootfs_ver[32];			//rootfs�汾
	char fw_ver[32]; 				//firmware�汾
	char appl_ver[32];				//appl�汾
	
	unsigned int local_ip;			//����IP
	unsigned int local_submask;		//������������
	unsigned int local_gateway;		//��������
	unsigned int local_dns;			//������ַ

	unsigned int wifi_ip;			//WIFI ����IP
	unsigned int wifi_submask;		//WIFI ��������
	unsigned int wifi_gateway;		//WIFI ����
	unsigned int wifi_dns;			//WIFI ������ַ
	
	unsigned char using_wireless;   //ʹ����������   {1:���� 2:����}
	
	unsigned int load_server1_ip;		//���ؾ��������1 IP ��ַ
	unsigned int load_server2_ip;		//���ؾ��������2 IP ��ַ
	unsigned short load_server1_port;	//���ؾ���1�˿�
	unsigned short load_server2_port;	//���ؾ���2�˿�
	unsigned short load_timeout;			//���ؾ��ⳬʱʱ��(��)
		
	unsigned int ftp_ip;
	unsigned short ftp_port;
	char ftp_username[32];
	char ftp_password[32];

	unsigned short heart_time;		//��������ʱ��
	unsigned short emerg_heart_time; //���Ľ�������ʱ��
	unsigned char timer_gate_value;  //ʱ������բֵ
	unsigned short term_id; 		//�ն˱��
	unsigned short area_info;       	//������Ϣ
	
	unsigned int flow_3g;   		// 3G����
	unsigned short first_level_3g;  // 3G������һբֵ
	unsigned short second_level_3g; // 3G�����ڶ�բֵ

	unsigned int lnt_ipaddr;   //����ͨ����IP
	unsigned short lnt_port;  //����ͨ���Ķ˿�
	unsigned char lnt_userid[16];  //����ͨ�û�ID
	unsigned char lnt_spno[2]; //�����̴���
	unsigned char lnt_conpa; //���ѳ�ʼ������
	unsigned char lnt_cvad; //��������Ч������	

	unsigned char light_ctrl_time_enable[2];  //hhmm
	unsigned char light_ctrl_time_disable[2];  //hhmm
	
	unsigned char site_name[50]; //վ������  252-50=202
	unsigned char site_QR_code[128];  //վ���ά��  202-32=170 //170-96=74 2016-11-25

	unsigned char rfu[74];       //���� {-4:light_ctrl_time_enable, light_ctrl_time_disable, site_name, site_QR_code}
}__attribute__((packed));
typedef  struct ndev_config ndev_config_t;

 struct stake_config
{
	unsigned short heart_time;       		//׮������ʱ��
	char firmware_uptime[7]; 			//�̼�����ʱ��
	unsigned char can_baud_rate;		//CAN������
	unsigned short quantity;   			//��׮����

	unsigned char rfu[128];       //����
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


