#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib_general.h"
#include "gps.h"



struct gps_handle
{
	lib_serial_t serial;
	struct gps_config conf;
}__attribute__((packed));


static int __nmea_crc_check(unsigned char *c_crc, const unsigned int size);
static int __ubx_crc_calc(unsigned char *c_crc, const unsigned char *s, const unsigned int size);
static int __set_rate(const int fd, const unsigned int rate);
static int __gga_off(const int fd);
static int __gga_on(const int fd);
static int __gll_off(const int fd);
static int __gll_on(const int fd);
static int __gsa_off(const int fd);
static int __gsa_on(const int fd);
static int __gsv_off(const int fd);
static int __gsv_on(const int fd);
static int __gprmc_off(const int fd);
static int __gprmc_on(const int fd);
static int __vtg_off(const int fd);
static int __vtg_on(const int fd);
static int __zda_off(const int fd);
static int __zda_on(const int fd);



static int __nmea_crc_check(unsigned char *c_crc, const unsigned int size)
{
	if(c_crc == NULL)
		return GPS_FALSE;

	int i, a, b;
	unsigned char c_ck = 0;

	for(i = 0; i < size; i++)
	{
		c_ck ^= c_crc[i];
	}
	
	a = c_ck /16 + 48;
	b = c_ck % 16 + 48;
	if((b > 57) && (b < 65))
		b = b + 7;	
	
	if((c_crc[strlen(c_crc) - 2] != a) || (c_crc[strlen(c_crc) - 1] != b))
		return GPS_FALSE;

	return GPS_TRUE;
}

static int __ubx_crc_calc(unsigned char *c_crc, const unsigned char *s, const unsigned int size)
{
	if((c_crc == NULL) || (s == NULL))
		return GPS_ERROR;

	int i;
	
	for(i = 0; i < size; i++)
	{
		c_crc[0] += s[i];
		c_crc[1] += c_crc[0];
	}
	
	return GPS_EOK;
}

static int __set_rate(const int fd, const unsigned int rate)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x08;
	txbuf[4] = 0x06;
	txbuf[5] = 0x00;
	txbuf[6] = rate % 256;
	txbuf[7] = rate / 256;
	txbuf[8] = 0x01;
	txbuf[9] = 0x00;
	txbuf[10] = 0x01;
	txbuf[11] = 0x00;

	__ubx_crc_calc(&(txbuf[12]), &txbuf[2], 10);

	 return lib_serial_send(fd, txbuf, 14);
}


static int __gga_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x00;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	txbuf[14] = 0x00;
	txbuf[15] = 0x24;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gga_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x00;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	txbuf[14] = 0x01;
	txbuf[15] = 0x29;
	
	 return lib_serial_send(fd, txbuf, 16);
}

static int __gll_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x01;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	txbuf[14] = 0x01;
	txbuf[15] = 0x2b;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gll_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x01;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x02;
	txbuf[15] = 0x30;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gsa_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x02;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x02;
	txbuf[15] = 0x32;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gsa_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x02;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x02;
	txbuf[15] = 0x32;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gsv_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x03;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x03;
	txbuf[15] = 0x39;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gsv_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x03;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x04;
	txbuf[15] = 0x3e;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __gprmc_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x04;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x04;
	txbuf[15] = 0x40;
	
	 return lib_serial_send(fd, txbuf, 16);
}

static int __gprmc_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x04;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x05;
	txbuf[15] = 0x45;

	 return lib_serial_send(fd, txbuf, 16);
}

static int __vtg_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x05;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x05;
	txbuf[15] = 0x47;
	
	 return lib_serial_send(fd, txbuf, 16);
}

static int __vtg_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x05;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x06;
	txbuf[15] = 0x4c;
	
	 return lib_serial_send(fd, txbuf, 16);
}

static int __zda_off(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};

	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x08;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;

	__ubx_crc_calc(&txbuf[14], &txbuf[2], 12);
	
	 return lib_serial_send(fd, txbuf, 16);
}

static int __zda_on(const int fd)
{
	int ret = -1;

	unsigned char txbuf[128] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x08;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;

	__ubx_crc_calc(&txbuf[14], &txbuf[2], 12);
	
	 return lib_serial_send(fd, txbuf, 16);
}

static int __split_info(char *d_src,  char (*line)[512], int *times)
{
	if((d_src == NULL) || (line == NULL) ||(times == NULL))
		return GPS_ERROR;

	char *p = NULL;
	
	p = strtok(d_src,"\n");
	while(p)
	{
		strcpy(line[(*times)++], p);
		p = strtok(NULL, "\n");
	}
	
	return GPS_EOK;
}

static int __leap(int y)
{
	 int leapyear;
	 if (y%4!=0)
	     leapyear=0;
	 else if (y%100!=0)
	     leapyear=1;
	 else if (y%400!=0)
	     leapyear=0;
	 else
	     leapyear=1;
	 return(leapyear);
}

static int __monthday(int m, int team)
{
   switch(m)
  {case 1:return(31);
   case 3:return(31);
   case 5:return(31);
   case 7:return(31);
   case 8:return(31);
   case 10:return(31);
   case 12:return(31);
   case 4:return(30);
   case 6:return(30);
   case 9:return(30);
   case 11:return(30);
   case 2:{if (team==1) {return(29);} else if (team==0) {return(28);}}}  
} 

static int __get_yday(int year,int mon,int day)
{
	int d = 0, m;
	int team;
	team = __leap(year);
	for(m=1;m<mon;m++)
		d+=__monthday(m,team);
	
	day+=d;

	return day;	
}

static int __get_gga(char *rxbuf, struct gps_gga *gga)
{
	if((rxbuf == NULL) || (gga == NULL))
		return GPS_ERROR;

	int ret = -1;
	char *p = NULL;
	char buf[32] = {0};
	char* pEnd = NULL;
	double db2;
	int tmp;
	
	ret = __nmea_crc_check(&rxbuf[1], strlen(rxbuf)-4);
	if(ret < 0)
		return GPS_ERROR;

	p = strtok(rxbuf,",");
	if(p){
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;
	
	if(p){
			p = strtok(NULL,",");
		}else
			return GPS_ERROR;

	if(p){	
		sscanf(p, "%lf", &db2);
		gga->latitude=db2;
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		strncpy(&(gga->ns_indicator),p,1);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		sscanf(p, "%lf", &db2);
		gga->longitude=db2;
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		strncpy(&(gga->ew_indicator),p,1);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		sscanf(p, "%d", &gga->status);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		sscanf(p, "%d", &gga->satellite);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	return GPS_EOK;
}

static int __get_tm(char *rxbuf, struct gps_tm *tm)
{
	if((rxbuf == NULL) || (tm == NULL))
		return GPS_ERROR;	

	int ret = -1;
	char *p = NULL;
	char buf[32] = {0};
	
	ret = __nmea_crc_check(&rxbuf[1], strlen(rxbuf)-4);
	if(ret < 0)
		return GPS_ERROR;
	
	p = strtok(rxbuf,",");
	
	if(p){
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		memset(buf,'\0',sizeof(buf));
		memcpy(buf, p, 2);
		tm->tm_hour=atoi(buf)+8;
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+2, 2);
		tm->tm_min=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+4, 2);
		tm->tm_sec=atoi(buf);
		
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;
		
	if(p){
		tm->tm_mday=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		tm->tm_mon=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		tm->tm_year=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	tm->tm_yday = __get_yday(tm->tm_year,tm->tm_mon,tm->tm_mday);
	tm->tm_wday = (tm->tm_mday+2*tm->tm_mon+3*(tm->tm_mon+1)/5+tm->tm_year+tm->tm_year/4-tm->tm_year/100+tm->tm_year/400)%7+1;

	#if 0
	int tm_mon;       /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
	int tm_year;      /* 年份，其值等于实际年份减去1900 */
	int tm_yday;      /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */

	#endif
	
	tm->tm_yday  = tm->tm_yday  -1;
	tm->tm_mon = tm->tm_mon -1;
	tm->tm_year = tm->tm_year - 1900;

	return GPS_EOK;
}

static int __get_zda(char *rxbuf, struct gps_zda *zda)
{
	if((rxbuf == NULL) || (zda == NULL))
		return GPS_ERROR;	

	int ret = -1;
	char *p = NULL;
	char buf[32] = {0};
	ret = __nmea_crc_check(&rxbuf[1], strlen(rxbuf)-4);
	if(ret < 0)
		return GPS_ERROR;
	
	p = strtok(rxbuf,",");
	
	if(p){
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		memset(buf,'\0',sizeof(buf));
		memcpy(buf, p, 2);
		zda->hour=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+2, 2);
		zda->min=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+4, 2);
		zda->sec=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+7, 2);
		zda->msec=atoi(buf);
		
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;
		
	if(p){
		zda->day=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		zda->mon=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		zda->year=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;


	return GPS_EOK;
}


void gps_show_zda(struct gps_zda *zda)
{
	fprintf(stderr, "zda->msec = %d\n", zda->msec);
	fprintf(stderr,"zda->sec = %d\n", zda->sec);
	fprintf(stderr,"zda->min = %d\n", zda->min);
	fprintf(stderr,"zda->hour = %d\n", zda->hour);
	fprintf(stderr,"zda->day = %d\n", zda->day);
	fprintf(stderr,"zda->mon = %d\n", zda->mon);
	fprintf(stderr,"zda->year = %d\n", zda->year);	
}

void gps_show_tm(struct gps_tm *tm)
{
	fprintf(stderr,"tm->sec = %d\n", tm->tm_sec);
	fprintf(stderr,"tm->min = %d\n", tm->tm_min);
	fprintf(stderr,"tm->hour = %d\n", tm->tm_hour);
	fprintf(stderr,"tm->mday = %d\n", tm->tm_mday);
	fprintf(stderr,"tm->yday = %d\n", tm->tm_yday);
	fprintf(stderr,"tm->wday = %d\n", tm->tm_wday);
	fprintf(stderr,"tm->mon = %d\n", tm->tm_mon);
	fprintf(stderr,"tm->year = %d\n", tm->tm_year);	

	fprintf(stderr,"tm->tm_isdst = %d\n", tm->tm_isdst);	
}


void gps_show_gga(struct gps_gga *gga)
{
	fprintf(stderr,"gga->latitude = %.7f\n", gga->latitude);
	fprintf(stderr,"gga->ns_indicator = %c\n", gga->ns_indicator);
	fprintf(stderr,"gga->longitude = %.7f\n", gga->longitude);
	fprintf(stderr,"gga->ew_indicator = %c\n", gga->ew_indicator);
	fprintf(stderr,"gga->gps_status = %d\n", gga->status);
	fprintf(stderr,"gga->positioning_satellite = %d\n", gga->satellite);
}





gps_handle_t *gps_create(struct gps_config *config)
{
	int ret = LIB_GE_ERROR;
	gps_handle_t *hndl = NULL;

	hndl = (gps_handle_t *)malloc(sizeof(gps_handle_t));
	if(hndl == NULL)
		return GPS_NULL;

	memcpy(&(hndl->conf), config, sizeof(struct gps_config));
		
	strcpy(hndl->serial.pathname, config->pathname);
	hndl->serial.speed = config->baudrate;
	hndl->serial.flags = O_RDWR;
	hndl->serial.databits = 8;
	hndl->serial.stopbits = 1;

	ret = lib_serial_init(&(hndl->serial));
	if(ret != LIB_GE_EOK)
	{
		free(hndl);
		hndl = NULL;
		
		return GPS_NULL;
	}

	memcpy(&(hndl->conf), config, sizeof(struct gps_config));
	
	__set_rate(hndl->serial.sfd, config->ms_rate); //设置每秒发送一次
	__gga_on(hndl->serial.sfd);
	__gll_off(hndl->serial.sfd);
	__gsa_off(hndl->serial.sfd);
	__gsv_off(hndl->serial.sfd);
	__gprmc_off(hndl->serial.sfd);
	__vtg_off(hndl->serial.sfd);
	__zda_on(hndl->serial.sfd);


	return hndl;
}

int gps_destroy(gps_handle_t *hndl)
{
	if(hndl == NULL)
		return GPS_ERROR;

	lib_serial_close(&(hndl->serial));

	free(hndl);
	hndl = NULL;

	return GPS_EOK;
}

int gps_get_fd(gps_handle_t *hndl)
{
	if(hndl == NULL)
		return GPS_ERROR;


	return hndl->serial.sfd;
}


int gps_get_info(char *string, gps_gga_t *gga, gps_tm_t *tm, gps_attr_t *attr)
{
	if((string == NULL) || (gga == NULL) || (tm == NULL) || (attr == NULL))
		return GPS_ERROR;

	int i;
	int ret = -1;
	char line[20][512] = {0};
	char buf[512] = {0};
	int times = 0;
	char *p = NULL;
	
	ret = __split_info(string, line, &times);
	if(ret == GPS_ERROR)
		return GPS_ERROR;

	for(i = 0; i < times; i++)
	{
		if(!strncmp(line[i],"$GPGGA", 6))
		{
			if(strlen(line[i]) < 14)
				fprintf(stderr, "GPS NOT to receive $GPGGA data!\n");
			
			p = strtok(line[i],"\r");
			if(p != NULL)	
			{
				strcpy(buf, p);
				ret = __get_gga(buf, gga);
				if(ret == GPS_ERROR)
					attr->gga = 0;
				else
					attr->gga = 1;
				
				p = NULL;
			}	
		}
		else if(!strncmp(line[i],"$GPZDA", 6))
		{
			if(strlen(line[i]) < 14)
				fprintf(stderr, "GPS NOT to receive $GPZDA data!\n");
			
			p = strtok(line[i],"\r");
			if(p != NULL)
			{
				strcpy(buf,p);
				ret = __get_tm(buf, tm);
				if(ret == GPS_ERROR)
					attr->tm = 0;
				else
					attr->tm = 1;
				
				p = NULL;
			}
		}		
	}

	return GPS_EOK;
}










