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
#include "lib_blacklist.h"


#define TBL_TOTAL							"tbl_total"
#define TBL_INCREMENT						"tbl_increment"
#define TBL_DECREMENT						"tbl_decrement"
#define TBL_TEMPORARY						"tbl_temporary"


union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};   

struct bl
{
	sqlite3 *db;
	char pathname[64];
	int sem_id;
	union semun arg;
};

void bl_sleep(const unsigned int s)
{
	struct timeval tv;

	tv.tv_sec = s;
	tv.tv_usec = 0;
	select(0, NULL, NULL, NULL, &tv);
}

void bl_msleep(const unsigned int ms)
{
	struct timeval tv;

	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms * 1000 % 1000000;

	select(0, NULL, NULL, NULL, &tv);
}

void bl_usleep(const unsigned int us)
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
	
	bl_usleep(1000); 	// 1ms
	return 1;
}

static int __sem_p(lib_bl_t *bl)
{
	struct sembuf P;	
	
	P.sem_num = 0;
	P.sem_op = -1; 	//��ʾΪ��1���� 
	P.sem_flg = SEM_UNDO;

	return semop(bl->sem_id, &P, 1);	
}

static int __sem_v(lib_bl_t *bl)
{
	struct sembuf V;
	V.sem_num = 0;
	V.sem_op = 1; //��ʾΪ��1���� 
	V.sem_flg = SEM_UNDO;

	return semop(bl->sem_id, &V, 1);
}


void lib_bl_destroy_v2(lib_bl_t *bl)
{
	assert(bl);

	if(bl->sem_id != -1)
	{
		semctl(bl->sem_id , 0, IPC_RMID, bl->arg);
		bl->sem_id = -1;
	}
	
	if(bl->db != NULL)
	{
		sqlite3_close(bl->db);
		bl->db = NULL;
	}

	free(bl);
	bl = NULL;
}


int lib_bl_update_data_v2(lib_bl_t *bl, const char *lid, const char *type)
{
	if((bl == NULL) || (lid == NULL) || (type == NULL))
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	strcpy(tbl, TBL_TOTAL);
	
	sprintf(sql_cmd, "UPDATE %s SET type=? WHERE lid=%s;", tbl, lid); //���º���������
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	sqlite3_bind_text(stmt, 1, type, 1, SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_OK;		
}

int lib_bl_replace_data_v2(lib_bl_t *bl, const char *lid, const char *type)
{
	if((bl == NULL) || (lid == NULL) || (type == NULL))
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	strcpy(tbl, TBL_TOTAL);
	
	sprintf(sql_cmd, "REPLACE INTO %s(lid, type) VALUES (?,?);",  tbl);

	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	sqlite3_bind_text(stmt, 1, lid, 16, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, type, 1, SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_OK;		
}

int lib_bl_delete_table_v2(lib_bl_t *bl)
{
	if(bl == NULL)
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	strcpy(tbl, TBL_TOTAL);
			
	sprintf(sql_cmd, "DELETE FROM %s;", tbl);
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_OK;			
}

int lib_bl_delete_table_any_v2(lib_bl_t *bl, const char *lid)
{
	if(bl == NULL)
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	strcpy(tbl, TBL_TOTAL);

	sprintf(sql_cmd, "DELETE FROM %s WHERE lid = %s;", tbl, lid);
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //ִ�гɹ�����SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_OK;			
}

int lib_bl_count_v2(lib_bl_t *bl, unsigned int *count)
{
	if((bl == NULL) || (count == NULL))
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	unsigned int m_count = 0;
	
	strcpy(tbl, TBL_TOTAL);
			
	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", tbl);
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //���rc==SQLITE_DONE��˵����ѯʧ��
	{
		m_count = sqlite3_column_int(stmt, 0);
		*count = m_count;
		if(stmt != NULL)
			sqlite3_finalize(stmt);
			
		__sem_v(bl);
		return LIB_BL_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_DONE;	//��ѯʧ��
}

int lib_bl_max_v2(lib_bl_t *bl, unsigned int *max)
{
	if((bl == NULL) || (max == NULL))
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	unsigned int m_max = 0;
	
	strcpy(tbl, TBL_TOTAL);

	sprintf(sql_cmd, "SELECT MAX(idx) FROM %s;", tbl);
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //���rc==SQLITE_DONE��˵����ѯʧ��
	{
		m_max = sqlite3_column_int(stmt, 0);
		*max = m_max;
		if(stmt != NULL)
			sqlite3_finalize(stmt);
			
		__sem_v(bl);
		return LIB_BL_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_DONE;	//��ѯʧ��	
}


/* ----------------------�޸İ棬��������ѯ������----------------------- */
static int __sqlite3_create_v2(struct bl *bl)
{
	assert(bl);
	
	int rc = SQLITE_ERROR;
	char *errmsg = NULL;	
	char sql_cmd[512] = {0};

	rc = sqlite3_open(bl->pathname, &(bl->db));
	if(rc != SQLITE_OK)
	{
		sqlite3_close(bl->db);
		fprintf(stderr, "Can't open sqlite3 %s: %s\n", bl->pathname, sqlite3_errmsg(bl->db));
		return LIB_BL_ERROR;
	}

	/* sem mutex */
	int id = time(NULL) % 100;
	key_t key = ftok(bl->pathname, id);
	fprintf(stderr, "sem key: %d\n", key);

	bl->sem_id = -1;
	bl->sem_id = semget(key, 1, 0666 | IPC_CREAT);
	bl->arg.val = 1;
	semctl(bl->sem_id, 0, SETVAL, bl->arg);

	sqlite3_busy_handler(bl->db, __sqlite_busy_callback, (void*)bl->db);

	/* journal_mode */
	strcpy(sql_cmd, "PRAGMA journal_mode=OFF");
	rc = sqlite3_exec(bl->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA journal_mode=OFF, errmsg = %s\n", errmsg);	
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/* synchronous */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	strcpy(sql_cmd, "PRAGMA synchronous=OFF");
	rc = sqlite3_exec(bl->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA synchronous=OFF, errmsg = %s\n", errmsg);	
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/* case_sensitive_like */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	strcpy(sql_cmd, "PRAGMA case_sensitive_like=1");
	rc = sqlite3_exec(bl->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA case_sensitive_like=1, errmsg = %s\n", errmsg);
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}	
		goto ERR;
	}

	/* 
	 * tbl_total
	*/
	memset(sql_cmd, 0, sizeof(sql_cmd));
	
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
					pid TEXT, lid TEXT, type TEXT)", TBL_TOTAL);
	
	rc = sqlite3_exec(bl->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", TBL_TOTAL, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	if(errmsg != NULL)
	{
		sqlite3_free(errmsg);
		errmsg = NULL;
	}
	
	return LIB_BL_OK;

ERR:
	if(errmsg != NULL)
	{
		sqlite3_free(errmsg);
		errmsg = NULL;
	}

	if(bl->sem_id != -1)
	{
		semctl(bl->sem_id , 0, IPC_RMID, bl->arg);
		bl->sem_id = -1;
	}
	
	if(bl->db != NULL)
	{
		sqlite3_close(bl->db);
		bl->db = NULL;
	}
	
	return LIB_BL_ERROR;
}

lib_bl_t *lib_bl_create_v2(const char *pathname)
{
	assert(pathname);	

	int err = -1;
	struct bl *bl = NULL;

	bl = (struct bl *)malloc(sizeof(struct bl));
	if(bl == NULL)
		goto ERR;

	memset(bl, 0, sizeof(struct bl));

	strcpy(bl->pathname, pathname);
	bl->db = NULL;

	err = __sqlite3_create_v2(bl);
	if(err != LIB_BL_OK)
		goto ERR;


	fprintf(stderr, "lib blacklist create ok\n");
	return bl;

ERR:
	if(bl->sem_id != -1)
	{
		semctl(bl->sem_id , 0, IPC_RMID, bl->arg);
		bl->sem_id = -1;
	}
	
	if(bl != NULL)
	{
		free(bl);
		bl = NULL;
	}

	fprintf(stderr, "lib blacklist create failed!\n");

	return LIB_BL_NULL;	
}


int lib_bl_insert_data_v2(lib_bl_t *bl, const char *pid, const char *lid, const char *type)
{
	if(bl == NULL || pid == NULL || lid == NULL || type == NULL)
		return LIB_BL_ERROR;
	
	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;

	
	strcpy(tbl, TBL_TOTAL);

	sprintf(sql_cmd, "INSERT INTO %s(pid, lid, type) VALUES (?,?,?);", tbl);
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	sqlite3_bind_text(stmt, 1, pid, 16, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, lid, 16, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, type, 1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_OK;
}

int lib_bl_select_data_v2(lib_bl_t *bl, const char *lid)
{
	if((bl == NULL) || (lid == NULL))
		return LIB_BL_ERROR;

	if(bl->db == NULL)
		return LIB_BL_ERROR;

	int rc = SQLITE_ERROR;
	char tbl[32] = {0};
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;
	
	strcpy(tbl, TBL_TOTAL);

	sprintf(sql_cmd, "SELECT * FROM %s WHERE lid = %s;", tbl, lid);
	
	__sem_p(bl);
	rc = sqlite3_prepare_v2(bl->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //���rc==SQLITE_DONE��˵����ѯʧ��
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(bl);
		return LIB_BL_OK;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(bl);
	
	return LIB_BL_DONE;	//��ѯʧ��
}

