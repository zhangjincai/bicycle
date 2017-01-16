#ifndef __LIB_FIRMWARE_H__
#define __LIB_FIRMWARE_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include "universal_file.h"

/*
 * 函数返回码定义
 */
#define LIB_FW_NULL			(NULL) 
#define LIB_FW_OK           		0   /* Successful result */
#define LIB_FW_ERROR        1   /* SQL error or missing database */
#define LIB_FW_INTERNAL     2   /* Internal logic error in SQLite */
#define LIB_FW_PERM         3   /* Access permission denied */
#define LIB_FW_ABORT        4   /* Callback routine requested an abort */
#define LIB_FW_BUSY         5   /* The database file is locked */
#define LIB_FW_LOCKED       6   /* A table in the database is locked */
#define LIB_FW_NOMEM        7   /* A malloc() failed */
#define LIB_FW_READONLY     8   /* Attempt to write a readonly database */
#define LIB_FW_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define LIB_FW_IOERR       10   /* Some kind of disk I/O error occurred */
#define LIB_FW_CORRUPT     11   /* The database disk image is malformed */
#define LIB_FW_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define LIB_FW_FULL        13   /* Insertion failed because database is full */
#define LIB_FW_CANTOPEN    14   /* Unable to open the database file */
#define LIB_FW_PROTOCOL    15   /* Database lock protocol error */
#define LIB_FW_EMPTY       16   /* Database is empty */
#define LIB_FW_SCHEMA      17   /* The database schema changed */
#define LIB_FW_TOOBIG      18   /* String or BLOB exceeds size limit */
#define LIB_FW_CONSTRAINT  19   /* Abort due to constraint violation */
#define LIB_FW_MISMATCH    20   /* Data type mismatch */
#define LIB_FW_MISUSE      21   /* Library used incorrectly */
#define LIB_FW_NOLFS       22   /* Uses OS features not supported on host */
#define LIB_FW_AUTH        23   /* Authorization denied */
#define LIB_FW_FORMAT      24   /* Auxiliary database format error */
#define LIB_FW_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define LIB_FW_NOTADB      26   /* File opened that is not a database file */
#define LIB_FW_NOTICE      27   /* Notifications from sqlite3_log() */
#define LIB_FW_WARNING     28   /* Warnings from sqlite3_log() */
#define LIB_FW_ROW         100  /* sqlite3_step() has another row ready */
#define LIB_FW_DONE        101  /* sqlite3_step() has finished executing */



typedef struct fw lib_fw_t;


lib_fw_t *lib_fw_create(const char *pathname);
void lib_fw_destroy(lib_fw_t *fw);

int lib_fw_insert_data(lib_fw_t *fw, const unsigned int ufile_idx, univ_file_st_t *univ);
int lib_fw_update_data(lib_fw_t *fw, const unsigned int ufile_idx, univ_file_st_t *univ);
int lib_fw_replace_data(lib_fw_t *fw, const unsigned int ufile_idx, univ_file_st_t *univ);
int lib_fw_select_data(lib_fw_t *fw, const unsigned int ufile_idx, univ_file_st_t *univ);
int lib_fw_delete_table(lib_fw_t *fw);
int lib_fw_count(lib_fw_t *fw, unsigned int *count);
int lib_fw_max(lib_fw_t *fw, unsigned int *max);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

