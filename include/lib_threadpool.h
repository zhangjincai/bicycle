#ifndef __LIB_THREADPOOL_H__
#define __LIB_THREADPOOL_H__


#ifdef __cplusplus__ 
extern "C" {
#endif

#define LIB_TP_NULL				NULL
#define LIB_TP_EOK				0
#define LIB_TP_ERROR			-1

#define LIB_TP_FALSE				0
#define LIB_TP_TRUE				1


typedef struct threadpool lib_threadpool_t;

char *lib_threadpool_version(void);
lib_threadpool_t *lib_threadpool_create(const unsigned int threads, const unsigned int queue_max_size);
int lib_threadpool_destroy(lib_threadpool_t *tpool);
int lib_threadpool_add(lib_threadpool_t *tpool, void*(*function)(void *arg), void *arg);
int lib_threadpool_all_threads(lib_threadpool_t *tpool);
int lib_threadpool_busy_threads(lib_threadpool_t *tpool);
int lib_threadpool_tasks(lib_threadpool_t *tpool);

size_t lib_threadpool_used_memory(void);



#ifdef __cplusplus__
}
#endif
#endif


