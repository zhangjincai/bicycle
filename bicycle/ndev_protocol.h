#ifndef __NDEV_PROTOCOL_H__
#define __NDEV_PROTOCOL_H__



/*
int ndev_protocol_init(void);
int ndev_protocol_destroy(void);
*/



int ndev_protocol_load_req(void *ptr);
int ndev_protocol_reg_req(void *ptr);
int ndev_protocol_bheart_level1_req(void *ptr, void *limit);
int ndev_protocol_blk_ack(void *d_ptr, void *s_ptr, void *n_ptr);
int ndev_protocol_blk_req(void *d_ptr, void *s_ptr);

int ndev_protocol_univ_update_ack(void *d_ptr, void *s_ptr, const unsigned int slen, const unsigned char sn);
int ndev_protocol_univ_update_ack_bc(void *d_ptr, void *s_ptr, const unsigned int slen);
int ndev_protocol_univ_update_req(void *d_ptr, void *s_ptr, const unsigned int slen);

int ndev_protocol_univ_trading_records_upload(void *d_ptr, void *s_ptr, const unsigned char dev_addr, const unsigned int slen);
int ndev_protocol_whole_pass_upload(const unsigned char op, void *d_ptr, void *s_ptr, const unsigned char dev_addr, const unsigned int slen);
int ndev_protocol_univ_admin_card_record(void *d_ptr, void *s_ptr, const unsigned int slen);
int ndev_protocol_univ_exception_handle_record(void *d_ptr, void *s_ptr, const unsigned int slen);

int ndev_protocol_exception_handle_req(void *d_ptr, void *s_ptr, const unsigned int slen);
int ndev_protocol_ftp_download_ctrl_ack(void *d_ptr, void *s_ptr, const unsigned int slen,  const unsigned char sn);
int ndev_protocol_rent_info_qry_req(void *d_ptr, void *s_ptr, const unsigned int slen);


void ndev_protocol_test(void);

int __lnt_firmware_version_fgets(char version[24]);




#endif

