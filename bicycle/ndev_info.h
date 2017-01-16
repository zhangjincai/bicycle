#ifndef __NDEV_INFO_H__
#define __NDEV_INFO_H__

#include "defines.h"

void ndev_info_init(void);
void ndev_info_get(ndev_info_t *info);
void ndev_info_put(ndev_info_t *info);
void ndev_info_stat(ndev_status_t *stat);
void ndev_info_stat_get(ndev_status_t *stat);
void ndev_info_stat_NOR_get(unsigned short *stat);
void ndev_info_stat_put(ndev_status_t *stat);
void ndev_info_set_time(char s_time[13]);
void ndev_info_get_time(char s_time[13]);



#endif


