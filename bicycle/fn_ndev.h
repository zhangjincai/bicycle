#ifndef __FN_NDEV_H__
#define __FN_NDEV_H__


struct fn_ndev_val
{
	unsigned char key[3];
	unsigned char data[128]; 
	unsigned int d_size;
	unsigned int sel;
	unsigned char sn;
	unsigned char ack;
	void *var_ptr;
};
typedef struct fn_ndev_val fn_ndev_val_t;



int fn_ndev_reg(fn_ndev_val_t *val);
int fn_ndev_stat(fn_ndev_val_t *val);
int fn_ndev_time(fn_ndev_val_t *val);
int fn_ndev_s_total_num(fn_ndev_val_t *val);
int fn_ndev_s_online_num(fn_ndev_val_t *val);
int fn_ndev_total_ver(fn_ndev_val_t *val);
int fn_ndev_inc_ver(fn_ndev_val_t *val);
int fn_ndev_dec_ver(fn_ndev_val_t *val);
int fn_ndev_temp_ver(fn_ndev_val_t *val);
int fn_ndev_s_blk_attr(fn_ndev_val_t *val);
int fn_ndev_s_attr(fn_ndev_val_t *val);
int fn_ndev_yct_stat(fn_ndev_val_t *val);
int fn_ndev_btheart_return(fn_ndev_val_t *val);
int fn_ndev_site_name_ctrl(fn_ndev_val_t *val);
int fn_ndev_univ_update_ctrl(fn_ndev_val_t *val);
int fn_ndev_btheart_time_set(fn_ndev_val_t *val);
int fn_ndev_s_lock_op(fn_ndev_val_t *val);
int fn_ndev_load_serv_ip_set(fn_ndev_val_t *val);
int fn_ndev_load_serv_port_set(fn_ndev_val_t *val);
int fn_ndev_load_serv2_ip_set(fn_ndev_val_t *val);
int fn_ndev_load_serv2_port_set(fn_ndev_val_t *val);
int fn_ndev_device_reboot(fn_ndev_val_t *val);
int fn_ndev_ftp_set(fn_ndev_val_t *val);
int fn_ndev_except_handle(fn_ndev_val_t *val);
int fn_ndev_ftp_download(fn_ndev_val_t *val);
int fn_ndev_update_ctrl(fn_ndev_val_t *val);
int fn_ndev_get_s_attr(fn_ndev_val_t *val);
int fn_ndev_sae_all_phy_info(fn_ndev_val_t *val);
int fn_ndev_gps_info(fn_ndev_val_t *val);
int fn_ndev_set_light_ctrl_time(fn_ndev_val_t *val);
int fn_ndev_set_site_QR_code(fn_ndev_val_t *val);


int fn_ndev_rent_info_explain(void *ptr, const unsigned int len);


#endif


