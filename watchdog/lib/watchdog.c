#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>


#include "lib_general.h"
#include "lib_watchdog.h"

#define WDT_MSG_KEY					0x36f9
#define WDT_MSG_TYPE1					0x01
#define WDT_MSG_TYPE2					0x02

enum WDT_MSG
{
	WDT_CMD_GET_TIMEOUT = 1,
	WDT_CMD_SET_TIMEOUT,
	WDT_CMD_SYS_REBOOT,
	WDT_CMD_END
};

struct wdt_message
{
	long dest_addr;
	unsigned char cmd;
	long src_addr;
	int result;
	unsigned char data[8];
}__attribute__((packed));
typedef struct wdt_message wdt_message_t;



static int g_wdt_qid = -1;





int lib_wdt_init(void)
{
	int err = LIB_GE_ERROR;

	/* 初始化消息队列 */
	err = lib_msg_init(&g_wdt_qid, WDT_MSG_KEY);
	if(err != LIB_GE_EOK)
	{
		fprintf(stderr, "watchdog message init failed!\n");
		return LIB_WDT_ERROR;
	}

	fprintf(stderr,"library watchdog qid = %d\n", g_wdt_qid);
	
	return LIB_WDT_EOK;
}

int lib_wdt_release(void)
{
	//return lib_msg_kill(g_wdt_qid);

	return LIB_WDT_EOK;
}

int lib_wdt_set_timeout(const int timeout)
{
	int n = -1;
	wdt_message_t msg;

	memset(&msg, 0, sizeof(wdt_message_t));

	msg.dest_addr = WDT_MSG_TYPE1;
	msg.src_addr = WDT_MSG_TYPE2;
	msg.cmd = WDT_CMD_SET_TIMEOUT;
	memcpy(&(msg.data), &timeout, sizeof(timeout));
	
	lib_msg_send(g_wdt_qid, &msg, sizeof(wdt_message_t));
	memset(&msg, 0, sizeof(wdt_message_t));
	n = lib_msg_recv(g_wdt_qid, &msg, sizeof(wdt_message_t), WDT_MSG_TYPE2);
	if(n > 0)
		return LIB_WDT_EOK;

	return LIB_WDT_ERROR;
}

int lib_wdt_get_timeout(void)
{
	int n = -1;
	int timeout = LIB_WDT_ERROR;
	wdt_message_t msg;

	memset(&msg, 0, sizeof(wdt_message_t));

	msg.dest_addr = WDT_MSG_TYPE1;
	msg.src_addr = WDT_MSG_TYPE2;
	msg.cmd = WDT_CMD_GET_TIMEOUT;
	
	lib_msg_send(g_wdt_qid, &msg, sizeof(wdt_message_t));
	memset(&msg, 0, sizeof(wdt_message_t));
	n = lib_msg_recv(g_wdt_qid, &msg, sizeof(wdt_message_t), WDT_MSG_TYPE2);
	if(n > 0)
	{
		memcpy(&timeout, &(msg.data), sizeof(timeout));
		return timeout;
	}

	return LIB_WDT_ERROR;
}

int lib_wdt_system_reboot(void)
{
	int n = -1;
	wdt_message_t msg;

	memset(&msg, 0, sizeof(wdt_message_t));

	msg.dest_addr = WDT_MSG_TYPE1;
	msg.src_addr = WDT_MSG_TYPE2;
	msg.cmd = WDT_CMD_SYS_REBOOT;

	lib_msg_send(g_wdt_qid, &msg, sizeof(wdt_message_t));
	memset(&msg, 0, sizeof(wdt_message_t));
	n = lib_msg_recv(g_wdt_qid, &msg, sizeof(wdt_message_t), WDT_MSG_TYPE2);
	if(n > 0)
		return LIB_WDT_EOK;

	return LIB_WDT_ERROR;
}



