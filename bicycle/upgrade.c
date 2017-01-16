#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <curl/curl.h>  
#include <signal.h>
#include <errno.h>

#include "defines.h"
#include "lib_general.h"
#include "lib_update.h"
#include "upgrade.h"

#define MTD_DEV_KERNEL			"/dev/mtd9"
#define MTD_DEV_APPL			"/dev/mtd10"

static lib_update_t *g_kernel_update = NULL;
static lib_update_t *g_appl_update = NULL;

static size_t __fn_write(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
	return fwrite(ptr, size, nmemb, (FILE*)stream);  
}  

static int __fn_progress(void *ptr, double total_download, double now_download, double total_upload, double now_upload)
{	
#if 0
	fprintf(stderr, "dltotal = %f\n", dltotal);
	fprintf(stderr, "dlnow = %f\n", dlnow);
	fprintf(stderr, "ultotal = %f\n", ultotal);
	fprintf(stderr, "ulnow = %f\n", ulnow);
	
	fprintf(stderr, "%f /%f (%g %%)\n", dlnow, dltotal, dlnow * 100.0 / dltotal);
#endif





	return B_DEV_EOK;
}

int upgrade_init(void)
{
	pthread_t up_thr;
	struct update_config kernel_config, appl_config;

	memset(&kernel_config, 0, sizeof(struct update_config));
	memset(&appl_config, 0, sizeof(struct update_config));

	strcpy(kernel_config.mtd_device, MTD_DEV_KERNEL);
	strcpy(appl_config.mtd_device, MTD_DEV_APPL);

	curl_global_init(CURL_GLOBAL_ALL); 
	
	g_kernel_update = lib_update_create(&kernel_config);
	if(g_kernel_update == LIB_UP_NULL)
		return B_DEV_ERROR;

	g_appl_update = lib_update_create(&appl_config);
	if(g_appl_update == LIB_UP_NULL)
		return B_DEV_ERROR;


	
	return B_DEV_EOK;
}

void upgrade_release(void)
{
	lib_update_destroy(g_kernel_update);
	lib_update_destroy(g_appl_update);
	
	curl_global_cleanup();  
}

static int __ftp_download(const char *username, const char *passwd, const char *remote_path, const char *local_path, const long connect_timeout)
{
	char userpass[128] = {0};
	FILE *fp = NULL;
	curl_off_t local_file_len = -1;
	long file_size = 0;
	CURLcode curl_r = CURLE_GOT_NOTHING;
	struct stat f_stat;
	int use_resume = 0; //断点续传 
	CURL *curl_download = NULL;

	memset(&f_stat, 0, sizeof(struct stat));
	sprintf(userpass, "%s:%s", username, passwd);

	/* 获取本地文件大小 */
	if(stat(local_path, &f_stat) == 0)
	{
		local_file_len = f_stat.st_size;
		use_resume = 1;
	}

	/* 断点续传 */
	fp = fopen(local_path, "ab+");
	if(fp == NULL)
	{
		fprintf(stderr, "ftp download fopen %s failed!\n", local_path);
		return B_DEV_ERROR;
	}

	curl_download = curl_easy_init();
	if(curl_download == NULL)
		return B_DEV_ERROR;
	
	curl_easy_setopt(curl_download, CURLOPT_URL, remote_path);  
	curl_easy_setopt(curl_download, CURLOPT_USERPWD, userpass);  //设置用户名与密码。参数是格式如 “user:password ”的字符串
	curl_easy_setopt(curl_download, CURLOPT_CONNECTTIMEOUT, connect_timeout);   //连接超时

	/* 设置断点续传 */
	curl_easy_setopt(curl_download, CURLOPT_RESUME_FROM_LARGE, use_resume ? local_file_len : 0);   //断点续传
	curl_easy_setopt(curl_download, CURLOPT_WRITEFUNCTION, __fn_write);   //写回调函数
	curl_easy_setopt(curl_download, CURLOPT_WRITEDATA, fp);    	//流指针

	/* 传输进度 */
	curl_easy_setopt(curl_download, CURLOPT_NOPROGRESS, 0L);		//0L开启进度输出
	curl_easy_setopt(curl_download, CURLOPT_PROGRESSFUNCTION, __fn_progress);
	curl_easy_setopt(curl_download, CURLOPT_PROGRESSDATA, NULL);
	curl_easy_setopt(curl_download, CURLOPT_VERBOSE, 0L);   		//将CURLOPT_VERBOSE属性设置为1，libcurl会输出通信过程中的一些细节

	curl_r = curl_easy_perform(curl_download);
	curl_easy_cleanup(curl_download);
	
	fclose(fp);  

	if(curl_r == CURLE_OK)  //下载成功
		return B_DEV_EOK;

	fprintf(stderr,"%s\n", curl_easy_strerror(curl_r));  
	
	return B_DEV_ERROR;
}

void *upgrade_handle_run(void *arg)
{
	int retval = -1;
	int ret = -1;
	lib_update_t *update = NULL;
	lib_parameter_t para;
	int fd = -1;
	struct stat filestat;
	enum UP_PARTITION eu_up_part;

	memset(&filestat, 0, sizeof(struct stat));
	memset(&para, 0, sizeof(lib_parameter_t));
	
	struct upgrade_config *config = (struct upgrade_config *)arg;
	if(config == NULL)
		return B_DEV_NULL;
	
	switch(config->update_type) //升级类型
	{
		case UP_PARTITION_KERNEL:
		{
			update = g_kernel_update;
			eu_up_part = UP_PARTITION_KERNEL;
		}
		break;

		case UP_PARTITION_APP:
		{
			update = g_appl_update;
			eu_up_part = UP_PARTITION_APP;
		}
		break;

		default:  
			goto err;
	}

	if(update == NULL)
		goto err;


	retval = __ftp_download(config->username, config->passwd, config->remote_path, config->local_path, config->connect_timeout);
	if(retval == 0) //成功
	{
		ret = lib_update_write_flash(update, config->local_path);
		if(ret != LIB_UP_EOK)
			goto err;

		fd = open(config->local_path, O_RDONLY);
		if(fd < 0)
			goto err;

		fstat(fd, &filestat);
		fprintf(stderr, "%s size: %d\n", config->local_path, filestat.st_size);
		close(fd);

		para.magic = LIB_UP_PARA_MAGIC;
	
		strcpy(para.partition[eu_up_part].firmware_name, config->firmware_name);
		para.partition[eu_up_part].len = filestat.st_size;
		lib_get_systime_bcd(para.partition[eu_up_part].datetime);
		para.partition[eu_up_part].is_need_update = LIB_UP_PARA_UPDATE;

		para.crc16 = lib_crc16_with_byte((unsigned char *)&para, sizeof(lib_parameter_t) - 2);		

		ret = lib_update_write_parameter(update, &para);
		if(ret != LIB_UP_EOK)
			goto err;

		
		
	}


err:	
	if(config != NULL)
	{
		free(config);
		config = NULL;
	}
	return B_DEV_NULL;
}



