#ifndef __LIB_CTOS_H__
#define __LIB_CTOS_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

/*
 * 函数返回码定义
 */
 #define LIB_CTOS_NULL			(NULL) 
#define LIB_CTOS_EOK				(0)		//正常
#define LIB_CTOS_ERROR			(-1)		//错误
#define LIB_CTOS_ETIMEOUT		(-2)		 //超时
#define LIB_CTOS_EFULL			(-3)		//满
#define LIB_CTOS_EEMPTY			(-4)		//空
#define LIB_CTOS_ENOMEM 		(-5)		//内存不够
#define LIB_CTOS_EXCMEM			(-6) 	//内存越界
#define LIB_CTOS_EBUSY			(-7)		 //忙

#define LIB_CTOS_TRUE			(1)
#define LIB_CTOS_FALSE			(0)


#define LIB_GE_TRUE				(1)
#define LIB_GE_FALSE				(0)

enum BAUD_RATE
{
	BAUD_RATE_50kbps = 1,
	BAUD_RATE_125kbps,
	BAUD_RATE_250kbps,
	BAUD_RATE_500kbps,
	BAUD_RATE_END
};

enum CAN_PRIORITY
{
	CAN_PRIORITY_HIGH = 0,
	CAN_PRIORITY_LOW	
};



struct ctos_config
{
	unsigned char can_bus;			// CAN 总线号
	unsigned char can_id;			// CAN ID 号
	unsigned int baud_rate;			// CAN 速度
	unsigned int can_buf_num;		// CAN 缓冲区个数, 单位: 个
	unsigned int rb_sz;				// 循环缓冲区大小，单位: 字节
}__attribute__((packed));
typedef struct ctos_config ctos_config_t;


typedef struct ctos lib_ctos_t;

char *lib_ctos_version(void);
lib_ctos_t *lib_ctos_create(ctos_config_t*conf);
int lib_ctos_destroy(lib_ctos_t *ctos);
int lib_ctos_set_baudrate(lib_ctos_t *ctos, const unsigned int baud_rate);
int lib_ctos_get(lib_ctos_t *ctos, unsigned char *data, const unsigned int len);
int lib_ctos_getchar(lib_ctos_t *ctos, unsigned char *ch);
int lib_ctos_put(lib_ctos_t *ctos, const unsigned char dest_addr, unsigned char *data, const unsigned int len);
int lib_ctos_put_priority(lib_ctos_t *ctos, const unsigned char dest_addr, unsigned char *data, const unsigned int len, const unsigned char priority);



/*@*/
#ifdef __cplusplus
}
#endif
#endif

