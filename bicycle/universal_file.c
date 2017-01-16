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


#include "default.h"
#include "lib_general.h"
#include "universal_file.h"



static univ_file_config_t g_ufile_conf;
static univ_file_hd_t g_ufile_hd[5];
static pthread_mutex_t g_ufile_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_ufile_hd_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int g_fw_stat = 0;


	
static void __univ_file_config_default(void)
{
	univ_file_config_t ufile_conf_def = {UNIV_BLK_CONFIG_DEFAULT,UNIV_FW_CONFIG_DEFAULT,UNIV_RFU_CONFIG_DEFAULT};

	memcpy(&g_ufile_conf, &ufile_conf_def, sizeof(univ_file_config_t));

	fprintf(stderr, "Univ File Config Default\n");
}

static int __univ_file_config_put(univ_file_config_t *conf)
{
	int err = -1;
	FILE *fp = NULL;
	
	pthread_mutex_lock(&g_ufile_mutex);

	fp = fopen(UNIV_CONF_FILE, "wb");
	if(fp == NULL)
		goto ERR;

	err = fwrite(conf, 1, sizeof(univ_file_config_t), fp);
	if(err != sizeof(univ_file_config_t))
		goto ERR;

	if(fp != NULL)
	{
		fflush(fp);
		fclose(fp);
		fp = NULL;
	}

	memcpy(&g_ufile_conf, conf, sizeof(univ_file_config_t));
	
	pthread_mutex_unlock(&g_ufile_mutex);
	
	return UNIV_CONF_EOK;	

ERR:

	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	pthread_mutex_unlock(&g_ufile_mutex);

	return UNIV_CONF_ERROR;
}

int univ_file_config_init(void)
{
	int err = -1;
	int i;
	FILE *fp = NULL;
	univ_file_config_t univ_conf;

	memset(&univ_conf, 0, sizeof(univ_file_config_t));
	memset(&g_ufile_conf, 0, sizeof(univ_file_config_t));

	for(i = 0; i < 5; i++)
	{
		memset(&g_ufile_hd[i], 0, sizeof(univ_file_hd_t));
	}

	err = access(UNIV_CONF_FILE, F_OK);
	if(err != 0)  //文件不存在
	{
		__univ_file_config_default();
		__univ_file_config_put(&g_ufile_conf);
		
		return UNIV_CONF_EOK;
	}
	else
	{
		fp = fopen(UNIV_CONF_FILE, "rb");
		if(fp == NULL)
			return UNIV_CONF_ERROR;

		err = fread(&univ_conf, 1, sizeof(univ_conf), fp);
		if(err < 0)
			goto ERR;

		memcpy(&g_ufile_conf, &univ_conf, sizeof(univ_file_config_t));
	}

	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
		
	return UNIV_CONF_EOK;

ERR:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return UNIV_CONF_ERROR;
}

void univ_file_config_destroy(void)
{
	
}

int univ_file_config_put(univ_file_config_t *conf)
{
	return __univ_file_config_put(conf);
}

void univ_file_config_get(univ_file_config_t *conf)
{
	pthread_mutex_lock(&g_ufile_mutex);
	memcpy(conf, &g_ufile_conf, sizeof(univ_file_config_t));
	pthread_mutex_unlock(&g_ufile_mutex);
}

void univ_file_config_printf(void)
{
	fprintf(stderr, "\n----------------- ufc_blacklist_version ---------------\n");

	char total_last_ver[16] = {0};
	char inc_last_ver[16] = {0};
	char dec_last_ver[16] = {0};
	char temp_last_ver[16] = {0};
	char last_ver[16] = {0};

	strncpy(total_last_ver, g_ufile_conf.blk.total_last_ver, 8);
	strncpy(inc_last_ver, g_ufile_conf.blk.inc_last_ver, 8);
	strncpy(dec_last_ver, g_ufile_conf.blk.dec_last_ver, 8);
	strncpy(temp_last_ver, g_ufile_conf.blk.temp_last_ver, 8);
	strncpy(last_ver, g_ufile_conf.sae.last_ver, 8);

	fprintf(stderr, "total_last_ver: %s\n", total_last_ver);
	fprintf(stderr, "inc_last_ver: %s\n", inc_last_ver);
	fprintf(stderr, "dec_last_ver: %s\n", dec_last_ver);
	fprintf(stderr, "temp_last_ver: %s\n", temp_last_ver);	


	
	fprintf(stderr, "use_total_db: %d\n", g_ufile_conf.blk.use_total_db);
	fprintf(stderr, "use_inc_db: %d\n", g_ufile_conf.blk.use_inc_db);
	fprintf(stderr, "use_dec_db: %d\n", g_ufile_conf.blk.use_dec_db);
	fprintf(stderr, "use_temp_db: %d\n", g_ufile_conf.blk.use_temp_db);

	fprintf(stderr, "use_total_seq: %d\n", g_ufile_conf.blk.use_total_seq);
	fprintf(stderr, "use_inc_seq: %d\n", g_ufile_conf.blk.use_inc_seq);
	fprintf(stderr, "use_dec_seq: %d\n", g_ufile_conf.blk.use_dec_seq);
	fprintf(stderr, "use_temp_seq: %d\n", g_ufile_conf.blk.use_temp_seq);

	

	fprintf(stderr, "\n----------------- ufc_stake_firmware ---------------\n");
	fprintf(stderr, "last_ver: %s\n", last_ver);
	fprintf(stderr, "use_db: %d\n", g_ufile_conf.sae.use_db);
	fprintf(stderr, "total_seq: %d\n", g_ufile_conf.sae.total_seq);
	

	fprintf(stderr, "\n----------------- ----------------- ---------------\n");
}

void univ_file_n4_to_str(unsigned char s_n4[9], univ_file_st_t *univ)
{
	lib_hex_to_str((unsigned char *)&(univ->hd.attr), 4, s_n4);
	s_n4[8] = '\0';
}

void univ_file_vd4_to_str(unsigned char s_vd4[9], univ_file_st_t *univ)
{
	lib_hex_to_str((unsigned char *)&(univ->hd.attr[4]), 4, s_vd4);
	s_vd4[8] = '\0';
}

void univ_hd_n4_to_str(unsigned char s_n4[9], univ_file_hd_t *hd)
{
	lib_hex_to_str((unsigned char *)&(hd->attr), 4, s_n4);
	s_n4[8] = '\0';
}

void univ_hd_vd4_to_str(unsigned char s_vd4[9], univ_file_hd_t *hd)
{
	lib_hex_to_str((unsigned char *)&(hd->attr[4]), 4, s_vd4);
	s_vd4[8] = '\0';
}


int univ_file_version_compare(const unsigned char s_new[4], const unsigned char s_old[4])
{
	int i;

	for(i = 0; i < 4; i++)
	{
		if(s_new[i] > s_old[i])
			return 1;  //版本新
		else if(s_new[i] < s_old[i])
			return -1;  //版本旧
		else if(s_new[i] == s_old[i])
			continue;
	}

	return 0;  //版本相同
}


void univ_file_hd_put(univ_file_hd_t *hd, const unsigned char type)
{
	pthread_mutex_lock(&g_ufile_hd_mutex);

	switch(type)
	{
		case UFC_TOTAL_BL:
		{
			memcpy(&g_ufile_hd[0], hd, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_INC_BL:
		{
			memcpy(&g_ufile_hd[1], hd, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_DEC_BL:
		{
			memcpy(&g_ufile_hd[2], hd, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_TEMP_BL:
		{
			memcpy(&g_ufile_hd[3], hd, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_NO1_STAKE_FIRMWARE:
		{
			memcpy(&g_ufile_hd[4], hd, sizeof(univ_file_hd_t));
		}
		break;
	}
	
	pthread_mutex_unlock(&g_ufile_hd_mutex);	

	
}

void univ_file_hd_get(univ_file_hd_t *hd, const unsigned char type)
{
	pthread_mutex_lock(&g_ufile_hd_mutex);
	
	switch(type)
	{
		case UFC_TOTAL_BL:
		{
			memcpy(hd, &g_ufile_hd[0], sizeof(univ_file_hd_t));
		}
		break;

		case UFC_INC_BL:
		{
			memcpy(hd, &g_ufile_hd[1], sizeof(univ_file_hd_t));
		}
		break;

		case UFC_DEC_BL:
		{
			memcpy(hd, &g_ufile_hd[2], sizeof(univ_file_hd_t));
		}
		break;

		case UFC_TEMP_BL:
		{
			memcpy(hd, &g_ufile_hd[3], sizeof(univ_file_hd_t));
		}
		break;

		case UFC_NO1_STAKE_FIRMWARE:
		{
			memcpy(hd, &g_ufile_hd[4], sizeof(univ_file_hd_t));
		}
		break;
	}
	
	pthread_mutex_unlock(&g_ufile_hd_mutex);	
}

void univ_file_hd_clean(const unsigned char type)
{
	pthread_mutex_lock(&g_ufile_hd_mutex);
	
	switch(type)
	{
		case UFC_TOTAL_BL:
		{
			memset(&g_ufile_hd[0], 0, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_INC_BL:
		{
			memset(&g_ufile_hd[1], 0, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_DEC_BL:
		{
			memset(&g_ufile_hd[2], 0, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_TEMP_BL:
		{
			memset(&g_ufile_hd[3], 0, sizeof(univ_file_hd_t));
		}
		break;

		case UFC_NO1_STAKE_FIRMWARE:
		{
			memset(&g_ufile_hd[4], 0, sizeof(univ_file_hd_t));
		}
		break;
	}
	
	pthread_mutex_unlock(&g_ufile_hd_mutex);		
}

void univ_fw_stat_put(univ_fw_stat_t *stat)
{
	unsigned int val = 0;
	
	memcpy(&val, stat, 4);
	lib_gcc_atmoic_set(&g_fw_stat, val);
}

void univ_fw_stat_get(univ_fw_stat_t *stat)
{
	unsigned int val = 0;

	val = lib_gcc_atmoic_get(&g_fw_stat);
	memcpy(stat, &val, 4);
}

unsigned int univ_fw_get_stat(void)
{
	univ_fw_stat_t fstat;
	unsigned int val = 0;

	val = lib_gcc_atmoic_get(&g_fw_stat);
	memcpy(&fstat, &val, 4);

	return fstat.stat;
}

void univ_fw_set_stat(const unsigned char stat)
{
	univ_fw_stat_t fstat;
	unsigned int val = 0;

	val = lib_gcc_atmoic_get(&g_fw_stat);
	memcpy(&fstat, &val, 4);
	fstat.stat = stat;
	val = 0;
	memcpy( &val, &fstat, 4);
	lib_gcc_atmoic_set(&g_fw_stat, &val);
}

unsigned short univ_fw_get_file_seq(void)
{
	univ_fw_stat_t fstat;
	unsigned int val = 0;

	val = lib_gcc_atmoic_get(&g_fw_stat);
	memcpy(&fstat, &val, 4);

	return fstat.file_seq;
}

void univ_fw_set_file_seq(const unsigned short file_seq)
{
	univ_fw_stat_t fstat;
	unsigned int val = 0;

	val = lib_gcc_atmoic_get(&g_fw_stat);
	memcpy(&fstat, &val, 4);
	fstat.file_seq = file_seq;
	val = 0;
	memcpy( &val, &fstat, 4);
	lib_gcc_atmoic_set(&g_fw_stat, &val);
}



