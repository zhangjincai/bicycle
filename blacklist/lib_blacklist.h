#ifndef __LIB_BLACKLIST_H__
#define __LIB_BLACKLIST_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include "universal_file.h"

/*
 * 函数返回码定义
 */
#define LIB_BL_NULL			(NULL) 
#define LIB_BL_OK           		0   /* Successful result */
#define LIB_BL_ERROR        1   /* SQL error or missing database */
#define LIB_BL_INTERNAL     2   /* Internal logic error in SQLite */
#define LIB_BL_PERM         3   /* Access permission denied */
#define LIB_BL_ABORT        4   /* Callback routine requested an abort */
#define LIB_BL_BUSY         5   /* The database file is locked */
#define LIB_BL_LOCKED       6   /* A table in the database is locked */
#define LIB_BL_NOMEM        7   /* A malloc() failed */
#define LIB_BL_READONLY     8   /* Attempt to write a readonly database */
#define LIB_BL_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define LIB_BL_IOERR       10   /* Some kind of disk I/O error occurred */
#define LIB_BL_CORRUPT     11   /* The database disk image is malformed */
#define LIB_BL_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define LIB_BL_FULL        13   /* Insertion failed because database is full */
#define LIB_BL_CANTOPEN    14   /* Unable to open the database file */
#define LIB_BL_PROTOCOL    15   /* Database lock protocol error */
#define LIB_BL_EMPTY       16   /* Database is empty */
#define LIB_BL_SCHEMA      17   /* The database schema changed */
#define LIB_BL_TOOBIG      18   /* String or BLOB exceeds size limit */
#define LIB_BL_CONSTRAINT  19   /* Abort due to constraint violation */
#define LIB_BL_MISMATCH    20   /* Data type mismatch */
#define LIB_BL_MISUSE      21   /* Library used incorrectly */
#define LIB_BL_NOLFS       22   /* Uses OS features not supported on host */
#define LIB_BL_AUTH        23   /* Authorization denied */
#define LIB_BL_FORMAT      24   /* Auxiliary database format error */
#define LIB_BL_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define LIB_BL_NOTADB      26   /* File opened that is not a database file */
#define LIB_BL_NOTICE      27   /* Notifications from sqlite3_log() */
#define LIB_BL_WARNING     28   /* Warnings from sqlite3_log() */
#define LIB_BL_ROW         100  /* sqlite3_step() has another row ready */
#define LIB_BL_DONE        101  /* sqlite3_step() has finished executing */


enum BL_TYPE
{
	BL_TYPE_TOTAL = 0x71,       	//总量黑名单
	BL_TYPE_INCREMENT = 0x72,	//增量黑名单
	BL_TYPE_DECREMENT = 0x73,	//减量黑名单
	BL_TYPE_TEMPORARY = 0x74,	//临时黑名单
	BL_TYPE_END
};


typedef struct bl lib_bl_t;



lib_bl_t *lib_bl_create_v2(const char *pathname);
int lib_bl_insert_data_v2(lib_bl_t *bl, const char *pid, const char *lid, const char *type);
int lib_bl_select_data_v2(lib_bl_t *bl, const char *lid);
int lib_bl_update_data_v2(lib_bl_t *bl, const char *lid, const char *type);
int lib_bl_replace_data_v2(lib_bl_t *bl, const char *lid, const char *type);
int lib_bl_delete_table_v2(lib_bl_t *bl);
int lib_bl_delete_table_any_v2(lib_bl_t *bl, const char *lid);
int lib_bl_count_v2(lib_bl_t *bl, unsigned int *count);
int lib_bl_max_v2(lib_bl_t *bl, unsigned int *max);
void lib_bl_destroy_v2(lib_bl_t *bl);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

