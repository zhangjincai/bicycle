#ifndef __LIB_UPDATE_H__
#define __LIB_UPDATE_H__



/*
 * 函数返回码定义
 */
#define LIB_UP_NULL				(NULL) 
#define LIB_UP_EOK				(0)  //正常
#define LIB_UP_ERROR				(-1) //错误
#define LIB_UP_ETIMEOUT			(-2) //超时
#define LIB_UP_EFULL				(-3) //满
#define LIB_UP_EEMPTY			(-4) //空
#define LIB_UP_ENOMEM 			(-5) //内存不够
#define LIB_UP_EXCMEM			(-6) //内存越界
#define LIB_UP_EBUSY				(-7) //忙
#define LIB_UP_ERR_COMMAND		(-8) //不支持该指令

#define LIB_UP_TRUE				(1)
#define LIB_UP_FALSE				(0)




#define LIB_UP_PARA_MAGIC			0xebecfc79
#define  LIB_UP_PARA_UPDATE			0x55

enum UP_NEED_UPDATE
{
	UP_NEED_UPDATE_INIT = 0x00,         		//不需要
	UP_NEED_UPDATE = 0x55,			//需要
	UP_NEED_UPDATE_OK = 0xaa		//升级完毕
};


enum UP_PARTITION
{
	 UP_PARTITION_KERNEL = 0,
	 UP_PARTITION_FIRMWARE,
	 UP_PARTITION_APP,
	 UP_PARTITION_END
};

struct partition
{
	char firmware_name[32];			//固件名称
	unsigned int len;		  			//固件长度
	unsigned char datetime[7];			// YY MM DD HH MM SS
	unsigned char md5[16];		 		//固件MD5 校验
	unsigned char is_need_update;      	// 0:不需要 0x55:需要
};

struct parameter
{
	unsigned int magic;
	unsigned char is_format;
	struct partition partition[UP_PARTITION_END];
	unsigned short crc16;
}__attribute__((packed));
typedef struct parameter lib_parameter_t;

struct update_config
{
	void *mtd_desc;
	char mtd_device[64];
};

typedef struct lib_update lib_update_t;


/*
 * strcpy(config.mtd_device, "/dev/mtd9");
 */

lib_update_t *lib_update_create( struct update_config *config);
void lib_update_destroy(lib_update_t *up);
int lib_update_read_parameter(lib_update_t *up, struct parameter *para);
int lib_update_write_parameter(lib_update_t *up, struct parameter *para);
int lib_update_write_flash(lib_update_t *up, const char *pathname);
int lib_update_erase_nand_flash(lib_update_t *up, const unsigned int eblock_start, const unsigned int eblock_cnt);
int lib_update_write_nand_flash(lib_update_t *up, const char *pathname);
int lib_update_write_flash_skip(lib_update_t *up, const char *pathname);

#endif


