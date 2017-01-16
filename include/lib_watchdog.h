#ifndef __LIB_WATCHDOG_H__
#define __LIB_WATCHDOG_H__



/*
 * 函数返回码定义
 */
#define LIB_WDT_NULL			(NULL) 
#define LIB_WDT_EOK				(0)  //正常
#define LIB_WDT_ERROR			(-1) //错误
#define LIB_WDT_ETIMEOUT		(-2) //超时
#define LIB_WDT_EFULL			(-3) //满
#define LIB_WDT_EEMPTY			(-4) //空
#define LIB_WDT_ENOMEM 			(-5) //内存不够
#define LIB_WDT_EXCMEM			(-6) //内存越界
#define LIB_WDT_EBUSY			(-7) //忙
#define LIB_WDT_ERR_COMMAND     	(-8) //不支持的命令

#define LIB_WDT_TRUE			(1)
#define LIB_WDT_FALSE			(0)




int lib_wdt_init(void);
int lib_wdt_release(void);
int lib_wdt_set_timeout(const int timeout);
int lib_wdt_get_timeout(void);
int lib_wdt_system_reboot(void);




#endif

