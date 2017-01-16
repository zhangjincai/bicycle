#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#define IPV4(a, b, c, d) 		((d << 24) | (c << 16) |( b << 8) | (a))


/*
 * �����ļ�·��
 */
//#define NDEV_CONFIG_FILE			"/mnt/config/ndev_config.cfg"
//#define UNIV_CONF_FILE				"/mnt/config/univ_config.cfg"

#define NDEV_CONFIG_FILE			"/opt/config/ndev_config.cfg"
#define UNIV_CONF_FILE				"/opt/config/univ_config.cfg"




/*
 * �ڵ��Ĭ������
 */
#define MAGIC_DEFAULT_HEAD					0x20101207				//ħ��ͷ��
#define MAGIC_DEFAULT_TAIL					0x20101207				//ħ��β��
#define CRC16_DEFAULT						0xFFFF					//Ĭ��CRC16

#define BOOT_VERSION_DEFAULT				"15010101"				//uboot�汾
#define KERNEL_VERSION_DEFAULT				"15010101"				//kernel�汾
#define ROOTFS_VERSION_DEFAULT				"15010101"				//rootfs�汾
#define FW_VERSION_DEFAULT					"15010101"				//firmware�汾
#define APPL_VERSION_DEFAULT				"15010101"				//appl�汾

#define LOCAL_IP_DEFAULT					IPV4(192,168,1,169)			//����IP��ַ
#define LOCAL_SUBMASK_DEFAULT				IPV4(255,255,255,0)			//������������
#define LOCAL_GATEWAY_DEFAULT				IPV4(192,168,1,1)       		//��������
#define LOCAL_DNS_DEFAULT					IPV4(202,96,128,143)		//������ַ

#define WIFI_IP_DEFAULT						IPV4(192,168,100,1)			//WIFI IP��ַ
#define WIFI_SUBMASK_DEFAULT				IPV4(255,255,255,0)			//WIFI ��������
#define WIFI_GATEWAY_DEFAULT				IPV4(192,168,100,1)       		//WIFI ����
#define WIFI_DNS_DEFAULT					IPV4(202,96,128,143)		//WIFI ��ַ

#define USING_WIRELESS_DEFAULT				1						//ʹ���������� 1:����, 2:����

#define LOAD_SERVER1_IP_DEFAULT				IPV4(120,24,97,95)			//���ؾ��������1 IP ��ַ
#define LOAD_SERVER2_IP_DEFAULT				IPV4(180,76,140,185)			//���ؾ��������2 IP ��ַ
#define LOAD_SERVER1_PORT_DEFAULT			20002 //20000					//���ؾ��������1�˿� zjc at 2016-09-29
#define LOAD_SERVER2_PORT_DEFAULT			20001					//���ؾ��������2�˿�
#define LOAD_TIMEOUT_DEFAULT				10						//���ؾ���������ȴ�ʱ��(��)

#define FTP_IP_DEFAULT						IPV4(121,8,152,2)			//FTP IP��ַ
#define FTP_PORT_DEFAULT					10040					//FTP�˿�
#define FTP_USERNAME_DEFAULT				"endpointuser"			//FTP�û���
#define FTP_PASSWORD_DEFAULT				"11142010"				//FTP����

#define NDEV_HEART_TIME_DEFAULT				180      					//�ڵ������ʱ�� ��λ:��
#define NDEV_HEART_EMERG_TIME_DEFAULT		90      					//�ڵ����������ʱ�� ��λ:��
#define NDEV_TIMER_GATE_VALUE_DEFAULT		30						//ʱ������բֵ
#define NDEV_TERM_ID_DEFAULT				0xFFFF 					//�ն˱��
#define NDEV_NAME_DEFAULT					""						//��������
#define NDEV_AREA_INFO_DEFAULT				0xFFFF					//������Ϣ
#define NDEV_FLOW_3G_DEFAULT					0					// 3G����  ��λ:KB
#define NDEV_FIRST_LEVEL_3G_DEFAULT	      		200						// 3G��һբֵ ��λ:MB
#define NDEV_SECOND_LEVEL_3G_DEFAULT	      	280                     			// 3G�ڶ�բֵ ��λ:MB

/*
 * ��ʽ���������IP:183.62.39.30
 * ��ʽ����������˿�:10022
 */
#if 0 //add by zjc at 2016-09-29(�޸�Ĭ�ϴ��������)
#define NDEV_LNT_IPADDR_DEFAULT			IPV4(121,8,152,2)			//����ͨ����IP ��������: �ĳ����˴�������IP
#define NDEV_LNT_PORT_DEFAULT				10030					//����ͨ���Ķ˿� : �ĳ����˴������Ķ˿�
#else
#define NDEV_LNT_IPADDR_DEFAULT			IPV4(183,62,39,30)			//����ͨ����IP ��������: �ĳ����˴�������IP
#define NDEV_LNT_PORT_DEFAULT				10022	
#endif
#define NDEV_LNT_USERID_DEFAULT			""						 //�û�ID
#define NDEV_LNT_SPNO_DEFAULT				"0x0101"					 //�����̴���
#define NDEV_LNT_CONPA_DEFAULT				0x80						//���ѳ�ʼ������  0x80:��������
#define NDEV_LNT_CVAD_DEFAULT				0x00						 //��������Ч������


#define NDEV_LIGHT_ENABLE_HH_DEFAULT		18			//���俪��ʱ��  6:30  ����
#define NDEV_LIGHT_ENABLE_MM_DEFAULT		30	

#define NDEV_LIGHT_DISABLE_HH_DEFAULT		6			//���俪��ʱ��  6:00 ����
#define NDEV_LIGHT_DISABLE_MM_DEFAULT		00


/*
 * ׮��Ĭ������
 */
#define SAE_HEART_TIME_DEFAULT					90						//׮������ʱ�� ��λ:��
#define SAE_FIRMWARE_UPDATE_TIME_DEFAULT    	""						//�̼�����ʱ�� 
#define SAE_CAN_BAUD_RATE_DEFAULT				2						//CAN ������	2:BAUD_RATE_125kbps
#define SAE_QUANTITY_DEFAULT					64						//��׮����



/*
 *  ���ļ�Ĭ������
 */
#define UNIV_BLK_TOTAL_DFAULT					"00000000"				//����������
#define UNIV_BLK_INC_DFAULT						"00000000"				//����������
#define UNIV_BLK_DEC_DFAULT						"00000000"				//����������
#define UNIV_BLK_TEMP_DFAULT					"00000000"				//��ʱ������
#define UNIV_SAE_FW_DEFAULT					"00000000"				//׮���̼�
#define UNIV_SAE_PARA_DEFAULT					"00000000"				//׮������
#define UNIV_UNIONPAY_KEY_DEFAULT				"00000000"				//������Կ

#define UNIV_USE_TOTAL_BLK_DB_DEFAULT			0						//����������ʹ�����ݿ�
#define UNIV_USE_INC_BLK_DB_DEFAULT			0						//����������ʹ�����ݿ�
#define UNIV_USE_DEC_BLK_DB_DEFAULT			0						//����������ʹ�����ݿ�
#define UNIV_USE_TEMP_BLK_DB_DEFAULT			0						//��ʱ��������ʹ�����ݿ�
#define UNIV_USE_SAE_FW_DB_DEFAULT			0						//׮���̼�ʹ�����ݿ�

#define UNIV_BLK_TOTAL_SEQ_DEFAULT				0
#define UNIV_BLK_INC_SEQ_DEFAULT				0
#define UNIV_BLK_DEC_SEQ_DEFAULT				0
#define UNIV_BLK_TEMP_SEQ_DEFAULT				0
#define UNIV_FW_SEQ_DEFAULT					0


#define UNIV_BLK_RFU_DEFAULT					""						//����������
#define UNIV_SAE_FW_RFU_DEFAULT				""						//׮������
#define UNIV_RFU_DEFAULT						""						//���ļ����� 




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



