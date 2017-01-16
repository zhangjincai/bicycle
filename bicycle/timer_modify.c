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
#include<time.h>

#include "defines.h"
#include "timer_modify.h"




static void show_tm(struct tm *tm)
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


static void __set_system_time(struct tm *tm)
{
	unsigned short year;
	unsigned char mon, mday, hour, min, sec;
	char s_time[32] = {0};
	char s_cmd[64] = {0};

	year = tm->tm_year + 1900;
	mon = tm->tm_mon + 1;
	mday = tm->tm_mday;
	hour = tm->tm_hour;
	min =tm->tm_min;
	sec = tm->tm_sec;

	sprintf(s_time, "%02d.%02d.%02d-%02d:%02d:%02d", year, mon, mday, hour, min, sec);
	strcpy(s_cmd, "date ");
	strcat(s_cmd, s_time);
	system(s_cmd);
	system("hwclock -w");
}


int time_modify_run(const unsigned char gate_value,  void *ptr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;

	int comp_val = 0;
	time_t gtime, stime;
	struct tm gtm;

	time(&stime);
	memcpy(&gtm, ptr, sizeof(struct tm));
	gtime = mktime(&gtm);

#if 0
	fprintf(stderr, "gate value=%d\n", gate_value);
	fprintf(stderr, "gtime=%ld\n", gtime);
	fprintf(stderr, "stime=%ld\n", stime);
#endif

	comp_val = abs(stime - gtime);
	if(comp_val >= gate_value)
	{
		struct timeval tv;
		tv.tv_sec = gtime;
    		tv.tv_usec = 0;

		settimeofday(&tv, (struct timezone *)0);

		__set_system_time(&gtm);

		SYS_LOG_DEBUG("TIMER COMPARE, gps time:%ld, system time:%ld, gate vale:%d, comp value:%d\n", gtime, stime, gate_value, comp_val);
	}

	
	return B_DEV_EOK;
}



