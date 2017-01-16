#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include "defines.h"
#include "lib_general.h"
#include "universal_file.h"
#include "utils.h"


typedef struct univ_update_notify  //泛文件通知事件
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned int notify;
	univ_file_hd_t hd;
}univ_update_notify_t;

static univ_update_notify_t g_univ_update_notify = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NDEV_NOTIFY_INIT, {0,0,0,0,0}};



void utils_get_sys_time_s(char s_time[13])
{
	unsigned char time[7] = {0};
	
	lib_get_systime_bcd(time);

	lib_hex_to_str(&time[1], 6, s_time);
	s_time[12] = '\0';
}

void utils_set_sys_time_s(char s_time[13])
{
	int i;
	unsigned char h_time[7] = {0};
	unsigned char time[7] = {0};

	lib_str_to_hex(s_time, 12, h_time);
	
	for(i = 0; i < 6; i++)
	{
		time[i+1] = lib_hex_to_bcd(h_time[i]);
	}

	time[0] = lib_dec_to_bcd(20);
	lib_set_systime_bcd(time);
}

void utils_notify_printf(const unsigned int notify)
{
	switch(notify)
	{
		case NDEV_NOTIFY_INIT:
		{
			fprintf(stderr, "NDEV_NOTIFY_INIT=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_RE_REGISTER:
		{
			fprintf(stderr, "NDEV_NOTIFY_RE_REGISTER=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_REGISTERED:
		{
			fprintf(stderr, "NDEV_NOTIFY_REGISTERED=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_RE_BTHEART:
		{
			fprintf(stderr, "NDEV_NOTIFY_RE_BTHEART=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_BTHEART:
		{
			fprintf(stderr, "NDEV_NOTIFY_BTHEART=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_TOTAL_BLK:
		{
			fprintf(stderr, "NDEV_NOTIFY_TOTAL_BLK=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_INC_BLK:
		{
			fprintf(stderr, "NDEV_NOTIFY_INC_BLK=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_DEC_BLK:
		{
			fprintf(stderr, "NDEV_NOTIFY_DEC_BLK=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_TEMP_BLK:
		{
			fprintf(stderr, "NDEV_NOTIFY_TEMP_BLK=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_UNIV_UPDATE:
		{
			fprintf(stderr, "NDEV_NOTIFY_UNIV_UPDATE=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_UNIV_CONTINUE:
		{
			fprintf(stderr, "NDEV_NOTIFY_UNIV_CONTINUE=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_UNIV_QUIT:
		{
			fprintf(stderr, "NDEV_NOTIFY_UNIV_QUIT=%d\n", notify);
		}
		break;

		case NDEV_NOTIFY_UNIV_REMOVE:
		{
			fprintf(stderr, "NDEV_NOTIFY_UNIV_REMOVE=%d\n", notify);
		}
		break;

		default:
		{
			fprintf(stderr, "NDEV_NOTIFY_UNKNOWN=%d\n", notify);
		}
		break;	
	}
}

void utils_string_to_bcd(unsigned char *string, unsigned char *bcd)
{
	lib_str_to_hex(string, strlen(string), bcd);
}

void utils_time_to_bcd_yymmddhhMMss(unsigned int *time, unsigned char tm[6])
{
	unsigned short year;
	unsigned char mon;
	struct tm *ptm = localtime((time_t *)time);
	if(ptm != NULL)
	{
		year = 1900 + ptm->tm_year;
		mon = 1 + ptm->tm_mon;
		tm[0] = lib_dec_to_bcd(year % 100);
		tm[1] = lib_dec_to_bcd(mon);
		tm[2] = lib_dec_to_bcd(ptm->tm_mday);
		tm[3] = lib_dec_to_bcd(ptm->tm_hour);
		tm[4] = lib_dec_to_bcd(ptm->tm_min);
		tm[5] = lib_dec_to_bcd(ptm->tm_sec);
	}	
}

void utils_set_systime_bcd(unsigned char tm[7])
{
	unsigned short year;
	unsigned char mon, mday, hour, min, sec;
	char s_time[32] = {0};
	char s_cmd[64] = {0};

	year = (tm[0] >> 4) * 1000 + (tm[0] & 0x0F) * 100 + (tm[1] >> 4) * 10 + (tm[1] & 0x0F);	
	mon = lib_bcd_to_dec(tm[2]);
	mday = lib_bcd_to_dec(tm[3]);
	hour = lib_bcd_to_dec(tm[4]);
	min = lib_bcd_to_dec(tm[5]);
	sec = lib_bcd_to_dec(tm[6]);

	sprintf(s_time, "%02d.%02d.%02d-%02d:%02d:%02d", year, mon, mday, hour, min, sec);
	strcpy(s_cmd, "date ");
	strcat(s_cmd, s_time);
	system(s_cmd);
	system("hwclock -w");
}

void utils_tm_to_bcd_time(struct tm *ptm, unsigned char s_time[7])
{
	unsigned short year;
	unsigned char mon;
	
	if(ptm != NULL)
	{
		year = 1900 + ptm->tm_year;
		mon = 1 + ptm->tm_mon;
		s_time[0] = lib_dec_to_bcd(year / 100);
		s_time[1] = lib_dec_to_bcd(year % 100);
		s_time[2] = lib_dec_to_bcd(mon);
		s_time[3] = lib_dec_to_bcd(ptm->tm_mday);
		s_time[4] = lib_dec_to_bcd(ptm->tm_hour);
		s_time[5] = lib_dec_to_bcd(ptm->tm_min);
		s_time[6] = lib_dec_to_bcd(ptm->tm_sec);
	}
}

