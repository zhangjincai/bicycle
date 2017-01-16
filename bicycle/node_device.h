#ifndef __NODE_DEVICE_H__
#define __NODE_DEVICE_H__

#include "lib_wireless.h"

int ndev_init();
int ndev_destroy(void);

int ndev_register_notify_put(const unsigned int notify);
int ndev_btheart_notify_put(const unsigned int notify);
int ndev_tcp_send_to_server(void *ptr, const unsigned int len, const unsigned int msec);
int ndev_tcp_send_to_server_with_escape(void *ptr, const unsigned int len, const unsigned int msec);
int downlink_escape(unsigned char *dest, unsigned char *src, const int slen);
int uplink_escape(unsigned char *dest, unsigned char *src, const int slen);
void ndev_set_sockfd(const unsigned int sockfd);
unsigned int ndev_get_sockfd(void);
unsigned int ndev_get_conn_stat(void);
unsigned int ndev_get_balance_stat(void);
void ndev_set_conn_stat(const unsigned int stat);
void *ndev_get_univ_async_mutex_queue(void);
int ndev_trading_records_upload(const unsigned char dev_addr, void *ptr, const unsigned int len);  //锁桩交易记录上传
int ndev_whole_pass_upload(const unsigned char op, const unsigned char dev_addr, void *ptr, const unsigned int len);





#endif


