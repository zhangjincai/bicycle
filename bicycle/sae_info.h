#ifndef __SAE_INFO_H__
#define __SAE_INFO_H__

#include "defines.h"

int sae_info_init(const unsigned char max, const unsigned char quantity);
void sae_info_destroy(void);
void sae_info_put(const unsigned char id, sae_info_t *info);
void sae_info_del(const unsigned char id); //删除某桩机
void sae_info_update(const unsigned char id, const unsigned short stat, const unsigned char s_sn[6]);
void sae_info_send_hb_times_inc(const unsigned char id);  //发送心跳自增1
void sae_info_send_hb_times_inc_all(const unsigned char s_max);  //全部桩机发送心跳自增1
void sae_info_set_send_hb_times_zero(const unsigned char id); //发送心跳清0
sae_info_t *sae_info_get_by_id(const unsigned char id);    //通过id获取总状态信息
void sae_info_set_quantity(const unsigned char quantity);  //设置桩机数量
unsigned char sae_info_get_quantity(void); //获取桩数量
unsigned char sae_info_get_online(void);  //在线锁桩数
unsigned char sae_info_get_register_count(void);  //在线锁桩数
unsigned char sae_info_get_bicycle_online(void);  //在线车数
void sae_info_set_online(const unsigned char id);  //设置在线锁桩数
void sae_info_set_register(const unsigned char id); //设置注册
void sae_info_get_stat(unsigned char s_stat[17]);  //获取桩机签到状态
void sae_info_get_stat_for_center(unsigned char s_stat[17]);  //获取桩机签到状态,对中心
void sae_info_get_bicycle_exist_stat(unsigned char s_stat[17]); //获取车存在状态
void sae_info_set_id(const unsigned char id, unsigned char s_id[4]);  //设置桩机ID
void sae_info_set_ver(const unsigned char id, unsigned char s_ver[9]); //设置锁桩编号
void sae_info_set_lock_id(const unsigned char id, unsigned char s_lock_id[4]);  //设置锁控编号
void sae_info_set_lock_ver(const unsigned char id, unsigned char s_lock_ver[9]);   //设置锁控版本号
void sae_info_set_sn(const unsigned char id, unsigned char s_sn[7]);  //设置锁桩流水号
void sae_info_set_total_blk_ver(const unsigned char id, unsigned char s_total_bl_ver[9]);  //设置总量黑名单版本
void sae_info_set_inc_blk_ver(const unsigned char id, unsigned char s_inc_bl_ver[9]);   //设置增量黑名单版本
void sae_info_set_dec_blk_ver(const unsigned char id, unsigned char s_dec_bl_ver[9]);   //设置减量黑名单版本
void sae_info_set_temp_blk_ver(const unsigned char id, unsigned char s_temp_bl_ver[9]);   //设置临时黑名单版本
void sae_info_set_stake_para_ver(const unsigned char id, unsigned char s_stake_para_ver[9]);  //设置锁桩参数配置
void sae_info_set_psam(const unsigned char id, unsigned char s_psam[7]);   //设置PSAM卡编号
void sae_info_set_single_stat(const unsigned char id, unsigned short stat);   //设置单桩机状态
void sae_info_set_stat(const unsigned char id, unsigned short stat); //设置锁桩状态
void sae_info_set_time(const unsigned char id, unsigned char s_time[13]);  //设置桩时间
void sae_info_get_all_statistics(void *statistics); //获取全部统计数据
void sae_info_get_all_info(struct sae_info *info);  //获取全部信息
void sae_info_set_register_time(const unsigned char id); //锁桩注册时间
unsigned char sae_info_get_online_count(void);  //获取在线锁桩数
void sae_info_set_phy_sn(const unsigned char id, unsigned char phy_sn[9]);  //锁桩物理编号


#endif



