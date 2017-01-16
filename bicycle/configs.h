#ifndef __CONFIGS_H__
#define __CONFIGS_H__





#define CONFS_CAN_BUS0						0				//CAN0 总线号
#define CONFS_CAN_BUS1						1				//CAN1 总线号
#define CONFS_CAN_BUF_NUM					(5 * 1024)			//CAN 缓冲区个数
#define CONFS_CAN_RB_SZ						(10 * 1024)		//CAN 循环缓冲区大小
#define CONFS_STAKE_NUM_MAX				(64)				//最大支持锁桩数量
#define CONFS_STAKE_FN_HASH_NUM			(64)				//锁桩最大哈希数
#define CONFS_STAKE_CMD_NUM				(48)				//锁桩最大支持命令数
#define CONFS_STAKE_TRY_HEART_TIMES		(3)				//锁桩尝试发送心跳次数
#define CONFS_NDEV_RECV_BUF_SZ				(1024  * 1024)		//节点机接收缓冲区
#define CONFS_NDEV_TRY_REG_TIMES			(3)				//节点机尝试注册次数
#define CONFS_NDEV_TRY_BTHEART_TIMES		(2)				//节点机心跳发送次数
#define CONFS_NDEV_BTHEART_WAITTIME		(30)				//心跳等待接收时间   //15->30 2016-11-25
#define CONFS_NDEV_WAIT_REG_TIMES			(30)				//节点机注册等待回应时间
#define CONFS_NDEV_FN_HASH_NUM			(64)				//节点机最大哈希数
#define CONFS_NDEV_TRY_SEND_TIMES			(3)				//节点机尝试发送次数


#define CONFS_UNIV_THREADS                          	(5)				//泛文件线程数
#define CONFS_UNIV_QUEUES					(5)				//泛文件线程池队列数



/*
 * GPS模块定义
 */
#define CONFS_USING_GPS_MODEL			1


/*
 * test by zjc  
 */
#define CONFS_USING_TEST_BY_ZJC			0

/*
 * 读卡器固件升级功能开启与否 1:开启 0:关闭
 */
#define CONFS_USING_READER_UPDATE		0

/*
 * 上传岭南通读卡器版本给中心，用于读卡器升级时判断成功与否 1:开启 0:关闭
 */
#define CONFS_USING_UPLOAD_READER_VER		1

/*
 * ftp下载超时(s)，下载时间跟网络速率有关，超时则不会续传，而是重新开始下载 add by zjc at 2016-10-12
 */
#define CONFS_FTP_DOWNLOAD_TIMEOUT		(90*60) //90分钟 2016-11-25 注意:实测最长为70分钟左右!

/*
 * 节点机和中心通信异常后3g模块自动与否 1:开启 0:关闭
 */
#define CONFS_USING_AUTO_REBOOT_3G		1


/*
 * FTP下载时进行版本比较，下载版本和本地版本不一致才允许下载 1:开启 0:关闭 2017-01-09
 */
#define CONFS_USING_FTP_DOWNLOAD_VERSION_CHECK		1


#endif



