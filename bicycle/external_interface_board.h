#ifndef __EXTERNAL_INTERFACE_BOARD_H__
#define __EXTERNAL_INTERFACE_BOARD_H__


#define EXT_IB_NULL					(NULL) 
#define EXT_IB_EOK					(0)  //����
#define EXT_IB_ERROR					(-1) //����
#define EXT_IB_ETIMEOUT				(-2) //��ʱ
#define EXT_IB_EFULL					(-3) //��
#define EXT_IB_EEMPTY				(-4) //��
#define EXT_IB_ENOMEM 				(-5) //�ڴ治��
#define EXT_IB_EXCMEM				(-6) //�ڴ�Խ��
#define EXT_IB_EBUSY					(-7) //æ
#define EXT_IB_NOT_SUPPORT			(-8) //��֧�ָ�ָ��

#define EXT_IB_TRUE					(1)
#define EXT_IB_FALSE					(0)


enum IBOARD_CTRL
{
	IBOARD_CTRL_ON = 1,
	IBOARD_CTRL_OFF = 2,
};


struct iboard_device_version
{
	unsigned short vendor_encoding; //��Ӧ�̱���
	unsigned char device_type;  //�豸����
	unsigned char rfu[5];   //����
	unsigned char device_sn[16];  //�豸���к�
	unsigned short product_batch; //��Ʒ����
	unsigned short hardware_version;  //Ӳ���汾��
	unsigned char software_version[8]; //����汾��
}__attribute__((packed));


int ext_iboard_LED4(enum IBOARD_CTRL ctrl);  //4��LED��
int ext_iboard_light_box(enum IBOARD_CTRL ctrl); //����
int ext_iboard_48V(enum IBOARD_CTRL ctrl);  //��׮48V
int ext_iboard_USB_charge(const int channel, enum IBOARD_CTRL ctrl);  //USB���
int ext_iboard_device_version(struct iboard_device_version *version, const unsigned int msec);  //�豸�汾




#endif

