#ifndef __UNITY_FILE_HANDLE_H__
#define __UNITY_FILE_HANDLE_H__



#include "lib_unity_file.h"

struct unity_file_struct
{
	unsigned short file_seq;		 //文件序号
	unsigned short total;		//分割总数
	unsigned short div_seq;		//分割序号

	int status;			//状态	1:正在下载, 2:正在广播
	time_t start_time;  		 //开始下载时间
	time_t end_time;		//结束下载时间
}__attribute__((packed));

struct unity_file_info
{
	struct unity_file_struct ndev_ufile_st[6];  //节点机下载
	struct unity_file_struct broadcast_ufile_st[6];  //广播泛文件
	struct unity_file_struct stake_ufile_st[6];  //锁桩下载  
}__attribute__((packed));
typedef struct unity_file_info unity_file_info_t;





void unity_file_handle_init(void);
void unity_file_handle_release(void);

void unity_file_hd_get(unity_file_hd_t *hd, const unsigned char type);
void unity_file_hd_put(unity_file_hd_t *hd, const unsigned char type);
void unity_file_atmoic_set_val(const unsigned char type, const unsigned int val);
unsigned int unity_file_atmoic_get_val(const unsigned char type);
int unity_file_notify_put(const unsigned int notify, unity_file_hd_t *hd, const unsigned char utype);
int unity_file_notify_timedwait(const unsigned int sec, unsigned int *notify, unity_file_hd_t *hd,  const unsigned char utype);
int unity_file_request_handle(unity_file_hd_t *hd);
int unity_file_except_handle(void);
int unity_file_handle_to_ndev(void *ptr, const unsigned int size);
int unity_file_handle_to_sae(void *ptr, const unsigned int size);

int unity_file_info_put(unity_file_info_t *info);
int unity_file_info_get(unity_file_info_t *info);
int unity_file_info_struct_ndev_put(struct unity_file_struct *file_struct, const enum UF_TYPE utype); 
int unity_file_info_struct_ndev_get(struct unity_file_struct *file_struct, const enum UF_TYPE utype);
int unity_file_info_struct_broadcast_put(struct unity_file_struct *file_struct, const enum UF_TYPE utype); 
int unity_file_info_struct_broadcast_get(struct unity_file_struct *file_struct, const enum UF_TYPE utype);
int unity_file_info_struct_broadcast_put_div_seq(const unsigned short div_seq, const enum UF_TYPE utype); 

int unity_file_info_struct_stake_put(struct unity_file_struct *file_struct, const enum UF_TYPE utype); 
int unity_file_info_struct_stake_get(struct unity_file_struct *file_struct, const enum UF_TYPE utype);
int unity_file_info_struct_stake_put_div_seq(const unsigned short div_seq, const enum UF_TYPE utype); 

void unity_file_config_printf(void);

int unity_file_info_struct_printf(struct unity_file_struct *file_struct);







#endif

