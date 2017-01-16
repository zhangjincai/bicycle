#ifndef __SAE_INFO_H__
#define __SAE_INFO_H__

#include "defines.h"

int sae_info_init(const unsigned char max, const unsigned char quantity);
void sae_info_destroy(void);
void sae_info_put(const unsigned char id, sae_info_t *info);
void sae_info_del(const unsigned char id); //ɾ��ĳ׮��
void sae_info_update(const unsigned char id, const unsigned short stat, const unsigned char s_sn[6]);
void sae_info_send_hb_times_inc(const unsigned char id);  //������������1
void sae_info_send_hb_times_inc_all(const unsigned char s_max);  //ȫ��׮��������������1
void sae_info_set_send_hb_times_zero(const unsigned char id); //����������0
sae_info_t *sae_info_get_by_id(const unsigned char id);    //ͨ��id��ȡ��״̬��Ϣ
void sae_info_set_quantity(const unsigned char quantity);  //����׮������
unsigned char sae_info_get_quantity(void); //��ȡ׮����
unsigned char sae_info_get_online(void);  //������׮��
unsigned char sae_info_get_register_count(void);  //������׮��
unsigned char sae_info_get_bicycle_online(void);  //���߳���
void sae_info_set_online(const unsigned char id);  //����������׮��
void sae_info_set_register(const unsigned char id); //����ע��
void sae_info_get_stat(unsigned char s_stat[17]);  //��ȡ׮��ǩ��״̬
void sae_info_get_stat_for_center(unsigned char s_stat[17]);  //��ȡ׮��ǩ��״̬,������
void sae_info_get_bicycle_exist_stat(unsigned char s_stat[17]); //��ȡ������״̬
void sae_info_set_id(const unsigned char id, unsigned char s_id[4]);  //����׮��ID
void sae_info_set_ver(const unsigned char id, unsigned char s_ver[9]); //������׮���
void sae_info_set_lock_id(const unsigned char id, unsigned char s_lock_id[4]);  //�������ر��
void sae_info_set_lock_ver(const unsigned char id, unsigned char s_lock_ver[9]);   //�������ذ汾��
void sae_info_set_sn(const unsigned char id, unsigned char s_sn[7]);  //������׮��ˮ��
void sae_info_set_total_blk_ver(const unsigned char id, unsigned char s_total_bl_ver[9]);  //���������������汾
void sae_info_set_inc_blk_ver(const unsigned char id, unsigned char s_inc_bl_ver[9]);   //���������������汾
void sae_info_set_dec_blk_ver(const unsigned char id, unsigned char s_dec_bl_ver[9]);   //���ü����������汾
void sae_info_set_temp_blk_ver(const unsigned char id, unsigned char s_temp_bl_ver[9]);   //������ʱ�������汾
void sae_info_set_stake_para_ver(const unsigned char id, unsigned char s_stake_para_ver[9]);  //������׮��������
void sae_info_set_psam(const unsigned char id, unsigned char s_psam[7]);   //����PSAM�����
void sae_info_set_single_stat(const unsigned char id, unsigned short stat);   //���õ�׮��״̬
void sae_info_set_stat(const unsigned char id, unsigned short stat); //������׮״̬
void sae_info_set_time(const unsigned char id, unsigned char s_time[13]);  //����׮ʱ��
void sae_info_get_all_statistics(void *statistics); //��ȡȫ��ͳ������
void sae_info_get_all_info(struct sae_info *info);  //��ȡȫ����Ϣ
void sae_info_set_register_time(const unsigned char id); //��׮ע��ʱ��
unsigned char sae_info_get_online_count(void);  //��ȡ������׮��
void sae_info_set_phy_sn(const unsigned char id, unsigned char phy_sn[9]);  //��׮������


#endif



