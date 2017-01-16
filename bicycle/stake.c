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
#include "defines.h"
#include "lib_ctos.h"
#include "lib_general.h"
#include "lib_zmalloc.h"
#include "device_config.h"
#include "fn_hash.h"
#include "var_data.h"
#include "sae_info.h"
#include "fn_stake.h"
#include "sae_protocol.h"
#include "node_device.h"
#include "universal_file.h"
#include "lib_blacklist.h"
#include "lib_firmware.h"
#include "lib_wireless.h"
#include "lib_threadpool.h"
#include "lib_logdb.h"
#include "lib_ctos.h"

#include "stake.h"

#define SAE_DBG_DEF

#ifdef SAE_DBG_DEF
#define SAE_DBG(fmt, args...)	 do{ \
	fprintf(stderr, "DEBUG: %s:%d, ", __FUNCTION__,__LINE__); \
	fprintf(stderr, fmt, ##args); \
}while(0)
#else
#define SAE_DBG(fmt, args...)
#endif


static lib_ctos_t *g_ctos = NULL;									//CAN BUS
static fn_hash_t *g_fn_hash_stake = NULL;							//锁桩哈希解释
extern lib_logdb_t *g_bicycle_logdb;
extern lib_wl_t *g_bicycle_wl;

static void *__sae_recv_thread(void *arg);
static void *__sae_heartbeat_thread(void *arg);
static int __sae_protocol_explain(sae_rx_buf_t *buf, const unsigned int len);
static void __sae_word_analysis(sae_rx_buf_t *buf, const unsigned int len);
static void __sae_word_run(fn_stake_val_t *val, const unsigned int len, unsigned char *ack_f);


static void __sae_register_info_printf(void)
{
	int i, j;
	int m = 0;
	unsigned char s_stat[17] = {0};

	memset(s_stat, 0, sizeof(s_stat));
	sae_info_get_stat(s_stat);

	for(i = 0; i <= 9; i++)
	{	
		if(i == 0)
			fprintf(stderr, "   ");
		fprintf(stderr, "%d  ",i);
	}
	
	fprintf(stderr, "\r\n");
	fprintf(stderr, "-------------------------------\n");
	
	for(j = 0; j <= 6; j++)
	{
		fprintf(stderr, "%d|", j);
		
		for(i = 0; i <= 9; i++)
		{
			if(i == 0)
				fprintf(stderr, " ");
			
			if(lib_chk_bit(s_stat, m) == 1)
				fprintf(stderr, "%d  ", 1);
			else
				fprintf(stderr, "%d  ", 0);
			m++;
		}
		
		fprintf(stderr, "\n-------------------------------\n");
	}
	
	fprintf(stderr, "\r\n");
}

/*
 * 锁桩指令注册函数
 */
static void __fn_stake_register(void)
{
	fn_hash_register(g_fn_hash_stake, SAE_REG_W, (fn_register)fn_sae_reg);					//锁桩主动签到和节点机应答
	fn_hash_register(g_fn_hash_stake, SAE_N_REG_W, (fn_register)fn_sae_n_reg);				//节点机主动签到和桩机应答
	fn_hash_register(g_fn_hash_stake, SAE_N_BHEART_W, (fn_register)fn_sae_n_bheart);			//节点机主动心跳和桩机心跳应答
	fn_hash_register(g_fn_hash_stake, SAE_ID_W, (fn_register)fn_sae_id);						//锁桩编号
	fn_hash_register(g_fn_hash_stake, SAE_VER_W, (fn_register)fn_sae_ver);					//锁桩版本号
	fn_hash_register(g_fn_hash_stake, SAE_LOCK_ID_W, (fn_register)fn_sae_lock_id);				//锁控编号
	fn_hash_register(g_fn_hash_stake, SAE_LOCK_VER_W, (fn_register)fn_sae_lock_ver);			//锁控版本号
	fn_hash_register(g_fn_hash_stake, SAE_PSAM_W, (fn_register)fn_sae_psam);					//PSAM卡编号
	fn_hash_register(g_fn_hash_stake, SAE_SN_W, (fn_register)fn_sae_sn);						//锁桩流水号
	fn_hash_register(g_fn_hash_stake, SAE_TOTAL_BLK_VER_W, (fn_register)fn_sae_total_ver);		//总量黑名单版本
	fn_hash_register(g_fn_hash_stake, SAE_INC_BLK_VER_W, (fn_register)fn_sae_inc_ver);			//增量黑名单版本
	fn_hash_register(g_fn_hash_stake, SAE_DEC_BLK_VER_W, (fn_register)fn_sae_dec_ver);			//减量黑名单版本
	fn_hash_register(g_fn_hash_stake, SAE_TEMP_BLK_VER_W, (fn_register)fn_sae_temp_ver);		//临时黑名单版本
	fn_hash_register(g_fn_hash_stake, SAE_TIME_W, (fn_register)fn_sae_time);					//临时黑名单版本
	fn_hash_register(g_fn_hash_stake, SAE_STATUS_W, (fn_register)fn_sae_status);				//桩机状态
	fn_hash_register(g_fn_hash_stake, SAE_N_ID_W, (fn_register)fn_sae_n_id);					//节点机编号
	fn_hash_register(g_fn_hash_stake, SAE_N_STATUS_W, (fn_register)fn_sae_n_status);			//节点机状态
	fn_hash_register(g_fn_hash_stake, SAE_N_TIME_W, (fn_register)fn_sae_n_time);				//节点机时间
	fn_hash_register(g_fn_hash_stake, SAE_N_MAC_W, (fn_register)fn_sae_n_mac);				//节点机MAC
	fn_hash_register(g_fn_hash_stake, SAE_PHY_SN_W, (fn_register)fn_sae_phy_sn);				//锁桩物理编号
	fn_hash_register(g_fn_hash_stake, SAE_CTRL_C, (fn_register)fn_sae_control); 				//桩机控制	
	fn_hash_register(g_fn_hash_stake, SAE_PARA_VER_W, (fn_register)fn_sae_para_ver); 		//桩机参数配置
}






int stake_init()
{
	int err = -1;
	int n = 0;
	unsigned char rxbuf[512] = {0};
	device_config_t dev_conf;
	ctos_config_t c_conf;
	ndev_status_t nstat;
	pthread_t sae_recv_thr;
	pthread_t sae_htbt_thr;
		
	memset(&dev_conf, 0, sizeof(device_config_t));
	memset(&c_conf, 0, sizeof(ctos_config_t));

	/* 获取配置文件 */
	err = device_config_get(&dev_conf);
	if(err != DEV_CONF_EOK)
		goto ERR;

	/* 初始化CAN BUS */
	c_conf.can_bus = CONFS_CAN_BUS1;
	c_conf.can_id = SAE_CAN_LOCAL_ID;
	c_conf.baud_rate = dev_conf.sconf.can_baud_rate;
	c_conf.can_buf_num = CONFS_CAN_BUF_NUM;
	c_conf.rb_sz = CONFS_CAN_RB_SZ;
	g_ctos = lib_ctos_create(&c_conf);
	if(g_ctos == LIB_CTOS_NULL)
	{
		SYS_LOG_ERR("ctos create failed!\n");
		goto ERR;
	}

	memset(&nstat, 0, sizeof(ndev_status_t));
	ndev_info_stat_get(&nstat);
	nstat.can = 1;  //can总线状态
	ndev_info_stat_put(&nstat);
	
	/* 记录锁桩信息初始化*/
	err = sae_info_init(CONFS_STAKE_NUM_MAX + 1, dev_conf.sconf.quantity);  //最大支持64个锁桩
	if(err != B_DEV_EOK)
	{
		SYS_LOG_ERR("stake info init failed!\n");
		goto ERR;
	}

	/* 指令解析哈希*/
	g_fn_hash_stake = fn_hash_create(CONFS_STAKE_FN_HASH_NUM);
	if(g_fn_hash_stake == NULL)
		goto ERR;
	
	__fn_stake_register(); //哈希注册

	fprintf(stderr, "stake fn hash create success\n");

	
	/* 锁桩数据接收线程*/
	err = lib_normal_thread_create(&sae_recv_thr, __sae_recv_thread, NULL);
	if(err != LIB_GE_EOK)
		goto ERR;

	/* 锁桩心跳线程*/
	err = lib_normal_thread_create(&sae_htbt_thr, __sae_heartbeat_thread, NULL);
	if(err != LIB_GE_EOK)
		goto ERR;

	/* 节点机上电签到*/

	lib_sleep(1);
	
	fprintf(stderr, "fist power on, send register package to all stake\n");
		
	n = sae_protocol_req_req(rxbuf, SAE_CAN_BROADCAST_ID);
	if(n > 0)
	{
		lib_ctos_put(g_ctos, SAE_CAN_BROADCAST_ID, rxbuf, n); 	//发送签到
		//lib_ctos_put_priority(g_ctos, SAE_CAN_BROADCAST_ID, rxbuf, n, CAN_PRIORITY_LOW); 
	}

	fprintf(stderr, "stake init ok\n");

	return B_DEV_EOK;

ERR:
	if(g_fn_hash_stake != NULL)
		fn_hash_destroy(g_fn_hash_stake);
	sae_info_destroy();
	if(g_ctos != NULL)
		lib_ctos_destroy(g_ctos);
	
	return B_DEV_ERROR;
}

int stake_destroy(void)
{
	if(g_fn_hash_stake != NULL)
		fn_hash_destroy(g_fn_hash_stake);
	sae_info_destroy();
	if(g_ctos != NULL)
		lib_ctos_destroy(g_ctos);
	
	return B_DEV_EOK;
}

int stake_ctos_put(const unsigned char dest_addr, unsigned char *data, const unsigned int len)
{
	return lib_ctos_put(g_ctos, dest_addr, data, len);
}

int stake_ctos_put_priority(const unsigned char dest_addr, unsigned char *data, const unsigned int len, const unsigned char priority)
{
	return lib_ctos_put_priority(g_ctos, dest_addr, data, len, priority);
}

/*
 * 锁桩心跳线程
 */
 #define HEARTBEAT_WAIT_TIME				(8)
static void *__sae_heartbeat_thread(void *arg)
{
	int i = 0;
	int n = B_DEV_ERROR;
	unsigned short heart_time = 0;
	unsigned short lower_heart_time;
	unsigned char rxbuf[512] = {0};
	unsigned char quantity = 0; 
	device_config_t conf;
	sae_info_t *info = NULL;
	unsigned char s_stat[17] = {0};
	
	memset(&conf, 0, sizeof(device_config_t));
	
	while(1)
	{
		device_config_get(&conf);
		heart_time = conf.sconf.heart_time;
		lower_heart_time = heart_time - HEARTBEAT_WAIT_TIME;
		
		/* 发送心跳包 */
		memset(rxbuf, 0, sizeof(rxbuf));
		n = sae_protocol_bheart_req(rxbuf, SAE_CAN_BROADCAST_ID);
		if(n != B_DEV_ERROR)
		{
			lib_ctos_put(g_ctos, SAE_CAN_BROADCAST_ID, rxbuf, n);

			//lib_printf_v2("BHEART", rxbuf, n, 16);  
			
			//lib_ctos_put_priority(g_ctos, SAE_CAN_BROADCAST_ID, rxbuf, n, CAN_PRIORITY_LOW);  //发送心跳
			sae_info_send_hb_times_inc_all(CONFS_STAKE_NUM_MAX + 1); // 增加发送次数		
		}

		/* 测试打印 */
		__sae_register_info_printf();
		
		lib_sleep(HEARTBEAT_WAIT_TIME); //上半部
		
		quantity = sae_info_get_quantity();  //获取锁桩总数
		for(i = 1; i <= quantity; i++) 
		{
			info = sae_info_get_by_id(i); //获取锁桩信息
			//if(info->send_hb_times >= CONFS_STAKE_TRY_HEART_TIMES)

			if(info->send_hb_times >= CONFS_STAKE_TRY_HEART_TIMES) //从CONFS_STAKE_TRY_HEART_TIMES改成1 
				sae_info_del(i);  
		}          

		sae_info_get_stat(s_stat); //获取锁桩状态
		for(i = 1; i <= quantity; i++) 
		{
			if(lib_chk_bit(s_stat, i) != 1)  
			{
				memset(rxbuf, 0, sizeof(rxbuf));    
				n = sae_protocol_req_req(rxbuf, i);
				if(n > 0)
				{
					lib_ctos_put_priority(g_ctos, i, rxbuf, n, CAN_PRIORITY_HIGH);
						
					fprintf(stderr, "Call stake id %d register\n", i);

					lib_msleep(10); //间隔10毫秒
				}	
			}
		}

		lib_sleep(lower_heart_time); //下半部
	}
	
	return lib_thread_exit((void *)NULL);
}

/*
 * 锁桩数据接收线程
 */
static void *__sae_recv_thread(void *arg)
{
	int ret = -1;
	unsigned char ch = 0;
	unsigned char s_flag = 0;
	unsigned char rx_flag = 0;
	unsigned int rx_len = 0;
	unsigned short data_len = 0;
	unsigned char buf[S_VAR_DATA_SZ] = {0};
	sae_rx_buf_t rxbuf;

	while(1)
	{
		if(rx_flag == 0)
		{
			ch = 0;
			ret = lib_ctos_getchar(g_ctos, &ch);
			if(ret > 0)
			{
				if(s_flag == SAE_HD_L)		//头部接收完毕
				{
					S_RX_DA_BV(rxbuf, rx_len++, ch);
					if(rx_len == 8)  //数据帧中数据体前的长度
					{
						s_flag = 0;
						rx_flag = 1;
					}
					continue;
				}
				else if((ch == SAE_HD_H) && (s_flag == 0)) //检查头部
				{
					memset(&rxbuf, 0, sizeof(sae_rx_buf_t));
					
					s_flag = SAE_HD_H;
					S_RX_DA_BV(rxbuf, rx_len++, SAE_HD_H);
					continue;
				}
				else if((ch == SAE_HD_L) && (s_flag == SAE_HD_H)) //检查尾部
				{
					s_flag = SAE_HD_L;
					S_RX_DA_BV(rxbuf, rx_len++, SAE_HD_L);
					continue;
				}
			}
			else
				lib_msleep(500);
		}
		else if(rx_flag == 1) //开始接收数据体
		{
			data_len = ntohs(S_RX_LEN(rxbuf));
			if(data_len > SAE_FRAME_MAX_SZ) //非法长度
			{
				ch = 0;
				rx_flag = 0;
				memset(&rxbuf, 0, sizeof(sae_rx_buf_t));	
				continue;
			}
			
			ret = lib_ctos_get(g_ctos, S_RX_DATA(rxbuf), data_len + 2); //+2:校验
			if(ret == data_len + 2) //完整一帧数据
			{
				__sae_protocol_explain(&rxbuf, ret + 8); //数据解释
			}
			
			s_flag = 0;
			rx_flag = 0;
			rx_len = 0;
			
			memset(&rxbuf, 0, sizeof(sae_rx_buf_t));			
		}
	}

	return lib_thread_exit((void *)NULL);
}

/*
 * 协议解释
 */
static int __sae_protocol_explain(sae_rx_buf_t *buf, const unsigned int len)
{
	unsigned short crc16 = -1;
	unsigned short last_crc16 = -1;
	unsigned int s_len = 0;
	
	//lib_printf_v2("\n-------------------------__sae_protocol_explain-----------------------\n", S_RX_DA_PTR(buf), len, 16);

	if(s_rx_buf_crc_check(buf) == B_DEV_FALSE)
		return B_DEV_ERROR;
	
	switch(S_RX_CMD_PTR(buf))  //解释命令
	{
		case SAE_DEV_REG_REQ: 	//设备签到
		{
			__sae_word_analysis(buf, len);	
		}
		break;

		case SAE_DEV_REG_ACK: //设备签到确认
		{
			__sae_word_analysis(buf, len);				
		}
		break;
		
		case SAE_DEV_CTRL_REQ:	//设备控制
		{
			__sae_word_analysis(buf, len);
		}
		break;

		case SAE_FILE_TRANS_REQ:  //泛文件传输,桩机向节点机请求泛文件
		{
			unity_file_handle_to_sae(buf, len); //新版本泛文件传输
		}
		break;

		case SAE_PASS_ACK:  //全透传 确认(节点机代桩机向中心确认)
		{
			s_len = ntohs(S_RX_LEN_PTR(buf));
			ndev_whole_pass_upload(NDEV_PASS_ACK, S_RX_SADDR_PTR(buf), &(S_RX_DATA_PTR(buf)), s_len);  	
		}
		break;

		case SAE_PASS_REQ:  //全透传 请求(节点机代桩机向中心请求)
		{
			s_len = ntohs(S_RX_LEN_PTR(buf));
			ndev_whole_pass_upload(NDEV_PASS_REQ, S_RX_SADDR_PTR(buf), &(S_RX_DATA_PTR(buf)), s_len);  	
		}
		break;
	}

	return B_DEV_EOK;
}

static void __sae_word_analysis(sae_rx_buf_t *buf, const unsigned int len)
{
	int i = 0;
	int cnt_a = 0, cnt_r = 0, cnt_reg = 0;
	unsigned char ack_f = 0;
	fn_stake_val_t fn_stake_val_a[32];   	
	fn_stake_val_t fn_stake_val_r[32];  
	fn_stake_val_t fn_stake_val_reg[6];  
	s_var_data_t *var = NULL;
	char *p = NULL;
	char *p1 = NULL;

	memset(&fn_stake_val_a, 0, sizeof(fn_stake_val_t) * 32);
	memset(&fn_stake_val_r, 0, sizeof(fn_stake_val_t) * 32);
	memset(&fn_stake_val_reg, 0, sizeof(fn_stake_val_t) * 6);
	
	p = S_RX_DATA_PTR(buf);
	while(p1 = strchr(p, '&')) //指令分割
	{
		#if 0
		if(strncmp(p, SAE_REG_W, 2) == 0) //锁桩主动签到，节点机应答
		{
			int n;
			unsigned char rxbuf[512] = {0};
			n = sae_protocol_bheart_req(rxbuf, SAE_CAN_BROADCAST_ID);
			if(n != B_DEV_ERROR)
				lib_ctos_put(g_ctos, SAE_CAN_BROADCAST_ID, rxbuf, n);
		}
		#endif

		*p1 = '\0';
		if(*(p+2) == '?')  //带? 号的请求
		{
			strncpy(fn_stake_val_r[cnt_r].key, p, 2);
			fn_stake_val_r[cnt_r].sel = FN_SEL_TRUE;
			fn_stake_val_r[cnt_r].d_size = 0;
			fn_stake_val_r[cnt_r].s_addr = S_RX_SADDR_PTR(buf);
			cnt_r++; //请求指令个数		
		}
		else  
		{
			strncpy(fn_stake_val_a[cnt_a].key, p, 2);
			fn_stake_val_a[cnt_a].sel = FN_SEL_FLASE;
			p += 2;
			strcpy(fn_stake_val_a[cnt_a].data, p);  //拷贝字符串
			fn_stake_val_a[cnt_a].d_size = strlen(p);
			fn_stake_val_a[cnt_a].s_addr = S_RX_SADDR_PTR(buf);
			cnt_a++; //响应指令个数
		}
		p = p1 + 1; //指向下一条指令
	}

	if(cnt_a > 0) //数据回复 (节点机回复给桩机)
	{
		var = s_var_data_alloc();
		if(var != NULL)
		{
			s_var_data_hd_ack(var, SAE_CAN_LOCAL_ID, S_RX_SADDR_PTR(buf), S_RX_CMD_PTR(buf), S_RX_SN_PTR(buf));

			for(i = 0; i < cnt_a; i++)
			{
				fn_stake_val_a[i].var_ptr = var;  //分配内存给应答报文
			}

			__sae_word_run(&fn_stake_val_a, cnt_a, &ack_f); //运行哈希函数
		}
	}
	
	/* 数据回应 */
	if((var != NULL) && (ack_f == 1))
	{
		s_var_data_crc(var);
		lib_ctos_put_priority(g_ctos, S_RX_SADDR_PTR(buf),var->data, var->total_len, CAN_PRIORITY_HIGH);
		ack_f = 0;
	}

	if(var != NULL)
		s_var_data_free(var);
	
	
	if(cnt_r > 0)  //带? 号的请求,回复
	{
		var = s_var_data_alloc();
		if(var != NULL)
		{
			s_var_data_hd_ack(var, S_RX_DADDR_PTR(buf), S_RX_SADDR_PTR(buf), S_RX_CMD_PTR(buf), S_RX_SN_PTR(buf));
			
			for(i = 0; i < cnt_r; i++)
			{
				fn_stake_val_r[i].var_ptr = var;
			}
			
			__sae_word_run(&fn_stake_val_r, cnt_r,  &ack_f);	
			s_var_data_crc(var);
			lib_ctos_put_priority(g_ctos, S_RX_SADDR_PTR(buf),var->data, var->total_len, CAN_PRIORITY_HIGH);
			s_var_data_free(var);
		}
	}
}

static void __sae_word_run(fn_stake_val_t *val, const unsigned int len, unsigned char *ack_f)
{
	int i;

	for(i = 0; i < len; i++)
	{
		fn_hash_run(g_fn_hash_stake, &val[i]);
		if(val[i].ack == 1)  //判断是否需要发确认报文
			*ack_f = 1;
	}
}



