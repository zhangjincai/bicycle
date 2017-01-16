#ifndef __SX1278__H_
#define __SX1278__H_


#define HEAD_H			0x55
#define HEAD_L			0xAA

#define CMD_IDX			(2)
#define DATA_LEN_IDX	(3)
#define DATA_IDX		(4)
#define PACKET_NOT_DATA_LEN		(5)


#define PACKET_MAX_LEN  200

enum packet_check {
	PACKET_OK = 0, //数据包正常
 	PACKET_HEADER_ERR,
 	PACKET_DATA_LEN_ERR,		
 	PACKET_XOR_CHECK_ERR,	
 	PACKET_NULL_ERR,
 	PACKET_NOTKNOW_ERR,
};

enum sx1278_mode {
	NORMAL_MODE,
	WAKEUP_MODE,
	POWER_SAVE_MODE,
	SLEEP_MODE,
}; 

/* 节点机和桩机通信命令字 S2N_XX:桩机到节点机，N2S_XX:节点机到桩机 */
#define S2N_BLACKLIST_CHECK	0xA1
#define N2S_BLACKLIST_CHECK_ACK	0xB1



/* -------------------------------sx1278工作参数配置 begin-------------------------------- */
//sx1278_configs_t中baud_rate:
/* 串口数据位、校验位、停止位 sx1278_configs_t中baud_rate的bit6、bit7 */
#define BITS_8N1_DEFAULT (0x00) //（默认）
#define BITS_8O1 (0x01)
#define BITS_8E1 (0x02) 
#define BITS_8N1 (0x03) //（等同 00）


/* 串口波特率 sx1278_configs_t中baud_rate的bit3--bit5 */
#define BAUD_RATE_1200 (0x00)
#define BAUD_RATE_2400 (0x01)
#define BAUD_RATE_4800 (0x02)
#define BAUD_RATE_9600 (0x03) //（默认） 
#define BAUD_RATE_19200 (0x04)
#define BAUD_RATE_38400 (0x05)
#define BAUD_RATE_57600 (0x06)
#define BAUD_RATE_115200 (0x07)

/* 无线空中速率（bps） sx1278_configs_t中baud_rate的bit0--bit2 */
#define WIRELESS_RATE_0_3K (0x00) //0.3K
#define WIRELESS_RATE_1_2K (0x01)
#define WIRELESS_RATE_2_4K (0x02) //（默认）
#define WIRELESS_RATE_4_8K (0x03)
#define WIRELESS_RATE_9_6K (0x04)
#define WIRELESS_RATE_19_2K_0 (0x05) //以下都为19.2K
#define WIRELESS_RATE_19_2K_1 (0x06)
#define WIRELESS_RATE_19_2K_2 (0x07)


/* 通信信道 sx1278_configs_t中channel的bit0--bit4, bit5--bit7未用*/
#define CHANNEL_433M (0x17) //433M（默认）


//sx1278_configs_t中option:
/* 定点发送使能位 sx1278_configs_t中option的bit7 */
#define WHOLE_PASS_MODE (0x00) //透明传输
#define POINT_PASS_MODE (0x01) //定点传输

/* IO 驱动方式（默认 1） sx1278_configs_t中option的bit6 */
#define IO_MODE_1 (0x01) //TXD、 AUX 推挽输出，RXD 上拉输入 (默认)
#define IO_MODE_2 (0x00) //TXD、 AUX 开路输出，RXD 开路输入

/* 无线唤醒时间 sx1278_configs_t中option的bit3--bit5 */
#define WAKEUP_250MS (0x00) //（默认）
#define WAKEUP_500MS (0x01) 
#define WAKEUP_750MS (0x02) 
#define WAKEUP_1000MS (0x03) 
#define WAKEUP_1250MS (0x04) 
#define WAKEUP_1500MS (0x05) 
#define WAKEUP_1750MS (0x06) 
#define WAKEUP_2000MS (0x07) 

/* FEC 开关 sx1278_configs_t中option的bit2 */
#define FEC_OFF (0x00) 
#define FEC_ON (0x01) //打开 FEC 纠错（默认）

/* 发射功率(大约值) sx1278_configs_t中option的bit0--bit1 */
#define POWER_20dBm (0x00) //20dBm（默认）
#define POWER_17dBm (0x01) 
#define POWER_14dBm (0x02) 
#define POWER_10dBm (0x03) 


#define CONFIGS_SAVE	1
#define CONFIGS_NOT_SAVE	0


struct speed_param {
	unsigned char wireless_rate:3; //无线空中速率（bps）
	unsigned char baud_rate:3; //串口速率（bps）
	unsigned char bits_8N1:2; //串口校验位
}__attribute__((packed));

struct options_param {
	unsigned char trans_power:2; //发射功率(大约值)
	unsigned char FEC_enalble:1; //FEC 开关
	unsigned char wakeup_timeout:3; //无线唤醒时间
	unsigned char io_driver_mode:1; //IO 驱动方式（默认 1）
	unsigned char point_enable:1; //定点发送使能位	
}__attribute__((packed));


struct sx1278_configs {
	unsigned char head; //控制命令头 0xC0:掉电保存，0xC2:掉电不保存
	unsigned char addr_h; //模块地址高字节（默认 00H）
	unsigned char addr_l; //模块地址低字节（默认 00H）

	struct speed_param speed; //速率参数，包括串口速率和空中速率

	unsigned char channel; //通信信道，默认 17H（433MHz）
	
	struct options_param options;
}__attribute__((packed));
typedef struct sx1278_configs sx1278_configs_t;

/* -------------------------------sx1278工作参数配置 end-------------------------------- */
static unsigned char __xor_check(const unsigned char *data, int len);
static enum packet_check sx1278_packet_check(const unsigned char *data, int len);
static int sx1278_packet_explain(const unsigned char *data, int len);
static int sx1278_send_data(unsigned short dest_addr, unsigned char cmd, unsigned char *data, int len);
static int sx1278_configs_set(int fd, const sx1278_configs_t *configs);
static int sx1278_configs_get(int fd, sx1278_configs_t *configs);
static int sx1278_reset(int fd);
#endif /* __SX1278__H_ */
