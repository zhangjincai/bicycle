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

#include "lib_general.h"
#include "ufile_ctrl_info.h"



static ufile_ctrl_info_t g_ufile_ctrl_info;
static pthread_mutex_t g_ufile_ctrl_mutex = PTHREAD_MUTEX_INITIALIZER;


void ufile_ctrl_info_init(void)
{
	g_ufile_ctrl_info.f_recv_total = F_RECV_STAT_INIT;
	g_ufile_ctrl_info.f_recv_inc= F_RECV_STAT_INIT;
	g_ufile_ctrl_info.f_recv_dec = F_RECV_STAT_INIT;
	g_ufile_ctrl_info.f_recv_fw = F_RECV_STAT_INIT;
}

int ufile_ctrl_info_set(const unsigned char cmd, ufile_ctrl_info_t *ufile)
{
	if(ufile == NULL)
		return -1;

	pthread_mutex_lock(&g_ufile_ctrl_mutex);

	fprintf(stderr, "%s:0x%02x\n", __FUNCTION__, cmd);
	
	switch(cmd)
	{
		case UCI_F_RECV_TOTAL:
			g_ufile_ctrl_info.f_recv_total = ufile->f_recv_total;
			break;

		case UCI_F_RECV_INC:
			g_ufile_ctrl_info.f_recv_inc = ufile->f_recv_inc;
			break;

		case UCI_F_RECV_DEC:
			g_ufile_ctrl_info.f_recv_dec = ufile->f_recv_dec;
			break;

		case UCI_F_RECV_TEMP:
			g_ufile_ctrl_info.f_recv_temp = ufile->f_recv_temp;
			break;
			
		case UCI_F_RECV_FW:
			g_ufile_ctrl_info.f_recv_fw = ufile->f_recv_fw;
			break;








		
	}

	pthread_mutex_unlock(&g_ufile_ctrl_mutex);

	return 0;
}

int ufile_ctrl_info_set_all(ufile_ctrl_info_t *ufile)
{
	if(ufile == NULL)
		return -1;

	pthread_mutex_lock(&g_ufile_ctrl_mutex);
	memcpy(&g_ufile_ctrl_info, ufile, sizeof(ufile_ctrl_info_t));		
	pthread_mutex_unlock(&g_ufile_ctrl_mutex);

	return 0;
}

int ufile_ctrl_info_get(const unsigned char cmd, ufile_ctrl_info_t *ufile)
{
	if(ufile == NULL)
		return -1;

	pthread_mutex_lock(&g_ufile_ctrl_mutex);

	fprintf(stderr, "%s:0x%02x\n", __FUNCTION__, cmd);
	
	switch(cmd)
	{
		case UCI_F_RECV_TOTAL:
			ufile->f_recv_total = g_ufile_ctrl_info.f_recv_total;
			break;

		case UCI_F_RECV_INC:
			ufile->f_recv_inc = g_ufile_ctrl_info.f_recv_inc;
			break;

		case UCI_F_RECV_DEC:
			ufile->f_recv_dec = g_ufile_ctrl_info.f_recv_dec;
			break;

		case UCI_F_RECV_TEMP:
			ufile->f_recv_temp = g_ufile_ctrl_info.f_recv_temp;
			break;

		case UCI_F_RECV_FW:
			ufile->f_recv_fw = g_ufile_ctrl_info.f_recv_fw;
			break;








		
	}

	pthread_mutex_unlock(&g_ufile_ctrl_mutex);

	return 0;
}

int ufile_ctrl_info_get_all(ufile_ctrl_info_t *ufile)
{
	if(ufile == NULL)
		return -1;

	pthread_mutex_lock(&g_ufile_ctrl_mutex);
	memcpy(ufile, &g_ufile_ctrl_info, sizeof(ufile_ctrl_info_t));		
	pthread_mutex_unlock(&g_ufile_ctrl_mutex);

	return 0;
}

unsigned char ufile_ctrl_info_get_f_recv(const unsigned char cmd)
{
	pthread_mutex_lock(&g_ufile_ctrl_mutex);

	fprintf(stderr, "%s:0x%02x\n", __FUNCTION__, cmd);

	unsigned f_recv_stat = F_RECV_STAT_INIT;
	
	switch(cmd)
	{
		case UCI_F_RECV_TOTAL:
			f_recv_stat = g_ufile_ctrl_info.f_recv_total;
			break;

		case UCI_F_RECV_INC:
			f_recv_stat = g_ufile_ctrl_info.f_recv_inc;
			break;

		case UCI_F_RECV_DEC:
			f_recv_stat = g_ufile_ctrl_info.f_recv_dec;
			break;

		case UCI_F_RECV_TEMP:
			f_recv_stat = g_ufile_ctrl_info.f_recv_temp;
			break;

		case UCI_F_RECV_FW:
			f_recv_stat = g_ufile_ctrl_info.f_recv_fw;
			break;








		
	}

	pthread_mutex_unlock(&g_ufile_ctrl_mutex);	

	return f_recv_stat;
}



