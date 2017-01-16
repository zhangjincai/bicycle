#ifndef __UNIVERSAL_FILE_H__
#define __UNIVERSAL_FILE_H__



#define UNIV_CONF_EOK					(0)		//����
#define UNIV_CONF_ERROR					(-1)		//����
#define UNIV_CONF_EMAGIC				(-2)       //ħ������
#define UNIV_CONF_ECRC					(-3)      //CRC����

#define UNIV_FILE_LEN					(256)    //���ļ����ݳ���

#define UNIV_IDLE						0
#define UNIV_USED_A						1
#define UNIV_USED_B						2

#define UNIV_INIT							0
#define UNIV_RUN							1
#define UNIV_STOP						2



/*
 * �ļ�����
 */
enum UNIV_FILE_CATEGORY
{
	UFC_TOTAL_BL = 0x71,						//����������
	UFC_INC_BL = 0x72,						//����������
	UFC_DEC_BL = 0x73,						//����������
	UFC_TEMP_BL = 0x74,						//��ʱ������
	UFC_NO1_STAKE_FIRMWARE = 0X81,   		// 1����׮�̼�
	UFC_NO2_STAKE_FIRMWARE = 0X82,   		// 2����׮�̼�
	UFC_NO3_STAKE_FIRMWARE = 0X83,   		// 3����׮�̼�
	UFC_STAKE_TRADE_RECORD = 0x91,    		//��׮���׼�¼
	UFC_UNIV_FILE_NOT_SEND_SET = 0xA1,   	//����δ�յ���¼ͳһ
	UFC_LOG_SET = 0xA2,   						//��־����
	UFC_ADMIN_CARD_RECORD = 0xA3,				//����ʹ�ü�¼
	UFC_EXCEPT_HANDLE_CONSUME_RECORD = 0xA4   //�쳣����۷ѽ��׼�¼
};


struct ufc_blacklist_version
{
	unsigned char total_last_ver[9]; 		//�����������汾
	unsigned char inc_last_ver[9];			//�����������汾
	unsigned char dec_last_ver[9]; 			//�����������汾
	unsigned char temp_last_ver[9];  		//��ʱ�������汾

	unsigned char use_total_db;			//��ǰʹ�õĺ��������ݿ�
	unsigned char use_inc_db;
	unsigned char use_dec_db;
	unsigned char use_temp_db;
	
	unsigned short use_total_seq;
	unsigned short use_inc_seq;
	unsigned short use_dec_seq;
	unsigned short use_temp_seq;

	
	

	unsigned char rfu[64];
}__attribute__((packed));

struct ufc_stake_firmware
{
	unsigned char last_ver[9];	  //����ֻ��8���ֽ�,����Ϊ9, ��Ҫ���������"\0"
	unsigned char use_db;
	unsigned short total_seq;
		
	unsigned char rfu[64];
}__attribute__((packed));


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
#define UNIV_DIV_SEQ_BC				0xffff  //�㲥���ļ�
#define UNIV_DIV_SEQ_ZERO			0x0000
#define UNIV_DIV_SEQ_ONE			0x0001
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
 #define UNIV_ACK_OK			0x00	//�ɹ�
 #define UNIV_ACK_RESEND		0xff	//�ط���ǰ����
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


typedef enum UNIV_FW_STAT
{
	UNIV_FW_STAT_INIT = 0,
	UNIV_FW_STAT_CHECK,
	UNIV_FW_STAT_RECV,
	UNIV_FW_STAT_SEND,
	UNIV_FW_STAT_END
}UNIV_FW_STAT_EU;

struct univ_fw_stat
{
	unsigned char stat:3;
	unsigned char rfu:5;
	unsigned char rsv;
	unsigned short file_seq;
}__attribute__((packed));
typedef struct univ_fw_stat univ_fw_stat_t;



int univ_file_config_init(void);
void univ_file_config_destroy(void);
int univ_file_config_put(univ_file_config_t *conf);
void univ_file_config_get(univ_file_config_t *conf);
void univ_file_config_printf(void);


void univ_file_hd_put(univ_file_hd_t *hd, const unsigned char type);
void univ_file_hd_get(univ_file_hd_t *hd, const unsigned char type);
void univ_file_hd_clean(const unsigned char type);

void univ_file_n4_to_str(unsigned char s_n4[9], univ_file_st_t *univ);
void univ_file_vd4_to_str(unsigned char s_vd4[9], univ_file_st_t *univ);

void univ_hd_n4_to_str(unsigned char s_n4[9], univ_file_hd_t *hd);
void univ_hd_vd4_to_str(unsigned char s_vd4[9], univ_file_hd_t *hd);




/* 1:�� -1:�� 0:��ͬ */
int univ_file_version_compare(const unsigned char s_new[4], const unsigned char s_old[4]);











void univ_fw_stat_put(univ_fw_stat_t *stat);
void univ_fw_stat_get(univ_fw_stat_t *stat);
unsigned int univ_fw_get_stat(void);
void univ_fw_set_stat(const unsigned char stat);
unsigned short univ_fw_get_file_seq(void);
void univ_fw_set_file_seq(const unsigned short file_seq);







#endif

