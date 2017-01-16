#ifndef __LIB_UNITY_FILE_H__
#define __LIB_UNITY_FILE_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

/*
 * 函数返回码定义
 */
#define LIB_UF_NULL			(NULL) 
#define LIB_UF_OK           		0   /* Successful result */
#define LIB_UF_ERROR        1   /* SQL error or missing database */
#define LIB_UF_INTERNAL     2   /* Internal logic error in SQLite */
#define LIB_UF_PERM         3   /* Access permission denied */
#define LIB_UF_ABORT        4   /* Callback routine requested an abort */
#define LIB_UF_BUSY         5   /* The database file is locked */
#define LIB_UF_LOCKED       6   /* A table in the database is locked */
#define LIB_UF_NOMEM        7   /* A malloc() failed */
#define LIB_UF_READONLY     8   /* Attempt to write a readonly database */
#define LIB_UF_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define LIB_UF_IOERR       10   /* Some kind of disk I/O error occurred */
#define LIB_UF_CORRUPT     11   /* The database disk image is malformed */
#define LIB_UF_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define LIB_UF_FULL        13   /* Insertion failed because database is full */
#define LIB_UF_CANTOPEN    14   /* Unable to open the database file */
#define LIB_UF_PROTOCOL    15   /* Database lock protocol error */
#define LIB_UF_EMPTY       16   /* Database is empty */
#define LIB_UF_SCHEMA      17   /* The database schema changed */
#define LIB_UF_TOOBIG      18   /* String or BLOB exceeds size limit */
#define LIB_UF_CONSTRAINT  19   /* Abort due to constraint violation */
#define LIB_UF_MISMATCH    20   /* Data type mismatch */
#define LIB_UF_MISUSE      21   /* Library used incorrectly */
#define LIB_UF_NOLFS       22   /* Uses OS features not supported on host */
#define LIB_UF_AUTH        23   /* Authorization denied */
#define LIB_UF_FORMAT      24   /* Auxiliary database format error */
#define LIB_UF_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define LIB_UF_NOTADB      26   /* File opened that is not a database file */
#define LIB_UF_NOTICE      27   /* Notifications from sqlite3_log() */
#define LIB_UF_WARNING     28   /* Warnings from sqlite3_log() */
#define LIB_UF_ROW         100  /* sqlite3_step() has another row ready */
#define LIB_UF_DONE        101  /* sqlite3_step() has finished executing */




#define UNITY_FILE_LEN					(256)    //泛文件数据长度


/*
 * 泛文件类型
 */
enum UF_TYPE
{
	UF_TYPE_TOTAL_BL = 0x71, 		//总量黑名单
	UF_TYPE_INCREMENT_BL = 0x72,	//增量黑名单
	UF_TYPE_DECREMENT_BL = 0x73,	//减量黑名单
	UF_TYPE_TEMPORARY_BL = 0x74,	//临时黑名单
	UF_TYPE_STAKE_FW = 0x81, 		//锁桩固件
	UF_TYPE_STAKE_TRADE_RECORD = 0x91,    //锁桩交易记录
	UF_TYPE_LOG_RECORD = 0xA2,        //日志记录
	UF_TYPE_ADMIN_RECORD = 0xA3,    //管理卡使用记录
	UF_TYPE_EX_CONSUME_RECORD = 0xA4, //异常处理扣费交易记录
	UF_TYPE_STAKE_PARA = 0xA6,        	//锁桩参数
	UF_TYPE_UNIONPAY_KEY = 0xA7,     //银联密钥
};

/*
 * 泛文件数据库
 */
enum UF_DB
{
	UF_DB_IDLE = 0,
	UF_DB_A = 1,
	UF_DB_B = 2,
	UF_DB_NOT = 3
};

/*
 * 泛文件原子状态
 */
enum UF_ATMOIC
{
	UF_ATMOIC_INIT = 0, 
	UF_ATMOIC_RUN = 1,
	UF_ATMOIC_STOP = 2,
	UF_ATMOIC_END = 3
};

/*
 * 泛文件头部
 */
 #define UNIV_DIV_SEQ_BC		0xffff  //广播泛文件
struct unity_file_hd
{
	unsigned short file_seq;	  //文件序号
	unsigned short total;	  //分割总数
	unsigned short div_seq;  //分割序号
	unsigned char ufc;        //文件类别
	unsigned char attr[9];   //特征定义
}__attribute__((packed));
typedef struct unity_file_hd unity_file_hd_t;

/*
 * 泛文件结构
 */
struct unity_file_st
{
	unsigned short length;
	struct unity_file_hd hd;
	unsigned char data[UNITY_FILE_LEN];
}__attribute__((packed));
typedef struct unity_file_st unity_file_st_t;

/*
 * 泛文件配置
 */
struct unity_file_config
{
	unsigned char total_bl_last_ver[12]; 		//总量黑名单版本
	unsigned char inc_bl_last_ver[12];			//增量黑名单版本
	unsigned char dec_bl_last_ver[12]; 		//减量黑名单版本
	unsigned char temp_bl_last_ver[12];  		//临时黑名单版本
	unsigned char stake_fw_last_ver[12]; 		//锁桩固件版本
	unsigned char stake_para_last_ver[12]; 	//锁桩参数版本
	unsigned char unionpay_key_last_ver[12];   //银联密钥参数
	
	unsigned char use_total_bl_db;			//当前使用的数据库
	unsigned char use_inc_bl_db;
	unsigned char use_dec_bl_db;
	unsigned char use_temp_bl_db;
	unsigned char use_stake_fw_db;
	unsigned char use_stake_para_db;
	unsigned char use_unionpay_key_db;
	
	unsigned short use_total_bl_seq;
	unsigned short use_inc_bl_seq;
	unsigned short use_dec_bl_seq;
	unsigned short use_temp_bl_seq;
	unsigned short use_stake_fw_seq;
	unsigned short use_stake_para_seq;
	unsigned short use_unionpay_key_seq;

	unsigned rfu[128];
}__attribute__((packed));
typedef struct unity_file_config unity_file_config_t;









typedef struct unity_file  lib_unity_file_t;

char *lib_unity_file_version(void);
lib_unity_file_t *lib_unity_file_create(const char *pathname);
void lib_unity_file_destroy(lib_unity_file_t *uf);
int lib_unity_file_config_replace_data(lib_unity_file_t *uf, unity_file_config_t *uconfig);
int lib_unity_file_config_select_data(lib_unity_file_t *uf, unity_file_config_t *uconfig);
int lib_unity_file_insert_data(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, const unsigned int uidx, unity_file_st_t *ufile);
int lib_unity_file_replace_data(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, const unsigned int uidx, unity_file_st_t *ufile);
int lib_unity_file_update_data(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, const unsigned int uidx, unity_file_st_t *ufile);
int lib_unity_file_select_data(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, const unsigned int uidx, unity_file_st_t *ufile);
int lib_unity_file_delete_table(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb);
int lib_unity_file_delete_table_any(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, const unsigned int uidx);
int lib_unity_file_count(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, unsigned int *count);
int lib_unity_file_max(lib_unity_file_t *uf, const enum UF_TYPE utype, const enum UF_DB udb, unsigned int *max);

void lib_unity_file_n4_to_str(unsigned char s_n4[9], unity_file_st_t *ufile);
void lib_unity_file_vd4_to_str(unsigned char s_vd4[9], unity_file_st_t *ufile);
void lib_unity_file_hd_n4_to_str(unsigned char s_n4[9], unity_file_hd_t *hd);
void lib_unity_file_hd_vd4_to_str(unsigned char s_vd4[9], unity_file_hd_t *hd);



/*@*/
#ifdef __cplusplus
}
#endif
#endif


