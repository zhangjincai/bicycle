#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/ioctl.h> 
#include <linux/watchdog.h> 

#include "lib_general.h"


#define WDT								"/dev/watchdog"
#define WDT_TIME_DEFAULT				(10)
#define WDT_MSG_KEY					0x36f9
#define WDT_MSG_TYPE1					0x01
#define WDT_MSG_TYPE2					0x02

/* 日志定义 */
#define WD_LOG_RUN
#define TERMINAL_NO_PATH			"/opt/config/terminal_no_file.txt"

#ifdef WD_LOG_RUN
#include <syslog.h>
#define SYS_LOG_EMERG(fmt, args...) 		syslog(LOG_EMERG, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 			syslog(LOG_ALERT, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			syslog(LOG_CRIT, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			syslog(LOG_ERR, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		syslog(LOG_WARNING, fmt, ##args) 
#define SYS_LOG_NOTICE(fmt, args...)  		syslog(LOG_NOTICE, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			syslog(LOG_INFO, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		syslog(LOG_DEBUG, fmt, ##args)
#else
#define SYS_LOG_EMERG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_NOTICE(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#endif



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
	int src_addr;
	int result;
	unsigned char data[8];
}__attribute__((packed));
typedef struct wdt_message wdt_message_t;


static int g_wdt_timeout = WDT_TIME_DEFAULT;
static int g_wdt_fd = -1;
static unsigned int g_system_reboot = 1;
static int g_wdt_qid = -1;


static void *__wdt_msg_thread(void *arg);

/*
 * SIGHUP 1 A 终端挂起或者控制进程终止 
 * SIGINT 2 A 键盘中断（如break键被按下） 
 * SIGQUIT 3 C 键盘的退出键被按下 
 * SIGILL 4 C 非法指令 
 * SIGABRT 6 C 由abort(3)发出的退出指令 
 * SIGFPE 8 C 浮点异常 
 * SIGKILL 9 AEF Kill信号 
 * SIGSEGV 11 C 无效的内存引用 
 * SIGPIPE 13 A 管道破裂: 写一个没有读端口的管道 
 * SIGALRM 14 A 由alarm(2)发出的信号 
 * SIGTERM 15 A 终止信号 
 * SIGUSR1 30,10,16 A 用户自定义信号1 
 * SIGUSR2 31,12,17 A 用户自定义信号2 
 * SIGCHLD 20,17,18 B 子进程结束信号 
 * SIGCONT 19,18,25 进程继续（曾被停止的进程） 
 * SIGSTOP 17,19,23 DEF 终止进程 
 * SIGTSTP 18,20,24 D 控制终端（tty）上按下停止键 
 * SIGTTIN 21,21,26 D 后台进程企图从控制终端读 
 * SIGTTOU 22,22,27 D 后台进程企图从控制终端写 
 */
static void __sigint(int sig)
{
	fprintf(stderr, "watchdog signal: %d\n", sig);

}

static void __signals_init(void)
{
#if 0
	struct sigaction sa;

	//signal(SIGPIPE, SIG_IGN);
	//signal(SIGCHLD, SIG_IGN);

	signal(SIGPIPE, SIG_IGN); //管道关闭
	signal(SIGCHLD, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	
	signal(SIGTTOU, SIG_IGN); //后台进程写控制终端
	signal(SIGTTIN, SIG_IGN); //后台进程读控制终端
	signal(SIGTSTP, SIG_IGN); //终端挂起
	
	sa.sa_flags = 0;
	//sigaddset(&sa.sa_mask, SIGINT);
    	sigaddset(&sa.sa_mask, SIGTERM);
   	//sigaddset(&sa.sa_mask, SIGHUP);
	//sigaddset(&sa.sa_mask, SIGPIPE);

	//sa.sa_handler = __sigint;
	//sigaction(SIGINT, &sa, NULL);
	
	sa.sa_handler = __sigint;
	sigaction(SIGTERM, &sa, NULL);
	
	//sa.sa_handler = __sigint;
	//sigaction(SIGHUP, &sa, NULL);	

	//sa.sa_handler = SIG_IGN;
	//sigaction(SIGPIPE, &sa, NULL);	
#endif

	struct sigaction sa;

	sa.sa_flags = 0;
	sigaddset(&sa.sa_mask, SIGPIPE);
    sigaddset(&sa.sa_mask, SIGCHLD); 
   	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM); 
	sigaddset(&sa.sa_mask, SIGTTOU);
	sigaddset(&sa.sa_mask, SIGTTIN);
	sigaddset(&sa.sa_mask, SIGTSTP);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sa, NULL);

	sa.sa_handler = __sigint;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGTTIN, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &sa, NULL);
	
}


static void __terminal_no_file_get(char *terminal_no)
{
	FILE *fp = NULL;
	int s_len = 0;
	char s_terminal_no[64] = {0};

	fp = fopen(TERMINAL_NO_PATH, "rt");  /* 读文本 */
	if(fp != NULL)
	{
		if(fgets(s_terminal_no, sizeof(s_terminal_no), fp) != NULL)
		{
			fprintf(stderr, "watchdog Terminal NO get: %s\n", s_terminal_no);
			
			s_len = strlen(s_terminal_no);
			strcpy(terminal_no, s_terminal_no);
			terminal_no[s_len] = '\0';
		}
		else
			strcpy(terminal_no, "65535");
		
		fclose(fp);
	}
}

int main(int argc, char *argv[])
{
	fprintf(stderr, "watchdog software compiled time: %s, %s.\r\n",__DATE__, __TIME__);

	int err = LIB_GE_ERROR;
	pthread_t wdt_thr;
	int timeout = 0;

	//daemon(0, 1);
		
	__signals_init();

	/* 初始化syslog日志*/
#ifdef WD_LOG_RUN
	char log_ident[64] = {0};
	char terminal_no[32] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};
	
	lib_get_macaddr("eth1", macaddr);
	lib_hex_to_str((unsigned char *)macaddr, 6, (unsigned char *)s_macaddr);
	__terminal_no_file_get(terminal_no);

	sprintf(log_ident, "------[WATCHDOG APP]-[%s]:[%s]", s_macaddr, terminal_no);
	
	fprintf(stderr, "watchdog log ident: %s\n", log_ident);
	
	openlog(log_ident, LOG_PID | LOG_NDELAY, LOG_DAEMON);
#endif	

	SYS_LOG_NOTICE("watchdog software compiled time: %s, %s.\r\n",__DATE__, __TIME__);

	/* 初始化消息队列 */
	err = lib_msg_init(&g_wdt_qid, WDT_MSG_KEY);
	if(err != LIB_GE_EOK)
	{
		SYS_LOG_ERR("watchdog message init failed!\n");
		return 0;
	}

	fprintf(stderr,"watchdog qid = %d\n", g_wdt_qid);

	g_wdt_fd = open(WDT, O_WRONLY);
	if(g_wdt_fd == -1)
	{
		SYS_LOG_ERR("watchdog open %s failed!\n", WDT);
		return -1;
	}

	ioctl(g_wdt_fd, WDIOC_SETTIMEOUT, &g_wdt_timeout); 

	timeout = 0;
	ioctl(g_wdt_fd, WDIOC_GETTIMEOUT, &timeout); 
	SYS_LOG_NOTICE("watchdog the timeout is %d seconds\n", timeout);

	lib_normal_thread_create(&wdt_thr, __wdt_msg_thread, NULL);	
	
	while(1)  //处理看门狗
	{
		if(lib_gcc_atmoic_get(&g_system_reboot) == 1)
		{
			write(g_wdt_fd, "\0", 1);  //喂狗
		}
		
		lib_sleep(g_wdt_timeout - 2);
	}

	lib_msg_kill(g_wdt_qid);

#ifdef WD_LOG_RUN
	closelog();
#endif

	fprintf(stderr, "watchdog quit!\n");
	
	return 0;
}

static void *__wdt_msg_thread(void *arg)
{
	int ret = -1;
	int n = -1;
	int timeout = 0;
	wdt_message_t msg;
	
	while(1)
	{
		memset(&msg, 0, sizeof(wdt_message_t));
		
		n = lib_msg_recv(g_wdt_qid, &msg, sizeof(wdt_message_t), WDT_MSG_TYPE1);
		if(n > 0)
		{
			switch(msg.cmd)
			{
				case WDT_CMD_GET_TIMEOUT:  //获取看门狗时间
				{
					//fprintf(stderr, "WDT_CMD_GET_TIMEOUT: 0x%02x\n", WDT_CMD_GET_TIMEOUT);
					
					ret = ioctl(g_wdt_fd, WDIOC_GETTIMEOUT, &timeout); 

					msg.dest_addr = msg.src_addr;
					msg.src_addr = WDT_MSG_TYPE1;
					msg.result = ret;
					memcpy(&(msg.data), &timeout, sizeof(timeout));
				
					lib_msg_send(g_wdt_qid, &msg, sizeof(wdt_message_t));
				}
				break;
 
 				case WDT_CMD_SET_TIMEOUT:  //设置看门狗时间
				{
					//fprintf(stderr, "WDT_CMD_SET_TIMEOUT: 0x%02x\n", WDT_CMD_SET_TIMEOUT);

					memcpy(&timeout, &(msg.data), sizeof(timeout));
					ret = ioctl(g_wdt_fd, WDIOC_SETTIMEOUT, &timeout); 
					g_wdt_timeout = timeout;

					msg.dest_addr = msg.src_addr;
					msg.src_addr = WDT_MSG_TYPE1;
					msg.result = ret;
					memset(&(msg.data), 0, sizeof(msg.data));

					lib_msg_send(g_wdt_qid, &msg, sizeof(wdt_message_t));
				}
				break;

				case WDT_CMD_SYS_REBOOT:  //系统重启
				{
					//fprintf(stderr, "WDT_CMD_SYS_REBOOT: 0x%02x\n", WDT_CMD_SYS_REBOOT);
					
					lib_gcc_atmoic_set(&g_system_reboot, 2); //规定时间内喂不了狗则系统重启

					msg.dest_addr = msg.src_addr;
					msg.src_addr = WDT_MSG_TYPE1;
					msg.result = 0; //OK
					memset(&(msg.data), 0, sizeof(msg.data));

					lib_msg_send(g_wdt_qid, &msg, sizeof(wdt_message_t));
				}
				break;
			}
		}
		else
			lib_sleep(3);
	}

#ifdef WD_LOG_RUN
	closelog();
#endif

	return lib_thread_exit((void *)NULL);
}




