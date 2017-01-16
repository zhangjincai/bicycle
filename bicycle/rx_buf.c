#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>
#include <arpa/inet.h>

#include "defines.h"
#include "lib_general.h"
#include "rx_buf.h"







unsigned short n_rx_buf_len(ndev_rx_buf_t *buf)
{
	return ntohs(buf->s_un.st.len);
}

unsigned short n_rx_buf_crc(ndev_rx_buf_t *buf)
{
	unsigned short crc_idx = ntohs(buf->s_un.st.len) + 7;
	unsigned short crc16 = 0;
	
	memcpy(&crc16, &(buf->s_un.data[crc_idx]), 2);
	return crc16;
}

int n_rx_buf_crc_check(ndev_rx_buf_t *buf)
{
	unsigned short total_len = ntohs(buf->s_un.st.len) + 12; //除去数据体部分的长度为12
	unsigned short crc_idx = ntohs(buf->s_un.st.len) + 9; //数据体之前的长度为9
	unsigned short calc_crc16, crc16;
	
	memcpy(&crc16, &(buf->s_un.data[crc_idx]), 2);
	calc_crc16 = ntohs(lib_crc16_with_table((char *)&(buf->s_un.data[1]), total_len - 4));//4         //4:帧头、帧尾、校验
	if(crc16 == calc_crc16)
		return B_DEV_TRUE;
	
	return B_DEV_FALSE;
}

int s_rx_buf_crc_check(sae_rx_buf_t *buf)
{
	unsigned short crc_len = ntohs(S_RX_LEN_PTR(buf)) + 8;
	unsigned short crc_idx = ntohs(S_RX_LEN_PTR(buf)) + 8;
	unsigned short calc_crc16, crc16;

	memcpy(&crc16, &(buf->s_un.data[crc_idx]), 2);
	calc_crc16 = lib_crc16_with_table((char *)&(S_RX_DA_PTR(buf)), crc_len);

	if(ntohs(crc16) == calc_crc16)
		return B_DEV_TRUE;

	return B_DEV_FALSE;
}

