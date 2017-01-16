#ifndef __DATABASE_H__
#define __DATABASE_H__


/*
 * 函数返回码定义
 */
#define DB_NULL			(NULL) 
#define DB_OK           		0   /* Successful result */
#define DB_ERROR        1   /* SQL error or missing database */
#define DB_INTERNAL     2   /* Internal logic error in SQLite */
#define DB_PERM         3   /* Access permission denied */
#define DB_ABORT        4   /* Callback routine requested an abort */
#define DB_BUSY         5   /* The database file is locked */
#define DB_LOCKED       6   /* A table in the database is locked */
#define DB_NOMEM        7   /* A malloc() failed */
#define DB_READONLY     8   /* Attempt to write a readonly database */
#define DB_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define DB_IOERR       10   /* Some kind of disk I/O error occurred */
#define DB_CORRUPT     11   /* The database disk image is malformed */
#define DB_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define DB_FULL        13   /* Insertion failed because database is full */
#define DB_CANTOPEN    14   /* Unable to open the database file */
#define DB_PROTOCOL    15   /* Database lock protocol error */
#define DB_EMPTY       16   /* Database is empty */
#define DB_SCHEMA      17   /* The database schema changed */
#define DB_TOOBIG      18   /* String or BLOB exceeds size limit */
#define DB_CONSTRAINT  19   /* Abort due to constraint violation */
#define DB_MISMATCH    20   /* Data type mismatch */
#define DB_MISUSE      21   /* Library used incorrectly */
#define DB_NOLFS       22   /* Uses OS features not supported on host */
#define DB_AUTH        23   /* Authorization denied */
#define DB_FORMAT      24   /* Auxiliary database format error */
#define DB_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define DB_NOTADB      26   /* File opened that is not a database file */
#define DB_NOTICE      27   /* Notifications from sqlite3_log() */
#define DB_WARNING     28   /* Warnings from sqlite3_log() */
#define DB_ROW         100  /* sqlite3_step() has another row ready */
#define DB_DONE        101  /* sqlite3_step() has finished executing */



/*
 * 管理卡使用记录
 */
struct admin_card_info
{
	unsigned short sn;			//流水号
	unsigned char pid[8];              //物理卡号
	unsigned char ticket[42];		//票卡信息
	unsigned char rentinfo[36];	//租还车记录
	unsigned char time[7]; 		 //YYYYMMDDHHmmSS
}__attribute__((packed));
typedef struct admin_card_info admin_card_info_t;

/*
 * 异常处理扣费交易记录
 */
 struct exception_handle_record
{
	unsigned short sn;			//流水号
	unsigned char pid[8];              //物理卡号
	unsigned char record[90];      //交易记录
}__attribute__((packed));
typedef struct exception_handle_record exception_handle_record_t;



typedef struct database database_t;

database_t *db_create(const char *pathname);
void db_destroy(database_t *db);
int db_insert_admin_card_info(database_t *db, const unsigned short sn, admin_card_info_t *info);
int db_update_admin_card_info(database_t *db, const unsigned short sn, admin_card_info_t *info);
int db_select_admin_card_info(database_t *db, const unsigned short sn, admin_card_info_t *info);
int db_delete_admin_card_info(database_t *db, const unsigned short sn);
int db_count_admin_card_info(database_t *db, unsigned int *count);
int db_insert_exception_handle_record(database_t *db, const unsigned short sn, exception_handle_record_t *record);
int db_update_exception_handle_record(database_t *db, const unsigned short sn, admin_card_info_t *info);
int db_select_exception_handle_record(database_t *db, const unsigned short sn, admin_card_info_t *info);
int db_delete_exception_handle_record(database_t *db, const unsigned short sn);
int db_count_exception_handle_record(database_t *db, unsigned int *count);



#endif



