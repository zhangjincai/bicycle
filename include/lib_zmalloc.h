#ifndef __LIB_ZMALLOC_H__
#define __LIB_ZMALLOC_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


char *lib_zmalloc_version(void);
char *lib_zmalloc_last_modify_datetime(void);
void *lib_zmalloc(size_t size);
void *lib_zcalloc(size_t size);
void *lib_zrealloc(void *ptr, size_t size);
void lib_zfree(void *ptr);
char *lib_zstrdup(const char *s);
size_t lib_zmalloc_used_memory(void);
void lib_zmalloc_enable_thread_safeness(void);
void lib_zmalloc_set_oom_handler(void (*oom_handler)(size_t));
float lib_zmalloc_get_fragmentation_ratio(size_t rss);
size_t lib_zmalloc_get_rss(void);
size_t lib_zmalloc_get_private_dirty(void);
void lib_zlibc_free(void *ptr);
size_t lib_zmalloc_size(void *ptr);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

