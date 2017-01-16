#ifndef __FIRMWARE_CHECK_H__
#define __FIRMWARE_CHECK_H__


enum FC_RET
{
	FC_RET_ERR = 1,
	FC_RET_SUCCESS = 2,
	FC_RET_FAIL = 3
};


struct FC_parameter  //256Bytes
{
    	unsigned int magic;
    	char name[32];
    	unsigned int length;
    	unsigned char type;
    	char datetime[7];
    	unsigned char check_mode;
    	unsigned char check_data[16];
    	unsigned char rfu[63];
    	unsigned char log[128];
}__attribute__((packed));
typedef struct FC_parameter FC_parameter_t;


enum FC_RET FC_check(const char *pathname, struct FC_parameter *fc_para);


#endif

