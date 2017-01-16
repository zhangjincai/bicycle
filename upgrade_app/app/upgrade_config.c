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
#include "upgrade_config.h"

#define UPGRADE_CONF_FILE		"/opt/config/upgrade.conf"

static struct upgrade_config g_upgrade_config;
static pthread_mutex_t g_upgrade_mutex = PTHREAD_MUTEX_INITIALIZER;


static void __upgrade_config_default(void)
{
	memset(&g_upgrade_config, 0, sizeof(struct upgrade_config));

	fprintf(stderr, "Upgrade Config Default\n");
}

static int __upgrade_config_put(struct upgrade_config *config)
{
	int err = -1;
	FILE *fp = NULL;
	
	pthread_mutex_lock(&g_upgrade_mutex);

	fp = fopen(UPGRADE_CONF_FILE, "wb");
	if(fp == NULL)
		goto ERR;

	err = fwrite(config, 1, sizeof(struct upgrade_config), fp);
	if(err != sizeof(struct upgrade_config))
		goto ERR;

	if(fp != NULL)
	{
		fflush(fp);
		fclose(fp);
		fp = NULL;
	}

	memcpy(&g_upgrade_config, config, sizeof(struct upgrade_config));
	
	pthread_mutex_unlock(&g_upgrade_mutex);
	
	return 0;	

ERR:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	pthread_mutex_unlock(&g_upgrade_mutex);

	return -1;
}

int upgrade_config_init(void)
{
	int err = -1;
	FILE *fp = NULL;
	upgrade_config_t config;

	memset(&config, 0, sizeof(upgrade_config_t));
	memset(&g_upgrade_config, 0, sizeof(upgrade_config_t));

	err = access(UPGRADE_CONF_FILE, F_OK);
	if(err != 0)  //文件不存在
	{
		__upgrade_config_default();
		__upgrade_config_put(&g_upgrade_config);
		
		return 0;
	}
	else
	{
		fp = fopen(UPGRADE_CONF_FILE, "rb");
		if(fp == NULL)
			return -1;

		err = fread(&config, 1, sizeof(upgrade_config_t), fp);
		if(err < 0)
			goto ERR;

		memcpy(&g_upgrade_config, &config, sizeof(upgrade_config_t));
	}

	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
		
	return 0;	

ERR:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return -1;
}

int upgrade_config_put(struct upgrade_config *config)
{
	return __upgrade_config_put(config);
}

void upgrade_config_get(struct upgrade_config *config)
{
	pthread_mutex_lock(&g_upgrade_mutex);
	memcpy(config, &g_upgrade_config, sizeof(upgrade_config_t));
	pthread_mutex_unlock(&g_upgrade_mutex);
}



