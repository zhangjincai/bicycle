#ifndef __UPGRADE_H__
#define __UPGRADE_H__



struct upgrade_config
{
	int update_type;
	char username[32];
	char passwd[32];
	char firmware_name[32];
	char remote_path[64];
	char local_path[64];
	long  connect_timeout;
};


int upgrade_init(void);
void upgrade_release(void);
void *upgrade_handle_run(void *arg);



#endif


