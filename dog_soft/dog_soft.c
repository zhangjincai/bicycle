#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
 
#include "lib_general.h"

/* ��־���� */
#define DOG_LOG_RUN

#ifdef DOG_LOG_RUN
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

#define PROCESS_CMD					"ps -e"
#define FREE_MEMORY_MIN_SZ			(20 * 1024) //KB
#define TERMINAL_NO_PATH			"/opt/config/terminal_no_file.txt"

static void __sigint(int sig)
{
	fprintf(stderr, "dog soft signal: %d\n", sig);
}

static void __signals_init(void)
{
#if 0
	struct sigaction sa;

	signal(SIGPIPE, SIG_IGN); //�ܵ��ر�
	signal(SIGCHLD, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	
	signal(SIGTTOU, SIG_IGN); //��̨����д�����ն�
	signal(SIGTTIN, SIG_IGN); //��̨���̶������ն�
	signal(SIGTSTP, SIG_IGN); //�ն˹���
	
	
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
	sigaddset(&sa.sa_mask, SIGPIPE); //�ܵ�����
    sigaddset(&sa.sa_mask, SIGCHLD); //�ӽ��̽���ʱ, �����̻��յ�����ź�
   	sigaddset(&sa.sa_mask, SIGINT); //������ֹ�ź�(ͨ����Ctrl-C)
	sigaddset(&sa.sa_mask, SIGHUP); //���ź����û��ն�����(�����������)����ʱ����
	sigaddset(&sa.sa_mask, SIGTERM); //�������(terminate)�ź�,���Ա������ʹ���
	sigaddset(&sa.sa_mask, SIGTTOU); //������SIGTTIN, ����д�ն�(���޸��ն�ģʽ)ʱ�յ�
	sigaddset(&sa.sa_mask, SIGTTIN); //����̨��ҵҪ���û��ն˶�����ʱ, ����ҵ�е����н��̻��յ�SIGTTIN�ź�. ȱʡʱ��Щ���̻�ִֹͣ��
	sigaddset(&sa.sa_mask, SIGTSTP); //ֹͣ���̵�����(ͨ����Ctrl-Z)

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

	fp = fopen(TERMINAL_NO_PATH, "rt");  /* ���ı� */
	if(fp != NULL)
	{
		if(fgets(s_terminal_no, sizeof(s_terminal_no), fp) != NULL)
		{
			fprintf(stderr, "DogSoft Terminal NO get: %s\n", s_terminal_no);
			
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
	int err = LIB_GE_ERROR;
	FILE *fp = NULL;
	char c_string[512] = {0};
	char c_kill[64] = {0};
	int iwatchdog = 0, iwireless = 0, iupgrade_app = 0, ibicycle_gui = 0, ibicycle = 0;
	unsigned int free_memory_size = 0;
	static unsigned int count = 0;
	char *p1, *p2;
	
	//daemon(0, 1);  
		
	__signals_init();

	/* ��ʼ��syslog��־*/
#ifdef DOG_LOG_RUN
	char log_ident[64] = {0};
	char terminal_no[32] = {0};
	char macaddr[8] = {0};
	char s_macaddr[16] = {0};
	
	lib_get_macaddr("eth1", macaddr);
	lib_hex_to_str((unsigned char *)macaddr, 6, (unsigned char *)s_macaddr);
	__terminal_no_file_get(terminal_no);
	
	sprintf(log_ident, "------[DOGSOFT]-[%s]:[%s]", s_macaddr, terminal_no);

	fprintf(stderr, "dogsoft log ident: %s\n", log_ident);
	
	openlog(log_ident, LOG_NDELAY, LOG_DAEMON); //Open the connection immediately
#endif	

	SYS_LOG_NOTICE("dog soft software compiled time: %s, %s\n",__DATE__, __TIME__);

	while(1)
	{
		iwatchdog = 0;
		iwireless = 0;
		iupgrade_app = 0;  
		ibicycle_gui = 0;
		ibicycle = 0;
		
		fp = popen(PROCESS_CMD, "r"); //ps -e:��ʾ���н���
		if(fp != NULL)
		{
			p1 = NULL;
			p2 = NULL;
			memset(c_string, 0, sizeof(c_string));
			memset(c_kill, 0, sizeof(c_kill));
			while(!feof(fp))
			{
				fgets(c_string, sizeof(c_string) - 1, fp);
				
				if(strstr(c_string, "watchdog")) //���Ź�
				{
					iwatchdog = 1;
					continue;
				}

				if(strstr(c_string, "wireless")) //����
				{
					iwireless= 1;
					continue;
				}
				
				if(strstr(c_string, "wireless_4g")) //���� 4G
				{
					iwireless= 1;
					continue;
				}
				
				if(strstr(c_string, "upgrade_app")) //����
				{
					iupgrade_app = 1;
					continue;
				}

				if(strstr(c_string, "bicycle_gui"))  //����
				{
					ibicycle_gui = 1;
					continue;
				}

				if(strstr(c_string, "bicycle"))  //���г�����
				{
					ibicycle = 1;
					continue;
				}

				/* ���ҷǷ�����, ��ɱ�� */
				if((p1 = strstr(c_string, "./")) != NULL)	
				{
					if(p1 != NULL)
					{
						p2 = p1 + strlen("./");
						if(p2 != NULL)
						{
							strcpy(c_kill, "killall -9 ");
							strcat(c_kill, p2);
							system(c_kill); //ɱ������
							SYS_LOG_DEBUG("%s", c_kill);
							SYS_LOG_DEBUG("%s", c_string);
						}
					}
				}
			}

			fclose(fp);

			/* ����Ƿ��кϷ������˳������ֽ����˳�,�豸���� */
			if((iwatchdog == 0) || (iwireless == 0) || (iupgrade_app == 0) || (ibicycle_gui == 0) || (ibicycle == 0))
			{
				fprintf(stderr, "Dog Soft: iwatchdog = %d, iwireless = %d, iupgrade_app = %d, ibicycle_gui = %d, ibicycle = %d\n", iwatchdog, iwireless, iupgrade_app, ibicycle_gui, ibicycle);

				SYS_LOG_DEBUG("Dog Soft: iwatchdog = %d, iwireless = %d, iupgrade_app = %d, ibicycle_gui = %d, ibicycle = %d", iwatchdog, iwireless, iupgrade_app, ibicycle_gui, ibicycle);


				system("reboot");  //�����豸
				system("/mnt/firmware/reboot_wdt");  //�����豸
			}
		
			//fprintf(stderr, "Dog Soft: iwatchdog = %d, ilogdb = %d, iwireless = %d, iupgrade_app = %d, ibicycle_gui = %d, ibicycle = %d\n", iwatchdog, ilogdb, iwireless, iupgrade_app, ibicycle_gui, ibicycle);
		}

#if 1
		/* �������ڴ� */
		err = lib_get_free_meminfo(&free_memory_size);
		if(err == LIB_GE_EOK)
		{
			//fprintf(stderr, "Dog Soft: free memory size = %d KB\n", free_memory_size);
			
			if(free_memory_size < FREE_MEMORY_MIN_SZ)
			{
				fprintf(stderr, "Dog Soft: free memory size = %d KB, less than %d KB", free_memory_size, FREE_MEMORY_MIN_SZ);
				
				SYS_LOG_DEBUG("Dog Soft: free memory size = %d KB, less than %d KB", free_memory_size, FREE_MEMORY_MIN_SZ);

				free_memory_size = 0;

				system("reboot");  //�����豸
				system("/mnt/firmware/reboot_wdt");  //�����豸
			}
		}
#endif

		lib_sleep(30);
		count++;

#if 0
		if(count >= 18)
			system("/mnt/firmware/reboot_wdt");  //�����豸
#endif

	}
	
#ifdef DOG_LOG_RUN
	closelog();
#endif
	
	return 0;
}


