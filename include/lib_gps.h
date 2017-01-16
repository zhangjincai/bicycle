#ifndef __LIB_GPS_H__
#define __LIB_GPS_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


#include <time.h>

/*
 * 函数返回码定义
 */
#define LIB_GPS_NULL				(NULL) 
#define LIB_GPS_EOK					(0)  //正常
#define LIB_GPS_ERROR				(-1) //错误
#define LIB_GPS_ETIMEOUT			(-2) //超时
#define LIB_GPS_EFULL				(-3) //满
#define LIB_GPS_EEMPTY				(-4) //空
#define LIB_GPS_ENOMEM 			(-5) //内存不够
#define LIB_GPS_EXCMEM				(-6) //内存越界
#define LIB_GPS_EBUSY				(-7) //忙
#define LIB_GPS_ERR_COMMAND		(-8) //不支持该指令

#define LIB_GPS_TRUE				(1)
#define LIB_GPS_FALSE				(0)



struct gpgga
{
	time_t ga_time;      //获取信息时间
	unsigned char location_stat;
	struct tm utc_time;
	double longitude;
	double latiiude;
	unsigned char satellite;
	double altitude;
	double geoid;  	
};

struct gpgsa
{
	time_t sa_time;      //获取信息时间

	
};

struct gpgsv
{
	time_t sv_time;      //获取信息时间

	
};

struct gprmc
{
	time_t mc_time;      //获取信息时间

	
};

struct gpvtg
{
	time_t get_time;      //获取信息时间
	
};

struct gpgll
{
	time_t ll_time;      //获取信息时间


};

struct gpzda
{
	time_t la_time;      //获取信息时间
	
};

struct gps_info
{
	struct gpgga ga;
	struct gpgsa sa;
	struct gpgsv sv;
	struct gprmc mc;
	struct gpvtg tg;
	struct gpgll ll;
	struct gpzda da;
}__attribute__((packed));
typedef struct gps_info gps_info_t;



int lib_gps_init(void);
void lib_gps_release(void);
int lib_gps_reconnect(void);
int lib_gps_version(char *ver, const unsigned int msec);
int lib_gps_gpgga_get(struct gpgga *ga, const unsigned int msec);
int lib_gps_gpgsa_get(struct gpgsa *sa, const unsigned int msec);
int lib_gps_gpgsv_get(struct gpgsv *sv, const unsigned int msec);
int lib_gps_gprmc_get(struct gprmc *mc, const unsigned int msec);
int lib_gps_gpvtg_get(struct gpvtg *tg, const unsigned int msec);
int lib_gps_gpgll_get(struct gpgll *ll, const unsigned int msec);
int lib_gps_gpzda_get(struct gpzda *da, const unsigned int msec);
int lib_gps_all_info_get(struct gps_info *info, const unsigned int msec);


/*@*/
#ifdef __cplusplus
}
#endif
#endif



