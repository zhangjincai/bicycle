#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <time.h>
#include <sys/ioctl.h>

#include "lib_general.h"
#include "lib_wireless.h"
#include "device_config.h"
#include "node_device.h"
#include "var_data.h"
#include "defines.h"
#include "stake.h"
#include "lib_watchdog.h"
#include "upgrade.h"
#include "gui.h"
#include "ndev_info.h"
#include "gpio_ctrl.h"
#include "ufile_ctrl_info.h"
#include "unity_file_handle.h"
#include "external_interface_board.h"

#define BOOT_VERSION_PATH			"/mnt/firmware/boot_ver.txt"
#define KERNEL_VERSION_PATH			"/mnt/firmware/kernel_ver.txt"
#define ROOTFS_VERSION_PATH			"/etc/rootfs_ver.txt"
#define FW_VERSION_PATH				"/mnt/firmware/fw_ver.txt"
#define APPL_VERSION_PATH			"/mnt/app/appl_ver.txt"
#define TERMINAL_NO_PATH			"/opt/config/terminal_no_file.txt"



lib_wl_t *g_bicycle_wl = NULL; //����
long g_last_time_power_on = 0; //���һ���ϵ�ʱ��

static void __sigint(int sig)
{
	fprintf(stderr, "bicycle signal: %d\n", sig);
	
	gui_stop();

	exit(-1);
}

static void __signals_init(void)
{
/*
 * signal�����ã��ᵼ�½����Զ��˳�
 * ������SIGTTOU,SIGTTIN,SIGTSTP�󣬴����˽����Զ��˳���BUG
 */
	
	struct sigaction sa;

	sa.sa_flags = 0; //�������κα�־
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

static int __boot_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;
	int s_len = 0;
	
	fp = fopen(BOOT_VERSION_PATH, "rb");
	if(fp == NULL)
		return B_DEV_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		fprintf(stderr, "boot version: %s\n", s_version);
		s_len = strlen(s_version);
		strcpy(version, s_version);
		version[s_len - 1] = '\0';
	}

	fclose(fp);
	
	return B_DEV_EOK;
}

static int __kernel_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;
	int s_len = 0;
	
	fp = fopen(KERNEL_VERSION_PATH, "rb");
	if(fp == NULL)
		return B_DEV_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		fprintf(stderr, "kernel version: %s\n", s_version);
		s_len = strlen(s_version);
		strcpy(version, s_version);
		version[s_len - 1] = '\0';
	}

	fclose(fp);
	
	return B_DEV_EOK;
}

static int __rootfs_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;
	int s_len = 0;
	
	fp = fopen(ROOTFS_VERSION_PATH, "rb");
	if(fp == NULL)
		return B_DEV_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		fprintf(stderr, "rootfs version: %s\n", s_version);
		s_len = strlen(s_version);
		strcpy(version, s_version);
		version[s_len -1] = '\0';
	}

	fclose(fp);
	
	return B_DEV_EOK;
}

static int __fw_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;
	int s_len = 0;
	
	fp = fopen(FW_VERSION_PATH, "rb");
	if(fp == NULL)
		return B_DEV_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		fprintf(stderr, "fw version: %s\n", s_version);
		s_len = strlen(s_version);
		strcpy(version, s_version);
		version[s_len - 1] = '\0';
	}

	fclose(fp);
	
	return B_DEV_EOK;
}

static int __appl_version_fgets(char version[32])
{
	char s_version[64] = {0};
	FILE *fp = NULL;
	int s_len = 0;

	fp = fopen(APPL_VERSION_PATH, "rb");
	if(fp == NULL)
		return B_DEV_ERROR;

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		fprintf(stderr, "appl version: %s\n", s_version);
		s_len = strlen(s_version);
		strcpy(version, s_version);
		version[s_len - 1] = '\0';
	}

	fclose(fp);
	
	return B_DEV_EOK;
}

static void __terminal_no_file_put(const char *terminal_no)
{
	FILE *fp = NULL;

	fp = fopen(TERMINAL_NO_PATH, "wt"); /* д�ı� */
	if(fp != NULL)
	{
		if(fputs(terminal_no, fp) != NULL)
		{
			fprintf(stderr, "Bicycle Terminal NO put: %s\n", terminal_no);

			fflush(fp);
		}

		fclose(fp);
	}
}

static void __terminal_no_file_get(char *terminal_no)
{
	FILE *fp = NULL;
	int s_len = 0;
	char s_terminal_no[64] = {0};

	fp = fopen(TERMINAL_NO_PATH, "rt");  /* ���ı� */
	if(fp != NULL)
	{
		if(fgets(s_terminal_no, sizeof(s_terminal_no), fp) != NULL)
		{
			fprintf(stderr, "Bicycle Terminal NO get: %s\n", s_terminal_no);
			
			s_len = strlen(s_terminal_no);
			strcpy(terminal_no, s_terminal_no);
			terminal_no[s_len] = '\0';
		}
		else
			__terminal_no_file_put("65535");
		
		fclose(fp);
	}
}


int main(int argc, char *argv[])
{
	int err = -1;
	int gpio_ctrl_fd = -1;  //gpio
	//char loadserver1[32] = {0};
	//char loadserver2[32] = {0};
	device_config_t dev_conf;
	ndev_info_t ninfo;

	memset(&dev_conf, 0, sizeof(device_config_t));
	memset(&ninfo, 0, sizeof(ndev_info_t));
	
	fprintf(stderr, "Bicycle App running, Software Compiled Time: %s, %s.\n", __DATE__, __TIME__);

	system("ulimit -s 6144");  //�����߳�ջ

	/* ʹ��core�ļ� add by zjc at 2017-01-12 */
	system("ulimit -c unlimited");
	system("sysctl -w kernel.core_pattern=/opt/logpath/%t.%e.core.%p.%s");
/*
	%p ��dump���̵Ľ���ID
	%s ���±���core dump���ź�
	%t core dump��ʱ�� (��1970��1��1�ռ��������)
	%e �����ļ���
*/

	
	/*
	 * int daemon (int __nochdir, int __noclose);
	 * ���__nochdir��ֵΪ0�����л�����Ŀ¼Ϊ��Ŀ¼��
	 * ���__nocloseΪ0���򽫱�׼���룬����ͱ�׼�����ض���/dev /null��
	*/
	/*
	 * ʹ��ϵͳ�Դ���daemon�������ᵼ��syslogd���ܱ������
	 *
	 */
	//daemon(0, 1); //�ػ�����

	__signals_init();

	/* ��ʼ��syslog��־*/
#ifdef BICYCLE_LOG_RUN
	char log_ident[64] = {0};
	char terminal_no[32] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};
	
	lib_get_macaddr("eth1", macaddr);
	lib_hex_to_str((unsigned char *)macaddr, 6, (unsigned char *)s_macaddr);
	__terminal_no_file_get(terminal_no);

	sprintf(log_ident, "------[BICYCLE]-[%s]:[%s]", s_macaddr, terminal_no);
		
	fprintf(stderr, "Bicycle log ident: %s\n", log_ident);
	
	openlog(log_ident, LOG_NDELAY, LOG_DAEMON);
#endif	

	gpio_ctrl_fd =  open("/dev/gpio_ctrl", O_RDWR);
	if(gpio_ctrl_fd < 0)
	{
		SYS_LOG_ERR("gpio control open failed!");
	}

	ioctl(gpio_ctrl_fd, GPIO_CTRL_SET_BACKLIGHTEN, GPIO_CTRL_SW_ON);  //LCD���⿪��
	ioctl(gpio_ctrl_fd, GPIO_CTRL_SET_PWR_YCT, GPIO_CTRL_SW_ON); 	//����ͨ�������ϵ�
	ioctl(gpio_ctrl_fd, GPIO_CTRL_SET_PWR_2232_2, GPIO_CTRL_SW_ON);  //WIFI�ϵ�
	if(gpio_ctrl_fd > 0)
		lib_close(gpio_ctrl_fd);
	
	time(&g_last_time_power_on); //���һ���ϵ�ʱ��

	lib_wdt_init(); //���п��Ź�
   
	SYS_LOG_NOTICE("Bicycle App running, Software Compiled Time: %s, %s", __DATE__, __TIME__);

	err = device_config_init();
	if(err != DEV_CONF_EOK)
	{
		SYS_LOG_ERR("device config init failed!");   
		goto err;
	}

	//device_config_printf();
	
#ifdef BICYCLE_LOG_RUN
	char s_terminal_no[32] = {0};
	device_config_get(&dev_conf);
	sprintf(s_terminal_no, "%d", dev_conf.nconf.term_id);
	__terminal_no_file_put(s_terminal_no);
#endif  

	device_config_get(&dev_conf);
	ufile_ctrl_info_init();


	/* �ӿڰ���� */  
	ext_iboard_LED4(IBOARD_CTRL_OFF);
	ext_iboard_light_box(IBOARD_CTRL_OFF);

	unsigned char tm[7] = {0};  //[1]=HH, [0]=MM
	lib_get_systime(&tm); ////YYYY MM DD HH mm SS
	unsigned char ctrl_time_enable_hh = dev_conf.nconf.light_ctrl_time_enable[1];
	unsigned char ctrl_time_disable_hh = dev_conf.nconf.light_ctrl_time_disable[1];

	if(((tm[4] >= ctrl_time_enable_hh) && (tm[4] <= 24)) || \
		((tm[4] >= 0) && (tm[4] < ctrl_time_disable_hh))) //����
	{
		ext_iboard_LED4(IBOARD_CTRL_ON);
		ext_iboard_light_box(IBOARD_CTRL_ON);

		system("echo 70 > /sys/class/backlight/pwm-backlight/brightness");
	}
	else
	{
		ext_iboard_LED4(IBOARD_CTRL_OFF);
		ext_iboard_light_box(IBOARD_CTRL_OFF);
		
		system("echo 100 > /sys/class/backlight/pwm-backlight/brightness");
	}
	

	
	g_bicycle_wl = lib_wl_new();  //�������ӿ�
	if(g_bicycle_wl == LIB_WL_NULL)
	{
		SYS_LOG_ERR("lib wl new failed!");
		goto err;
	}

	lib_set_ifaddr("eth1", dev_conf.nconf.local_ip);
	lib_set_ifnetmask("eth1", dev_conf.nconf.local_submask);

	lib_set_ifaddr("eth0", dev_conf.nconf.wifi_ip);
	lib_set_ifnetmask("eth0", dev_conf.nconf.wifi_submask);
	
	if(dev_conf.nconf.using_wireless == 1)  //ʹ����������
	{
		#if 0
		strcpy(loadserver1, lib_iaddr_to_saddr(dev_conf.nconf.load_server1_ip));
		strcpy(loadserver2, lib_iaddr_to_saddr(dev_conf.nconf.load_server2_ip));

		fprintf(stderr, "load server1: %s, load server2: %s\n", loadserver1, loadserver2);
		
		lib_ppp0_add_route(loadserver1, 0);  //��ӵ������������̬·��
		lib_ppp0_add_route(loadserver2, 0);
		#endif

		lib_wl_set_model(g_bicycle_wl, WL_NETWORK_MODEL_WIRELESS, 300); //����
	}
	else if(dev_conf.nconf.using_wireless == 2) //ʹ����������
	{
		lib_set_ifgateway("eth1", dev_conf.nconf.local_gateway);
		lib_wl_set_model(g_bicycle_wl, WL_NETWORK_MODEL_WIRED, 300);  //����
	}

	__boot_version_fgets(dev_conf.nconf.boot_ver);
	__kernel_version_fgets(dev_conf.nconf.kernel_ver);
	__rootfs_version_fgets(dev_conf.nconf.rootfs_ver);
	__fw_version_fgets(dev_conf.nconf.fw_ver); 
	__appl_version_fgets(dev_conf.nconf.appl_ver);
	device_config_put(&dev_conf);

	var_data_init();   		//�ɱ����ݽṹ
	ndev_info_init();		//�ڵ����Ϣ��ʼ�� 
	ninfo.terminal_no = dev_conf.nconf.term_id;  //�ն˱��
	ninfo.network_type = dev_conf.nconf.using_wireless; //��������
	ndev_info_put(&ninfo);   
	
	unity_file_handle_init(); //���ļ���ʼ�� (��ʼ�������������������)
	stake_init();			//��׮��ʼ��
	ndev_init();			//�ڵ����ʼ��

	gui_init();  		//�����ʼ��
	gui_loop(); 		//while(1){}

err:   
	gui_destroy();
	ndev_destroy();
	stake_destroy();
	var_data_destroy();
	device_config_destroy();
	lib_wl_destroy(g_bicycle_wl);
	lib_wdt_release();
	unity_file_handle_release();
	
#ifdef BICYCLE_LOG_RUN
	closelog();
#endif
	fprintf(stderr, "BICYCLE Software Quit!\n");

	return 0;
}





