#ifndef __LIB_LOGDB_H__
#define __LIB_LOGDB_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


/*
 * 函数返回码定义
 */
#define LIB_LOG_NULL					(NULL) 
#define LIB_LOG_EOK					(0)  //正常
#define LIB_LOG_ERROR				(-1) //错误
#define LIB_LOG_ETIMEOUT			(-2) //超时
#define LIB_LOG_EFULL				(-3) //满
#define LIB_LOG_EEMPTY				(-4) //空
#define LIB_LOG_ENOMEM 				(-5) //内存不够
#define LIB_LOG_EXCMEM				(-6) //内存越界
#define LIB_LOG_EBUSY				(-7) //忙
#define LIB_LOG_ERR_COMMAND		(-8) //不支持该指令

#define LIB_LOG_TRUE					(1)
#define LIB_LOG_FALSE				(0)



enum LOG_TYPE
{
	LOG_TYPE_NDEV = 1,	   //节点机
	LOG_TYPE_SAE,		   //锁桩
	LOG_TYPE_YCT,		  //羊城通
	LOG_TYPE_WL,             //无线 
	LOG_TYPE_UPGRADE,  //固件升级   
	LOG_TYPE_DOG_SOFT, //软狗  
	LOG_TYPE_GUI, //UI
	LOG_TYPE_END
};

enum LOG_LEVEL
{
	LOG_LEVEL_ERR = 1,       //出错
	LOG_LEVEL_WARNING,   //警告
	LOG_LEVEL_NOTICE,     //正常然而重要
	LOG_LEVEL_INFO,        //通知消息
	LOG_LEVEL_DEBUG,     //调戏消息
	LOG_LEVEL_END
};

typedef struct logdb lib_logdb_t;

lib_logdb_t *lib_logdb_new(void);
void lib_logdb_destroy(lib_logdb_t *log);
int lib_logdb_reconnect(lib_logdb_t *log);
int lib_logdb_version(lib_logdb_t *log, char *ver, const unsigned int msec);
int lib_logdb_put_string(lib_logdb_t *log, const unsigned char type, const unsigned char level, const char *s_log, const unsigned int msec);
int lib_logdb_sprintf(lib_logdb_t *log, const unsigned char type, const unsigned char level,  const unsigned int msec, const char *format, ...);
int lib_logdb_set_space_size(lib_logdb_t *log, const unsigned int size, const unsigned int msec);
int lib_logdb_set_clear_log_quantity(lib_logdb_t *log, const unsigned int quantity, const unsigned int msec);




/*@*/
#ifdef __cplusplus
}
#endif
#endif

