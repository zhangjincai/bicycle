#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>

#include "lib_list.h"
#include "lib_general.h"
#include "fn_hash.h"


struct hash_gw
{
	unsigned char key[3];
	int (*fn)(fn_hash_val_t *);
	
	lib_hlist_node_t hlist;	
};

struct fn_hash
{
	lib_hlist_head_t *hlist;
	unsigned int nr_hash;
};


fn_hash_t *fn_hash_create(const unsigned int nr_hash)
{
	int i;
	struct fn_hash *fn = NULL;

	fn = (struct fn_hash *)malloc(sizeof(struct fn_hash));
	if(fn == NULL)
		return FN_HASH_NULL;

	fn->hlist = (lib_hlist_head_t *)malloc(nr_hash * sizeof(lib_hlist_head_t));
	if(fn->hlist == NULL)
		return FN_HASH_NULL;

	fn->nr_hash = nr_hash;
	
	for(i = 0; i < nr_hash; i++)
	{
		LIB_INIT_HLIST_HEAD(&(fn->hlist[i]));
	}
	
	return fn;
}

void fn_hash_destroy(fn_hash_t *fn)
{
	if(fn == NULL)
		return;
	
	int i;
	struct hash_gw *tpos = NULL;
	lib_hlist_node_t *pos = NULL;
	
	for(i = 0; i < fn->nr_hash; i++)
	{
		lib_hlist_for_each_entry(tpos, pos, &(fn->hlist[i]), hlist)
		{
			if(tpos != NULL)
			{
				free(tpos);
				tpos = NULL;
			}
		}
	}

	if(fn != NULL)
	{
		if(fn->hlist != NULL)
		{
			free(fn->hlist);
			fn->hlist = NULL;
		}

		free(fn);
		fn = NULL;
	}
}

int fn_hash_val(const unsigned char key[3])
{
	assert(key);
	
	unsigned short n = 0;
	
	n |= key[1];
	n |= key[0] << 8;

	return lib_hashfn_shift(n, 5, 6);	//6: 2^6 = 64 
}

inline int fn_key_val(const unsigned char key[3])
{
	assert(key);
	
	unsigned short n = 0;
	
	n |= key[1];
	n |= key[0] << 8;

	return n;
}

int fn_hash_register(fn_hash_t *fn, const char key[3], fn_register func)
{
	assert(fn);
	assert(func);
	assert(key);
	
	int h_val = 0;
	struct hash_gw *hash = NULL;

	h_val = fn_hash_val(key); //?

	fprintf(stderr, "register hash val = %d\n", h_val);
		
	hash = (struct hash_gw *)malloc(sizeof(struct hash_gw));
	if(hash == NULL)
		return FN_HASH_ERROR;

	strcpy(hash->key, key);
	hash->fn = func;
	lib_hlist_add_head(&hash->hlist, &(fn->hlist[h_val]));	

	return FN_HASH_EOK;
}

int fn_hash_run(fn_hash_t *fn, fn_hash_val_t *val)
{
	assert(fn);
	assert(val);

	struct hash_gw *tpos = NULL;
	lib_hlist_node_t *pos = NULL;
	int h_val = fn_hash_val(val->key); //?

	lib_hlist_for_each_entry(tpos, pos, &(fn->hlist[h_val]), hlist)
	{
		if(strncmp((char *)tpos->key, (char *)val->key, 2) == 0)
		{
			tpos->fn(val);
		}
	}

	return FN_HASH_EOK;
}










