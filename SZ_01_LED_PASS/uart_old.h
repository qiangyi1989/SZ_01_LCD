#ifndef USER_H
#define USER_H

//#define FRAME_TYPE_PAR 0 //参数字
//#define FRAME_TYPE_CMD 1 //命令字
//#define FRAME_TYPE_INFO 2//信息字

/* 下行 */
#define DOWN_READ_PARAM        0  	  /* 下行读参数 */
#define DOWN_READ_STATE   	   1  	  /* 下行读状态 */
#define DOWN_READ_PARAM_RANGE  2  	  /* 下行读参数范围 */
#define DOWN_WRITE_PARAM  	   3  	  /* 下行写配置参数 */
#define DOWN_WRITE_TEST_MODE   4      /* 下行写进入测试模式 */
#define DOWN_WRITE_CLEAR_CNT   5      /* 下行写清除已出筒数 */
#define DOWN_WRITE_KEY   	   6      /* 下行写按键操作 */
#define DOWN_WRITE_LEARN_MODE  7      /* 下行写进入学习模式 */
#define DOWN_WRITE_SPEED_MODE  8      /* 下行写切换速度模式 */
#define DOWN_WRITE_WARNING_MSG  9      /* 下行写报警消息 */

/* 上行 */
#define UP_WRITE_PARAM  	    10   	    /* 上行写参数 */
#define UP_WRITE_STATE  	    11   		/* 上行写状态 */
#define UP_WRITE_PARAM_RANGE    12 		    /* 上行写参数范围 */
#define UP_WRITE_REC_PARAM_DONE 13  		/* 上行写接收参数完成 */
#define UP_WRITE_TEST_MODE_DONE 14          /* 上行写进入测试模式成功 */
#define UP_WRITE_CLEAR_CNT_DONE 15          /* 上行写清除已出筒数成功 */
#define UP_WRITE_KEY_DONE 		16          /* 下行写按键操作成功 */
#define UP_WRITE_LEARN_MODE_DONE 17          /* 上行写进入学习模式成功 */
#define UP_WRITE_SPEED_MODE_DONE 18          /* 上行写切换速度模式成功 */
#define UP_WRITE_WARNING_MSG_DONE 19         /* 上行写报警消息成功 */


#define UP_WRITE_CHECK_FAIL     20    	    /* 上行写校验失败 */


#define DOWN_READ_PARAM_SENT  		50  
#define DOWN_READ_STATE_SENT  		51
#define DOWN_READ_PARAM_RANGE_SENT  52
#define DOWN_WRITE_PARAM_SENT       53
#define DOWN_WRITE_TEST_MODE_SENT   54
#define DOWN_WRITE_CLEAR_CNT_SENT   55
#define DOWN_WRITE_KEY_SENT         56
#define DOWN_WRITE_LEARN_MODE_SENT  57
#define DOWN_WRITE_SPEED_MODE_SENT  58
#define DOWN_WRITE_WARNING_MSG_SENT 59

#define UP_REPLY_DONE   0x7f


//信息字类型
#define GET_VER_CMD 0x01 //获取版本号
#define GET_DEFAUT_VAL 0x02 //获取默认参数值
#define GET_MENU_CFG_INFO 0x03 //获取菜单配置信息
#define GET_STATU_INFO 0x04 //获取主控板状态
#define GET_PAR_INFO 0x05 //获取主控板菜单参数 上载参数
#define SET_VER_IDX_CMD 0x07 //设置主控板版本序号

//命令值逻辑序号
#define CMD_INDX_0X88 0x88 //参数下载完成,进入工作状态
#define CMD_INDX_0XAA 0xAA //BOOT

//工作状态序号
#define WORK_STATU_WAIT 0x01 //等待操作屏下发参数
#define WORK_STATU_START 0x02 //进入工作状态,完成调零等动作

//其他上传标志 位索引
#define BIT_FLAG_DISP_ON 0x01  //1:允许操作菜单 0:不允许操作菜单

//通讯处理逻辑序号
#define COMD_LOG_STEP_0X550XAA 0  //握手
#define COMD_LOG_STEP_0X550XAA_DLY 1  //握手延时
#define COMD_LOG_STEP_GET_VER 2 //获取版本号
#define COMD_LOG_STEP_GET_DEF 10 //版本不匹配获取默认值
#define COMD_LOG_STEP_GET_CFG 20 //版本不匹配获取菜单配置信息
#define COMD_LOG_STEP_DOWN 30 //下发菜单数据到主控板
#define COMD_LOG_STEP_SART 40 //初始化完成进入工作
#define COMD_LOG_STEP_BOOT_RUN 50 //运行烧写程序

#define COMD_LOG_STEP_OTHER_CMD 0x77 //发送其他操作指令
#define COMD_LOG_STEP_WAIT 0x88    //巡检
#define COMD_LOG_STEP_DELAY 0x99   //延时置发送标志

#define COMD_LOG_DELAY_DZ 200 //配置帧间隔时间 单位ms
#define COMD_LOG_DELAY_PAR 100 //参数帧延时时间 单位ms 
#define COMD_LOG_DELAY_WORK 500 //巡检帧延时时间 单位ms 

//错误号
#define ERR_NUM_COM 18 //通讯错误



void uart_init();
void SendData(unsigned char dat);
void Send_UartBuf(char * buf);
void check_uart_rec(void);
void rec_data_func(void);
void send_data_func(void);

#endif

