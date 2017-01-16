#ifndef __DEFINES_H__
#define __DEFINES_H__

/*
 * ���������붨��
 */
#define B_DEV_NULL					(NULL) 
#define B_DEV_EOK					(0)  //����
#define B_DEV_ERROR					(-1) //����
#define B_DEV_ETIMEOUT				(-2) //��ʱ
#define B_DEV_EFULL					(-3) //��
#define B_DEV_EEMPTY				(-4) //��
#define B_DEV_ENOMEM 				(-5) //�ڴ治��
#define B_DEV_EXCMEM				(-6) //�ڴ�Խ��
#define B_DEV_EBUSY					(-7) //æ
#define B_DEV_NOT_SUPPORT			(-8) //��֧�ָ�ָ��

#define B_DEV_TRUE					(1)
#define B_DEV_FALSE					(0)

#define B_DEV_CHAR_MAX				0xff
#define B_DEV_SHORT_MAX			0xffff
#define B_DEV_INT_MAX				0xffffffff


#define FN_SEL_TRUE					(1)
#define FN_SEL_FLASE					(0)


/* ���ջ�������С */
#define B_DEV_RXBUF_SZ				(2048)


/* ��־���� */
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
 * ***************************************  ׮������ **************************************
 */
#define SAE_HD_H						0x55
#define SAE_HD_L						0xAA

#define SAE_CAN_LOCAL_ID			0x00	//CAN ����ID
#define SAE_CAN_BROADCAST_ID		0x7F	//CAN �㲥ID

#define SAE_CAN_BUS_0				0	//CAN ����0
#define SAE_CAB_BUS_1				1	//CAN ����1

#define SAE_DEV_REG_REQ				0x81	//�豸ǩ������
#define SAE_DEV_REG_ACK				0x91	//�豸ǩ��ȷ��
#define SAE_DEV_CTRL_REQ			0x82	//�豸��������
#define SAE_DEV_CTRL_ACK			0x92	//�豸����ȷ��
#define SAE_FILE_TRANS_REQ			0x83	//���ļ���������
#define SAE_FILE_TRANS_ACK			0x93	//���ļ�����ȷ��
#define SAE_PASS_REQ				0x84 //����͸��
#define SAE_PASS_ACK				0x94



/*
 *  ��׮Э��������
 */
#define SAE_REG_W					"aa"		//��׮����ǩ���ͽڵ��Ӧ��
#define SAE_N_REG_W					"ab"		//�ڵ������ǩ����׮��Ӧ��
#define SAE_N_BHEART_W				"ac"		//�ڵ������������׮������Ӧ��
#define SAE_ID_W						"ad"		//��׮���
#define SAE_VER_W					"ae"		//��׮�汾��
#define SAE_LOCK_ID_W				"af"		//���ر��
#define SAE_LOCK_VER_W				"ag"		//���ذ汾��
#define SAE_PSAM_W					"ah"		//PSAM�����
#define SAE_SN_W					"ba"		//��׮��ˮ��
#define SAE_TOTAL_BLK_VER_W		"bb"		//�����������汾
#define SAE_INC_BLK_VER_W			"bc"		//�����������汾
#define SAE_DEC_BLK_VER_W			"bd"		//�����������汾
#define SAE_TEMP_BLK_VER_W			"be"		//��ʱ�������汾
#define SAE_TIME_W					"bf"		//׮��ʱ��
#define SAE_STATUS_W				"AA"		//׮��״̬
#define SAE_N_ID_W					"bg"		//�ڵ�����
#define SAE_N_STATUS_W				"AB"		//�ڵ��״̬
#define SAE_N_TIME_W				"bh"		//�ڵ��ʱ��
#define SAE_N_MAC_W					"bi"          //�ڵ��MAC��ַ
#define SAE_PHY_SN_W				"bj"		//��׮������
#define SAE_CTRL_C					"AA"		//׮������
#define SAE_PARA_VER_W				"bk"         //��׮���ð汾
#define SAE_QUANTITY_W				"bl"		//��׮����

/*
 * ��׮���Ľṹ
 */
 #define SAE_FRAME_MAX_SZ			(272)	//Ӧ������֡�ĳ���Ϊ256
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

//ָ�뷽ʽ
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
 * ��׮״̬
 */
struct sae_status
{
	unsigned short in:1;		//�����ջؼ��, ��ʾ�޳�
	unsigned short out:1;		//����������, ��ʾ�г�
	unsigned short bike:1;		//�Ƿ��г�
	unsigned short op:1;		//�����ɹ�
	unsigned short rc531:1;		//RC531��Ӧ
	unsigned short lock:1;		//��������Ӧ
	unsigned short time:1;		//ʱ��ͬ��״̬
	unsigned short volt:1;		//��Դ��ѹ״̬
	unsigned short rtc:1;		//RTC״̬
	unsigned short can:1;		//CAN����״̬
	unsigned short rfu:6;		//����
}__attribute__((packed));
typedef struct sae_status sae_status_t;

/*
 * ��׮��Ϣ
 */
struct sae_info
{
	unsigned char healthy_flag;			//��־ 1:���� 0:��Ч
	unsigned char send_hb_times;		//��������ʱ��һ,���ճɹ����һ	
	unsigned char id;					//��׮���-- ʮ����
	unsigned short status;				//׮��״̬
	unsigned char s_psam[7];			//PSAM�����
	unsigned char s_id[4];				//��׮���
	unsigned char s_ver[9];				//��׮�汾��
	unsigned char s_lock_id[4];			//���ر��
	unsigned char s_lock_ver[9];			//���ذ汾��
	unsigned char s_sn[7];				//��׮��ˮ��
	unsigned char s_total_bl_ver[9];		//�����������汾
	unsigned char s_inc_bl_ver[9];		//�����������汾
	unsigned char s_dec_bl_ver[9];		//�����������汾
	unsigned char s_temp_bl_ver[9];		//��ʱ�������汾
	unsigned char s_stake_para[9];           //��׮����
	unsigned char s_unionpay_key[9];     //��������
	unsigned char s_time[13]; 			//��׮ʱ��
	unsigned int s_reg_time;			//ǩ��ʱ��
	unsigned char s_phy_sn[9];			//��׮������
}__attribute__((packed));
typedef struct sae_info sae_info_t;

/*
 * ׮������������
 */
struct sae_blk_attr
{
	unsigned char temp_co:1;  //һ����
	unsigned char dec_co:1;
	unsigned char inc_co:1;
	unsigned char total_co:1;
	
	unsigned char temp_va:1;  //��Ч��
	unsigned char dec_va:1;
	unsigned char inc_va:1;
	unsigned char total_va:1;
}__attribute__((packed));
typedef struct sae_blk_attr sae_blk_attr_t;

/*
 * ׮������
 */
struct sae_attribute
{
	unsigned char id;					//׮�����
	unsigned char physn[4];				//׮��������
	unsigned short sn;					//׮��������ˮ��
	unsigned char psam[4];				//PSAM�����
	unsigned char version[4];			//׮���汾��
	unsigned char time[6];				//׮��ʱ��
	unsigned char register_time[6];		//׮��ע��ʱ��
	unsigned char lock_id;				//���ر��
	unsigned char lock_version[4];		//���ذ汾��
	unsigned char total_blk_ver[4];		//�����������汾��
	unsigned char inc_blk_ver[4];		//�����������汾��
	unsigned char dec_blk_ver[4];		//�����������汾��
	unsigned char temp_blk_ver[4];		//��ʱ�������汾��
	struct sae_status stat;				//׮��״̬
}__attribute__((packed));
typedef  struct sae_attribute sae_attribute_t;


/*
 * ׮������ [AA]
 */
 struct sae_control  
{
	/* ��������λ : 0��ֹ, 1���� */
	unsigned int f_op:1;		//����/��ֹ׮���軹��
	unsigned int f_reboot:1;       //׮������
	unsigned int f_cpu:1;		//CPUǮ��
	unsigned int f_cpu_to_m1:1; //CPUģ��M1Ǯ��
	unsigned int f_m1:1;		//M1Ǯ��
	unsigned int f_lock:2;		//��������
	unsigned int f_led:2;		//״̬�ƿ���
	unsigned int f_rfu:7;		//����

	/* ��������λ : 0��ֹ, 1���� */
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
 * ***************************************  ���Ĳ��� **************************************
 */
#define NDEV_HD					0xF1
#define NDEV_TAIL				0xF2
#define NDEV_ESC_F0H			0xF0	//ת���ַ�
#define NDEV_ESC_00H			0x00
#define NDEV_ESC_01H			0x01
#define NDEV_ESC_02H			0x02

#define UFC_NULL					0
#define REG_NO					0
#define REG_OK					1

#define NDEV_REGISTER_REQ		0xA1	//�豸ǩ��
#define NDEV_REGISTER_ACK		0xB1
#define NDEV_CTRL_REQ			0xA2	//�豸����
#define NDEV_CTRL_ACK			0xB2
#define NDEV_FILE_TRANS_REQ	0xA3	//���ļ�����
#define NDEV_FILE_TRANS_ACK	0xB3
#define NDEV_LOAD_REQ			0xA4	//���ؾ�������
#define NDEV_LOAD_ACK			0xB4
#define NDEV_PASS_REQ			0xA5  //����͸��
#define NDEV_PASS_ACK			0xB5 


#define KERNEL_KEY					"kernel_"
#define FIRMWARE_KEY				"fw_"
#define APP_KEY						"app_"

#define SQLITE3_DB_PATHNAME			"/opt/universal/database.db"
#define UNITY_FILE_DB_PATHNAME		"/opt/universal/unity_file.db"


/*
 *  Э��������
 */
#define NDEV_LOAD_W					"DP"		//���ؾ���
#define NDEV_REG_W						"AN"		//�豸ע��
#define NDEV_STAT_W						"AB"		//�ڵ��״̬
#define NDEV_N_TIME_W					"bh"		//�ڵ��ʱ��
#define NDEV_S_TOTAL_NUM_W			"ai"		//׮��������
#define NDEV_S_ONLINE_NUM_W			"aj"		//׮����������
#define NDEV_TOTAL_BLK_VER_W			"bb"		//�����������汾
#define NDEV_INC_BLK_VER_W				"bc"		//�����������汾
#define NDEV_DEC_BLK_VER_W				"bd"		//�����������汾
#define NDEV_TEMP_BLK_VER_W			"be"		//��ʱ�������汾
#define NDEV_S_BLK_ATTR_W				"AL"		//׮������������
#define NDEV_S_ATTR_W					"AM"		//׮������
#define NDEV_YCT_STAT_W				"AK"		//���ͨ������״̬
#define NDEV_BTHEART_RETURN_W			"al"		//������Ӧ
#define NDEV_S_ALL_STAT					"AA"         //ȫ����׮״̬
#define NDEV_S_ONLINE_F					"AC"		//׮�����߱�־
#define NDEV_FW_VER_W					"ao"        //�ڵ����汾
#define NDEV_APPL_VER_W				"ap"        //�ڵ���̼��汾
#define NDEV_FTP_DNLS_W				"BR"		//�ڵ��FTP����״̬
#define NDEV_SAE_VER_W					"aq"         //׮���̼��汾


#define NDEV_LOAD_SERV_IP_SET_W		"BA"		//���ø��ط�����IP
#define NDEV_LOAD_SERV_PORT_SET_W		"BB"		//���ø��ط������˿�
#define NDEV_LOAD_SERV2_IP_SET_W		"BC"		//���ñ��ݸ��ط�����IP
#define NDEV_LOAD_SERV2_PORT_SET_W	"BD"		//���ñ��ݸ��ط������˿�
#define NDEV_FTP_SET_W					"BE"		//FTP����
#define NDEV_DEVICE_REBOOT_W			"BM"		//�豸����	
#define NDEV_S_LOCK_OP_W				"BO"		//��������/����
#define NDEV_BTHEART_TIME_SET_W		"BF"		//��������ʱ��
#define NDEV_SITE_NAME_W				"BQ"        //�·�վ������
#define NDEV_UNIV_UPDATE_W				"BP"		//���·��ļ�
#define NDEV_EXCEPT_HANDLE_W			"BU"		//�쳣����
#define NDEV_FTP_DNL_W					"BR"		//�ڵ��FTP�̼�����
#define NDEV_UPDATE_CTRL_W				"BS"          //�ڵ���̼����¿���
#define NDEV_RENT_INFO_W				"BV"		//�⻹����Ϣ
#define NDEV_GET_S_ATTR_W				"BT"		//��ȡ׮����
#define NDEV_GET_ALL_SAE_PHY_INFO_W	"BW"       //ȫ����׮������Ϣ
#define NDEV_GET_GPS_INFO_W			"by"		//��ȡgps��Ϣ
#define NDEV_SET_LIGHT_CTRL_TIME_W		"BX"		//���俪��ʱ��
#define NDEV_SAE_PARA_VER_W			"br"     	 //��׮�����汾��
#define NDEV_SITE_QR_CODE_W			"ca"		//�����ά��

#define NDEV_LNT_FIRMWARE_VER_W		"cd"	//����ͨ�������̼��汾
#define NDEV_NEARBY_SITE_INFO_W		"BZ"		//����������Ϣ add by zjt at 2016-11-03




/* ��������״̬ */
typedef enum NDEV_NETSTAT
{
	NDEV_NETSTAT_INIT = 0,				//��ʼ�� 
	NDEV_NETSTAT_CONNECT,				//�ɹ���������
	NDEV_NETSTAT_REGISTERED,			//�ɹ�ע�������
	NDEV_NETSTAT_SESSION,				//���ڻػ�
	NDEV_NETSTAT_UNKNOWN				// δ֪״̬
}NDEV_NETSTAT_EU;

/* �¼�֪ͨ */
typedef enum NDEV_NOTIFY
{
	NDEV_NOTIFY_INIT = 0,	
	NDEV_NOTIFY_RE_REGISTER,		//����ע��
	NDEV_NOTIFY_REGISTERED,		//�Ѿ�ע��
	NDEV_NOTIFY_RE_BTHEART,         //���·�����
	NDEV_NOTIFY_BTHEART,			//�������ճɹ�
	NDEV_NOTIFY_TOTAL_BLK,
	NDEV_NOTIFY_INC_BLK,
	NDEV_NOTIFY_DEC_BLK,
	NDEV_NOTIFY_TEMP_BLK,
	NDEV_NOTIFY_UNIV_UPDATE,      //���ļ�����
	NDEV_NOTIFY_UNIV_CONTINUE,  //���ļ���������
	NDEV_NOTIFY_UNIV_QUIT,         //���ļ������˳�
	NDEV_NOTIFY_UNIV_REMOVE, //���ļ��Ƴ�
	NDEV_NOTIFY_UNKNOWN		
}NDEV_NOTIFY_EU;

/* ���ؾ�������״̬ */
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
 * �ڵ�����Ľṹ
 */
#define NDEV_DATA_SZ					B_DEV_RXBUF_SZ  //512->B_DEV_RXBUF_SZ
#define NDEV_FRAME_MAX_SZ				(B_DEV_RXBUF_SZ - 128)		//Ӧ������֡�ĳ���Ϊ320->B_DEV_RXBUF_SZ
#define NDEV_ATTR_BIT0_PASS(c)			((c) | (1 << 0))
#define NDEV_ATTR_BIT0_NOT_PASS(c)		((c) &= ~(1 << 0))
#define NDEV_ATTR_BIT1_RECV(c)			((c) | (1 << 1))
#define NDEV_ATTR_BIT1_NOT_RECV(c)		((c) &= ~(1 << 1))
struct n_struct_buf
{
	unsigned char hd;
	unsigned char sn;
	unsigned short term_id;
	unsigned char dev_addr;  //�豸��ַ
	unsigned char data_attr; //��������
	unsigned char cmd; 	//���������
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

/* ָ�뷽ʽ */
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
 * ���ؾ���
 */

struct load_balance_req
{
	unsigned char mac_addr[6];		//MAC��ַ
	unsigned short term_id;			//�ն˱���
	unsigned short operators;     	//��Ӫ��
	unsigned short dev_ver;  		//�豸����
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
 * �豸ǩ��
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
 * FTP����������
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
 * �쳣����
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
 * ������������ //�Ƿ�Ҫ���͵ı�־
 */
struct ndev_bheart_limit
{
	unsigned int ndev_stat:1;    //�ڵ��״̬
	unsigned int ndev_time:1;       //�ڵ��ʱ��
	unsigned int sae_quantity:1;  //׮��������
	unsigned int sae_online:1;	//׮����������
	unsigned int total_bl_ver:1;	//�����������汾
	unsigned int inc_bl_ver:1;	//�����������汾
	unsigned int dec_bl_ver:1;	//�����������汾
	unsigned int temp_bl_ver:1;	//��ʱ�������汾
	unsigned int sae_bl_attr:1;	//׮������������
	unsigned int sae_attr:1;		//׮������
	unsigned int lnt_card_stat:1;	//���ͨ������״̬
	unsigned int all_sae_stat:1;		//ȫ��׮��״̬
	unsigned int sae_online_mark:1;	//׮�����߱�־
	unsigned int ndev_fw_ver:1;		//�ڵ����汾
	unsigned int ndev_app_ver:1;	//�ڵ��Ӧ�ð汾
	unsigned int sae_fw_ver:1;		//׮���̼��汾
	unsigned int ndev_ftp_stat:1;	//�ڵ��FTP����״̬
	unsigned int sae_para_ver:1; 	//��׮�����汾
	
	unsigned int lnt_reader_firmware_ver:1; //����ͨ�������̼��汾 add by zjc CONFS_USING_UPLOAD_READER_VER
	unsigned int rfu:13;		//���� 14-1
}__attribute__((packed));
typedef struct ndev_bheart_limit ndev_bheart_limit_t;





/*
 * �ڵ��״̬
 */
struct ndev_status
{
	unsigned short power:1;				//��Դ��ѹ״̬
	unsigned short rtc:1;				//RTC״̬
	unsigned short can:1;				//CAN����״̬
	unsigned short center:1;				//������ͨѶ״̬
	unsigned short wireless:1;			// 3G/4G����״̬
	unsigned short yct:1;				//���ͨ������
	unsigned short gps:1;				//GPS
	unsigned short keyboard:1;			//���̰�״̬
	unsigned short ext_mcu:1;			//����CPU��״̬
	unsigned short time:1;				//ʱ��ͬ��״̬
	unsigned short rfu:6;				//����
}__attribute__((packed));
typedef struct ndev_status ndev_status_t;


struct ndev_info
{
	NDEV_NETSTAT_EU netstat;
	struct ndev_status stat;
	struct load_balance_ack load;
	char center_ip[32];
	unsigned short center_port;
	char terminal_name[50];    //�ն�����
	unsigned short terminal_no; //�ն˱��
	unsigned long register_time;  //ע��ʱ��
	unsigned char network_type; //��������
};
typedef struct ndev_info ndev_info_t;


/*
 * ����ʹ�ü�¼
 */
struct admin_card_info_req
{
	unsigned int sn;			//��ˮ��
	unsigned char pid[8];              //������
	unsigned char ticket[42];		//Ʊ����Ϣ
	unsigned char rentinfo[36];	//�⻹����¼
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
 * �쳣����۷ѽ��׼�¼
 */
 struct exception_handle_record_req
{
	unsigned int sn;			//��ˮ��
	unsigned char pid[8];              //������
	unsigned char record[90];      //���׼�¼
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
 * �쳣����
 */
struct exception_handle_req
{
	unsigned int sn;
	unsigned char pid[8];              //������
	unsigned char ticket[42];		//Ʊ����Ϣ
	unsigned char rentinfo[36];	//�⻹����¼	
}__attribute__((packed));
typedef struct exception_handle_req exception_handle_req_t;

struct exception_handle_ack
{
	unsigned int sn;
	unsigned char pid[8];
	unsigned char result;	
	unsigned int fee; //�۷�
	unsigned char info[48];    //��ʾ��Ϣ
}__attribute__((packed));
typedef struct exception_handle_ack exception_handle_ack_t;


/*
 * FTP�̼�����
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
	int isvaild;  //�Ƿ���Ч
	int ftype;   //�̼�����
	unsigned int ftime;
	
	struct ndev_ftp_download_ctrl_ack fdnl_ctrl;
}__attribute__((packed));
typedef struct ndev_ftp_download_ctrl_info ndev_ftp_download_ctrl_info_t;


/*
 * �̼����¿���
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
 * ��Ա����
 */
 struct ndev_member_apply_req
{
	unsigned int sn;				//��ˮ��
	unsigned char apply_type;		//��������
	unsigned char id_number[18];	//���֤
	unsigned char phone_number[11];	//�ֻ�����
	unsigned char apply_member_type; //�����Ա����
	unsigned char pid[8];				//Ʊ��������
	unsigned char ticket_info[42];		//Ʊ����Ϣ
	unsigned char rent_info[36];			//�⻹����Ϣ
	unsigned char time[6];				//ʱ��
}__attribute__((packed));
typedef  struct ndev_member_apply_req ndev_member_apply_req_t;

struct ndev_member_apply_ack
{
	unsigned int sn;				//��ˮ��
	unsigned char apply_type;		//��������
	unsigned char pid[8];			//Ʊ��������
	unsigned int deposit;			//Ӧ��Ѻ��
	unsigned char result;			//������
	unsigned char time[6];			//ʱ��
	unsigned char showinfo[48];  		//��ʾ��Ϣ
}__attribute__((packed));
typedef  struct ndev_member_apply_ack ndev_member_apply_ack_t;

/*
 * ��Ա����ر�
 */
struct ndev_member_handle_req
{
	unsigned int sn;				//��ˮ��
	unsigned char apply_type;		//��������
	unsigned char handle_type;		//�ر�����
	unsigned char pid[8];			//Ʊ��������
	unsigned char result;			//������
	unsigned char time[6];			//ʱ��	
	unsigned char errcode[12];		//�������
}__attribute__((packed));
typedef struct ndev_member_handle_req ndev_member_handle_req_t;

struct ndev_member_handle_ack
{
	unsigned int sn;				//��ˮ��
	unsigned char apply_type;		//��������
	unsigned char handle_type;		//�ر�����
	unsigned char pid[8];			//Ʊ��������
	unsigned char result;			//������
	unsigned char time[6];			//ʱ��	
}__attribute__((packed));
typedef struct ndev_member_handle_ack ndev_member_handle_ack_t;

/*
 * �������ѽ��׼�¼
 */
struct ndev_apply_consume_req
{
	unsigned int sn;			//��ˮ��
	unsigned char pid[8];		//Ʊ��������
	unsigned char record[90];	//���׼�¼
}__attribute__((packed));
typedef struct ndev_apply_consume_req ndev_apply_consume_req_t;

struct ndev_apply_consume_ack
{
	unsigned int sn;		//��ˮ��
	unsigned char pid[8];	//Ʊ��������
	unsigned char result;	//���
}__attribute__((packed));
typedef struct ndev_apply_consume_ack ndev_apply_consume_ack_t;


/*
 * �첽��������
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
 * �⻹����¼��ѯ
 */
struct ndev_rent_info_qry_req
{
	unsigned char pid[8];		//Ʊ��������
	unsigned char item;       //��¼����
}__attribute__((packed));
typedef struct ndev_rent_info_qry_req ndev_rent_info_qry_req_t;

struct ndev_rent_info_qry_ack
{
	unsigned char pid[8];		//Ʊ��������
	unsigned char item;       		//��¼����
	unsigned short length;   //��¼����
	unsigned char record[2048];  //��¼
}__attribute__((packed));
typedef struct ndev_rent_info_qry_ack ndev_rent_info_qry_ack_t;

struct rent_info_fmt		//���ü�¼��ʽ
{ 
	unsigned char bike_NO[32];		//���г����
	unsigned char rent_name[64];	//�⳵������
	unsigned char rent_time[32];		//�⳵ʱ��
	unsigned char return_name[64];	//����������
	unsigned char return_time[32];	//����ʱ��
	unsigned char used_time[16];		//����ʱ��
	unsigned char fee[16];			//�۷�
}__attribute__((packed));
typedef struct rent_info_fmt rent_info_fmt_t;


/*
 * gps��λ��Ϣ
 */
struct gps_all_info
{
	unsigned char vaild;                       //������Чλ
	unsigned char latitude[10];  		//γ�ȣ���ʽΪ ddmm.mmmmm���ȷָ�ʽ��
	unsigned char ns_indicator;  		//γ�Ȱ���N �� S����γ����γ��
	unsigned char longitude[10]; 		//���ȣ���ʽΪ dddmm.mmmmm���ȷָ�ʽ��
	unsigned char ew_indicator; 		//���Ȱ���E �� W��������������
	unsigned char status; 			// GPS ״̬��0=δ��λ��1=�ǲ�ֶ�λ��2=��ֶ�λ  
	unsigned char satellite[2]; 			//����ʹ�õ����ڶ�λ������������00~12�� 
	unsigned char times[7];                //GPSʱ��,  BCD, YYYYMMDDHHmmSS
}__attribute__((packed));
typedef struct gps_all_info gps_all_info_t;

struct gps_rxbuf
{
	int vaild;
	int size;
	unsigned char string[512];
}__attribute__((packed));


/*
 * ����ͨ��Ϣ
 */
struct lnt_all_info
{
	unsigned char lib_version[32];   //��汾
	unsigned char hw_version[32];  //�������汾

	unsigned int register_stat;   //ע��״̬
	unsigned char pki_r[4];  //PSAM����
	unsigned char psam[4];   //PKI
	
	unsigned int pkt_sn_RO; //�������к�
	
	unsigned char proc:4;     //ִ�в���
	unsigned char fresult:4;  //ִ�н��
	unsigned char stat;      	//����״̬
	unsigned short sw;  	//������
	time_t time;  		//ִ��ʱ��
	unsigned char agent_err; //���������
}__attribute__((packed));
typedef struct lnt_all_info lnt_all_info_t;

/*
 * �������ʱ��
 */ 
 struct light_ctrl_time
{
	unsigned char enable[2];
	unsigned char disable[2];
}__attribute__((packed));
typedef  struct light_ctrl_time light_ctrl_time_t;





/*
 * ��׮������ˮ�� �����桿
 */
static unsigned char g_sae_ctrl_sn = 1;


/*
 * ����������Ϣ��ѯ add by zjc at 2016-11-03
 */
struct ndev_nearby_site_info_qry_req
{
	unsigned char siteNo[2];		//�����ն˱��
	unsigned char gpsInfo[32];       //���㾭γ����Ϣ
}__attribute__((packed));
typedef struct ndev_nearby_site_info_qry_req ndev_nearby_site_info_qry_req_t;

struct ndev_nearby_site_info_qry_ack
{
	unsigned char siteNo[2];		//�����ն˱��
	unsigned char item;       		//������Ϣ����
	unsigned short length;   //������Ϣ����
	unsigned char info[2048];  //������Ϣ����
}__attribute__((packed));
typedef struct ndev_nearby_site_info_qry_ack ndev_nearby_site_info_qry_ack_t;

struct nearby_site_info_fmt		//����������Ϣ��ʽ
{ 
	unsigned char siteName[64];		//��������
	unsigned char distance[16];	//����
	unsigned char bikes[16];	//���г���
	unsigned char stacks[16];	//��׮��
}__attribute__((packed));
typedef struct nearby_site_info_fmt nearby_site_info_fmt_t;
/* end of ����������Ϣ��ѯ */

#endif


