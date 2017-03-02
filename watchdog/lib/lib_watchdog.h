#ifndef __LIB_WATCHDOG_H__
#define __LIB_WATCHDOG_H__



/*
 * ���������붨��
 */
#define LIB_WDT_NULL			(NULL) 
#define LIB_WDT_EOK				(0)  //����
#define LIB_WDT_ERROR			(-1) //����
#define LIB_WDT_ETIMEOUT		(-2) //��ʱ
#define LIB_WDT_EFULL			(-3) //��
#define LIB_WDT_EEMPTY			(-4) //��
#define LIB_WDT_ENOMEM 			(-5) //�ڴ治��
#define LIB_WDT_EXCMEM			(-6) //�ڴ�Խ��
#define LIB_WDT_EBUSY			(-7) //æ
#define LIB_WDT_ERR_COMMAND     	(-8) //��֧�ֵ�����

#define LIB_WDT_TRUE			(1)
#define LIB_WDT_FALSE			(0)




int lib_wdt_init(void);
int lib_wdt_release(void);
int lib_wdt_set_timeout(const int timeout);
int lib_wdt_get_timeout(void);
int lib_wdt_system_reboot(void);




#endif

