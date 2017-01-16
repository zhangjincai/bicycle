#ifndef __CONFIGS_H__
#define __CONFIGS_H__





#define CONFS_CAN_BUS0						0				//CAN0 ���ߺ�
#define CONFS_CAN_BUS1						1				//CAN1 ���ߺ�
#define CONFS_CAN_BUF_NUM					(5 * 1024)			//CAN ����������
#define CONFS_CAN_RB_SZ						(10 * 1024)		//CAN ѭ����������С
#define CONFS_STAKE_NUM_MAX				(64)				//���֧����׮����
#define CONFS_STAKE_FN_HASH_NUM			(64)				//��׮����ϣ��
#define CONFS_STAKE_CMD_NUM				(48)				//��׮���֧��������
#define CONFS_STAKE_TRY_HEART_TIMES		(3)				//��׮���Է�����������
#define CONFS_NDEV_RECV_BUF_SZ				(1024  * 1024)		//�ڵ�����ջ�����
#define CONFS_NDEV_TRY_REG_TIMES			(3)				//�ڵ������ע�����
#define CONFS_NDEV_TRY_BTHEART_TIMES		(2)				//�ڵ���������ʹ���
#define CONFS_NDEV_BTHEART_WAITTIME		(30)				//�����ȴ�����ʱ��   //15->30 2016-11-25
#define CONFS_NDEV_WAIT_REG_TIMES			(30)				//�ڵ��ע��ȴ���Ӧʱ��
#define CONFS_NDEV_FN_HASH_NUM			(64)				//�ڵ������ϣ��
#define CONFS_NDEV_TRY_SEND_TIMES			(3)				//�ڵ�����Է��ʹ���


#define CONFS_UNIV_THREADS                          	(5)				//���ļ��߳���
#define CONFS_UNIV_QUEUES					(5)				//���ļ��̳߳ض�����



/*
 * GPSģ�鶨��
 */
#define CONFS_USING_GPS_MODEL			1


/*
 * test by zjc  
 */
#define CONFS_USING_TEST_BY_ZJC			0

/*
 * �������̼��������ܿ������ 1:���� 0:�ر�
 */
#define CONFS_USING_READER_UPDATE		0

/*
 * �ϴ�����ͨ�������汾�����ģ����ڶ���������ʱ�жϳɹ���� 1:���� 0:�ر�
 */
#define CONFS_USING_UPLOAD_READER_VER		1

/*
 * ftp���س�ʱ(s)������ʱ������������йأ���ʱ�򲻻��������������¿�ʼ���� add by zjc at 2016-10-12
 */
#define CONFS_FTP_DOWNLOAD_TIMEOUT		(90*60) //90���� 2016-11-25 ע��:ʵ���Ϊ70��������!

/*
 * �ڵ��������ͨ���쳣��3gģ���Զ���� 1:���� 0:�ر�
 */
#define CONFS_USING_AUTO_REBOOT_3G		1


/*
 * FTP����ʱ���а汾�Ƚϣ����ذ汾�ͱ��ذ汾��һ�²��������� 1:���� 0:�ر� 2017-01-09
 */
#define CONFS_USING_FTP_DOWNLOAD_VERSION_CHECK		1


#endif



