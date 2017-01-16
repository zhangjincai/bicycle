#ifndef __RX_BUF_H__
#define __RX_BUF_H__




unsigned short n_rx_buf_len(ndev_rx_buf_t *buf);
unsigned short n_rx_buf_crc(ndev_rx_buf_t *buf);
int n_rx_buf_crc_check(ndev_rx_buf_t *buf);


int s_rx_buf_crc_check(sae_rx_buf_t *buf);



#endif



