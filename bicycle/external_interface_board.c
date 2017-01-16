#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h> 
#include <assert.h>

#include "lib_general.h"
#include "external_interface_board.h"

#define	DEV_NAME	"/dev/ttyO3"


int ext_iboard_LED4(enum IBOARD_CTRL ctrl)
{
	int ret = -1;
	lib_serial_t serial;
	unsigned short crc16 = 0;
	unsigned char txbuf[128] = {0};
	unsigned char rxbuf[128] = {0};
	
	memset(&serial, 0, sizeof(lib_serial_t));

	strcpy(serial.pathname, DEV_NAME);
  	serial.flags = O_RDWR;
  	serial.speed = 9600;
  	serial.databits = 8;
  	serial.stopbits = 1;

	ret = lib_serial_init(&serial);
	if(ret != LIB_GE_EOK)
		return EXT_IB_ERROR;


	txbuf[0] = 0x55;
	txbuf[1] = 0xaa;
	txbuf[2] = 0x82; //√¸¡Ó
	txbuf[3] = 1;

	if(ctrl == IBOARD_CTRL_ON)
		txbuf[4] = 0x01;
	else if(ctrl == IBOARD_CTRL_OFF)
		txbuf[4] = 0x00;
	
	crc16 = htons((unsigned short)lib_crc16_with_table((char *)&txbuf[3], 2));
	memcpy(&txbuf[5], &crc16, 2);

	lib_serial_send(serial.sfd, txbuf, 7);
	lib_serial_recv_select(&serial, rxbuf, 32, 1000);
	
	lib_serial_close(&serial);
	
	return EXT_IB_EOK;
}

int ext_iboard_light_box(enum IBOARD_CTRL ctrl)
{
	int ret = -1;
	lib_serial_t serial;
	unsigned short crc16 = 0;
	unsigned char txbuf[128] = {0};
	unsigned char rxbuf[128] = {0};
	
	memset(&serial, 0, sizeof(lib_serial_t));

	strcpy(serial.pathname, DEV_NAME);
  	serial.flags = O_RDWR;
  	serial.speed = 9600;
  	serial.databits = 8;
  	serial.stopbits = 1;

	ret = lib_serial_init(&serial);
	if(ret != LIB_GE_EOK)
		return EXT_IB_ERROR;


	txbuf[0] = 0x55;
	txbuf[1] = 0xaa;
	txbuf[2] = 0x87;  //√¸¡Ó
	txbuf[3] = 1;

	if(ctrl == IBOARD_CTRL_ON)
		txbuf[4] = 0x01;
	else if(ctrl == IBOARD_CTRL_OFF)
		txbuf[4] = 0x00;
	
	crc16 = htons((unsigned short)lib_crc16_with_table((char *)&txbuf[3], 2));
	memcpy(&txbuf[5], &crc16, 2);

	lib_serial_send(serial.sfd, txbuf, 7);
	lib_serial_recv_select(&serial, rxbuf, 32, 1000);
	
	lib_serial_close(&serial);
	
	return EXT_IB_EOK;	
}

int ext_iboard_48V(enum IBOARD_CTRL ctrl)
{
	int ret = -1;
	lib_serial_t serial;
	unsigned short crc16 = 0;
	unsigned char txbuf[128] = {0};
	unsigned char rxbuf[128] = {0};
	
	memset(&serial, 0, sizeof(lib_serial_t));

	strcpy(serial.pathname, DEV_NAME);
  	serial.flags = O_RDWR;
  	serial.speed = 9600;
  	serial.databits = 8;
  	serial.stopbits = 1;

	ret = lib_serial_init(&serial);
	if(ret != LIB_GE_EOK)
		return EXT_IB_ERROR;


	txbuf[0] = 0x55;
	txbuf[1] = 0xaa;
	txbuf[2] = 0x86;  //√¸¡Ó
	txbuf[3] = 1;

	if(ctrl == IBOARD_CTRL_ON)
		txbuf[4] = 0x01;
	else if(ctrl == IBOARD_CTRL_OFF)
		txbuf[4] = 0x00;
	
	crc16 = htons((unsigned short)lib_crc16_with_table((char *)&txbuf[3], 2));
	memcpy(&txbuf[5], &crc16, 2);

	lib_serial_send(serial.sfd, txbuf, 7);
	lib_serial_recv_select(&serial, rxbuf, 32, 1000);
	
	lib_serial_close(&serial);
	
	return EXT_IB_EOK;	
}

int ext_iboard_USB_charge(const int channel, enum IBOARD_CTRL ctrl)
{
	int ret = -1;
	lib_serial_t serial;
	unsigned short crc16 = 0;
	unsigned char txbuf[128] = {0};
	unsigned char rxbuf[128] = {0};
	
	memset(&serial, 0, sizeof(lib_serial_t));

	strcpy(serial.pathname, DEV_NAME);
  	serial.flags = O_RDWR;
  	serial.speed = 9600;
  	serial.databits = 8;
  	serial.stopbits = 1;

	ret = lib_serial_init(&serial);
	if(ret != LIB_GE_EOK)
		return EXT_IB_ERROR;


	txbuf[0] = 0x55;
	txbuf[1] = 0xaa;
	txbuf[2] = 0x80;  //√¸¡Ó
	txbuf[3] = 1;

	switch(channel)
	{
		case 1:
		{
			if(ctrl == IBOARD_CTRL_ON)
				txbuf[4] = 0x11;
			else if(ctrl == IBOARD_CTRL_OFF)
				txbuf[4] = 0x10;
		}
		break;

		case 2:
		{
			if(ctrl == IBOARD_CTRL_ON)
				txbuf[4] = 0x22;
			else if(ctrl == IBOARD_CTRL_OFF)
				txbuf[4] = 0x20;
		}
		break;

		default:
			return EXT_IB_ERROR;
	}

	crc16 = htons((unsigned short)lib_crc16_with_table((char *)&txbuf[3], 2));
	memcpy(&txbuf[5], &crc16, 2);

	lib_serial_send(serial.sfd, txbuf, 7);
	lib_serial_recv_select(&serial, rxbuf, 32, 1000);
	
	lib_serial_close(&serial);
	
	return EXT_IB_EOK;		
}

int ext_iboard_device_version(struct iboard_device_version *version, const unsigned int msec)
{
	if(version == NULL)
		return EXT_IB_ERROR;

	int ret = -1;
	lib_serial_t serial;
	unsigned short crc16 = 0;
	unsigned char txbuf[128] = {0};
	unsigned char rxbuf[128] = {0};
	
	memset(&serial, 0, sizeof(lib_serial_t));

	strcpy(serial.pathname, DEV_NAME);
  	serial.flags = O_RDWR;
  	serial.speed = 9600;
  	serial.databits = 8;
  	serial.stopbits = 1;

	ret = lib_serial_init(&serial);
	if(ret != LIB_GE_EOK)
		return EXT_IB_ERROR;

	lib_setfd_noblock(serial.sfd);

	txbuf[0] = 0x55;
	txbuf[1] = 0xaa;
	txbuf[2] = 0x8b;  //√¸¡Ó
	txbuf[3] = 0;

	crc16 = htons((unsigned short)lib_crc16_with_table((char *)&txbuf[3], 1));
	memcpy(&txbuf[4], &crc16, 2);
	
	ret = lib_serial_send(serial.sfd, txbuf, 6);
	if(ret <= 0) //∑¢ÀÕ ß∞‹‘Úπÿ±’¥Æø⁄ add by zjc at 2016-09-26
	{
		lib_serial_close(&serial);
		return EXT_IB_ERROR;
	}

	ret = lib_serial_readn_select(serial.sfd, rxbuf, 42, msec);
	if(ret <= 0)
	{
		lib_serial_close(&serial);
		return EXT_IB_ERROR;
	}

	memcpy(version, &rxbuf[4], sizeof(struct iboard_device_version));
	
	lib_serial_close(&serial);
	
	return EXT_IB_EOK;
}

