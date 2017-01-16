#ifndef __GPS_H__
#define __GPS_H__



/*
 * ���������붨��
 */
#define GPS_NULL				(NULL) 
#define GPS_EOK				(0)  //����
#define GPS_ERROR			(-1) //����
#define GPS_ETIMEOUT			(-2) //��ʱ
#define GPS_EFULL				(-3) //��
#define GPS_EEMPTY			(-4) //��
#define GPS_ENOMEM 			(-5) //�ڴ治��
#define GPS_EXCMEM			(-6) //�ڴ�Խ��
#define GPS_EBUSY				(-7) //æ

#define GPS_TRUE				(1)
#define GPS_FALSE				(0)


#define GPS_PATHNAME			"/dev/ttyO4"
#define GPS_BAUDRATE			9600
#define GPS_MS_RATE				5000


struct gps_config
{
	int ms_rate;
	char pathname[32];
	unsigned int baudrate;
};

struct gps_attr
{
	unsigned char gga:1;
	unsigned char tm:1;
	unsigned char rfu:6;
}__attribute__((packed));
typedef struct gps_attr gps_attr_t;

struct gps_zda
{
	int  msec;
	int sec;     
	int min;     
	int hour;    
	short day;     
	short mon;     
	short year; 
};
typedef struct gps_zda gps_zda_t;

struct gps_tm 
{
	int tm_sec;       /* �� �C ȡֵ����Ϊ[0,59] */
	int tm_min;       /* �� - ȡֵ����Ϊ[0,59] */
	int tm_hour;      /* ʱ - ȡֵ����Ϊ[0,23] */
	int tm_mday;      /* һ�����е����� - ȡֵ����Ϊ[1,31] */
	int tm_mon;       /* �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11] */
	int tm_year;      /* ��ݣ���ֵ����ʵ����ݼ�ȥ1900 */
	int tm_wday;      /* ���� �C ȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ����� */
	int tm_yday;      /* ��ÿ���1��1�տ�ʼ������ �C ȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ����� */
	
	int tm_isdst;   /*����ʱ��ʶ����ʹ������ʱ��tm_isdstΪ������ʹ������ʱ��tm_isdstΪ0�����˽����ʱ��tm_isdstΪ��*/ 
};
typedef struct gps_tm gps_tm_t;

struct gps_gga
{
	double latitude;  				//γ�ȣ���ʽΪ ddmm.mmmmm���ȷָ�ʽ��
	unsigned char ns_indicator;  		//γ�Ȱ���N �� S����γ����γ��
	double longitude; 				//���ȣ���ʽΪ dddmm.mmmmm���ȷָ�ʽ��
	unsigned char ew_indicator; 		//���Ȱ���E �� W��������������
	int status; 					// GPS ״̬��0=δ��λ��1=�ǲ�ֶ�λ��2=��ֶ�λ  
	int satellite; 					//����ʹ�õ����ڶ�λ������������00~12�� 
};
typedef struct gps_gga gps_gga_t;

typedef struct gps_handle gps_handle_t;


gps_handle_t *gps_create(struct gps_config *config);
int gps_destroy(gps_handle_t *hndl);
int gps_get_fd(gps_handle_t *hndl);
int gps_get_info(char *string, gps_gga_t *gga, gps_tm_t *tm, gps_attr_t *attr);
void gps_show_zda(struct gps_zda *zda);
void gps_show_tm(struct gps_tm *tm);
void gps_show_gga(struct gps_gga *gga);














#endif

