#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <curl/curl.h>  
#include <sys/stat.h>

#include "lib_general.h"
#include "lib_upgrade.h"


#define UPE_DATA_SZ				512   //消息队列数据最大为8192字节
#define UPE_MSG_KEY				0x2015
#define UPE_MSG_APP				0x01


enum UPE_MSG
{
	UPE_CMD_SET_FTP_DOWNLOAD_ENABLE = 1,
	UPE_CMD_SET_FTP_DOWNLOAD_DISABLE,
	UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT,
	UPE_CMD_SET_UPGRADE_START,
	UPE_CMD_SET_FTP_CONFIG,
	UPE_CMD_SET_NETWORK_STAT,
	UPE_CMD_SET_FDNL_CTRL_INFO,

	
	UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT,
	UPE_CMD_GET_FTP_CONFIG,
	UPE_CMD_GET_FIRMWARE_CONFIG,
	UPE_CMD_GET_FTP_UPSTAT,
	UPE_CMD_GET_FDNL_CTRL_INFO,
	
	UPE_CMD_END
};

struct upgrade_message
{
	long d_addr;
	unsigned char cmd;
	int s_addr;
	int result;
	unsigned char data[UPE_DATA_SZ];
}__attribute__((packed));
typedef struct upgrade_message upgrade_message_t;

struct lib_upgrade
{
	int qid;
	unsigned int msg_type;
	struct ftp_config ftp;
}__attribute__((packed));







lib_upgrade_t *lib_upgrade_create(const unsigned int msg_type)
{
	lib_upgrade_t *up = (lib_upgrade_t *)malloc(sizeof(lib_upgrade_t));
	if(up == NULL)
		return LIB_UPE_NULL;

	int err = LIB_UPE_ERROR;

	up->msg_type = msg_type;
	err = lib_msg_init(&(up->qid), UPE_MSG_KEY);
	if(err != LIB_GE_EOK)
	{
		free(up);
		up = NULL;
		return LIB_UPE_NULL;
	}

	fprintf(stderr, "lib upgrade qid: %d\n", up->qid);

	return up;
}

void lib_upgrade_destroy(lib_upgrade_t *up)
{
	if(up != NULL)
	{
		//lib_msg_kill(up->qid);
		
		free(up);
		up = NULL;

		fprintf(stderr, "%s\n", __FUNCTION__);
	}
}

int lib_upgrade_set_ftp_download_timeout(lib_upgrade_t *up, const int timeout)
{
	if(up == NULL)
		return LIB_UPE_ERROR;

	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_SET_FTP_DOWNLOAD_TIMEOUT;
	msg.result = LIB_UPE_EOK;
	memcpy(&(msg.data), &timeout, sizeof(timeout));

	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
			return LIB_UPE_EOK;
	}
	
	return LIB_UPE_ERROR;
}

int lib_upgrade_get_ftp_download_timeout(lib_upgrade_t *up, int *timeout)
{
	if((up == NULL) || (timeout == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;
	int ftp_timeout = 0;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_GET_FTP_DOWNLOAD_TIMEOUT;
	msg.result = LIB_UPE_EOK;

	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			memcpy(&ftp_timeout, &(msg.data), sizeof(ftp_timeout));
			*timeout = ftp_timeout;
			return LIB_UPE_EOK;
		}
	}

	return LIB_UPE_ERROR;
}

int lib_upgrade_set_ftp_download_switch(lib_upgrade_t *up, enum UPE_FTP_DOWNLOAD_SWITCH sw)
{
	if(up == NULL)
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;
	unsigned char cmd = 0xff;
	memset(&msg, 0, sizeof(upgrade_message_t));

	switch(sw)
	{
		case UPE_FTP_DOWNLOAD_SW_ON:
		{
			cmd = UPE_CMD_SET_FTP_DOWNLOAD_ENABLE;
		}
		break;

		case UPE_FTP_DOWNLOAD_SW_OFF:
		{
			cmd = UPE_CMD_SET_FTP_DOWNLOAD_DISABLE;
		}
		break;	
	}

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = cmd;
	msg.result = LIB_UPE_EOK;

	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			return LIB_UPE_EOK;
		}
	}

	return LIB_UPE_ERROR;	
}

int lib_upgrade_set_upgrade_start(lib_upgrade_t *up, enum UPE_TYPE type)
{
	if(up == NULL)
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_SET_UPGRADE_START;
	msg.result = LIB_UPE_EOK;
	msg.data[0] = type;
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			return LIB_UPE_EOK;
		}
	}

	return LIB_UPE_ERROR;	
}

int lib_upgrade_set_ftp_config(lib_upgrade_t *up, struct ftp_config *ftp)
{
	if((up == NULL) || (ftp == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_SET_FTP_CONFIG;
	msg.result = LIB_UPE_EOK;
	memcpy(&(msg.data), ftp, sizeof(struct ftp_config));
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			return LIB_UPE_EOK;
		}
	}	

	return LIB_UPE_ERROR;
}

int lib_upgrade_get_ftp_config(lib_upgrade_t *up, struct ftp_config *ftp)
{
	if((up == NULL) || (ftp == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_GET_FTP_CONFIG;
	msg.result = LIB_UPE_EOK;
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			memcpy(ftp, &(msg.data), sizeof(struct ftp_config));
			return LIB_UPE_EOK;
		}
	}	

	return LIB_UPE_ERROR;
}

int lib_upgrade_get_firmware_config(lib_upgrade_t *up, enum UPE_FIRMWARE_TYPE type, struct firmware_config *firmware)
{
	if((up == NULL) || (firmware == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_GET_FIRMWARE_CONFIG;
	msg.result = LIB_UPE_EOK;
	msg.data[0] = type;
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			memcpy(firmware, &(msg.data), sizeof(struct firmware_config));
			return LIB_UPE_EOK;
		}
	}	

	return LIB_UPE_ERROR;	
}

int lib_upgrade_get_upstat(lib_upgrade_t *up, enum UPE_FIRMWARE_TYPE type, struct ftp_upgrade_status *status)
{
	if((up == NULL) || (status == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_GET_FTP_UPSTAT;
	msg.result = LIB_UPE_EOK;
	msg.data[0] = type;
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			memcpy(status, &(msg.data), sizeof(struct ftp_upgrade_status));
			return LIB_UPE_EOK;
		}
	}	

	return LIB_UPE_ERROR;		
}

int lib_upgrade_set_ftp_dnl_ctrl_info(lib_upgrade_t *up, struct ftp_download_ctrl_info *fdnl_ctrl_info)
{
	if((up == NULL) || (fdnl_ctrl_info == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_SET_FDNL_CTRL_INFO;
	msg.result = LIB_UPE_EOK;
	memcpy(&(msg.data), fdnl_ctrl_info, sizeof(struct ftp_download_ctrl_info));
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			return LIB_UPE_EOK;
		}
	}	

	return LIB_UPE_ERROR;
}

int lib_upgrade_get_ftp_dnl_ctrl_info(lib_upgrade_t *up, struct ftp_download_ctrl_info *fdnl_ctrl_info)
{
	if((up == NULL) || (fdnl_ctrl_info == NULL))
		return LIB_UPE_ERROR;
	
	int n = LIB_UPE_ERROR;
	upgrade_message_t msg;

	memset(&msg, 0, sizeof(upgrade_message_t));

	msg.d_addr = UPE_MSG_APP;
	msg.s_addr = up->msg_type;
	msg.cmd = UPE_CMD_GET_FDNL_CTRL_INFO;
	msg.result = LIB_UPE_EOK;
	
	lib_msg_send(up->qid, &msg, sizeof(upgrade_message_t));
	memset(&msg, 0, sizeof(upgrade_message_t));
	n = lib_msg_recv(up->qid, &msg, sizeof(upgrade_message_t), up->msg_type);
	if(n > 0)
	{
		if(msg.result == LIB_UPE_EOK)
		{
			memcpy(fdnl_ctrl_info, &(msg.data), sizeof(struct ftp_download_ctrl_info));
			return LIB_UPE_EOK;
		}
	}	

	return LIB_UPE_ERROR;			
}







