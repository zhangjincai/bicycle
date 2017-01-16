#ifndef __LIB_CRYPTO_H__
#define __LIB_CRYPTO_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

/*
 * AES
 */
void lib_aes_encrypt(unsigned char *out, unsigned char *in, const int inlen, unsigned char *key, const unsigned int keysize);
void lib_aes_decrypt(unsigned char *out, unsigned char *in, const int inlen, unsigned char *key, const unsigned int keysize);

/*
 * DES3
 */ 
void lib_des3_encrypt(unsigned char *out, unsigned char *in, const int inlen, unsigned char *key);
void lib_des3_decrypt(unsigned char *out, unsigned char *in, const int inlen, unsigned char *key);
void lib_des3(unsigned char *out, unsigned char *in, unsigned char *key, unsigned int direct);

/*
* DES
*/
void lib_des_encrypt(unsigned char *out, unsigned char *in, const int inlen, unsigned char *key);
void lib_des_decrypt(unsigned char *out, unsigned char *in, const int inlen, unsigned char *key);
void lib_des(unsigned char *out, unsigned char *in, unsigned char *key, unsigned int direct);

/*
 * MD5
 */
void lib_md5_checksum(unsigned char *in, const int inlen, unsigned char *out);




/*@*/
#ifdef __cplusplus
}
#endif
/*@*/

#endif


