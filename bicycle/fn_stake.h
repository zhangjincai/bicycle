#ifndef __FN_STAKE_H__
#define __FN_STAKE_H__


struct fn_stake_val
{
	unsigned char key[3];
	unsigned char data[64];
	unsigned int d_size;
	unsigned int sel;       //ÅĞ¶ÏÊÇ´ø'?'×Ö·û
	unsigned char s_addr;    //Ëø×®Ô­µØÖ·
	unsigned char ack;
	void *var_ptr;
};
typedef struct fn_stake_val fn_stake_val_t;

int fn_sae_reg(fn_stake_val_t *val);
int fn_sae_n_reg(fn_stake_val_t *val);
int fn_sae_n_bheart(fn_stake_val_t *val);
int fn_sae_id(fn_stake_val_t *val);
int fn_sae_ver(fn_stake_val_t *val); //Ëø×®¹Ì¼ş°æ±¾ºÅ
int fn_sae_lock_id(fn_stake_val_t *val);
int fn_sae_lock_ver(fn_stake_val_t *val);
int fn_sae_psam(fn_stake_val_t *val);
int fn_sae_sn(fn_stake_val_t *val);
int fn_sae_total_ver(fn_stake_val_t *val);
int fn_sae_inc_ver(fn_stake_val_t *val);
int fn_sae_dec_ver(fn_stake_val_t *val);
int fn_sae_temp_ver(fn_stake_val_t *val);
int fn_sae_time(fn_stake_val_t *val);
int fn_sae_n_id(fn_stake_val_t *val);
int fn_sae_status(fn_stake_val_t *val);
int fn_sae_n_status(fn_stake_val_t *val);
int fn_sae_n_time(fn_stake_val_t *val);
int fn_sae_n_mac(fn_stake_val_t *val);
int fn_sae_phy_sn(fn_stake_val_t *val);
int fn_sae_control(fn_stake_val_t *val);
int fn_sae_para_ver(fn_stake_val_t *val);





#endif


