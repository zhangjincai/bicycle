#ifndef __GUI_INFO_H__
#define __GUI_INFO_H__

#include "defines.h"
#include "device_config.h"
#include "lib_wireless.h"

#include "../libgui/gui_header.h"   //����GUI LIBRARYͷ�ļ�

void gui_get_ndev_stat(ndev_status_t *stat);
void gui_get_sae_stat(sae_status_t *stat, const unsigned char id);
void gui_get_ndev_version(char ver[32]);
void gui_get_ndev_device_config(struct ndev_config *config);
void gui_get_stake_device_config(struct stake_config *config);

int gui_get_ndev_page_config(struct gui_ndev_page_config *config);  //��ȡ�ڵ��ҳ�����ò���
int gui_get_ndev_access_pattern_config(struct gui_ndev_access_pattern_config *config); //��ȡ�ڵ��������ģʽ
int gui_set_ndev_ftp_config(struct gui_ndev_ftp_config *config); //����FTP��ַ
int gui_set_ndev_center_config(struct gui_ndev_center_ip_config *config);  //���г���������
int gui_set_ndev_wifi_config(struct gui_ndev_wifi_ip_config *config);	//WIFI����
int gui_set_ndev_local_config(struct gui_ndev_local_ip_config *config);  //����IP����
int gui_set_ndev_parameter_config(struct gui_ndev_parameter_config *config);  //�ڵ����������
int gui_set_stake_parameter_config(struct gui_stake_parameter_config *config); //��׮��������
int gui_set_ndev_access_pattern_config(struct gui_ndev_access_pattern_config *config); //������뷽ʽ����

int gui_get_basic_info_page_config(struct gui_basic_info_page_config *config);   //��ȡ������Ϣҳ������
int gui_get_stake_info_page_config(struct gui_stake_info_page_config *config);  //��ȡ��׮��Ϣ

int gui_get_ndev_home_page_info(struct gui_ndev_home_page_info *info);  //��ҳ��Ϣ
int gui_get_all_stake_info_config(struct gui_stake_all_info_page_config *config); //ȫ����׮��Ϣ


int gui_set_lnt_card_status(struct gui_lnt_card_status *stat);		//����ͨ������״̬
int gui_set_admin_card_record(gui_admin_card_info_t *info); //����ʹ�ü�¼

int gui_set_lnt_config(struct gui_lnt_page_config *config); //����ͨ����

int gui_get_exception_handle_req(struct gui_except_handle_req *req); //�쳣��������
int gui_set_exception_handle(struct gui_except_handle_ack *ack, const unsigned int flag);
int gui_get_exception_handle_ack(struct gui_except_handle_ack *ack);  


int gui_get_sae_comparison_status(struct gui_sae_comparison_status comp[65]); //�Ա�״̬
int gui_get_access_state(struct gui_access_state *state);


int gui_get_rent_info_qry_req(struct gui_rent_info_qry_req *req);  //�⻹����¼��ѯ
int gui_set_rent_info_qry(struct gui_rent_info_qry_ack *ack, const unsigned int flag);
int gui_get_rent_info_qry_ack(struct gui_rent_info_qry_ack *ack);

int gui_light_ctrl_time_get(struct gui_light_ctrl_time *ctrl_time);
int gui_light_ctrl_time_set(struct gui_light_ctrl_time *ctrl_time);

int gui_delay_return_bicycle_ack(struct gui_delay_return_bicycle *bicycle);





#endif


