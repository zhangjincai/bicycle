#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#define IPV4(a, b, c, d) 		((d << 24) | (c << 16) |( b << 8) | (a))


/*
 * 配置文件路径
 */
//#define NDEV_CONFIG_FILE			"/mnt/config/ndev_config.cfg"
//#define UNIV_CONF_FILE				"/mnt/config/univ_config.cfg"

#define NDEV_CONFIG_FILE			"/opt/config/ndev_config.cfg"
#define UNIV_CONF_FILE				"/opt/config/univ_config.cfg"




/*
 * 节点机默认配置
 */
#define MAGIC_DEFAULT_HEAD					0x20101207				//魔数头部
#define MAGIC_DEFAULT_TAIL					0x20101207				//魔数尾部
#define CRC16_DEFAULT						0xFFFF					//默认CRC16

#define BOOT_VERSION_DEFAULT				"15010101"				//uboot版本
#define KERNEL_VERSION_DEFAULT				"15010101"				//kernel版本
#define ROOTFS_VERSION_DEFAULT				"15010101"				//rootfs版本
#define FW_VERSION_DEFAULT					"15010101"				//firmware版本
#define APPL_VERSION_DEFAULT				"15010101"				//appl版本

#define LOCAL_IP_DEFAULT					IPV4(192,168,1,169)			//本地IP地址
#define LOCAL_SUBMASK_DEFAULT				IPV4(255,255,255,0)			//本地子网掩码
#define LOCAL_GATEWAY_DEFAULT				IPV4(192,168,1,1)       		//本地网关
#define LOCAL_DNS_DEFAULT					IPV4(202,96,128,143)		//域名地址

#define WIFI_IP_DEFAULT						IPV4(192,168,100,1)			//WIFI IP地址
#define WIFI_SUBMASK_DEFAULT				IPV4(255,255,255,0)			//WIFI 子网掩码
#define WIFI_GATEWAY_DEFAULT				IPV4(192,168,100,1)       		//WIFI 网关
#define WIFI_DNS_DEFAULT					IPV4(202,96,128,143)		//WIFI 地址

#define USING_WIRELESS_DEFAULT				1						//使用有线网络 1:无线, 2:有线

#define LOAD_SERVER1_IP_DEFAULT				IPV4(120,24,97,95)			//负载均衡服务器1 IP 地址
#define LOAD_SERVER2_IP_DEFAULT				IPV4(180,76,140,185)			//负载均衡服务器2 IP 地址
#define LOAD_SERVER1_PORT_DEFAULT			20002 //20000					//负载均衡服务器1端口 zjc at 2016-09-29
#define LOAD_SERVER2_PORT_DEFAULT			20001					//负载均衡服务器2端口
#define LOAD_TIMEOUT_DEFAULT				10						//负载均衡服务器等待时间(秒)

#define FTP_IP_DEFAULT						IPV4(121,8,152,2)			//FTP IP地址
#define FTP_PORT_DEFAULT					10040					//FTP端口
#define FTP_USERNAME_DEFAULT				"endpointuser"			//FTP用户名
#define FTP_PASSWORD_DEFAULT				"11142010"				//FTP密码

#define NDEV_HEART_TIME_DEFAULT				180      					//节点机心跳时间 单位:秒
#define NDEV_HEART_EMERG_TIME_DEFAULT		90      					//节点机紧急心跳时间 单位:秒
#define NDEV_TIMER_GATE_VALUE_DEFAULT		30						//时钟修正闸值
#define NDEV_TERM_ID_DEFAULT				0xFFFF 					//终端编号
#define NDEV_NAME_DEFAULT					""						//网点名称
#define NDEV_AREA_INFO_DEFAULT				0xFFFF					//分区信息
#define NDEV_FLOW_3G_DEFAULT					0					// 3G流量  单位:KB
#define NDEV_FIRST_LEVEL_3G_DEFAULT	      		200						// 3G第一闸值 单位:MB
#define NDEV_SECOND_LEVEL_3G_DEFAULT	      	280                     			// 3G第二闸值 单位:MB

/*
 * 正式代理服务器IP:183.62.39.30
 * 正式代理服务器端口:10022
 */
#if 0 //add by zjc at 2016-09-29(修改默认代理服务器)
#define NDEV_LNT_IPADDR_DEFAULT			IPV4(121,8,152,2)			//岭南通中心IP 测试中心: 改成中运代理服务的IP
#define NDEV_LNT_PORT_DEFAULT				10030					//岭南通中心端口 : 改成中运代理服务的端口
#else
#define NDEV_LNT_IPADDR_DEFAULT			IPV4(183,62,39,30)			//岭南通中心IP 测试中心: 改成中运代理服务的IP
#define NDEV_LNT_PORT_DEFAULT				10022	
#endif
#define NDEV_LNT_USERID_DEFAULT			""						 //用户ID
#define NDEV_LNT_SPNO_DEFAULT				"0x0101"					 //服务商代码
#define NDEV_LNT_CONPA_DEFAULT				0x80						//消费初始化参数  0x80:正常消费
#define NDEV_LNT_CVAD_DEFAULT				0x00						 //卡离线有效期启动


#define NDEV_LIGHT_ENABLE_HH_DEFAULT		18			//灯箱开启时间  6:30  晚上
#define NDEV_LIGHT_ENABLE_MM_DEFAULT		30	

#define NDEV_LIGHT_DISABLE_HH_DEFAULT		6			//灯箱开启时间  6:00 早上
#define NDEV_LIGHT_DISABLE_MM_DEFAULT		00


/*
 * 桩机默认配置
 */
#define SAE_HEART_TIME_DEFAULT					90						//桩机心跳时间 单位:秒
#define SAE_FIRMWARE_UPDATE_TIME_DEFAULT    	""						//固件更新时间 
#define SAE_CAN_BAUD_RATE_DEFAULT				2						//CAN 波特率	2:BAUD_RATE_125kbps
#define SAE_QUANTITY_DEFAULT					64						//锁桩数量



/*
 *  泛文件默认配置
 */
#define UNIV_BLK_TOTAL_DFAULT					"00000000"				//总量黑名单
#define UNIV_BLK_INC_DFAULT						"00000000"				//增量黑名单
#define UNIV_BLK_DEC_DFAULT						"00000000"				//减量黑名单
#define UNIV_BLK_TEMP_DFAULT					"00000000"				//临时黑名单
#define UNIV_SAE_FW_DEFAULT					"00000000"				//桩机固件
#define UNIV_SAE_PARA_DEFAULT					"00000000"				//桩机参数
#define UNIV_UNIONPAY_KEY_DEFAULT				"00000000"				//银联密钥

#define UNIV_USE_TOTAL_BLK_DB_DEFAULT			0						//总量黑名单使用数据库
#define UNIV_USE_INC_BLK_DB_DEFAULT			0						//增量黑名单使用数据库
#define UNIV_USE_DEC_BLK_DB_DEFAULT			0						//减量黑名单使用数据库
#define UNIV_USE_TEMP_BLK_DB_DEFAULT			0						//临时量黑名单使用数据库
#define UNIV_USE_SAE_FW_DB_DEFAULT			0						//桩机固件使用数据库

#define UNIV_BLK_TOTAL_SEQ_DEFAULT				0
#define UNIV_BLK_INC_SEQ_DEFAULT				0
#define UNIV_BLK_DEC_SEQ_DEFAULT				0
#define UNIV_BLK_TEMP_SEQ_DEFAULT				0
#define UNIV_FW_SEQ_DEFAULT					0


#define UNIV_BLK_RFU_DEFAULT					""						//黑名单保留
#define UNIV_SAE_FW_RFU_DEFAULT				""						//桩机保留
#define UNIV_RFU_DEFAULT						""						//泛文件保留 




#define NDEV_CONFIG_DEFAULT		{BOOT_VERSION_DEFAULT, KERNEL_VERSION_DEFAULT, ROOTFS_VERSION_DEFAULT, FW_VERSION_DEFAULT, APPL_VERSION_DEFAULT, \
								 LOCAL_IP_DEFAULT,LOCAL_SUBMASK_DEFAULT,LOCAL_GATEWAY_DEFAULT,LOCAL_DNS_DEFAULT,\
								 WIFI_IP_DEFAULT,WIFI_SUBMASK_DEFAULT,WIFI_GATEWAY_DEFAULT,WIFI_DNS_DEFAULT, \
								 USING_WIRELESS_DEFAULT, \
								 LOAD_SERVER1_IP_DEFAULT, LOAD_SERVER2_IP_DEFAULT, LOAD_SERVER1_PORT_DEFAULT, LOAD_SERVER2_PORT_DEFAULT, LOAD_TIMEOUT_DEFAULT, \
								 FTP_IP_DEFAULT, FTP_PORT_DEFAULT,FTP_USERNAME_DEFAULT,FTP_PASSWORD_DEFAULT, \
								 NDEV_HEART_TIME_DEFAULT,NDEV_HEART_EMERG_TIME_DEFAULT, NDEV_TIMER_GATE_VALUE_DEFAULT, NDEV_TERM_ID_DEFAULT,NDEV_AREA_INFO_DEFAULT, \
								 NDEV_FLOW_3G_DEFAULT, NDEV_FIRST_LEVEL_3G_DEFAULT, NDEV_SECOND_LEVEL_3G_DEFAULT, \
								 NDEV_LNT_IPADDR_DEFAULT, NDEV_LNT_PORT_DEFAULT, NDEV_LNT_USERID_DEFAULT,NDEV_LNT_SPNO_DEFAULT,NDEV_LNT_CONPA_DEFAULT,NDEV_LNT_CVAD_DEFAULT, \
								 NDEV_LIGHT_ENABLE_MM_DEFAULT, NDEV_LIGHT_ENABLE_HH_DEFAULT, NDEV_LIGHT_DISABLE_MM_DEFAULT, NDEV_LIGHT_DISABLE_HH_DEFAULT}




#define STAKE_CONFIG_DEFAULT		{SAE_HEART_TIME_DEFAULT,SAE_FIRMWARE_UPDATE_TIME_DEFAULT, SAE_CAN_BAUD_RATE_DEFAULT,SAE_QUANTITY_DEFAULT}


#define UNIV_BLK_CONFIG_DEFAULT	{UNIV_BLK_TOTAL_DFAULT,UNIV_BLK_INC_DFAULT,UNIV_BLK_DEC_DFAULT,UNIV_BLK_TEMP_DFAULT, \
									 UNIV_USE_TOTAL_BLK_DB_DEFAULT,UNIV_USE_INC_BLK_DB_DEFAULT,UNIV_USE_DEC_BLK_DB_DEFAULT,UNIV_USE_TEMP_BLK_DB_DEFAULT,\
									 UNIV_BLK_TOTAL_SEQ_DEFAULT,UNIV_BLK_INC_SEQ_DEFAULT,UNIV_BLK_DEC_SEQ_DEFAULT,UNIV_BLK_TEMP_SEQ_DEFAULT,\
									 UNIV_BLK_RFU_DEFAULT}



#define UNIV_FW_CONFIG_DEFAULT      {UNIV_SAE_FW_DEFAULT,UNIV_USE_SAE_FW_DB_DEFAULT,UNIV_FW_SEQ_DEFAULT,UNIV_SAE_FW_RFU_DEFAULT}
#define UNIV_RFU_CONFIG_DEFAULT	{UNIV_RFU_DEFAULT}







#endif



