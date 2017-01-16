#ifndef __UFILE_CTRL_INFO_H__
#define __UFILE_CTRL_INFO_H__




/*
 * 泛文件状态
 */
enum UNIV_CTRL_INFO
{


	
	UCI_F_RECV_TOTAL = 0x71,
	UCI_F_RECV_INC = 0x72,
	UCI_F_RECV_DEC = 0x73,
	UCI_F_RECV_TEMP = 0x74,
	UCI_F_RECV_FW = 0x81,
	
};


/*
 * 泛文件接收状态
 */
enum F_RECV_STAT
{
	F_RECV_STAT_INIT = 0, //初始化
	F_RECV_STAT_ING,  //正在接收
	F_RECV_STAT_QUIT, //退出接收
};

 struct ufile_ctrl_info
{
	unsigned char f_recv_total;
	unsigned char f_recv_inc;
	unsigned char f_recv_dec;
	unsigned char f_recv_temp;
	unsigned char f_recv_fw;


	
}__attribute__((packed));
typedef  struct ufile_ctrl_info ufile_ctrl_info_t;



void ufile_ctrl_info_init(void);
int ufile_ctrl_info_set(const unsigned char cmd, ufile_ctrl_info_t *ufile);
int ufile_ctrl_info_set_all(ufile_ctrl_info_t *ufile);
int ufile_ctrl_info_get(const unsigned char cmd, ufile_ctrl_info_t *ufile);
int ufile_ctrl_info_get_all(ufile_ctrl_info_t *ufile);
unsigned char ufile_ctrl_info_get_f_recv(const unsigned char cmd);










#endif


