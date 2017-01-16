#ifndef __LIB_LNT_H__
#define __LIB_LNT_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include "lnt.h"

#define LIB_LNT_NULL					LNT_NULL 
#define LIB_LNT_EOK					LNT_EOK	
#define LIB_LNT_ERROR				LNT_ERROR
#define LIB_LNT_ETIMEOUT				LNT_ETIMEOUT
#define LIB_LNT_EFULL				LNT_EFULL
#define LIB_LNT_EEMPTY				LNT_EEMPTY
#define LIB_LNT_ENOMEM 				LNT_ENOMEM 
#define LIB_LNT_EXCMEM				LNT_EXCMEM
#define LIB_LNT_EBUSY				LNT_EBUSY
#define LIB_LNT_NOT_SUPPORT			LNT_NOT_SUPPORT	

#define LIB_LNT_TRUE					LNT_TRUE
#define LIB_LNT_FALSE				LNT_FALSE



typedef lnt_getversion_ack_t lib_lnt_getversion_ack_t;
typedef lnt_core_init_req_t lib_lnt_core_init_req_t;
typedef lnt_core_init_ack_t lib_lnt_core_init_ack_t;
typedef lnt_parameter_set_req_t lib_lnt_parameter_set_req_t;
typedef lnt_qry_ticket_info_ack_t lib_lnt_qry_ticket_info_ack_t;
typedef rent_info_t lib_rent_info_t;
typedef lnt_rent_info_req_t lib_lnt_rent_info_req_t;
typedef lnt_rent_info_ack_t lib_lnt_rent_info_ack_t;
typedef lnt_rent_info_t lib_lnt_rent_info_t;
typedef lnt_blacklist_record_ack_t lib_lnt_blacklist_record_ack_t;
typedef lnt_trade_record_ack_t lib_lnt_trade_record_ack_t;
typedef lnt_history_record_ack_t lib_lnt_history_record_ack_t;
typedef lnt_consume_req_t lib_lnt_consume_req_t;
	
typedef lnt_r_init_ack_t lib_lnt_r_init_ack_t;



typedef struct lnt_config
{
	char tty[32];
	unsigned int baudrate;
	int delay_ms;
	
	char ipaddr[16];
	unsigned short port;

	unsigned char spno[2]; //服务商代码
	unsigned char conpa; //消费初始化参数
	unsigned char cvad; //卡离线有效期启动
	unsigned char userid[16]; //用户ID

	unsigned char pki_RO[4]; //用户只读,不写
	unsigned char pki[4]; //PKI
	unsigned char psam[4];  //PSAM卡号
	unsigned char pki_r[4]; //PKI
	unsigned char macaddr[6]; //节点机MAC地址

	unsigned int pkt_sn_RO; //报文序列号，只读
}lib_lnt_config_t;



int lnt_init(lib_lnt_config_t *config);

int lib_lnt_init(lib_lnt_config_t *config);
int lib_lnt_release(void);

int lib_lnt_set_TTY_BAUDRATE(const char tty[32], const unsigned int baudrate);
int lib_lnt_set_IP_PORT(const char ipaddr[16], const unsigned short port);
int lib_lnt_set_SPNO_CONPA_CVAD_USERID(const unsigned char spno[2], const unsigned char conpa, const unsigned char cvad, const unsigned char userid[16]);
int lib_lnt_get_config(lib_lnt_config_t *config);
enum LNT_REGISTER_STAT lib_lnt_register_stat_get(void);
lib_lnt_register_proc_code_t lib_lnt_register_proc_code_get(void);
void lib_lnt_register_notify_put(const unsigned int notify);


int lib_lnt_get_firmware_version(char *version);
void lib_lnt_utils_time_bcd_yymmddhhMMss(unsigned char tm[6]);
void lib_lnt_utils_time_bcd_yymmddhhMM(unsigned char tm[5]);
void lib_lnt_utils_time_bcd_yymmdd(unsigned char tm[3]);


int lib_lnt_get_version(lib_lnt_getversion_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_core_init(lib_lnt_core_init_req_t *req, lib_lnt_core_init_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_set_parameter(lib_lnt_parameter_set_req_t *req, unsigned char *stat, const unsigned int msec);
int lib_lnt_qry_ticket_info(lib_lnt_qry_ticket_info_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_qry_rent_info(lib_lnt_rent_info_req_t *req, lib_lnt_rent_info_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_set_rent_info(lib_lnt_rent_info_t *rent, unsigned char *stat, const unsigned int msec);
int lib_lnt_get_history_record(lib_lnt_history_record_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_set_blacklist_flag(lib_lnt_blacklist_record_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_get_trade_record(lib_lnt_trade_record_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_consume(lib_lnt_consume_req_t *req, unsigned char *stat, const unsigned int msec);

int lib_lnt_CLA_INS_pkg(void *s_ptr, const unsigned int s_len, void *d_ptr, const unsigned int d_len, unsigned char *d_stat, const unsigned int msec);

int lib_lnt_r_init(lnt_r_init_ack_t *ack, unsigned char *stat, const unsigned int msec); 
int lib_lnt_r_ac_login_1(lnt_r_ac_login_1_ack_t *ack, unsigned char *stat, const unsigned int msec);


int lib_lnt_r_deposit_init(lnt_r_deposit_init_req_t *req, lnt_r_deposit_init_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_r_deposit_process(lnt_r_deposit_process_req_t *req, lnt_r_deposit_process_ack_t *ack, unsigned char *stat, const unsigned int msec); 

#if 0
int lib_lnt_member_deposit_process(enum LNT_MEMBER_OP op, unsigned char pid[8], lnt_member_proc_code_t *proc); //押金处理
#endif

#if 0
typedef int (*fn_agent_proc)(int, void *, int);
int lib_lnt_on_card_process(lnt_agent_on_card_stage1_CTS_t *stage1, lnt_member_proc_code_t *proc, fn_agent_proc fn_agent); //开卡处理
#endif

int lib_lnt_str_to_hex(unsigned char *str_data, unsigned char *hex_data, int str_len);
int lib_lnt_is_same_card(const unsigned char pid[8]); //判断是否同一张卡
int lib_lnt_consume_and_get_record(lib_lnt_consume_req_t *consume, lib_lnt_trade_record_ack_t *record, unsigned char *stat, unsigned char *f_type);  //消费获取交易记录
int lib_lnt_on_card_connect_to_agent(void);  //开卡连接到代理网关
void lib_lnt_on_card_close_to_agent(int sockfd);   //关闭连接

unsigned int lib_pkt_sn_RO_get(void);  //报文流水号号

enum AGENT_ERR lib_lnt_on_card_stage1TO2_process(const int sockfd, lnt_agent_on_card_stage1_CTS_t *stage1, lnt_agent_on_card_stage2_STC_t *stage2); //开卡1到2阶段
enum AGENT_ERR lib_lnt_on_card_stage3TO4_process(const int sockfd, lnt_agent_on_card_stage3_CTS_t *stage3, lnt_agent_on_card_stage4_STC_t *stage4, agent_retransmission_data_t *retrans); 
enum AGENT_ERR lib_lnt_on_card_stage5TO6_process(const int sockfd, agent_extra_data_t *extra, lnt_member_process_stage2_STC_t *member_stage2, unsigned char *stat); 
enum AGENT_ERR lib_lnt_on_card_stage7TO8_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_on_card_stage8_STC_t *stage8, agent_retransmission_data_t *retrans); 

enum AGENT_ERR lib_lnt_off_card_stage1TO2_process(const int sockfd, lnt_agent_off_card_stage1_CTS_t *stage1, lnt_agent_off_card_stage2_STC_t *stage2, agent_extra_data_t *extra);
enum AGENT_ERR lib_lnt_off_card_stage3TO4_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_off_card_stage4_STC_t *stage4);
enum AGENT_ERR lib_lnt_off_card_stage5TO6_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_off_card_stage6_STC_t *stage6);
enum AGENT_ERR lib_lnt_off_card_stage7TO8_process(const int sockfd, agent_extra_data_t *extra, lnt_member_process_stage2_STC_t *member_stage2);
enum AGENT_ERR lib_lnt_off_card_stage9TO10_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_off_card_stage10_STC_t *stage10, agent_retransmission_data_t *retrans); 
enum AGENT_ERR lib_lnt_off_card_stage11TO12_process(const int sockfd, agent_extra_data_t *extra, recharge_info_t *recharge, lnt_packet_recharge_stage2_STC_t *recharge_stage2);
enum AGENT_ERR lib_lnt_off_card_stage13TO14_process(const int sockfd, agent_extra_data_t *extra,  lnt_packet_recharge_stage2_STC_t *recharge_stage2, lnt_packet_recharge_stage4_STC_t *recharge_stage4);
enum AGENT_ERR lib_lnt_off_card_stage15TO16_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage4_STC_t *recharge_stage4, lnt_packet_recharge_stage6_STC_t *recharge_stage6);
enum AGENT_ERR lib_lnt_off_card_stage17TO18_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage6_STC_t *recharge_stage6, lnt_packet_recharge_stage8_STC_t *recharge_stage8);
enum AGENT_ERR lib_lnt_off_card_stage19TO20_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage8_STC_t *recharge_stage8, lnt_packet_recharge_stage10_STC_t *recharge_stage10, int *r_cpu_load);
enum AGENT_ERR lib_lnt_off_card_stage21TO22_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage10_STC_t *recharge_stage10, lnt_packet_recharge_stage12_STC_t *recharge_stage12);
enum AGENT_ERR lib_lnt_off_card_stage23TO24_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_off_card_stage24_STC_t *stage24, agent_retransmission_data_t *retrans); 

enum AGENT_ERR lib_lnt_on_card_blacklist_process(const int sockfd, agent_extra_data_t *extra);
enum AGENT_ERR lib_lnt_off_card_blacklist_process(const int sockfd, agent_extra_data_t *extra);

/*----------------------充值函数--------------------------*/
enum AGENT_ERR lib_lnt_recharge_stage1TO2_process(const int sockfd, lnt_agent_recharge_stage1_CTS_t *stage1, lnt_agent_recharge_stage2_STC_t *stage2, agent_extra_data_t *extra);
enum AGENT_ERR lib_lnt_recharge_stage3TO4_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_recharge_stage4_STC_t *stage4);
enum AGENT_ERR lib_lnt_recharge_stage5TO6_process(const int sockfd, agent_extra_data_t *extra, recharge_info_t *recharge,  lnt_packet_recharge_stage2_STC_t *recharge_stage2);
enum AGENT_ERR lib_lnt_recharge_stage7TO8_process(const int sockfd, agent_extra_data_t *extra,  lnt_packet_recharge_stage2_STC_t *recharge_stage2, lnt_packet_recharge_stage4_STC_t *recharge_stage4);
enum AGENT_ERR lib_lnt_recharge_stage9TO10_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage4_STC_t *recharge_stage4, lnt_packet_recharge_stage6_STC_t *recharge_stage6);
enum AGENT_ERR lib_lnt_recharge_stage11TO12_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage6_STC_t *recharge_stage6, lnt_packet_recharge_stage8_STC_t *recharge_stage8);
enum AGENT_ERR lib_lnt_recharge_stage13TO14_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage8_STC_t *recharge_stage8, lnt_packet_recharge_stage10_STC_t *recharge_stage10, int *r_cpu_load);
enum AGENT_ERR lib_lnt_recharge_stage15TO16_process(const int sockfd, agent_extra_data_t *extra, lnt_packet_recharge_stage10_STC_t *recharge_stage10, lnt_packet_recharge_stage12_STC_t *recharge_stage12);
enum AGENT_ERR lib_lnt_recharge_stage17TO18_process(const int sockfd, agent_extra_data_t *extra, lnt_agent_off_card_stage24_STC_t *stage24, agent_retransmission_data_t *retrans);


int lib_lnt_do_admin_card(void);
enum LNT_ADMIN_CARD_TYPE lib_lnt_is_admin_card(void);



int LIB_LNT_log_open(void);
void LIB_LNT_log_close(void);
int LIB_LNT_log_vsprintf(char *fmt, ...);
int LIB_LNT_log_vsprintf_debug(const int debug, char *fmt, ...);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

