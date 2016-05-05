#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//#define SZ_01
#define SZ_02        
//#define SZ_03

#define YES   1
#define NO    0

#define KEY_IO1_PAPER_FW_MASK   (unsigned short)0x0010         /* 进纸 */
#define KEY_IO2_PAPER_BW_MASK   (unsigned short)0x0020	       /* 退纸 */
#define KEY_IO3_CUT_MASK 	    (unsigned short)0x0040         /* 切纸 */
#define KEY_IO4_PAPER_DOWN_MASK (unsigned short)0x0080		   /* 下料 */
#define KEY_IO5_ROLL_MASK 	    (unsigned short)0x0100         /* 搓条 */
#define KEY_IO9_FZ_FW_MASK      (unsigned short)0x1000         /* 发纸 */
#define KEY_RED_FZ_BW_MASK      (unsigned short)0x2000         /* 收纸 */

//#define FOOT_DOWN_FLAG      1  /* 踏板按下完成标志:未使用 */
#define KEY_LONG_DOWN_FLAG 1  /* 踏板长按按下标志 */
#define KEY_SHORT_UP_FLAG  2  /* 踏板短按抬起标志 */
#define KEY_LONG_UP_FLAG   3  /* 踏板长按抬起标志 */


//#define READ_STATE_PERIOD  100    /* 读取底层状态的周期:ms */
#define READ_STATE_PERIOD  1000    /* 读取底层状态的周期:ms */

typedef struct
{
	//unsigned short const		*pus_inData;				///<开入变量地址
	unsigned short const		us_inMask;					///<开入判断掩码
	unsigned short				us_logStep;					///<逻辑步骤序号	
	//unsigned long				ul_JitDelay;				///<去抖动延时:ms
	//unsigned long               ul_LongDelay;               ///<长按延时:ms
	unsigned long				us_DownCnt;				    ///<按下持续时间计数:ms
	unsigned long               us_UpCnt;                   ///<抬起持续时间计数:ms
	unsigned short				us_finishFlag;				///<逻辑完成标志
}_T_CHECK_EDGE2;


extern char write_param_flag;     /* 当前是否要写配置参数到主板:1需要，0不需要 */
extern char already_read_param;
extern char already_read_param_range;

extern unsigned int cnt_read_state;
extern unsigned int cnt_read_param;
extern unsigned int cnt_read_param_range;
extern char send_frame_type;

extern char write_test_mode_flag;
extern char write_learn_mode_flag;
extern char write_speed_mode_flag;
extern char write_warning_flag;
extern char write_clear_cnt_flag;
extern char main_borad_finish_clear;
extern char write_key_flag;

extern void start_key_detect();

#endif
