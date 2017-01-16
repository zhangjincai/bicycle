#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "lib_general.h"
#include "lib_crypto.h"
#include "firmware_check.h"


//#define FC_PRINTF
#define FC_MAGIC			0x20101207


/* 固件类型 */
enum FC_TYPE
{
	FC_TYPE_BOOT = 0xa1,
    	FC_TYPE_KERNEL = 0xa2,
   	FC_TYPE_ROOTFS = 0xa3,
    	FC_TYPE_FW = 0xa4,
    	FC_TYPE_APP = 0xa5,
    	FC_TYPE_DOCK = 0xa6
};


/* 固件校验方式 */
enum FC_CHECK
{
	FC_CHECK_MD5 = 0x51,
    	FC_CHECK_CRC32 = 0x52
};

#if 0
struct FC_parameter  //256Bytes
{
    	unsigned int magic;
    	char name[32];
    	unsigned int length;
    	unsigned char type;
    	char datetime[7];
    	unsigned char check_mode;
    	unsigned char check_data[16];
    	unsigned char rfu[63];
    	unsigned char log[128];
}__attribute__((packed));
typedef struct FC_parameter FC_parameter_t;
#endif


enum FC_RET FC_check(const char *pathname, struct FC_parameter *fc_para)
{
	if((pathname == NULL) || (fc_para == NULL))
		return FC_RET_ERR;

	int ret = -1;
	int i, r_flag = 0;
	FILE *f_fp = NULL;
	FC_parameter_t f_param;
	struct stat f_stat;
	unsigned char *f_ptr = NULL;
	unsigned char f_check_data[16] = {0};
	
	memset(&f_param, 0, sizeof(FC_parameter_t));
	memset(&f_stat, 0, sizeof(struct stat));
	
	stat(pathname, &f_stat); //获取文件大小

	f_fp =  fopen(pathname, "r");
	if(f_fp <= 0)
		goto Done;

	rewind(f_fp);
	fseek(f_fp,  f_stat.st_size - sizeof(FC_parameter_t), SEEK_CUR);

	ret = fread(&f_param, 1, sizeof(FC_parameter_t), f_fp);
	if(ret  <= 0)
		goto Done;

#ifdef FC_PRINTF
	fprintf(stderr, "magic:0x%02x\n", f_param.magic);
	fprintf(stderr, "name:%s\n", f_param.name);
	fprintf(stderr, "length:%d(Bytes)\n", f_param.length);
	fprintf(stderr, "type:0x%02x\n", f_param.type);
	fprintf(stderr, "check_mode:0x%02x\n", f_param.check_mode);
	fprintf(stderr, "datetime:");
	for(i = 0; i < 7; i++)
	{
		fprintf(stderr, "%02d", lib_bcd_to_dec(f_param.datetime[i]));
	}
	fprintf(stderr, "\n");

	lib_printf("check data", f_param.check_data, 16);
#endif

	if(f_param.magic != FC_MAGIC)  //检测魔数
		goto Done;

	rewind(f_fp); //重设文件流的读写位置为文件开头

	f_ptr = (unsigned char *)malloc(f_stat.st_size);
	if(f_ptr == NULL)
		goto Done;
	
	memset(f_ptr, 0, f_stat.st_size);

	int h_cnt = f_stat.st_size / 1024;
   	int l_cnt = f_stat.st_size % 1024;
	
	if(l_cnt > 0)
        	h_cnt++;

	ret = fread(f_ptr, 1024, h_cnt, f_fp); //将文件读到内存中去
	if(ret  <= 0)
		goto Done;

	/* 检测校验码 */
	if(f_param.check_mode == FC_CHECK_MD5)
	{
		lib_md5_checksum(f_ptr, f_stat.st_size - sizeof(FC_parameter_t), f_check_data); //计算MD5
		
		lib_printf("MD5", f_check_data, 16);

		if(memcmp(f_param.check_data, f_check_data, 16) == 0)
			r_flag = 1;
		else
			r_flag = 0;
	}
	else if(f_param.check_mode == FC_CHECK_CRC32)  //CRC32
	{
		//CRC32 CAL
		if(memcmp(f_param.check_data, f_check_data, 16) == 0)
			r_flag = 1;
		else
			r_flag = 0;
	}

	if(f_ptr != NULL)
	{
		free(f_ptr);
		f_ptr = NULL;
	}

	if(f_fp !=  NULL)
		fclose(f_fp);

	if(r_flag == 1)
	{
		memcpy(fc_para, &f_param, sizeof(FC_parameter_t));
		return FC_RET_SUCCESS;
	}
	else if(r_flag == 0)
		return FC_RET_FAIL;

Done:
	if(f_ptr != NULL)
	{
		free(f_ptr);
		f_ptr = NULL;
	}
	
	if(f_fp !=  NULL)
		fclose(f_fp);

	return FC_RET_ERR;
}










