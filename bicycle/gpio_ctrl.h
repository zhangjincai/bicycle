#ifndef __GPIO_CTRL_H__
#define __GPIO_CTRL_H__


enum GPIO_CTRL_SW
{
	GPIO_CTRL_SW_OFF = 0,
	GPIO_CTRL_SW_ON = 1
};

/* ioctl MAX number */
#define GPIO_CTRL_MAXNR							11


/* use 'I' as magic number */
#define GPIO_CTRL_MAGIC							'I'
#define GPIO_CTRL_SET_PWR_YCT					_IOW(GPIO_CTRL_MAGIC, 0, int)
#define GPIO_CTRL_SET_PWR_2232_1				_IOW(GPIO_CTRL_MAGIC, 1, int)
#define GPIO_CTRL_SET_PWR_2232_2				_IOW(GPIO_CTRL_MAGIC, 2, int)
#define GPIO_CTRL_SET_BACKLIGHTEN				_IOW(GPIO_CTRL_MAGIC, 3, int)
#define GPIO_CTRL_SET_PWR_3G					_IOW(GPIO_CTRL_MAGIC, 4, int)
#define GPIO_CTRL_SET_PWR_LED0					_IOW(GPIO_CTRL_MAGIC, 5, int)
#define GPIO_CTRL_SET_PWR_LED1					_IOW(GPIO_CTRL_MAGIC, 6, int)
#define GPIO_CTRL_SET_PWR_LED2					_IOW(GPIO_CTRL_MAGIC, 7, int)
#define GPIO_CTRL_SET_PWR_485CTL				_IOW(GPIO_CTRL_MAGIC, 8, int)
#define GPIO_CTRL_SET_PWR_4G					_IOW(GPIO_CTRL_MAGIC, 9, int)

#define GPIO_CTRL_SET_PWR_USB_HUB				_IOW(GPIO_CTRL_MAGIC, 10, int)

#endif

