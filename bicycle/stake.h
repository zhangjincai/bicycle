#ifndef __STAKE_H__
#define __STAKE_H__


#include "lib_logdb.h"
#include "lib_wireless.h"

int stake_init();
int stake_destroy(void);
int stake_ctos_put(const unsigned char dest_addr, unsigned char *data, const unsigned int len);
int stake_ctos_put_priority(const unsigned char dest_addr, unsigned char *data, const unsigned int len, const unsigned char priority);




#endif

