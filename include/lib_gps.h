#ifndef __LIB_GPS_H__
#define __LIB_GPS_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


#include <time.h>

/*
 * ���������붨��
 */
#define LIB_GPS_NULL				(NULL) 
#define LIB_GPS_EOK					(0)  //����
#define LIB_GPS_ERROR				(-1) //����
#define LIB_GPS_ETIMEOUT			(-2) //��ʱ
#define LIB_GPS_EFULL				(-3) //��
#define LIB_GPS_EEMPTY				(-4) //��
#define LIB_GPS_ENOMEM 			(-5) //�ڴ治��
#define LIB_GPS_EXCMEM				(-6) //�ڴ�Խ��
#define LIB_GPS_EBUSY				(-7) //æ
#define LIB_GPS_ERR_COMMAND		(-8) //��֧�ָ�ָ��

#define LIB_GPS_TRUE				(1)
#define LIB_GPS_FALSE				(0)



struct gpgga
{
	time_t ga_time;      //��ȡ��Ϣʱ��
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
	time_t sa_time;      //��ȡ��Ϣʱ��

	
};

struct gpgsv
{
	time_t sv_time;      //��ȡ��Ϣʱ��

	
};

struct gprmc
{
	time_t mc_time;      //��ȡ��Ϣʱ��

	
};

struct gpvtg
{
	time_t get_time;      //��ȡ��Ϣʱ��
	
};

struct gpgll
{
	time_t ll_time;      //��ȡ��Ϣʱ��


};

struct gpzda
{
	time_t la_time;      //��ȡ��Ϣʱ��
	
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



