#ifndef __LIB_NETWORK_CHECK_H__
#define __LIB_NETWORK_CHECK_H__


/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


enum CHECK_RESULT {
	
	CHECK_RESULT_PACKET_OK = 1,	//百分百成功
	CHECK_RESULT_PACKET_LOSS,	//部分丢包
	CHECK_RESULT_PACKET_ERROR,	//百分百丢包
	
	CHECK_RESULT_FAILED,   		//测试失败( 发送数据包前失败)
};


/*检查网络状态*/
enum CHECK_RESULT lib_get_network_status(const char *ip_addr);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

