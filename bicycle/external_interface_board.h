#ifndef __EXTERNAL_INTERFACE_BOARD_H__
#define __EXTERNAL_INTERFACE_BOARD_H__


#define EXT_IB_NULL					(NULL) 
#define EXT_IB_EOK					(0)  //正常
#define EXT_IB_ERROR					(-1) //错误
#define EXT_IB_ETIMEOUT				(-2) //超时
#define EXT_IB_EFULL					(-3) //满
#define EXT_IB_EEMPTY				(-4) //空
#define EXT_IB_ENOMEM 				(-5) //内存不够
#define EXT_IB_EXCMEM				(-6) //内存越界
#define EXT_IB_EBUSY					(-7) //忙
#define EXT_IB_NOT_SUPPORT			(-8) //不支持该指令

#define EXT_IB_TRUE					(1)
#define EXT_IB_FALSE					(0)


enum IBOARD_CTRL
{
	IBOARD_CTRL_ON = 1,
	IBOARD_CTRL_OFF = 2,
};


struct iboard_device_version
{
	unsigned short vendor_encoding; //供应商编码
	unsigned char device_type;  //设备类型
	unsigned char rfu[5];   //保留
	unsigned char device_sn[16];  //设备序列号
	unsigned short product_batch; //产品批次
	unsigned short hardware_version;  //硬件版本号
	unsigned char software_version[8]; //软件版本号
}__attribute__((packed));


int ext_iboard_LED4(enum IBOARD_CTRL ctrl);  //4个LED灯
int ext_iboard_light_box(enum IBOARD_CTRL ctrl); //灯箱
int ext_iboard_48V(enum IBOARD_CTRL ctrl);  //锁桩48V
int ext_iboard_USB_charge(const int channel, enum IBOARD_CTRL ctrl);  //USB充电
int ext_iboard_device_version(struct iboard_device_version *version, const unsigned int msec);  //设备版本




#endif

