#ifndef __GPS_H__
#define __GPS_H__



/*
 * 函数返回码定义
 */
#define GPS_NULL				(NULL) 
#define GPS_EOK				(0)  //正常
#define GPS_ERROR			(-1) //错误
#define GPS_ETIMEOUT			(-2) //超时
#define GPS_EFULL				(-3) //满
#define GPS_EEMPTY			(-4) //空
#define GPS_ENOMEM 			(-5) //内存不够
#define GPS_EXCMEM			(-6) //内存越界
#define GPS_EBUSY				(-7) //忙

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
	int tm_sec;       /* 秒 – 取值区间为[0,59] */
	int tm_min;       /* 分 - 取值区间为[0,59] */
	int tm_hour;      /* 时 - 取值区间为[0,23] */
	int tm_mday;      /* 一个月中的日期 - 取值区间为[1,31] */
	int tm_mon;       /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
	int tm_year;      /* 年份，其值等于实际年份减去1900 */
	int tm_wday;      /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	int tm_yday;      /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
	
	int tm_isdst;   /*夏令时标识符，使用夏令时，tm_isdst为正，不使用夏令时，tm_isdst为0，不了解情况时，tm_isdst为负*/ 
};
typedef struct gps_tm gps_tm_t;

struct gps_gga
{
	double latitude;  				//纬度，格式为 ddmm.mmmmm（度分格式）
	unsigned char ns_indicator;  		//纬度半球，N 或 S（北纬或南纬）
	double longitude; 				//经度，格式为 dddmm.mmmmm（度分格式）
	unsigned char ew_indicator; 		//经度半球，E 或 W（东经或西经）
	int status; 					// GPS 状态，0=未定位，1=非差分定位，2=差分定位  
	int satellite; 					//正在使用的用于定位的卫星数量（00~12） 
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

