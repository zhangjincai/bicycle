#ifndef __LIB_EVENTLOOP_H__
#define __LIB_EVENTLOOP_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#define LIB_EP_NULL					NULL
#define LIB_EP_ERR					-1
#define LIB_EP_OK					0

#define LIB_EP_NONE					0
#define LIB_EP_READ					1
#define LIB_EP_WRITE					2

#define LIB_EP_FILE_EV				1
#define LIB_EP_TIME_EV 				2
#define LIB_EP_ALL_EV		 		(LIB_EP_FILE_EV | LIB_EP_TIME_EV)
#define LIB_EP_DONT_WAIT 			4

#define LIB_EP_NOMORE				-1

typedef struct event_loop lib_event_loop_t;
typedef void ep_file_proc(lib_event_loop_t *ep, int fd, void *client_data, int mask);
typedef int ep_time_proc(lib_event_loop_t *ep, long long id, void *client_data);
typedef void ep_time_finalizer_proc(lib_event_loop_t *ep, void *client_data);
typedef void ep_before_sleep_proc(lib_event_loop_t *ep);

char *lib_event_loop_version(void);
char *lib_event_loop_last_modify_datetime(void);
lib_event_loop_t *lib_event_loop_create(int size);
int lib_event_loop_destroy(lib_event_loop_t *ep);
int lib_event_loop_add(lib_event_loop_t *ep, int fd, int add_mask, ep_file_proc *proc, void *client_data, const int client_data_size);
int lib_event_loop_mod(lib_event_loop_t *ep, int fd, int mod_mask);
int lib_event_loop_del(lib_event_loop_t *ep, int fd, int del_mask);
int lib_event_loop_send_clientdata(lib_event_loop_t *ep, int fd, void *client_data, const int client_data_size);
int lib_event_loop_file_count(lib_event_loop_t *ep);
void lib_event_loop_stop(lib_event_loop_t *ep);
void lib_event_before_sleep_proc_set(lib_event_loop_t *ep, ep_before_sleep_proc *before_sleep_proc);
long long lib_event_loop_time_create(lib_event_loop_t *ep, long long ms, ep_time_proc *time_proc, 
			void *client_data, ep_time_finalizer_proc *final_proc); 
int lib_event_time_del(lib_event_loop_t *ep, long long id);
int lib_event_loop_time_count(lib_event_loop_t *ep);
int lib_event_loop(lib_event_loop_t *ep);


/*@*/
#ifdef __cplusplus
}
#endif

#endif



