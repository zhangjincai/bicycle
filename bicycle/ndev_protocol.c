#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "configs.h"
#include "lib_general.h"
#include "lib_zmalloc.h"
#include "device_config.h"
#include "var_data.h"
#include "universal_file.h"
#include "utils.h"
#include "sae_info.h"
#include "ndev_info.h"
#include "database.h"
#include "unity_file_handle.h"
#include "ndev_protocol.h"

#include "lib_lnt.h" //获取岭南通读卡器固件版本
#include "lib_upgrade.h"



#define NDEV_PRO_DBG_DEF

#ifdef NDEV_PRO_DBG_DEF
#define PRO_DBG(fmt, args...)	 do{ \
	fprintf(stderr, "DEBUG: %s:%d, ", __FUNCTION__,__LINE__); \
	fprintf(stderr, fmt, ##args); \
}while(0)
#else
#define PRO_DBG(fmt, args...)
#endif



extern ndev_ftp_download_ctrl_info_t g_ftp_dnl_ctrl_info; 
extern lib_unity_file_t *g_unity_file_db;

extern lib_upgrade_t *g_upgrade; 



/* 获取岭南通读卡器固件版本号 add by zjc at 2016-08-08 */
#define LNT_FIRMWARE_PATH			"/opt/config/lnt_firmware_version_file.txt"

static int __lnt_firmware_version_fgets(char version[24])
{
	char s_version[32] = {0};
	FILE *fp = NULL;
	int s_len = 0;
	
	fp = fopen(LNT_FIRMWARE_PATH, "rb");
	if(fp == NULL)
		return B_DEV_ERROR;       

	if(fgets(s_version, sizeof(s_version), fp) != NULL)
	{
		//fprintf(stderr, "Bicycle, __lnt_firmware_version_fgets: %s\n", s_version);
		s_len = strlen(s_version);
		//printf("s_len:%d\n", s_len); //24
		strcpy(version, s_version);
		version[s_len] = '\0';
	}

	fclose(fp);
	
	return B_DEV_EOK;
}


/*
 * 负载均衡请求
 */
int ndev_protocol_load_req(void *ptr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	n_data_attr_t attr;
	n_var_data_t *var = NULL;
	device_config_t conf;
	load_balance_req_t load;

	memset(&attr, 0, sizeof(n_data_attr_t));
	memset(&conf, 0, sizeof(device_config_t));
	memset(&load, 0, sizeof(load_balance_req_t));

	var = n_var_data_alloc();
	if(var != NULL)
	{
		memset(var, 0, sizeof(n_var_data_t));
		
		lib_get_macaddr("eth1", load.mac_addr);
		device_config_get(&conf);
		load.term_id = htons(conf.nconf.term_id); //终端 编号

		load.operators = htons(1); //运营商
		load.dev_ver = htons(1); //设备类型

		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_LOAD_REQ, SAE_CAN_LOCAL_ID, &attr);
		n_var_data_set(var, NDEV_LOAD_W, 2);
		n_var_data_b64_set(var, &load, sizeof(load_balance_req_t));  
		n_var_data_len(var);
		n_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

/*
 * 签到请求
 */
int ndev_protocol_reg_req(void *ptr)
{
	if(ptr == NULL)
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	n_data_attr_t attr;
	n_var_data_t *var = NULL;
	ndev_info_t info;
	ndev_reg_req_t reg;

	memset(&attr, 0, sizeof(n_data_attr_t));
	memset(&info, 0, sizeof(ndev_info_t));
	memset(&reg, 0, sizeof(ndev_reg_req_t));
	
	var = n_var_data_alloc();
	if(var != NULL)
	{
		ndev_info_get(&info);
		lib_get_macaddr("eth1", reg.mac_addr);
		memcpy(&(reg.encry_key), &(info.load.encry_key), 16);

		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_REGISTER_REQ, SAE_CAN_LOCAL_ID, &attr);
		n_var_data_set(var, NDEV_REG_W, 2);
		n_var_data_b64_set(var, &reg, sizeof(ndev_reg_req_t));  
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;
}

/*
 * 心跳级别1
 */
int ndev_protocol_bheart_level1_req(void *ptr, void *limit)
{
	//printf("-----------ndev_protocol_bheart_level1_req\n");
	if((ptr == NULL) || (limit == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;
	unity_file_config_t uconfig;
	device_config_t dev_conf;
	char s_time[13] = {0};
	unsigned short  n_stat;
	unsigned short __sstat, s_stat;
	unsigned char quantity = 0;
	unsigned char online_count = 0;
	char s_quantity[8] = {0};
	char s_online_count[8] = {0};
	unsigned char yct_device_stat = 0x00;
	unsigned char s_blk_attr[64] = {0};
	sae_info_t *sinfo = NULL;
	sae_attribute_t sattr;
	sae_status_t sstat[64];
	static unsigned char s_sae_id = 1;   
	int i = 0;

	
	ndev_bheart_limit_t *p_limit = (ndev_bheart_limit_t *)limit;

	memset(&dev_conf, 0, sizeof(device_config_t));
	memset(&attr, 0, sizeof(n_data_attr_t));
	memset(&sattr, 0, sizeof(sae_attribute_t));
	memset(&uconfig, 0, sizeof(unity_file_config_t));
	memset(&sstat, 0, sizeof(sae_status_t) * 64);
	
	var = n_var_data_alloc();
	if(var != NULL)
	{
		device_config_get(&dev_conf);
		utils_get_sys_time_s(s_time);
		lib_unity_file_config_select_data(g_unity_file_db, &uconfig); //读取泛文件配置
		
		quantity = dev_conf.sconf.quantity;

		if(p_limit->sae_bl_attr == 1) //锁桩黑名单属性[1]
		{
			sae_blk_attr_t blk_attr;
			for(i = 1; i <= quantity; i++ )
			{
				memset(&blk_attr, 0, sizeof(sae_blk_attr_t));
				
				sinfo = sae_info_get_by_id(i);
				if(sinfo != NULL)
				{
					if(sinfo->healthy_flag == 1)  //锁桩注册标志
					{
						blk_attr.total_va = 1;
						blk_attr.inc_va = 1;
						blk_attr.dec_va = 1;
						blk_attr.temp_va = 1;
					}

					if(strncmp((char *)uconfig.total_bl_last_ver,  "00000000", 8) == 0) //检查版本是否默认值
						blk_attr.total_co = 0;
					else
					{
						if(strncmp((char *)uconfig.total_bl_last_ver, (char *)sinfo->s_total_bl_ver, 8) == 0)  //相等
							blk_attr.total_co = 1; 
						else
							blk_attr.total_co = 0;
					}

					if(strncmp((char *)uconfig.inc_bl_last_ver,  "00000000", 8) == 0)
						blk_attr.inc_co = 0;
					else
					{
						if(strncmp((char *)uconfig.inc_bl_last_ver, (char *)sinfo->s_inc_bl_ver, 8) == 0) 
							blk_attr.inc_co = 1;
						else
							blk_attr.inc_co = 0;
					}

					if(strncmp((char *)uconfig.dec_bl_last_ver,  "00000000", 8) == 0)
						blk_attr.dec_co = 0;
					else
					{
						if(strncmp((char *)uconfig.dec_bl_last_ver, (char *)sinfo->s_dec_bl_ver, 8) == 0) 
							blk_attr.dec_co = 1;
						else
							blk_attr.dec_co = 0;

					}

					if(strncmp((char *)uconfig.temp_bl_last_ver,  "00000000", 8) == 0)
						blk_attr.temp_co = 0;
					else
					{
						if(strncmp((char *)uconfig.temp_bl_last_ver, (char *)sinfo->s_temp_bl_ver, 8) == 0)
							blk_attr.temp_co = 1;
						else
							blk_attr.temp_co = 0;
					}
					
					memcpy(&s_blk_attr[i-1], &blk_attr, sizeof(sae_blk_attr_t));

					sinfo = NULL;
				}
			}
		}
		
		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_REGISTER_REQ, SAE_CAN_LOCAL_ID, &attr);

		if(p_limit->ndev_stat == 1)  //节点机状态
		{
			ndev_info_stat_NOR_get(&n_stat);

		//	lib_printf("---n_stat---", &n_stat, 2);
			
			n_var_data_set(var, NDEV_STAT_W, 2);				
			n_var_data_b64_set(var, &n_stat, 2);  
			n_var_data_char(var, '&');
		}

		if(p_limit->ndev_time == 1)  //节点机时间
		{
			n_var_data_set(var, NDEV_N_TIME_W, 2);			
			n_var_data_set(var, s_time, 12);
			n_var_data_char(var, '&');
		}

		if(p_limit->sae_quantity == 1)  //桩机总数量
		{
			quantity =  dev_conf.sconf.quantity; 				
			sprintf(s_quantity, "%03d", quantity); /* 3个字符 */
			n_var_data_set(var, NDEV_S_TOTAL_NUM_W, 2);
			n_var_data_set(var, s_quantity, 3);
			n_var_data_char(var, '&');
		}

		if(p_limit->sae_online == 1)  //桩机在线数量
		{
			online_count = sae_info_get_online_count();  		
			sprintf(s_online_count, "%03d", online_count);  /* 3个字符 2016-0320*/
			n_var_data_set(var, NDEV_S_ONLINE_NUM_W, 2);
			n_var_data_set(var, s_online_count, 3);
			n_var_data_char(var, '&');
		}

		if(p_limit->total_bl_ver == 1)  //总量黑名单版本
		{
			n_var_data_set(var, NDEV_TOTAL_BLK_VER_W, 2);	
			n_var_data_set(var, &(uconfig.total_bl_last_ver), 8);
			n_var_data_char(var, '&');
		}

		if(p_limit->inc_bl_ver == 1)  //增量黑名单版本
		{
			n_var_data_set(var, NDEV_INC_BLK_VER_W, 2);		
			n_var_data_set(var, &(uconfig.inc_bl_last_ver), 8);
			n_var_data_char(var, '&');
		}

		if(p_limit->dec_bl_ver == 1)  //减量黑名单版本
		{
			n_var_data_set(var, NDEV_DEC_BLK_VER_W, 2);			
			n_var_data_set(var, &(uconfig.dec_bl_last_ver), 8);
			n_var_data_char(var, '&');
		}

		if(p_limit->temp_bl_ver == 1)  //临时黑名单版本
		{
			n_var_data_set(var, NDEV_TEMP_BLK_VER_W, 2);		
			n_var_data_set(var, &(uconfig.temp_bl_last_ver), 8);
			n_var_data_char(var, '&');
		}

		if(p_limit->sae_bl_attr == 1) //桩机黑名单属性[1]
		{
			n_var_data_set(var, NDEV_S_BLK_ATTR_W, 2);			
			n_var_data_b64_set(var, s_blk_attr, sizeof(s_blk_attr));  
			n_var_data_char(var, '&');
		}

		if(p_limit->lnt_card_stat == 1)  //羊城通读卡器状态
		{
			n_var_data_set(var, NDEV_YCT_STAT_W, 2);			
			n_var_data_b64_set(var, &yct_device_stat, sizeof(yct_device_stat));  
			n_var_data_char(var, '&');
		}

		if(p_limit->sae_attr == 1) //桩机属性 
		{
			for(i = s_sae_id; i <= quantity; i++ ) 
			{
				memset(&sattr, 0, sizeof(sae_attribute_t));

				sinfo = sae_info_get_by_id(i);
				if(sinfo != NULL)
				{
					if(sinfo->healthy_flag == 1)
					{
						sattr.id = i;		
						sattr.sn = atoi(sinfo->s_sn);		//桩机最新流水号
						utils_string_to_bcd(sinfo->s_phy_sn, sattr.physn);		//桩机物理编号
						utils_string_to_bcd(sinfo->s_psam, sattr.psam);			//PSAM卡编号
						utils_string_to_bcd(sinfo->s_ver, sattr.version);			//桩机版本号
						utils_string_to_bcd(sinfo->s_time,  sattr.time); //桩机时间
						utils_time_to_bcd_yymmddhhMMss(&(sinfo->s_reg_time), sattr.register_time);  //桩机注册时间
						sattr.lock_id = atoi(sinfo->s_lock_id);		//锁控编号
						utils_string_to_bcd(sinfo->s_lock_ver, sattr.lock_version);		 //锁控版本号
						utils_string_to_bcd(sinfo->s_total_bl_ver, sattr.total_blk_ver);	//总量黑名单版本号
						utils_string_to_bcd(sinfo->s_inc_bl_ver, sattr.inc_blk_ver);		//增量黑名单版本号
						utils_string_to_bcd(sinfo->s_dec_bl_ver, sattr.dec_blk_ver);	//减量黑名单版本号
						utils_string_to_bcd(sinfo->s_temp_bl_ver, sattr.temp_blk_ver);	//临时黑名单版本号
						memcpy(&(sattr.stat), &(sinfo->status), 2);					//桩机状态
		
						s_sae_id = i;
						s_sae_id++;
						
						break;
					}
				}
			}

			if(i > quantity) //回转 //?
			{
				s_sae_id = 1;
				for(i = s_sae_id; i <= quantity; i++ ) 
				{
					memset(&sattr, 0xfb, sizeof(sae_attribute_t));

					sinfo = sae_info_get_by_id(i);
					if(sinfo != NULL)
					{
						if(sinfo->healthy_flag == 1)
						{
							sattr.id = i;		//桩机编号
							sattr.sn = atoi(sinfo->s_sn);		//桩机最新流水号
							utils_string_to_bcd(sinfo->s_phy_sn, sattr.physn);		//桩机物理编号
							utils_string_to_bcd(sinfo->s_psam, sattr.psam);			//PSAM卡编号
							utils_string_to_bcd(sinfo->s_ver, sattr.version);			//桩机版本号
							utils_string_to_bcd(sinfo->s_time,  sattr.time); //桩机时间
							utils_time_to_bcd_yymmddhhMMss(&(sinfo->s_reg_time), sattr.register_time);  //桩机注册时间
							sattr.lock_id = atoi(sinfo->s_lock_id);		//锁控编号
							utils_string_to_bcd(sinfo->s_lock_ver, sattr.lock_version);		 //锁控版本号
							utils_string_to_bcd(sinfo->s_total_bl_ver, sattr.total_blk_ver);	//总量黑名单版本号
							utils_string_to_bcd(sinfo->s_inc_bl_ver, sattr.inc_blk_ver);		//增量黑名单版本号
							utils_string_to_bcd(sinfo->s_dec_bl_ver, sattr.dec_blk_ver);	//减量黑名单版本号
							utils_string_to_bcd(sinfo->s_temp_bl_ver, sattr.temp_blk_ver);	//临时黑名单版本号
							memcpy(&(sattr.stat), &(sinfo->status), 2);					//桩机状态

							s_sae_id = i;
							s_sae_id++;
			
							break;
						}
					}
				}
			}

			memcpy(&__sstat, &(sattr.stat), 2);
			s_stat = htons(__sstat);
			memcpy(&sattr.stat, &s_stat, 2);
			n_var_data_set(var, NDEV_S_ATTR_W, 2);	 
			n_var_data_b64_set(var, &sattr, sizeof(sae_attribute_t));  
			n_var_data_char(var, '&');
		}

		if(p_limit->all_sae_stat == 1)  //全部桩机状态
		{
			for(i = 1; i <= quantity; i++ )   
			{
				sinfo = sae_info_get_by_id(i);
				if(sinfo != NULL)
				{
					if(sinfo->healthy_flag == 1)  //锁桩注册标志
					{
						memcpy(&__sstat, &(sinfo->status), sizeof(sae_status_t));
						s_stat = htons(__sstat);

						//fprintf(stderr, "-------1--------ID:%d-0x%04x\n", i, s_stat); //0500
						//fprintf(stderr, "-------2--------ID:%d-0x%04x\n", i, __sstat); //0005						
						
						memcpy(&sstat[i], &s_stat, 2); //sstat[64]不会空间不够?
					}
					else
					{
						s_stat = 0xFFFF;  //没有签到的桩,默认状态
						memcpy(&sstat[i], &s_stat, 2);
					}
					
					sinfo = NULL;
				}
			}
			n_var_data_set(var, NDEV_S_ALL_STAT, 2);	 
			n_var_data_b64_set(var, &sstat, sizeof(sae_status_t) * 64);  
			n_var_data_char(var, '&');
		}

		if(p_limit->sae_online_mark == 1)  //桩机在线标志
		{
			unsigned char s_stat[17] = {0};
			unsigned char sae_online_f[9] = {0};
			sae_info_get_stat_for_center(&s_stat);
			memcpy(sae_online_f, s_stat, 9); 

			//lib_printf("----------------SAE ONLINE F", sae_online_f, 9);
			
			n_var_data_set(var, NDEV_S_ONLINE_F, 2);	 
			n_var_data_b64_set(var, sae_online_f, 9);  
			n_var_data_char(var, '&');
		}

		if(p_limit->ndev_fw_ver == 1) //节点机库版本
		{
			n_var_data_set(var, NDEV_FW_VER_W, 2);
			n_var_data_set(var, &(dev_conf.nconf.fw_ver), 12);
			n_var_data_char(var, '&');
		}
				
		if(p_limit->ndev_app_ver == 1) //节点机应用版本
		{
			n_var_data_set(var, NDEV_APPL_VER_W, 2);
			n_var_data_set(var, &(dev_conf.nconf.appl_ver), 12);
			n_var_data_char(var, '&');
		}

		if(p_limit->sae_fw_ver == 1) //桩机固件版本
		{
			n_var_data_set(var, NDEV_SAE_VER_W, 2);
			n_var_data_set(var, &(uconfig.stake_fw_last_ver), 8);  
			n_var_data_char(var, '&');
		}	

		if(p_limit->sae_para_ver == 1)  //锁桩参数版本号
		{
			n_var_data_set(var, NDEV_SAE_PARA_VER_W, 2);
			n_var_data_set(var, &(uconfig.stake_para_last_ver), 8);  
			n_var_data_char(var, '&');
		}

		#if CONFS_USING_UPLOAD_READER_VER
		if(p_limit->lnt_reader_firmware_ver == 1)  //岭南通读卡器固件版本 add by zjc at 2016-08-08
		{
			char version[32] = {0};
			
			__lnt_firmware_version_fgets(version);
			printf("Bicycle, __lnt_firmware_version_fgets:%s\n", version);
			
			n_var_data_set(var, NDEV_LNT_FIRMWARE_VER_W, 2);
			n_var_data_set(var, &(version), 24);  
			n_var_data_char(var, '&');
		}
		#endif	

		if(p_limit->ndev_ftp_stat == 1)  //节点机FTP下载状态
		{
			fprintf(stderr, "----------ftype=%d\n", g_ftp_dnl_ctrl_info.ftype);
			fprintf(stderr, "----------isvaild=%d\n", g_ftp_dnl_ctrl_info.isvaild);
			fprintf(stderr, "----------ftime=%d\n", g_ftp_dnl_ctrl_info.ftime);
			fprintf(stderr, "----------upsn=%d\n", ntohs(g_ftp_dnl_ctrl_info.fdnl_ctrl.upsn));
			fprintf(stderr, "----------supplier=%d\n", g_ftp_dnl_ctrl_info.fdnl_ctrl.supplier);
			fprintf(stderr, "----------download_status=%d\n", g_ftp_dnl_ctrl_info.fdnl_ctrl.download_status);
			fprintf(stderr, "----------err=%d\n", g_ftp_dnl_ctrl_info.fdnl_ctrl.err);
			fprintf(stderr, "----------ftpcode=%d\n", g_ftp_dnl_ctrl_info.fdnl_ctrl.ftpcode);
			lib_printf("----------time=", g_ftp_dnl_ctrl_info.fdnl_ctrl.time, 7);

			struct ndev_ftp_download_ctrl_ack fdnl_ctrl;
			memcpy(&fdnl_ctrl, &g_ftp_dnl_ctrl_info.fdnl_ctrl, sizeof(struct ndev_ftp_download_ctrl_ack ));
			
			n_var_data_set(var, NDEV_FTP_DNLS_W, 2);	 
			n_var_data_b64_set(var, &fdnl_ctrl, sizeof(struct ndev_ftp_download_ctrl_ack));  
			n_var_data_char(var, '&');

			#if 0  
			if(g_ftp_dnl_ctrl_info.fdnl_ctrl.download_status == 0xFF) //ftp下载失败的状态只上报一次 2016-12-20
			{
				#if 1        
				ftp_download_ctrl_info_t fdnl_ctrl_info;
				
				g_ftp_dnl_ctrl_info.isvaild = 0; //无效
				memset(&g_ftp_dnl_ctrl_info, 0, sizeof(g_ftp_dnl_ctrl_info));//清空
				   
				memcpy(&fdnl_ctrl_info, &g_ftp_dnl_ctrl_info, sizeof(ftp_download_ctrl_info_t));
				lib_upgrade_set_ftp_dnl_ctrl_info(g_upgrade, &fdnl_ctrl_info);
				#endif 
			}
			#endif
		} 
  
		n_var_data_len(var);
		n_var_data_crc(var);
		
		memcpy(ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

int ndev_protocol_blk_ack(void *d_ptr, void *s_ptr, void *n_ptr)
{
	if((d_ptr == NULL) || (s_ptr == NULL) || (n_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	ndev_rx_buf_t	 *rxbuf = (ndev_rx_buf_t *)n_ptr;
		
	var = n_var_data_alloc();
	if(var != NULL)
	{
		n_var_data_hd_ack(var, N_RX_CMD_PTR(rxbuf), N_RX_SN_PTR(rxbuf));
		n_var_data_set(var, s_ptr, sizeof(univ_file_ack_t));
		n_var_data_len(var);
		n_var_data_crc(var);
		
		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;
}

int ndev_protocol_blk_req(void *d_ptr, void *s_ptr)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_FILE_TRANS_REQ, SAE_CAN_LOCAL_ID, &attr);
		n_var_data_set(var, s_ptr, sizeof(univ_file_ack_t));
		n_var_data_len(var);
		n_var_data_crc(var);
		
		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;
}

int ndev_protocol_univ_update_ack(void *d_ptr, void *s_ptr, const unsigned int slen, const unsigned char sn)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
		
	var = n_var_data_alloc();
	if(var != NULL)
	{
		n_var_data_hd_ack(var, NDEV_CTRL_REQ, sn);  //控制命令
		n_var_data_set(var, NDEV_UNIV_UPDATE_W, 2);
		n_var_data_b64_set(var, s_ptr, slen);
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;
}

int ndev_protocol_univ_update_ack_bc(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_CTRL_ACK, SAE_CAN_LOCAL_ID, &attr);  //控制命令
		n_var_data_set(var, NDEV_UNIV_UPDATE_W, 2);
		n_var_data_b64_set(var, s_ptr, slen);
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

int ndev_protocol_univ_update_req(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;
	
	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr; //通信数据帧中的控制位

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_FILE_TRANS_REQ, SAE_CAN_LOCAL_ID, &attr);  
		n_var_data_set(var, s_ptr, slen);
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

/*
 * 透传消费记录
 */
int ndev_protocol_univ_trading_records_upload(void *d_ptr, void *s_ptr, const unsigned char dev_addr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 1; //透传
		attr.recv = 0;  //节点机不接收
		n_var_data_hd_req(var, NDEV_FILE_TRANS_REQ, dev_addr, &attr);  
		n_var_data_set(var, s_ptr, slen);
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

/*
 * 全透传
 */
int ndev_protocol_whole_pass_upload(const unsigned char op, void *d_ptr, void *s_ptr, const unsigned char dev_addr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 1; //透传
		attr.recv = 0;  //节点机不接收

		n_var_data_hd_req(var, op, dev_addr, &attr);  
		n_var_data_set(var, s_ptr, slen);
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

int ndev_protocol_univ_admin_card_record(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;
	univ_file_hd_t univ_hd;
	admin_card_info_req_t card_info;
	
	memset(&attr, 0, sizeof(n_data_attr_t));
	memset(&univ_hd, 0, sizeof(univ_file_hd_t));
	memset(&card_info, 0, sizeof(admin_card_info_req_t));

	if(slen != sizeof(admin_card_info_req_t))
		return B_DEV_ERROR;

	memcpy(&card_info, s_ptr, slen);
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;  //不透传
		attr.recv = 1;  
		
		univ_hd.file_seq = card_info.sn;
		univ_hd.total = 1;
		univ_hd.div_seq = 1;
		univ_hd.ufc = UFC_ADMIN_CARD_RECORD; //文件类别
		
		n_var_data_hd_req(var, NDEV_FILE_TRANS_REQ, SAE_CAN_LOCAL_ID, &attr);
		n_var_data_set(var, &univ_hd, sizeof(univ_file_hd_t));
		n_var_data_set(var, &card_info, sizeof(admin_card_info_req_t));
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;
}

int ndev_protocol_univ_exception_handle_record(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;
	unity_file_hd_t ufile_hd;
	unsigned char ufile_data[UNITY_FILE_LEN] = {0};
	exception_handle_record_req_t except_record;
	
	memset(&attr, 0, sizeof(n_data_attr_t));
	memset(&ufile_hd, 0, sizeof(unity_file_hd_t));
	memset(&except_record, 0, sizeof(exception_handle_record_req_t));

	if(slen != sizeof(exception_handle_record_req_t))
		return B_DEV_ERROR;

	memcpy(&except_record, s_ptr, slen);
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;  //不透传
		attr.recv = 1;  
		
		ufile_hd.file_seq = except_record.sn;
		ufile_hd.total = htons(1);
		ufile_hd.div_seq = htons(1);
		ufile_hd.ufc = UFC_EXCEPT_HANDLE_CONSUME_RECORD; //文件类别

		memcpy(&ufile_data, &except_record, sizeof(exception_handle_record_req_t));
		
		n_var_data_hd_req(var, NDEV_FILE_TRANS_REQ, SAE_CAN_LOCAL_ID, &attr);
		n_var_data_set(var, &ufile_hd, sizeof(unity_file_hd_t));
		n_var_data_set(var, &ufile_data, sizeof(ufile_data));
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

int ndev_protocol_exception_handle_req(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;
		attr.recv = 1;
		
		n_var_data_hd_req(var, NDEV_CTRL_REQ, SAE_CAN_LOCAL_ID, &attr);  
		n_var_data_set(var, NDEV_EXCEPT_HANDLE_W, 2);
		n_var_data_b64_set(var, s_ptr, slen);
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}

int ndev_protocol_ftp_download_ctrl_ack(void *d_ptr, void *s_ptr, const unsigned int slen,  const unsigned char sn)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
	
		n_var_data_hd_ack(var, NDEV_CTRL_REQ, sn);  
		n_var_data_set(var, NDEV_FTP_DNL_W, 2);
		n_var_data_b64_set(var, s_ptr, slen);
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}


	return len;	
}

int ndev_protocol_rent_info_qry_req(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;
		attr.recv = 1;
		
		n_var_data_hd_req(var, NDEV_CTRL_REQ, SAE_CAN_LOCAL_ID, &attr);  
		n_var_data_set(var, NDEV_RENT_INFO_W, 2);
		n_var_data_b64_set(var, s_ptr, slen);
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}




void ndev_protocol_test(void)
{
	int i;
	unsigned char buf[32] = {0};
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	
	for(i = 0; i < 32; i++)
	{
		buf[i] = i + 1;
	}

	var = n_var_data_alloc();
	if(var != NULL)
	{
		memset(var, 0, sizeof(n_var_data_t));

		attr.pass = 0;
		attr.recv = 1;
		n_var_data_hd_req(var, NDEV_REGISTER_REQ, SAE_CAN_LOCAL_ID, &attr);
		n_var_data_set(var, buf, 32);
		n_var_data_b64_set(var, buf, 32);
		n_var_data_len(var);
		n_var_data_crc(var);

		lib_printf_v2("TEST", var->data, var->total_len, 16);


		n_var_data_free(var);
	}
}


/* 附近网点信息查询 add by zjc at 2016-11-03 */
int ndev_protocol_nearby_siteinfo_qry_req(void *d_ptr, void *s_ptr, const unsigned int slen)
{
	//printf("-----------ndev_protocol_nearby_siteinfo_qry_req\n");
	if((d_ptr == NULL) || (s_ptr == NULL))
		return B_DEV_ERROR;

	int len = B_DEV_ERROR;
	n_var_data_t *var = NULL;
	n_data_attr_t attr;

	memset(&attr, 0, sizeof(n_data_attr_t));
	var = n_var_data_alloc();
	if(var != NULL)
	{
		attr.pass = 0;
		attr.recv = 1;
		
		n_var_data_hd_req(var, NDEV_CTRL_REQ, SAE_CAN_LOCAL_ID, &attr);  
		n_var_data_set(var, NDEV_NEARBY_SITE_INFO_W, 2);
		n_var_data_b64_set(var, s_ptr, slen);
		n_var_data_char(var, '&');
		n_var_data_len(var);
		n_var_data_crc(var);

		memcpy(d_ptr, var->data, var->total_len);
		len = var->total_len;
		
		n_var_data_free(var);
	}

	return len;	
}







