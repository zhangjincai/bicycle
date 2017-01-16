#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "sqlite3.h"
#include "database.h"


#define TBL_ADMIN_CARD						"tbl_admin_card"
#define TBL_EXCEPT_HANDLE_RECORD			"tbl_except_hanle_record"


union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};  

struct database
{
	sqlite3 *db;
	char pathname[64];
	int sem_id;
	union semun arg;
};




static void __sleep(const unsigned int s)
{
	struct timeval tv;

	tv.tv_sec = s;
	tv.tv_usec = 0;
	select(0, NULL, NULL, NULL, &tv);
}

static void __msleep(const unsigned int ms)
{
	struct timeval tv;

	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms * 1000 % 1000000;

	select(0, NULL, NULL, NULL, &tv);
}

static void __usleep(const unsigned int us)
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = us;
	select(0, NULL, NULL, NULL, &tv);
}

static int __sqlite_busy_callback(void *ptr, int count)
{
	if(count > 10)
		return 0;

	fprintf(stderr, "sqlite busy callback!\n");
	
	__usleep(1000); 	// 1ms
	return 1;
}

static int __sem_p(struct database *db)
{
	struct sembuf P;	
	
	P.sem_num = 0;
	P.sem_op = -1; 	//表示为减1操作 
	P.sem_flg = SEM_UNDO;

	return semop(db->sem_id, &P, 1);	
}

static int __sem_v(struct database *db)
{
	struct sembuf V;
	V.sem_num = 0;
	V.sem_op = 1; //表示为加1操作 
	V.sem_flg = SEM_UNDO;

	return semop(db->sem_id, &V, 1);
}



static int __sqlite3_create(struct database *db)
{
	if(db == NULL)
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char *errmsg = NULL;	
	char sql_cmd[512] = {0};	
	
	rc = sqlite3_open(db->pathname, &(db->db));
	if(rc != SQLITE_OK)
	{
		if(db->db != NULL)
		{
			sqlite3_close(db->db);
			db->db = NULL;
		}
		
		fprintf(stderr, "Can't open sqlite3 %s: %s, errno: %d\n",db->pathname, sqlite3_errmsg(db->db), rc);
		return DB_ERROR;
	}

	/* sem mutex */
	int id = time(NULL) % 100;
	key_t key = ftok(db->pathname, id);
	fprintf(stderr, "sem key: %d\n", key);

	db->sem_id = -1;
	db->sem_id = semget(key, 1, 0666 | IPC_CREAT);
	db->arg.val = 1;
	semctl(db->sem_id, 0, SETVAL, db->arg);

	sqlite3_busy_handler(db->db, __sqlite_busy_callback, (void*)db->db);	

	/* journal_mode */
	strcpy(sql_cmd, "PRAGMA journal_mode=OFF");
	rc = sqlite3_exec(db->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA journal_mode=OFF, errmsg = %s\n", errmsg);	
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		
		goto err;
	}

	/* synchronous */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	strcpy(sql_cmd, "PRAGMA synchronous=OFF");
	rc = sqlite3_exec(db->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA synchronous=OFF, errmsg = %s\n", errmsg);	
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		
		goto err;
	}

	/* case_sensitive_like */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	strcpy(sql_cmd, "PRAGMA case_sensitive_like=1");
	rc = sqlite3_exec(db->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA case_sensitive_like=1, errmsg = %s\n", errmsg);
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}	
		
		goto err;
	}

	/* 
	 * tbl_admin_card
	*/
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(sn INTEGER PRIMARY KEY DEFAULT 0, record BLOB)", TBL_ADMIN_CARD);
	
	rc = sqlite3_exec(db->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", TBL_ADMIN_CARD, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto err;
	}
		
	/* 
	 * tbl_except_hanle_record
	*/
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(sn INTEGER PRIMARY KEY DEFAULT 0, record BLOB)", TBL_EXCEPT_HANDLE_RECORD);

	rc = sqlite3_exec(db->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", TBL_EXCEPT_HANDLE_RECORD, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto err;
	}
		




	return DB_OK;

err:
	if(errmsg != NULL)
	{
		sqlite3_free(errmsg);
		errmsg = NULL;
	}

	if(db->sem_id != -1)
	{
		semctl(db->sem_id , 0, IPC_RMID, db->arg);
		db->sem_id = -1;
	}
	
	if(db->db != NULL)
	{
		sqlite3_close(db->db);
		db->db = NULL;
	}
	
	return DB_ERROR;
}





database_t *db_create(const char *pathname)
{
	if(pathname == NULL)
		return DB_NULL;
		
	struct database *db =  (struct database *)malloc(sizeof(struct database));
	if(db == NULL)
		return DB_NULL;

	memset(db, 0, sizeof(struct database));

	strcpy(db->pathname, pathname);
	db->db = NULL;

	if(__sqlite3_create(db) == DB_OK);
		return db;

	if(db->sem_id != -1)
	{
		semctl(db->sem_id , 0, IPC_RMID, db->arg);
		db->sem_id = -1;
	}
	
	if(db != NULL)
	{
		free(db);
		db = NULL;
	}

	fprintf(stderr, "db create failed!\n");

	return DB_NULL;
	
}

void db_destroy(database_t *db)
{
	if(db == NULL)
		return;
	
	if(db->sem_id != -1)
	{
		semctl(db->sem_id , 0, IPC_RMID, db->arg);
		db->sem_id = -1;
	}
	
	if(db->db != NULL)
	{
		sqlite3_close(db->db);
		db->db = NULL;
	}

	free(db);
	db = NULL;	
}

int db_insert_admin_card_info(database_t *db, const unsigned short sn, admin_card_info_t *info)
{
	if((db == NULL) || (info == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;

	sprintf(sql_cmd, "INSERT INTO %s(sn, record) VALUES (?,?);", TBL_ADMIN_CARD);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	sqlite3_bind_int(stmt, 1, (unsigned int)sn);
	sqlite3_bind_blob(stmt, 2, info, sizeof(admin_card_info_t), SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_OK;
}

int db_update_admin_card_info(database_t *db, const unsigned short sn, admin_card_info_t *info)
{
	if((db == NULL) || (info == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;

	sprintf(sql_cmd, "UPDATE %s SET record=? WHERE sn=%u;", TBL_ADMIN_CARD, sn);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	sqlite3_bind_int(stmt, 1, sn);
	sqlite3_bind_blob(stmt, 2, info, sizeof(admin_card_info_t), SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_OK;	
}

int db_select_admin_card_info(database_t *db, const unsigned short sn, admin_card_info_t *info)
{
	if((db == NULL) || (info == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	admin_card_info_t *pinfo = NULL;

	sprintf(sql_cmd, "SELECT * FROM %s WHERE sn = %u;", TBL_ADMIN_CARD, sn);

	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		sqlite3_column_int(stmt, 0);
		pinfo = (admin_card_info_t *)sqlite3_column_blob(stmt, 1);

		if(pinfo != NULL)
		{
			memcpy(info, pinfo, sizeof(admin_card_info_t));
			sqlite3_finalize(stmt);
			
			__sem_v(db);
			return DB_OK;	
		}		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);

	return DB_OK;
}

int db_delete_admin_card_info(database_t *db, const unsigned short sn)
{
	if(db == NULL)
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	sprintf(sql_cmd, "DELETE FROM %s WHERE sn = %u;", TBL_ADMIN_CARD, sn);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //执行成功返回SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_OK;
}

int db_count_admin_card_info(database_t *db, unsigned int *count)
{
	if((db == NULL) || (count == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	unsigned int m_count = 0;

	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", TBL_ADMIN_CARD);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		m_count = sqlite3_column_int(stmt, 0);
		*count = m_count;
		sqlite3_finalize(stmt);
			
		__sem_v(db);
		return DB_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_DONE;	//查询失败
}

int db_insert_exception_handle_record(database_t *db, const unsigned short sn, exception_handle_record_t *record)
{
	if((db == NULL) || (record == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;

	sprintf(sql_cmd, "INSERT INTO %s(sn, record) VALUES (?,?);", TBL_EXCEPT_HANDLE_RECORD);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	sqlite3_bind_int(stmt, 1, sn);
	sqlite3_bind_blob(stmt, 2, record, sizeof(exception_handle_record_t), SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_OK;	
}

int db_update_exception_handle_record(database_t *db, const unsigned short sn, admin_card_info_t *info)
{
	if((db == NULL) || (info == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;

	sprintf(sql_cmd, "UPDATE %s SET record=? WHERE sn=%u;", TBL_EXCEPT_HANDLE_RECORD, sn);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	sqlite3_bind_int(stmt, 1, sn);
	sqlite3_bind_blob(stmt, 2, info, sizeof(admin_card_info_t), SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_OK;	
}

int db_select_exception_handle_record(database_t *db, const unsigned short sn, admin_card_info_t *info)
{
	if((db == NULL) || (info == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	admin_card_info_t *pinfo = NULL;

	sprintf(sql_cmd, "SELECT * FROM %s WHERE sn = %u;", TBL_EXCEPT_HANDLE_RECORD, sn);

	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		sqlite3_column_int(stmt, 0);
		pinfo = (admin_card_info_t *)sqlite3_column_blob(stmt, 1);

		if(pinfo != NULL)
		{
			memcpy(info, pinfo, sizeof(admin_card_info_t));
			sqlite3_finalize(stmt);
			
			__sem_v(db);
			return DB_OK;	
		}		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);

	return DB_OK;
}

int db_delete_exception_handle_record(database_t *db, const unsigned short sn)
{
	if(db == NULL)
		return DB_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	sprintf(sql_cmd, "DELETE FROM %s WHERE sn = %u;", TBL_EXCEPT_HANDLE_RECORD, sn);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //执行成功返回SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_OK;
}

int db_count_exception_handle_record(database_t *db, unsigned int *count)
{
	if((db == NULL) || (count == NULL))
		return DB_ERROR;

	int rc = SQLITE_ERROR;   
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	unsigned int m_count = 0;

	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", TBL_EXCEPT_HANDLE_RECORD);
	
	__sem_p(db);
	rc = sqlite3_prepare_v2(db->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(db);
		return DB_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		m_count = sqlite3_column_int(stmt, 0);
		*count = m_count;
		sqlite3_finalize(stmt);
			
		__sem_v(db);
		return DB_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(db);
	
	return DB_DONE;	//查询失败
}














