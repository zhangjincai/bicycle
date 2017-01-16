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
#include "lib_zmalloc.h"
#include "utils.h"
#include "ndev_info.h"


static ndev_info_t g_ndev_info;
pthread_mutex_t g_info_mutex = {PTHREAD_MUTEX_INITIALIZER};


void ndev_info_init(void)
{
	memset(&g_ndev_info, 0, sizeof(ndev_info_t));
}

void ndev_info_get(ndev_info_t *info)
{
	pthread_mutex_lock(&g_info_mutex);
	memcpy(info, &g_ndev_info, sizeof(ndev_info_t));
	pthread_mutex_unlock(&g_info_mutex);
}

void ndev_info_put(ndev_info_t *info)
{
	pthread_mutex_lock(&g_info_mutex);
	memcpy(&g_ndev_info, info, sizeof(ndev_info_t));
	pthread_mutex_unlock(&g_info_mutex);
}

void ndev_info_stat(ndev_status_t *stat)
{
	pthread_mutex_lock(&g_info_mutex);
	memcpy(stat, &(g_ndev_info.stat), sizeof(ndev_status_t));
	pthread_mutex_unlock(&g_info_mutex);
}

void ndev_info_stat_get(ndev_status_t *stat)
{
	pthread_mutex_lock(&g_info_mutex);
	memcpy(stat, &(g_ndev_info.stat), sizeof(ndev_status_t));
	pthread_mutex_unlock(&g_info_mutex);
}

void ndev_info_stat_NOR_get(unsigned short *stat)
{
	pthread_mutex_lock(&g_info_mutex);

	unsigned short __nstat, Not_nstat;

	memcpy(&__nstat, &(g_ndev_info.stat), sizeof(ndev_status_t));
	Not_nstat = ~__nstat; //ȫΪ1?
	*stat = htons(Not_nstat);
	
	pthread_mutex_unlock(&g_info_mutex);
}

void ndev_info_stat_put(ndev_status_t *stat)
{
	pthread_mutex_lock(&g_info_mutex);
	memcpy(&(g_ndev_info.stat), stat, sizeof(ndev_status_t));
	pthread_mutex_unlock(&g_info_mutex);
}

void ndev_info_set_time(char s_time[13])
{
	utils_set_sys_time_s(s_time);
}

void ndev_info_get_time(char s_time[13])
{
	utils_get_sys_time_s(s_time);
}



