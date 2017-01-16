#ifndef __VAR_DATA_H__
#define __VAR_DATA_H__



#define VAR_DATA_NULL		NULL
#define VAR_DATA_EOK		0
#define VAR_DATA_ERROR		-1


/* 锁桩协议 */
#define S_VAR_DATA_LEN_IDX		(6)
#define S_VAR_DATA_IDX			(8)
#define S_VAR_DATA_SZ			(320)
#define S_VAR_DATA_NUM			(6)  
struct s_var_data
{
	unsigned short idx;
	unsigned short data_len;
	unsigned short total_len;
	unsigned char data[S_VAR_DATA_SZ];	
};
typedef struct s_var_data s_var_data_t;

/* 节点机协议 */
#define N_VAR_DATA_LEN_IDX		(7)
#define N_VAR_DATA_IDX			(9)
#define N_VAR_ATTR_IDX			(5)
#define N_VAR_DATA_SZ			(1024)   //从512修改为1024
#define N_VAR_DATA_NUM			(6) 
struct n_var_data
{
	unsigned short idx;
	unsigned short data_len;
	unsigned short total_len;
	unsigned char data[N_VAR_DATA_SZ];	
};
typedef struct n_var_data n_var_data_t;

struct n_data_attr
{
	unsigned char pass:1;
	unsigned char recv:1;
	unsigned char broadcast:1;
	unsigned char rfu:5;
}__attribute__((packed));
typedef struct n_data_attr n_data_attr_t;

int var_data_init(void);
void var_data_destroy(void);


unsigned char get_stake_sn(void);
unsigned char get_ndev_sn(void);

s_var_data_t *s_var_data_alloc(void);
int s_var_data_free(s_var_data_t *var);
void s_var_data_char(s_var_data_t *var, const unsigned char ch);
void s_var_data_short(s_var_data_t *var, const unsigned short sot);
void s_var_data_int(s_var_data_t *var, const unsigned short it);
void s_var_data_set(s_var_data_t *var, void *ptr, const unsigned int size);
void s_var_data_b64_set(s_var_data_t *var, void *ptr, const unsigned int size);
void s_var_data_hd_req(s_var_data_t *var, const unsigned char s_addr, const unsigned char d_addr, const unsigned char cmd);
void s_var_data_hd_ack(s_var_data_t *var, const unsigned char s_addr, const unsigned char d_addr, const unsigned char cmd, const unsigned char sn);
void s_var_data_crc(s_var_data_t *var);

n_var_data_t *n_var_data_alloc(void);
int n_var_data_free(n_var_data_t *var);
void n_var_data_char(n_var_data_t *var, const unsigned char ch);
void n_var_data_short(n_var_data_t *var, const unsigned short sot);
void n_var_data_int(n_var_data_t *var, const unsigned short it);
void n_var_data_set(n_var_data_t *var, void *ptr, const unsigned int size);
void n_var_data_b64_set(n_var_data_t *var, void *ptr, const unsigned int size);
void s_var_data_b64_w_set(s_var_data_t *var, const unsigned char word[3], void *ptr, const unsigned int size);
void n_var_data_hd_req(n_var_data_t *var, const unsigned char cmd, const unsigned char dev_addr,  n_data_attr_t *attr);
void n_var_data_hd_ack(n_var_data_t *var, const unsigned char cmd, const unsigned char sn);
void n_var_data_len(n_var_data_t *var);
void n_var_data_crc(n_var_data_t *var);



#endif

