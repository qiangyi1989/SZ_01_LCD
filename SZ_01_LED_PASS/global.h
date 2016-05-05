#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//#define SZ_01
#define SZ_02        
//#define SZ_03

#define YES   1
#define NO    0

#define KEY_IO1_PAPER_FW_MASK   (unsigned short)0x0010         /* ��ֽ */
#define KEY_IO2_PAPER_BW_MASK   (unsigned short)0x0020	       /* ��ֽ */
#define KEY_IO3_CUT_MASK 	    (unsigned short)0x0040         /* ��ֽ */
#define KEY_IO4_PAPER_DOWN_MASK (unsigned short)0x0080		   /* ���� */
#define KEY_IO5_ROLL_MASK 	    (unsigned short)0x0100         /* ���� */
#define KEY_IO9_FZ_FW_MASK      (unsigned short)0x1000         /* ��ֽ */
#define KEY_RED_FZ_BW_MASK      (unsigned short)0x2000         /* ��ֽ */

//#define FOOT_DOWN_FLAG      1  /* ̤�尴����ɱ�־:δʹ�� */
#define KEY_LONG_DOWN_FLAG 1  /* ̤�峤�����±�־ */
#define KEY_SHORT_UP_FLAG  2  /* ̤��̰�̧���־ */
#define KEY_LONG_UP_FLAG   3  /* ̤�峤��̧���־ */


//#define READ_STATE_PERIOD  100    /* ��ȡ�ײ�״̬������:ms */
#define READ_STATE_PERIOD  1000    /* ��ȡ�ײ�״̬������:ms */

typedef struct
{
	//unsigned short const		*pus_inData;				///<���������ַ
	unsigned short const		us_inMask;					///<�����ж�����
	unsigned short				us_logStep;					///<�߼��������	
	//unsigned long				ul_JitDelay;				///<ȥ������ʱ:ms
	//unsigned long               ul_LongDelay;               ///<������ʱ:ms
	unsigned long				us_DownCnt;				    ///<���³���ʱ�����:ms
	unsigned long               us_UpCnt;                   ///<̧�����ʱ�����:ms
	unsigned short				us_finishFlag;				///<�߼���ɱ�־
}_T_CHECK_EDGE2;


extern char write_param_flag;     /* ��ǰ�Ƿ�Ҫд���ò���������:1��Ҫ��0����Ҫ */
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
