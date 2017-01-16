#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>  
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/tcp.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>

#include "lib_general.h"
#include "lib_eventloop.h"
#include "blacklist.h"
#include "sx1278.h"
#include <unistd.h>



#define SX1278_DEV			"/dev/ttyUSB3"
#define SC1278_BRATE		9600

static lib_event_loop_t *g_ep = NULL;

lib_bl_t *g_bl_a = NULL;
#define SQLITE3_PATHNAME		"/opt/universal/blacklist.db" 

lib_serial_t serial_conf;

#define STAK_ADDR	0x0300 //00 02  模块地址



/*
 * 事件触发连续读函数
 */
static ssize_t __readn_select(int fd, void *buf, size_t count, const unsigned int msec)
{
	int retval;
	ssize_t n, nread = 0;
	size_t nleft = count;
	char *ptr = (char *)buf;
	fd_set readfds;
	struct timeval timeout;

	while(nleft > 0)
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		timeout.tv_sec = msec / 1000;
		timeout.tv_usec = msec * 1000 % 1000000;
		
		retval = select(fd + 1, &readfds, NULL, NULL, &timeout);
		if(retval < 0)
			return LIB_GE_ERROR;
		else if(retval == 0)
		{
			if(nread > 0)
				return nread;
			else
				return LIB_GE_ETIMEOUT;  //超时
		}
		else if(FD_ISSET(fd, &readfds)) 		//IO可读
		{
			n = read(fd, ptr, nleft);	
			if(n < 0)
			{
				if((errno == EAGAIN) || (errno == EINTR))
					continue;
				else
					return LIB_GE_ERROR;
			}
			else if(n == 0)
				break;
		
			nleft -= n; 	
			ptr += n;
			nread += n;
		}
	}

	return(count - nleft);
}


/*
 * 事件触发读函数
 */
static ssize_t __read_select(int fd, void *buf, size_t count, const unsigned int msec)
{
	ssize_t n;
	char *ptr = (char *)buf;
	fd_set readfds;
	struct timeval timeout;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	timeout.tv_sec = msec / 1000;
	timeout.tv_usec = msec * 1000 % 1000000;

	int retval = select(fd + 1, &readfds, NULL, NULL, &timeout);
	if(retval < 0)
		return LIB_GE_ERROR;
	else if(retval == 0)
		return LIB_GE_ETIMEOUT;  			//超时
	else if(FD_ISSET(fd, &readfds))  		//IO可读
		n = read(fd, ptr, count);	
	
	return n;
}


/*
 * 阻塞模式连续写函数
 */
static ssize_t  __writen(int  fd, const void *buf, size_t count)
{
	ssize_t nwrite = 0;
	size_t nleft = count;
	const char *ptr = (char *)buf;

	while(nleft > 0)
	{
		if((nwrite = write(fd, ptr, nleft)) < 0)
		{
			/*
			 * EINTR:错误码4，中断的系统调用
			 */
			if(errno == EINTR)
				continue;
			else
				return nwrite;
		}
		else if(nwrite == 0)
			break;
		
		nleft -= nwrite;
		ptr += nwrite;
	}

	return(count - nleft);
}


/*
 * 事件触发连续写函数
 */
static ssize_t __writen_select(int fd, const void *buf, size_t count, const unsigned int msec)
{
	int retval;
	ssize_t n, nwrite;
	size_t nleft = count;
	const char *ptr = (char *)buf;
	fd_set writefds;
	struct timeval timeout;

	while(nleft > 0)
	{
		FD_ZERO(&writefds);
		FD_SET(fd, &writefds);
		timeout.tv_sec = msec / 1000;
		timeout.tv_usec = msec * 1000 % 1000000;
		
		retval = select(fd + 1, NULL, &writefds, NULL, &timeout);
		if(retval < 0)  //出错
			return LIB_GE_ERROR;
		else if(retval == 0)  //超时
		{
			if(nwrite > 0)
				return nwrite;
			else
				return LIB_GE_ETIMEOUT;  
		}
		else if(FD_ISSET(fd, &writefds)) 		
		{
			n = write(fd, ptr, nleft);	
			if(n < 0)
			{
				if((errno == EAGAIN) || (errno == EINTR))
					continue;
				else
					return LIB_GE_ERROR;
				}
				else if(n == 0)
					break;
		
				nleft -= n; 	
				ptr += n;
				nwrite += n;
			}
	}

	return(count - nleft);
}








void printf_v2(char *title, unsigned char *data, const int len, const unsigned int cb)
{
	int i;

	fprintf(stderr, "%s", title);

	for(i = 0; i < len; i++)
	{
		if((i % cb == 0) && (i != 0))
			fprintf(stderr, "\n");
		
		fprintf(stderr, "%02x ", data[i]);
	}

	fprintf(stderr, "\n");	
}



static FILE *g_sx1278_log_fp = NULL;



static int LIB_SX1278_log_open(void)
{
	char s_sx1278_log_path[64] = {0};

	 time_t timep;
	 time(&timep);
	struct tm *ptm = localtime(&timep);
	if(ptm == NULL)
		return -1;
		              			   
	sprintf(s_sx1278_log_path, "/opt/logpath/sx1278_log_%02d%02d%02d.log", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday);
	fprintf(stderr, "SX1278 LOG PATH:%s\n", s_sx1278_log_path);

	g_sx1278_log_fp = fopen(s_sx1278_log_path, "a");
	if(g_sx1278_log_fp == NULL)
		return -1;

	return 0;
}

static void LIB_SX1278_log_close(void)
{
	if(g_sx1278_log_fp != NULL)
	{
		fclose(g_sx1278_log_fp);
		g_sx1278_log_fp = NULL;
	}
}


static int LIB_SX1278_log_vsprintf(char *fmt, ...)
{
	char s_lnt_log[512] = {0};
	char s_log[512] = {0};
	time_t timep;
	time(&timep);
	struct tm *ptm = localtime(&timep);
	if(ptm == NULL)
		return -1;

	int cnt;
	va_list argptr; 

	va_start(argptr, fmt);
	cnt = vsprintf(s_log, fmt, argptr); 
	va_end(argptr);
	
	sprintf(s_lnt_log, "%02d-%02d-%02d %02d:%02d:%02d  %s", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, s_log);
	fputs(s_lnt_log, g_sx1278_log_fp);
	fflush(g_sx1278_log_fp);

	return cnt;
}



static void __hex_to_str(unsigned char *hex, const unsigned int hex_len, unsigned char *str)
{
	int i,j;
	unsigned char tmp[32] = {0};
	
	for(i = 0, j = 0; i < hex_len; i++)
	{
		memset(tmp, 0, 32);
		sprintf((char *)tmp, "%02x", hex[i]);
		str[j++] = tmp[0];
		str[j++] = tmp[1];
	}	

	str[hex_len * 2] = '\0';
}





static void __sx1278_read_ep_file_proc(lib_event_loop_t *ep, int fd, void *cls, int mask);




static void __sigint(int sig)
{
	fprintf(stderr, "sx1278 signal: %d\n", sig);
	

	lib_event_loop_stop(g_ep);
	
	lib_serial_close(&serial_conf);
	LIB_SX1278_log_close();
}


static void __signals_init(void)
{
/*
 * signal处理不好，会导致进程自动退出
 * 加上了SIGTTOU,SIGTTIN,SIGTSTP后，处理了进程自动退出的BUG
 */
	
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

	//sa.sa_handler = SIG_IGN;
	sa.sa_handler = __sigint;
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


void mydelay(int cnt)
{
	int tmp;

	tmp = 100*cnt;
	
	while(tmp--)
		;
}

int main(int argc, char *argv[])
{
	//fprintf(stderr, "SX1278 App running, Software Compiled Time: %s, %s.\n", __DATE__, __TIME__);
	
	__signals_init();

	int ret = -1;
	
	memset(&serial_conf, 0, sizeof(lib_serial_t));

	//模块串口参数设置	
	strcpy(serial_conf.pathname, SX1278_DEV);
	serial_conf.flags = O_RDWR;
	serial_conf.speed = SC1278_BRATE;
	serial_conf.databits = 8;
	serial_conf.stopbits = 1;

	ret = lib_serial_init(&serial_conf);
	if(ret != LIB_GE_EOK)
	{
		fprintf(stderr, "sx1278 serial init fail!\n");
		
		return -1;
	}
	
	lib_setfd_noblock(serial_conf.sfd);

	//查询模块工作参数
	//sx1278_configs_t configs;
	//sx1278_configs_get(serial_conf.sfd, &configs);

	//重启模块
	//sx1278_reset(serial_conf.sfd);
	
	//设置工作参数测试
	#if 0
	sx1278_configs_t *conf = NULL;
	conf = (sx1278_configs_t *)malloc(sizeof(sx1278_configs_t));
	memset(conf, 0, sizeof(sx1278_configs_t));

	conf->head = 0xC0; //掉电保存
	conf->addr_h = 0x00;   
	conf->addr_l = 0x01; 
	
 	conf->speed.bits_8N1 = BITS_8N1_DEFAULT; 
	conf->speed.baud_rate = BAUD_RATE_9600;  
	conf->speed.wireless_rate = WIRELESS_RATE_1_2K;
	
	conf->channel = CHANNEL_433M;
	
	conf->options.point_enable = POINT_PASS_MODE;
	conf->options.io_driver_mode = IO_MODE_1;
	conf->options.wakeup_timeout = WAKEUP_250MS;  
	conf->options.FEC_enalble = FEC_ON;  
	conf->options.trans_power = POWER_20dBm;
	ret = sx1278_configs_set(serial_conf.sfd, conf);
#endif	
	#if 0
	//while(1)
	{
		//主动发送数据给锁桩
		unsigned char data[] = {0x01,0x02};
		ret = sx1278_send_data(STAK_ADDR, 0xBB, data, sizeof(data));
		//if(ret <= 0)
			//break;
		
		//sleep(5);
	}
	#endif
	//主动发送数据给锁桩
	unsigned char data[200] = {0};
	memset(&data, 0xaa, sizeof(data));

	#if 0
	int i = 0;
	for(; i < 3; i++)
	{
		ret = sx1278_send_data(STAK_ADDR, 0xBB, data, 58 - PACKET_NOT_DATA_LEN);
		lib_msleep(5);
	}
	#else
	memset(&data, 0xaa, sizeof(data));
	ret = sx1278_send_data(STAK_ADDR, 0xBB, data, 58 - PACKET_NOT_DATA_LEN);
	lib_msleep(5);
	#if 0
	memset(&data, 0xbb, sizeof(data));
	ret = sx1278_send_data(STAK_ADDR, 0xBB, data, 58 - PACKET_NOT_DATA_LEN);
	lib_msleep(5);

	memset(&data, 0xcc, sizeof(data));
	ret = sx1278_send_data(STAK_ADDR, 0xBB, data, 58 - PACKET_NOT_DATA_LEN);
	lib_msleep(5);
	
	memset(&data, 0xdd, sizeof(data));
	ret = sx1278_send_data(STAK_ADDR, 0xBB, data, 26 - PACKET_NOT_DATA_LEN);
	#endif
	#endif
	
	//事件轮询读取模块数据
	#if 1
	g_ep = lib_event_loop_create(5);
	
	ret = lib_event_loop_add(g_ep, serial_conf.sfd,  LIB_EP_READ, __sx1278_read_ep_file_proc, NULL, -1);
	if(ret == LIB_EP_ERR)
	{
		fprintf(stderr, "sx1278 event loop add fd fail!\n");

		return -2;
	}


	LIB_SX1278_log_open();

	fprintf(stderr, "sx1278 loop run\n");
	lib_event_loop(g_ep);

	lib_serial_close(&serial_conf);
	LIB_SX1278_log_close();
	fprintf(stderr, "sx1278 loop quit\n");
	#endif

	
	//lib_serial_close(&serial_conf);

	return 0;	
}

static void __sx1278_read_ep_file_proc(lib_event_loop_t *ep, int fd, void *cls, int mask)
{
	unsigned char rxbuf[512] = {0};
	unsigned char s_hex[512] = {0};
	int ret = 0;

	
	//printf("in __sx1278_read_ep_file_proc ...\n");
	int n = __readn_select(fd, rxbuf, sizeof(rxbuf), 300);
	if(n > 0)
	{
		//fprintf(stderr, "\nSX1278 read bytes: %d\n", n);

		printf_v2("SX1278 read data:", (unsigned char *)rxbuf, n, 16);

		#if 1
		//和桩机通信数据解释
		ret = sx1278_packet_check(rxbuf, n);
		if(ret != PACKET_OK)
		{
			printf("\nsx1278_packet_check, error num:%d\n", ret);
			return;
		}

		ret = sx1278_packet_explain(rxbuf, n); //命令解释执行
		#endif
				
		__hex_to_str(rxbuf, n, s_hex);
		//fprintf(stderr, "%s\n", s_hex);
		LIB_SX1278_log_vsprintf("%s\n", s_hex);
		
		#if 0
		int nsend = __writen(fd, rxbuf, n);
		//int nsend = __writen_select(fd, rxbuf, n, 300);
		fprintf(stderr, "\nSX1278 write bytes: %d\n", nsend);
		#endif
	}
}

/* --------------------------test begin---------------------------- */
/* 异或校验 */
static unsigned char __xor_check(const unsigned char *data, int len)
{
	if((NULL == data) || (len <= 0))
		return -1;
	
	int i = 0;
	unsigned char check_sum = data[0];

	for(i = 1; i < len; i++)
	{
		check_sum ^= data[i];
	}

	return check_sum;
}

static enum packet_check sx1278_packet_check(const unsigned char *data, int len)
{
	if(NULL == data)
		return PACKET_NULL_ERR;
	if((len > PACKET_MAX_LEN) || (len <= 0))
		return PACKET_DATA_LEN_ERR;

	unsigned char xor;
	
	if((data[0] == HEAD_H) && (data[1] == HEAD_L))
	{
		xor = __xor_check(&data[CMD_IDX], len - 3); //本地异或校验 -3:报文头和校验位
		printf("local xor:%02X, read xor:%02X\n", xor, data[DATA_IDX + len - 5]); //-5:除去数据之外的部分
		if(xor != data[DATA_IDX + len - 5])
			return PACKET_XOR_CHECK_ERR;

		return PACKET_OK;
	}

	return PACKET_NOTKNOW_ERR;
}

static int sx1278_packet_explain(const unsigned char *data, int len)
{
	if(NULL == data)
		return PACKET_NULL_ERR;
	if((len > PACKET_MAX_LEN) || (len <= 0))
		return PACKET_DATA_LEN_ERR;

	unsigned char s_hex[512] = {0};
	int ret = -1;
	
	//命令解释
	switch(data[CMD_IDX]) //cmd
	{
		case S2N_BLACKLIST_CHECK: //黑名单查询  
		{
			printf("S2N_BLACKLIST_CHECK:%02X\n", S2N_BLACKLIST_CHECK);

			__hex_to_str((unsigned char *)&data[DATA_IDX], 8, s_hex);

			#if 1
			//打开黑名单数据库
			g_bl_a = lib_bl_create_v2(SQLITE3_PATHNAME);
			if(g_bl_a == LIB_BL_NULL)
			{
				fprintf(stderr, "black list open failed!\n");
				return -1;
			}

			
			//查找黑名单数据库
			//char *s_lid = "5100000295112587";
			ret = lib_bl_select_data_v2(g_bl_a, (const char *)s_hex); 
			printf("lib_bl_select_data, ret:%d\n", ret);
			#endif
			
			#if 1 
			//将黑名单查询结果返回给桩机	
			unsigned char black_resault;
			
			if(ret == 0)
				black_resault = 1; //查询到黑名单
			else 
				black_resault = 2; //查询不到黑名单
			
			ret = sx1278_send_data(STAK_ADDR, N2S_BLACKLIST_CHECK_ACK, &black_resault, sizeof(black_resault));
			#endif
		}
		break;

		default:
			return PACKET_NOTKNOW_ERR;
		break;
	}

	return PACKET_OK;
}


static int sx1278_send_data(unsigned short dest_addr, unsigned char cmd, unsigned char *data, int len)
{
	if((NULL == data) || (len <= 0))
		return -1;

	int i = 0, ret = -1;
	unsigned char xor_sum;
	unsigned char txbuf[256] = {0};

	//数据格式:目的地址(2) + 信道(1) + 应用数据帧(2+1+1+N+1)
	memcpy(txbuf, &dest_addr, 2); //433目的地址
	i += 2;
	
	#if 1 //e32ttl100要发送信道号码,YL-800T则不需要
	txbuf[i++] = 0x17; //433M
	#endif
	txbuf[i++] = HEAD_H;
	txbuf[i++] = HEAD_L;
	txbuf[i++] = cmd; //命令
	txbuf[i++] = len; //数据体长度
	memcpy(&txbuf[i], data, len); //应用层数据
	i += len;

	//e32ttl100:CMD_IDX + 3; YL-800T:CMD_IDX + 2
	xor_sum = __xor_check(&txbuf[CMD_IDX + 3], len + 2); //+3:目的地址(2) + 信道(1)，+2:命令和数据长度位
	txbuf[i++] = xor_sum;

	ret = __writen(serial_conf.sfd, txbuf, i);
	if(ret <= 0)
		return -1;
	
	//printf("sx1278_send_data, send ret:%d\n", ret);
	printf_v2("sx1278_send_data:\n", txbuf, ret, 16);

	return ret;
}

/* 工作参数封装成结构体形式 */
static int sx1278_configs_set(int fd, const sx1278_configs_t *configs)
{
	if(NULL == configs)
		return -1;  

	int ret = -1, len = sizeof(sx1278_configs_t);
	unsigned char txbuf[256] = {0}, rxbuf[64] = {0};

	printf_v2("sx1278_configs_set, write:", (unsigned char *)configs, len, 16);
	
	memcpy(txbuf, configs, len);
#if 1
	ret = __writen(fd, txbuf, len);
	if(ret != len)
	{
		printf("write error! ret:%d\n", ret);
		return -2;
	}
	//printf_v2("set configs:", txbuf, ret, 16);

	ret = __readn_select(fd, rxbuf, sizeof(rxbuf), 500);
	if(ret > 0)
	{
		//fprintf(stderr, "\nSX1278 read bytes: %d\n", ret);

		printf_v2("sx1278_configs_set, read:", (unsigned char *)rxbuf, ret, 16);
	}
#endif

	return 0;
}

/* 读取模块工作参数 */
static int sx1278_configs_get(int fd, sx1278_configs_t *configs)
{
	if(NULL == configs)
		return -1;

	unsigned char txbuf[32] = {0}, rxbuf[32] = {0};
	int i = 0, ret = -1;

	txbuf[i++] = 0xc1;
	txbuf[i++] = 0xc1;
	txbuf[i++] = 0xc1;
	
	ret = __writen(fd, txbuf, i);
	if(ret <= 0)
		return -2;
	
	//printf("sx1278_configs_get, send ret:%d\n", ret);
	printf_v2("sx1278_configs_get, write:", txbuf, ret, 16);

	ret = __readn_select(fd, rxbuf, sizeof(rxbuf), 300);
	if(ret <= 0)
		return -3;
	
	//fprintf(stderr, "\nSX1278 read bytes: %d\n", ret);
	if(ret > 0)
	{
		printf_v2("sx1278_configs_get, read:", (unsigned char *)rxbuf, ret, 16);
	}
	
	return 0;
}

/* 模块复位 */
static int sx1278_reset(int fd)
{
	unsigned char txbuf[32] = {0}, rxbuf[32] = {0};
	int i = 0, ret = -1;

	txbuf[i++] = 0xc4;
	txbuf[i++] = 0xc4;
	txbuf[i++] = 0xc4;
	
	ret = __writen(fd, txbuf, i);
	//printf("sx1278_reset, send ret:%d\n", ret);
	printf_v2("sx1278_reset, write:", txbuf, ret, 16);

	ret = __readn_select(fd, rxbuf, sizeof(rxbuf), 1000);
	//fprintf(stderr, "\nSX1278 read bytes: %d\n", ret);
	if(ret > 0)
	{
		printf_v2("sx1278_reset read:", (unsigned char *)rxbuf, ret, 16);
	}
	
	return 0;
}

/* --------------------------test end---------------------------- */


