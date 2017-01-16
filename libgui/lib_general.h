#ifndef __LIB_GENERAL_H__
#define __LIB_GENERAL_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <sys/un.h>

/*
 * 函数返回码定义
 */
#define LIB_GE_NULL				(NULL) 
#define LIB_GE_EOK				(0)  //正常
#define LIB_GE_ERROR			(-1) //错误
#define LIB_GE_ETIMEOUT			(-2) //超时
#define LIB_GE_EFULL			(-3) //满
#define LIB_GE_EEMPTY			(-4) //空
#define LIB_GE_ENOMEM 			(-5) //内存不够
#define LIB_GE_EXCMEM			(-6) //内存越界
#define LIB_GE_EBUSY			(-7) //忙

#define LIB_GE_TRUE				(1)
#define LIB_GE_FALSE			(0)


/*
 * 获取通用库版本
 */
char *lib_general_version(void);

/*
 * 获取通用库最后修改时间
 */
void lib_general_last_modify_datetime(char s_time[64]);
 
/*
 * 通用函数
 */
void lib_printf(char *title, unsigned char *data, const int len);
void lib_printf_v2(char *title, unsigned char *data, const int len, const unsigned int cb);


void lib_vprintf(const char *format, ...);
void lib_vsprintf(char *str, const char *format, ...);
void lib_vfprintf(const char *format, ...);


/*
 * 程序运行时间测试
 */
void lib_begin_runtime_printf(void);
void lib_end_runtime_printf(void);


/*
 * 时间定义
 */
struct lib_datetime
{
	unsigned short year;
	unsigned char mon;
	unsigned char day;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
}__attribute__((packed));
typedef struct lib_datetime lib_datetime_t;


/*
 * 串口操作
 */
 typedef struct lib_serial
{
	int sfd;   				//设备套接字
	char pathname[128]; 	//设备描述符
	unsigned int flags;		//读写权限		
		
	unsigned int speed;			
	unsigned int databits;       //数据位
	unsigned int stopbits;      //停止位
}lib_serial_t;

/*
  * 波特率支持:2400,4800,9600,38400,57600,115200,   默认9600
  */
int lib_serial_init(lib_serial_t *sel);
int lib_serial_setup(lib_serial_t *sel);  //串口设置
void lib_serial_close(lib_serial_t *sel);

int lib_serial_send_select(lib_serial_t *sel, const void *buff, const unsigned int size, const unsigned int msec);
int lib_serial_recv_select(lib_serial_t *sel, void *buff, const unsigned int size, const unsigned int msec);

int lib_serial_write_select(int fd, const void *buff, const unsigned int size, const unsigned int msec);
int lib_serial_read_select(int fd, void *buff, const unsigned int size, const unsigned int msec);

int lib_serial_writen_select(int fd, const void *buff, const unsigned int size, const unsigned int msec);
int lib_serial_readn_select(int fd, void *buff, const unsigned int size, const unsigned int msec);

int lib_serial_send(int fd, const void *buff, const unsigned int size);
int lib_serial_recv(int fd, void *buff, const unsigned int size);


/*
 * 脱离线程操作
 */
typedef void *(*thread_entry_func)(void *);
int lib_normal_thread_create(pthread_t *pid, thread_entry_func entry, void *para);
void *lib_thread_exit(void *value_ptr);


/*
 * PV操作
 */
int lib_get_random_key(char *kpath);
int lib_sem_init(int *sem_id, const int key);
int lib_sem_destroy(const int sem_id);
int lib_sem_p(const int sem_id);
int lib_sem_v(const int sem_id);


/*
 * 互斥锁
 */
typedef void lib_mutex_t;
lib_mutex_t *lib_mutex_create(void);
int lib_mutex_destroy(lib_mutex_t *mutex);
int lib_mutex_lock(lib_mutex_t *mutex);
int lib_mutex_unlock(lib_mutex_t *mutex);


/*
 * 读写锁
 */
typedef void lib_rwlock_t;
lib_rwlock_t *lib_rwlock_create(void);
int lib_rwlock_destroy(lib_rwlock_t *rwlock);
int lib_rwlock_rdlock(lib_rwlock_t *rwlock);
int lib_rwlock_wrlock(lib_rwlock_t *rwlock);
int lib_rwlock_unlock(lib_rwlock_t *rwlock);

/*
 * 消息队列
 */
int lib_msg_init(int *qid, const int key);
int lib_msg_kill(const int qid);
int lib_msg_send(const int qid, void *pdata, const int size);
int lib_msg_recv(const int qid, void *pdata, const int size, const int msg_type);
int lib_msg_send_and_recv(const int qid, void *pdata, const int size, const int msg_type);


/*
 * 共享内存
 */
int lib_share_mem_init(int *mid, const int key, const unsigned mem_size);
int lib_share_mem_destroy(const int mid);
int lib_share_mem_stop(const void *mem);
void lib_share_mem_read(const int mid, const int offset, char *pdata, const int size);
void lib_share_mem_write(const int mid, const int offset, char *pdata, const int size);


/*
* bcd
*/
#define lib_hex_to_bcd(hex)		(((hex) / 16) * 10 + (hex) % 16)
#define lib_bcd_to_hex(bcd)		(((bcd) / 10) * 16 + (bcd) % 10)
#define lib_dec_to_bcd(dec)		((((dec) / 10) << 4) | ((dec) % 10))
#define lib_bcd_to_dec(bcd)		((((bcd) >> 4) * 10) + ((bcd) & 0xf))

/*
 * atomic 原子操作 (gcc 4.1.2版本之后才支持该功能)
 */
#define lib_atomic_add(ptr, val)			__sync_add_and_fetch((ptr), (val))
#define lib_atomic_sub(ptr, val)			__sync_sub_and_fetch((ptr), (val))
#define lib_atmoic_inc(ptr)					__sync_add_and_fetch((ptr), 1)
#define lib_atmoic_dec(ptr)					__sync_add_and_fetch((ptr), -1)
#define lib_atmoic_set(ptr, val)		 	__sync_lock_test_and_set(ptr, val) 
#define lib_atmoic_get(ptr)					__sync_val_compare_and_swap((ptr), 0, 0)
#define lib_atmoic_set_bit(ptr, pos)		__sync_or_and_fetch((ptr), 1 << (pos))
#define lib_atmoic_clear_bit(ptr, pos)		__sync_and_and_fetch((ptr), ~(1 << (pos)))


/*
* 位操作
*/
#define array_idx(idx, bit)		((idx) / (bit))
#define bit_idx(idx, bit)		((idx) % (bit))

#define lib_clr_bit_num(addr, idx, bit, type)  \
		*(addr + array_idx(idx, bit)) &= (~(((type)0x01) << bit_idx(idx, bit))) \

#define lib_set_bit_num(addr, idx, bit, type)  \
		*(addr + array_idx(idx, bit)) |= (((type)0x01) << bit_idx(idx, bit)) \

#define lib_chk_bit_num(addr, idx, bit, type)  \
		(*(addr + array_idx(idx, bit)) >> bit_idx(idx, bit)) & ((type)0x01) \

#define lib_clr_bit_byte(addr, idx) 		(lib_clr_bit_num((addr), (idx), 8, unsigned char))
#define lib_set_bit_bype(addr, idx) 		(lib_set_bit_num((addr), (idx), 8, unsigned char))
#define lib_chk_bit_bype(addr, idx) 		(lib_chk_bit_num((addr), (idx), 8, unsigned char))

#define lib_clr_bit_short(addr, idx) 		(lib_clr_bit_num((addr), (idx), 16, unsigned short))
#define lib_set_bit_short(addr, idx) 		(lib_set_bit_num((addr), (idx), 16, unsigned short))
#define lib_chk_bit_short(addr, idx) 		(lib_chk_bit_num((addr), (idx), 16, unsigned short))

#define lib_clr_bit_int(addr, idx) 			(lib_clr_bit_num((addr), (idx), 32, unsigned int))
#define lib_set_bit_int(addr, idx) 			(lib_set_bit_num((addr), (idx), 32, unsigned int))
#define lib_chk_bit_int(addr, idx) 			(lib_chk_bit_num((addr), (idx), 32, unsigned int))

#define lib_clr_bit_ll(addr, idx) 			(lib_clr_bit_num((addr), (idx), 64, unsigned long long))
#define lib_set_bit_ll(addr, idx) 			(lib_set_bit_num((addr), (idx), 64, unsigned long long))
#define lib_chk_bit_ll(addr, idx) 			(lib_chk_bit_num((addr), (idx), 64, unsigned long long))

#define lib_clr_bit(addr, idx)				lib_clr_bit_byte((addr), (idx)) 
#define lib_set_bit(addr, idx)				lib_set_bit_bype((addr), (idx)) 
#define lib_chk_bit(addr, idx)				lib_chk_bit_bype((addr), (idx)) 

#define lib_bit_to_byte(bit)				(((bit) + 7) / 8) 

/*
 * 时间操作
 */
void lib_sleep(const unsigned int s);
void lib_msleep(const unsigned int ms);
void lib_usleep(const unsigned int us);
void lib_get_systime_bcd(unsigned char tm[7]);  //YYYY MM DD HH mm SS
void lib_set_systime_bcd(unsigned char tm[7]);  //YYYY MM DD HH MM SS
void lib_get_systime(unsigned char tm[7]);  //YYYY MM DD HH MM SS

/*
 * IO操作
 */
typedef enum _LIB_BLOCK_
{
	LIB_BLOCK = 0,
	LIB_NOBLOCK = 1,
}LIB_BLOCK_EM;

int lib_setfd_blockattr(int fd, LIB_BLOCK_EM type);
int lib_setfd_block(int fd);
int lib_setfd_noblock(int fd);
int lib_readn(int fd, void *buff, const unsigned int nbytes);
int lib_writen(int fd, const void *buff, const unsigned int nbytes);
int lib_read_select(int fd, void *buff, const unsigned int nbytes, const unsigned int msec);
int lib_write_select(int fd, const void *buff, const unsigned int nbytes, const unsigned int msec);
int lib_readn_select(int fd, void *buff, const unsigned int nbytes, const unsigned int msec);
int lib_writen_select(int fd, const void *buff, const unsigned int nbytes, const unsigned int msec);
int lib_close(int fd);

/*
 * 系统信息
 */
struct proc_meminfo
{
	unsigned int MemTotal;
	unsigned int MemFree;
	unsigned int Buffers;
	unsigned int Cached;
	unsigned int SwapCached;
	unsigned int Active;
	unsigned int Inactive;
	unsigned int Active_anon;
	unsigned int Inactive_anon;
	unsigned int Active_file;
	unsigned int Inactive_file;
	unsigned int Unevictable;
	unsigned int Mlocked;
	unsigned int SwapTotal;
	unsigned int SwapFree;
	unsigned int Dirty;
	unsigned int Writeback;
	unsigned int AnonPages;
	unsigned int Mapped;
	unsigned int Shmem;
	unsigned int Slab;
	unsigned int SReclaimable;
	unsigned int SUnreclaim;
	unsigned int KernelStack;
	unsigned int PageTables;
	unsigned int NFS_Unstable;
	unsigned int Bounce;
	unsigned int WritebackTmp;
	unsigned int CommitLimit;
	unsigned int Committed_AS;
	unsigned int VmallocTotal;
	unsigned int VmallocUsed;
	unsigned int VmallocChunk;
};

int lib_get_proc_meminfo(struct proc_meminfo *meminfo);
int lib_get_free_meminfo(unsigned int *size);
int lib_get_total_meminfo(unsigned int *size);
int lib_get_network_flow(const char *ifname, long long *rx_bytes, long long *tx_bytes);

/*
 * 环形缓冲区
 */
typedef struct lib_rb lib_rb_t;
lib_rb_t *lib_rb_create(const unsigned int size);
int lib_rb_destroy(lib_rb_t *rb);
int lib_rb_put(lib_rb_t *rb, const unsigned char *ptr, const unsigned int len);
int lib_rb_get(lib_rb_t *rb, unsigned char *ptr, const unsigned int len);

/*
 * 固定大小的缓冲区-加线程锁
 */
typedef struct lib_regular_rb lib_regular_rb_t;
lib_regular_rb_t *lib_regular_rb_create(const unsigned int blk_cnt, const unsigned int blk_size);
int lib_regular_rb_destroy(lib_regular_rb_t *rb);
int lib_regular_rb_put(lib_regular_rb_t *rb, const unsigned char *ptr, const unsigned int len);
int lib_regular_rb_get(lib_regular_rb_t *rb, unsigned char *ptr, const unsigned int len);
int lib_regular_rb_used_cnt(lib_regular_rb_t *rb);
int lib_regular_rb_free_cnt(lib_regular_rb_t *rb);
int lib_regular_rb_size(lib_regular_rb_t *rb);

/*
 * 循环缓冲区--不带线程锁--优化版
 */
enum LIB_RINGBUFFER_STATE
{
    LIB_RINGBUFFER_EMPTY = 0,       //空
    LIB_RINGBUFFER_FULL,        	//满
    LIB_RINGBUFFER_HALFFULL,    //既不是满,也不是空
};

typedef struct lib_ringbuffer lib_ringbuffer_t;
int lib_ringbuffer_init(lib_ringbuffer_t *rb, unsigned char *pool, const unsigned int size);
lib_ringbuffer_t *lib_ringbuffer_create(const unsigned int size);
int lib_ringbuffer_destroy(lib_ringbuffer_t *rb);
int lib_ringbuffer_put(lib_ringbuffer_t *rb, const unsigned char *ptr, const unsigned int len);
int lib_ringbuffer_get(lib_ringbuffer_t *rb, unsigned char *ptr, const unsigned int len);
int lib_ringbuffer_putchar(lib_ringbuffer_t *rb, const unsigned char ch);
int lib_ringbuffer_getchar(lib_ringbuffer_t *rb, unsigned char *ch);
int lib_ringbuffer_getforce(lib_ringbuffer_t *rb, unsigned char *ptr, const unsigned int len);
int lib_ringbuffer_putforce(lib_ringbuffer_t *rb, const unsigned char *ptr, const unsigned int len);
int lib_ringbuffer_putcharforce(lib_ringbuffer_t *rb, const unsigned char ch);
inline unsigned int lib_ringbuffer_get_size(lib_ringbuffer_t *rb);
inline enum LIB_RINGBUFFER_STATE lib_ringbuffer_get_state(lib_ringbuffer_t *rb);
inline unsigned int lib_ringbuffer_data_len(lib_ringbuffer_t *rb);
inline unsigned int lib_ringbuffer_space_len(lib_ringbuffer_t *rb);


/*
 * 循环双链表
 */
typedef struct lib_list
{
	struct lib_list *next;
	struct lib_list *prev;
}lib_list_t;

#define lib_list_entry(node, type, member) \
	((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

#define lib_list_for_each(node, list) \
	for(node = (list)->next; node != (list); \
		node = node->next)
		
#define lib_list_for_each_prev(node, list) \
	for(node = (list)->prev; node != (list); \
		node = node->prev)

#define lib_list_for_each_safe(node, lnext, list) \
	for(node = (list)->next, lnext = node->next; node != (list);\
		node = lnext, lnext = node->next)

#define LIB_LIST_INIT(l)	{&(l), &(l)}

void lib_list_init(lib_list_t *l);
void lib_list_insert_after(lib_list_t *l, lib_list_t *n);
void lib_list_insert_before(lib_list_t *l, lib_list_t *n);
void lib_list_remove(lib_list_t *n);
int lib_list_isempty(lib_list_t *l);

/*
 * 内存池
 */
typedef struct lib_mem_pool lib_mem_pool_t;
lib_mem_pool_t *lib_mp_create(const unsigned int blk_cnt, const unsigned int blk_size);
void lib_mp_destroy(lib_mem_pool_t *mp);
void *lib_mp_alloc(lib_mem_pool_t *mp, const unsigned int size, int *err);
int lib_mp_free(lib_mem_pool_t *mp, void *ptr);
int lib_mp_used_cnt(lib_mem_pool_t *mp);
int lib_mp_free_cnt(lib_mem_pool_t *mp);
int lib_mp_blk_cnt(lib_mem_pool_t *mp);
int lib_mp_blk_size(lib_mem_pool_t *mp);

/* 
 * CRC16
 */
 /*
 * 按位求法,计算速度慢,占用内存空间少
 */
unsigned int lib_crc16_with_bit(char *pdata, unsigned short length);
/*
 * 使用查找表方法,实现CRC16校验,速度快,占用内存空间大
 */
unsigned int lib_crc16_with_table(char *pdata, unsigned short length);
/*
 * 按半字节查找表方法,占用内存空间较少,速度较快,均衡方法
 */
unsigned int lib_crc16_with_byte(unsigned char *pdata, unsigned int length);

/*
 * 网络操作
 */
#define LIB_IPV4(a, b, c, d)		((d << 24) | (c << 16) |( b << 8) | (a))

char *lib_iaddr_to_saddr(in_addr_t iaddr);
int lib_get_ipaddr(const char *ifname, char *ipaddr);
int lib_get_macaddr(const char *ifname, char *macaddr);
int lib_get_netmask(const char *ifname, char *mask);
int lib_get_gateway(const char *ifname, char *gateway);
int lib_get_dns(char *dns);
in_addr_t lib_get_ifnetmask(const char *ifname);
in_addr_t lib_get_ifdns(void);
in_addr_t lib_get_ifgateway(const char *ifname);
in_addr_t lib_get_ifaddr(const char *ifname);

int lib_set_ifaddr(char *ifname, in_addr_t addr);  
int lib_set_ipaddr(char *ifname, char *ipaddr); 
int lib_set_dns(char *dns);  
int lib_set_ifnetmask(char *ifname, in_addr_t mask); 
int lib_set_netmask(char *ifname, char *mask);  
int lib_set_macaddr(char *ifname, char *macaddr); 
int lib_set_ifgateway(char *ifname, in_addr_t gate); 
int lib_set_gateway(char *ifname, char *gate); 

char *lib_inet_ntoa(in_addr_t ipaddr);	  // 整型网络地址转成字符串地址 
in_addr_t lib_inet_addr(const char *ipaddr); // 字符串地址转成整型网络地址 

int lib_ppp0_add_route(char *ipaddr, const int mode); 

/*
* 非阻塞connect
*/
int lib_connect_nonb(int sockfd, const struct sockaddr *saptr, socklen_t salen, int nsec);

/*
* 套接口API
*/
int lib_socket(int domain, int type, int protocol);
int lib_connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
int lib_listen(int sockfd, int backlog);
int lib_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int lib_disconnect(int sockfd);

/*
*  SHUT_RD : SHUT_WR : SHUT_RDWR
*/
int lib_shutdown(int sockfd, int howto);

/*
* 套接口设置API
*/
typedef enum _LIB_TYPE_BLOCK
{
	LIB_TYPE_BLOCK = 0,
	LIB_TYPE_NOBLOCK = 1
}LIB_TYPE_BLOCK_EM;

int lib_setsock_blockattr(int sockfd, LIB_TYPE_BLOCK_EM type);
int lib_setsock_block(int sockfd);
int lib_setsock_noblock(int sockfd);
int lib_setsock_rcvbuf(int sockfd, const int size);
int lib_setsock_sndbuf(int sockfd, const int size);
int lib_setsock_rcvtimeo(int sockfd, const int sec);
int lib_setsock_sndtimeo(int sockfd, const int sec);
int lib_setsock_reuseaddr(int sockfd, const int sel);
int lib_setsock_reuseport(int sockfd, const int sel);
int lib_setsock_broadcast(int sockfd, const int sel);
int lib_setsock_linger(int sockfd, const int sec);

int lib_getsock_rcvbuf(int sockfd, int *size);
int lib_getsock_sndbuf(int sockfd, int *size);
int lib_getsock_rcvtimeo(int sockfd, int *sec);
int lib_getsock_sndtimeo(int sockfd, int *sec);
int lib_getsock_reuseaddr(int sockfd, int *sel);
int lib_getsock_reuseport(int sockfd, int *sel);

/*
* TCP套接口API
*/
int lib_tcp_connect(const char *ipaddr, const unsigned short port);
int lib_tcp_connect_nonb(const char *ipaddr, const unsigned short port, const unsigned int nsec);

int lib_tcp_accept(int sockfd, struct sockaddr_in *addr);
int lib_tcp_disconnect(int sockfd);

int lib_tcp_read(int sockfd, void *buff, const unsigned int len);
int lib_tcp_write(int sockfd, const void *buff, const unsigned int len);

int lib_tcp_readn(int sockfd, void *buff, const unsigned int len);
int lib_tcp_writen(int sockfd, const void *buff, const unsigned int len);

int lib_tcp_recv(int sockfd, void *buff, const unsigned int len);
int lib_tcp_send(int sockfd, const void *buff, const unsigned int len);

int lib_tcp_recv_nonb(int sockfd, void *buff, const unsigned int len);
int lib_tcp_send_nonb(int sockfd, const void *buff, const unsigned int len);

int lib_tcp_read_select(int sockfd, void *buff, const unsigned int len, const unsigned int msec);
int lib_tcp_write_select(int sockfd, const void *buff, const unsigned int len, const unsigned int msec);

int lib_tcp_readn_select(int sockfd, void *buff, const unsigned int len, const unsigned int msec);
int lib_tcp_writen_select(int sockfd, const void *buff, const unsigned int len, const unsigned int msec);

int lib_tcp_server_new(const unsigned short port, const unsigned int backlog);
int lib_tcp_client_new(const char *ipaddr, const unsigned short port);

int lib_tcp_server_nonb_new(const unsigned short port, const unsigned int backlog);
int lib_tcp_client_nonb_new(const char *ipaddr, const unsigned short port, const unsigned int nsec);

int lib_tcp_isclient_connect_v1(int sockfd);  //检测客户端是否在线: LIB_GE_TRUE=在线，LIB_GE_FALSE=不在线
/*
 * enable_keepalive: 开启keepalive属性
 * keepidle: 如果在 keepidle 时间内没有任何数据传输，则进行探测
 * keepinterval: 探测发包时间间隔为 keepinterval
 * keepcount: 探测次数
 */
int lib_tcp_keepalive_set(int sockfd, int enable_keepalive, int keepidle, int keepinterval, int keepcount);

/*
 * UNIX net API
 */
int lib_unix_connect(const char *path);
int lib_unix_connect_nonb(const char *path);
int lib_unix_client_new(const char *path);
int lib_unix_server_new(const char *path, mode_t perm, int backlog);
int lib_unix_accept(int sockfd, struct sockaddr_un *addr);
int lib_unix_close(int sockfd);
int lib_unix_unlink(const char *pathname);

/*
 * 原子操作
 */
void lib_gcc_atmoic_set(unsigned int *ptr, const unsigned int val);
unsigned int lib_gcc_atmoic_get(unsigned int *ptr);
void lib_gcc_atmoic_inc(unsigned int *ptr);
void lib_gcc_atmoic_dec(unsigned int *ptr);

/*
 * 字符操作
 */
void lib_str_to_hex(unsigned char *str, const unsigned int str_len, unsigned char *hex);
void lib_hex_to_str(unsigned char *hex, const unsigned int hex_len, unsigned char *str);
void lib_dec_to_str(unsigned char *dec, const unsigned int dec_len, unsigned char *str);
int lib_hex_order_convert(unsigned char *s_hex, const unsigned int s_len, unsigned char *d_hex);

/*
 * 事件循环队列
 */
typedef struct async_mutex_queue lib_async_mutex_queue_t;
lib_async_mutex_queue_t *lib_async_mutex_queue_create(const unsigned int size);
void lib_async_mutex_queue_destroy(lib_async_mutex_queue_t *queue);
int lib_async_mutex_queue_put(lib_async_mutex_queue_t *queue, void *ptr);
void *lib_async_mutex_queue_get(lib_async_mutex_queue_t *queue);
void *lib_async_mutex_queue_get_timedwait(lib_async_mutex_queue_t *queue, const unsigned int sec);
unsigned int lib_async_mutex_queue_size(lib_async_mutex_queue_t *queue);
unsigned int lib_async_mutex_queue_capcity(lib_async_mutex_queue_t *queue);

typedef struct async_event_queue lib_async_event_queue_t;
lib_async_event_queue_t *lib_async_event_queue_create(const unsigned int size);
void lib_async_event_queue_destroy(lib_async_event_queue_t *queue);
int lib_async_event_queue_put(lib_async_event_queue_t *queue, void *ptr);
void *lib_async_event_queue_get(lib_async_event_queue_t *queue);
void *lib_async_event_queue_get_timedwait(lib_async_event_queue_t *queue, const unsigned int msec);
unsigned int lib_async_event_queue_size(lib_async_event_queue_t *queue);
unsigned int lib_event_queue_capcity(lib_async_event_queue_t *queue);

/*
 * Base64 Encode And Decode
 */
int lib_b64_encode(char *s_dst, const char *s_src);
int lib_b64_decode(char *s_dest, const char *s_src);
int lib_b64_encode_hex(char *s_dst, const char *s_src, const unsigned int size);
int lib_b64_decode_hex(char *s_dest, const char *s_src, const unsigned int size);

/*
 * UDP
 */
int lib_udp_client_new(void);
int lib_udp_client_nonb_new(void);
int lib_udp_sendto(const int sockfd, const char *ipaddr, const unsigned short port, const void *ptr, const unsigned int len);
int lib_udp_recvfrom(const int sockfd, const char *ipaddr, const unsigned short port, void *ptr, const unsigned int len);
int lib_udp_sendto_select(const int sockfd, const char *ipaddr, const unsigned short port, const void *ptr, const unsigned int len, const unsigned int msec);
int lib_udp_recvfrom_select(const int sockfd, const char *ipaddr, const unsigned short port, void *ptr, const unsigned int len, const unsigned int msec);
int lib_udp_sendton_select(const int sockfd, const char *ipaddr, const unsigned short port, const void *ptr, const unsigned int len, const unsigned int msec);
int lib_udp_recvfromn_select(const int sockfd, const char *ipaddr, const unsigned short port, void *ptr, const unsigned int len, const unsigned int msec);
int lib_udp_close(const int sockfd);


/*
 * Timer
 */
typedef struct lib_timer
{
	unsigned long value_tv_sec;   		//定时器到期间隔
	unsigned long value_tv_nsec;
	unsigned long interval_tv_sec;  	//定时器的间隔
	unsigned long interval_tv_nsec;
	void *ptr;
}lib_timer_t;

typedef int (* fn_timer)(lib_timer_t *);
int lib_timer_register(fn_timer func, lib_timer_t *timer);







void lib_daemonize(void);





/*@*/
#ifdef __cplusplus
}
#endif
#endif

