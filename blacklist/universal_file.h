#ifndef __UNIVERSAL_FILE_H__
#define __UNIVERSAL_FILE_H__



#define UNIV_CONF_EOK					(0)		//����
#define UNIV_CONF_ERROR				(-1)		//����
#define UNIV_CONF_EMAGIC				(-2)       //ħ������
#define UNIV_CONF_ECRC					(-3)      //CRC����

#define UNIV_FILE_LEN					(256)    //���ļ����ݳ���

#define UNIV_IDLE						0
#define UNIV_USED_A						1
#define UNIV_USED_B						2



/*
 * ����������
 */
struct black_list_info
{
	unsigned char pid[8];			//������
	unsigned char lid[8];           //�߼�����
	//unsigned char flag; //��������ʶ:A:�Ƿ�����B:��ʧ��
}__attribute__((packed));
typedef struct black_list_info black_list_info_t;


/*
 * �ļ�����
 */
enum UNIV_FILE_CATEGORY
{
	UFC_TOTAL_BL = 0x71,					//����������
	UFC_INC_BL = 0x72,						//����������
	UFC_DEC_BL = 0x73,						//����������
	UFC_TEMP_BL = 0x74,						//��ʱ������
	UFC_NO1_STAKE_FIRMWARE = 0X81,   		// 1����׮�̼�
	UFC_NO2_STAKE_FIRMWARE = 0X82,   		// 2����׮�̼�
	UFC_NO3_STAKE_FIRMWARE = 0X83,   		// 3����׮�̼�
	UFC_STAKE_TRADE_RECORD = 0x91,    		//��׮���׼�¼
	UFC_UNIV_FILE_NOT_SEND_SET = 0xA1,   	//����δ�յ���¼ͳһ
	UFC_LOG_SET = 0xA2   					//��־����
};


struct ufc_blacklist_version
{
	unsigned char total_last_ver[8]; 		//�����������汾
	unsigned char inc_last_ver[8];			//�����������汾
	unsigned char dec_last_ver[8]; 			//�����������汾
	unsigned char temp_last_ver[8];  		//��ʱ�������汾

	unsigned char use_total_db;			//��ǰʹ�õĺ��������ݿ�
	unsigned char use_inc_db;
	unsigned char use_dec_db;
	unsigned char use_temp_db;
	
	unsigned short use_total_seq;
	unsigned short use_inc_seq;
	unsigned short use_dec_seq;
	unsigned short use_temp_seq;

	
	

	unsigned char rfu[64];
};

struct ufc_stake_firmware
{
	unsigned char last_ver[8];	
	unsigned char use_db;

	unsigned char rfu[64];
};


struct univ_file_config
{
	struct ufc_blacklist_version blk;
	struct ufc_stake_firmware sae;
	unsigned char rfu[512];		//����
}__attribute__((packed));
typedef struct univ_file_config univ_file_config_t;













/*
 * ��������������
 */
struct blacklist_attr
{
	unsigned char n4[4];
	unsigned char vd4[4];
	unsigned char f1;
}__attribute__((packed));
typedef struct blacklist_attr blacklist_attr_t;






/*
 * ���ļ�ͷ��
 */
 #define UNIV_DIV_SEQ_BC		0xffff  //�㲥���ļ�
struct univ_file_hd
{
	unsigned short file_seq;	  //�ļ����
	unsigned short total;	  //�ָ�����
	unsigned short div_seq;  //�ָ����
	unsigned char ufc;        //�ļ����
	unsigned char attr[9];   //��������
}__attribute__((packed));
typedef struct univ_file_hd univ_file_hd_t;

/*
 * ���ļ��ṹ
 */
struct univ_file_st
{
	unsigned short length;
	struct univ_file_hd hd;
	unsigned char data[UNIV_FILE_LEN];
}__attribute__((packed));
typedef struct univ_file_st univ_file_st_t;


/*
 * ���ļ��ر�
 */
 #define UNIV_ACK_OK		0x00	//�ɹ�
 #define UNIV_ACK_RESEND	0xff	//�ط���ǰ����
struct univ_file_ack
{
	struct univ_file_hd hd;
	unsigned char result;            //0x00:�ɹ� 0xff:�ط���ǰ���
}__attribute__((packed));
typedef struct univ_file_ack univ_file_ack_t;



struct univ_blacklist_handle
{
	unsigned short total_seq;
	unsigned char bl_type;
	void *bl_db;
	void *univ_hd;
	unsigned char cmd;
	unsigned char sn;
}__attribute__((packed));
typedef struct univ_blacklist_handle univ_blacklist_handle_t;

struct univ_firmware_handle
{
	unsigned short total_seq;
	void *fw_db;
	void *univ_hd;
	unsigned char cmd;
	unsigned char sn;
}__attribute__((packed));
typedef struct univ_firmware_handle univ_firmware_handle_t;




int univ_file_config_init(void);
void univ_file_config_destroy(void);
int univ_file_config_put(univ_file_config_t *conf);
void univ_file_config_get(univ_file_config_t *conf);
void univ_file_config_printf(void);


void univ_file_n4_to_str(unsigned char s_n4[8], univ_file_st_t *univ);
void univ_file_vd4_to_str(unsigned char s_vd4[8], univ_file_st_t *univ);










#endif


