#ifndef __SAE_PROTOCOL_H__
#define __SAE_PROTOCOL_H__




int sae_protocol_req_req(void *ptr, const unsigned char d_addr);
int sae_protocol_bheart_req(void *ptr, const unsigned char d_addr);
int sae_protocol_ndev_stat(void *ptr, const unsigned char d_addr);
int sae_protocol_blacklist_version_broadcast(void *ptr);
int sae_protocol_firmware_version_broadcast(void *ptr);
int sae_protocol_blacklist_broadcast(void *d_ptr, void *s_ptr, const unsigned int s_len);
int sae_protocol_blacklist_unicast(const unsigned char d_addr, const unsigned char sn, void *d_ptr, void *s_ptr, const unsigned int s_len);
int sae_protocol_firmware_broadcast(void *d_ptr, void *s_ptr, const unsigned int s_len);
int sae_protocol_firmware_unicast(const unsigned char d_addr, const unsigned char sn, void *d_ptr, void *s_ptr, const unsigned int s_len);
int sae_protocol_trading_records_pass(const unsigned char d_addr, void *d_ptr, void *s_ptr, const unsigned int s_len);
int sae_protocol_whole_pass(const unsigned char op, const unsigned char d_addr, void *d_ptr, void *s_ptr, const unsigned int s_len);
int sae_protocol_led_refresh(void *ptr, const unsigned char d_addr);
int sae_protocol_control_for_lock(void *ptr, const unsigned char d_addr,  sae_control_t *ctrl);



#endif


