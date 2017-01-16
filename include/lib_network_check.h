#ifndef __LIB_NETWORK_CHECK_H__
#define __LIB_NETWORK_CHECK_H__


/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


enum CHECK_RESULT {
	
	CHECK_RESULT_PACKET_OK = 1,	//�ٷְٳɹ�
	CHECK_RESULT_PACKET_LOSS,	//���ֶ���
	CHECK_RESULT_PACKET_ERROR,	//�ٷְٶ���
	
	CHECK_RESULT_FAILED,   		//����ʧ��( �������ݰ�ǰʧ��)
};


/*�������״̬*/
enum CHECK_RESULT lib_get_network_status(const char *ip_addr);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

