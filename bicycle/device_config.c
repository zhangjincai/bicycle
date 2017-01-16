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


#include "default.h"
#include "defines.h"
#include "lib_general.h"
#include "lib_logdb.h"
#include "device_config.h"

#define DEV_CONF_LOG_DEBUG

static device_config_t *g_dev_conf = NULL;
static lib_mutex_t *g_dev_conf_mutex = NULL;



int device_config_put(device_config_t *conf);

static void __device_config_default(void)
{
	device_config_t dev_conf_def = {MAGIC_DEFAULT_HEAD, NDEV_CONFIG_DEFAULT, STAKE_CONFIG_DEFAULT, CRC16_DEFAULT, MAGIC_DEFAULT_TAIL};

	//dev_conf_def.crc16 = lib_crc16_with_table((char *)&(dev_conf_def.nconf), sizeof(device_config_t) - 10);

	memcpy(g_dev_conf, &dev_conf_def, sizeof(device_config_t));


	SYS_LOG_INFO("device config default");
}

int device_config_init(void)
{
//	int i;
//	char conf_file[32] = {0};
//	int err = -1;  
	int n = 0;
	FILE *fp = NULL;
	device_config_t dev_conf;

	memset(&dev_conf, 0, sizeof(device_config_t));

	g_dev_conf_mutex = lib_mutex_create();
	if(g_dev_conf_mutex == NULL)
		goto ERR1;

	g_dev_conf = (device_config_t *)malloc(sizeof(device_config_t));
	if(g_dev_conf == NULL)
		goto ERR2;

#if 0
	for(i = 0; i < 3; i++)
	{
		sprintf(conf_file, "%s%d", NDEV_CONFIG_FILE, i);
		err = access(conf_file, F_OK);
		if(err != 0)  //文件不存在
		{
			__device_config_default();
			device_config_put(g_dev_conf);
			
			return DEV_CONF_EOK;
		}
		else
		{
			fp = fopen(conf_file, "rb");
			if(fp == NULL)
				goto ERR3;

			err = fread(&dev_conf, 1, sizeof(device_config_t), fp);
			if(err > 0)
			{
				if((dev_conf.magic_head != MAGIC_DEFAULT_HEAD) || (dev_conf.magic_tail != MAGIC_DEFAULT_TAIL) || (err != sizeof(device_config_t)))
				{
					if(fp != NULL)
					{
						fclose(fp);
						fp = NULL;
					}
					continue;
				}
				else
				{
					memcpy(g_dev_conf, &dev_conf, sizeof(device_config_t));
					
					if(fp != NULL)
					{
						fclose(fp);
						fp = NULL;
					}
					return DEV_CONF_EOK;
				}
			}
		}
	}
#endif

	/* 检查配置文件是否存在 */
	n = access(NDEV_CONFIG_FILE, F_OK);
	if(n != 0)  //文件不存在
	{
		SYS_LOG_ERR("%s: %s is NOT exist, errno=%d\n", __FUNCTION__, NDEV_CONFIG_FILE, errno);
		
		__device_config_default();
		device_config_put(g_dev_conf);

		return DEV_CONF_EOK;
	}
	else
	{
		fp = fopen(NDEV_CONFIG_FILE, "rb");
		if(fp == NULL)
		{
			SYS_LOG_ERR("%s: fopen device config failed!", __FUNCTION__);
			
			goto ERR3;
		}

		n = fread(&dev_conf, 1, sizeof(device_config_t), fp);
		if(n > 0)
		{
			if((dev_conf.magic_head != MAGIC_DEFAULT_HEAD) || (dev_conf.magic_tail != MAGIC_DEFAULT_TAIL) || (n != sizeof(device_config_t)))
			{
				if(fp != NULL)
				{
					fclose(fp);
					fp = NULL;
				}

				if(dev_conf.magic_head != MAGIC_DEFAULT_HEAD)
					SYS_LOG_ERR("%s: dev_conf.magic_head != MAGIC_DEFAULT_HEAD", __FUNCTION__);

				if(dev_conf.magic_tail != MAGIC_DEFAULT_TAIL)
					SYS_LOG_ERR("%s: dev_conf.magic_tail != MAGIC_DEFAULT_TAIL", __FUNCTION__);

				if(n != sizeof(device_config_t))
					SYS_LOG_ERR("%s: n != sizeof(device_config_t), n = %d, sizeof(device_config_t) = %d", __FUNCTION__, n, sizeof(device_config_t));	
			}
			else
			{
				memcpy(g_dev_conf, &dev_conf, sizeof(device_config_t));
					
				if(fp != NULL)
				{
					fclose(fp);
					fp = NULL;
				}
				return DEV_CONF_EOK;
			}
		}
	}

	/* 读取配置失败 */
	__device_config_default();
	device_config_put(g_dev_conf);

	SYS_LOG_ERR("%s: get device config failed!", __FUNCTION__);
	
	return DEV_CONF_EOK;
	
ERR3:
	if(g_dev_conf != NULL)
	{
		free(g_dev_conf);
		g_dev_conf = NULL;
	}
ERR2:
	if(g_dev_conf_mutex != NULL)
		lib_mutex_destroy(g_dev_conf_mutex);
ERR1:
	return DEV_CONF_ERROR;
}

void device_config_destroy(void)
{
	if(g_dev_conf_mutex != NULL)
		lib_mutex_destroy(g_dev_conf_mutex);

	if(g_dev_conf != NULL)
	{
		free(g_dev_conf);	
		g_dev_conf = NULL;
	}
}

int device_config_put(device_config_t *conf)
{
	if(conf == NULL)
		return DEV_CONF_ERROR;

//	int i;
//	char conf_file[32] = {0};
//	int err = -1;
	int n = 0;
	FILE *fp = NULL;
	
	lib_mutex_lock(g_dev_conf_mutex);

#if 0
	for(i = 0; i < 3; i++)
	{
		sprintf(conf_file, "%s%d", NDEV_CONFIG_FILE, i);
		fp = fopen(conf_file, "wb");
		if(fp == NULL)
			goto ERR1;

		conf->magic_head = MAGIC_DEFAULT_HEAD;
		conf->magic_tail = MAGIC_DEFAULT_TAIL;
		conf->crc16 = lib_crc16_with_table((char *)&(conf->nconf), sizeof(device_config_t) - 10);
		err = fwrite(conf, 1, sizeof(device_config_t), fp);
		if(err != sizeof(device_config_t))
			goto ERR2;

		if(fp != NULL)
		{
			fflush(fp);
			fclose(fp);
			fp = NULL;
		}
		memcpy(g_dev_conf, conf, sizeof(device_config_t));
	}
#endif

	fp = fopen(NDEV_CONFIG_FILE, "wb");
	if(fp == NULL)
	{
		SYS_LOG_ERR("%s: fopen device config failed!", __FUNCTION__);
		
		goto ERR1;
	}
	
	conf->magic_head = MAGIC_DEFAULT_HEAD;
	conf->magic_tail = MAGIC_DEFAULT_TAIL;
	//conf->crc16 = lib_crc16_with_table((char *)&(conf->nconf), sizeof(device_config_t) - 10);
	n = fwrite(conf, 1, sizeof(device_config_t), fp);
	if(n != sizeof(device_config_t))
	{
		SYS_LOG_ERR("%s: n != sizeof(device_config_t), n = %d, sizeof(device_config_t) = %d", __FUNCTION__, n, sizeof(device_config_t));
		
		goto ERR2;
	}

	if(fp != NULL)
	{
		fflush(fp);
		fclose(fp);
		fp = NULL;
	}
	
	memcpy(g_dev_conf, conf, sizeof(device_config_t));

	lib_mutex_unlock(g_dev_conf_mutex);

	return DEV_CONF_EOK;

ERR2:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
ERR1:	
	lib_mutex_unlock(g_dev_conf_mutex);
	return DEV_CONF_ERROR;
}

int device_config_get(device_config_t *conf)
{
	if(conf == NULL)
		return DEV_CONF_ERROR;

	lib_mutex_lock(g_dev_conf_mutex);
	memcpy(conf, g_dev_conf, sizeof(device_config_t));
	lib_mutex_unlock(g_dev_conf_mutex);	
	
	return DEV_CONF_EOK;
}

int device_config_check(device_config_t *conf)
{
	if(conf == NULL)
		return DEV_CONF_ERROR;

//	unsigned short crc16 = 0;
	
	if((conf->magic_head != MAGIC_DEFAULT_HEAD) || (conf->magic_tail != MAGIC_DEFAULT_TAIL))
		return DEV_CONF_EMAGIC;

//	crc16 = lib_crc16_with_table((char *)&(conf->nconf), sizeof(device_config_t) - 10);
//	if(crc16 != conf->crc16)
//		return DEV_CONF_ECRC;

	return DEV_CONF_EOK;
}



void device_config_printf(void)
{
	fprintf(stderr, "-----------------------ndev_config--------------------\n");
	
	fprintf(stderr, "timer_gate_value:%d\n", g_dev_conf->nconf.timer_gate_value);

	fprintf(stderr, "timer_gate_value:%d\n", g_dev_conf->nconf.timer_gate_value);
}


