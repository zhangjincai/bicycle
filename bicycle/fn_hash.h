#ifndef __FN_HASH_H__
#define __FN_HASH_H__

#define FN_HASH_NULL		NULL
#define FN_HASH_EOK		0
#define FN_HASH_ERROR		-1


struct fn_hash_val
{
	unsigned char key[3];
	unsigned char data[64];
	unsigned int d_size;
	unsigned int sel;
	unsigned char s_addr;    //Ëø×®Ô­µØÖ·
	unsigned char ack;
	void *var_ptr;
};
typedef struct fn_hash_val fn_hash_val_t;

typedef struct fn_hash fn_hash_t;
typedef int (* fn_register)(fn_hash_val_t *);


fn_hash_t *fn_hash_create(const unsigned int nr_hash);
void fn_hash_destroy(fn_hash_t *fn);
int fn_hash_val(const unsigned char key[3]);
int fn_key_val(const unsigned char key[3]);
int fn_hash_register(fn_hash_t *fn, const char key[3], fn_register func);
int fn_hash_run(fn_hash_t *fn, fn_hash_val_t *val);





#endif

