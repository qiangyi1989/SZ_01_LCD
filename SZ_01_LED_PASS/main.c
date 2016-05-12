#include "led_display.h"
#include "sys.h"
//#include"led.h"	
#include "eeprom.h"
#include "uart.h"
#include "global.h"
#include "main.h"
#include <string.h>

//#define uint unsigned int
//#define uchar unsigned char


#define FOSC 11059200L

#define T1MS (65536-FOSC/12/1000) //12T模式


extern xdata public_stru public_val;

char eeprom_en = 0;
int iInitCnt = 0;

/* 下行发送的帧类型 */
//char send_frame_type = 0xff;
char send_frame_type = UP_REPLY_DONE;

unsigned int cnt_read_state = 0;
unsigned int cnt_read_param = 0;
unsigned int cnt_read_param_range = 0;

char already_read_param = 0;   		 /* 是否已读到配置参数:1已读到，0未读到 */
//char send_read_param_flag = 0;       /* 是否已发送 */

char already_read_param_range = 0;   /* 是否已读到配置参数范围:1已读到，0未读到 */

char write_param_flag = 0;           /* 当前是否要写配置参数到主板:1需要，0不需要 */

char write_test_mode_flag = 0;       /* 当前是否要写测试模式到主板:1需要，0不需要 */

char write_learn_mode_flag = 0;      /* 当前是否要写学习模式到主板:1需要，0不需要 */

char write_speed_mode_flag = 0;      /* 当前是否要写速度模式到主板:1需要，0不需要 */

char write_warning_flag = 0;         /* 当前是否要写报警消息到主板:1需要，0不需要 */

char write_clear_cnt_flag = 0;       /* 当前是否要写清除已出筒数到主板:1需要，0不需要 */
char main_borad_finish_clear = 1;    /* 主板收到清除命令后已完成计数清零:1已完成，0未完成 */

char write_key_flag = 0;             /* 当前是否要写按键到主板:1需要，0不需要 */

char start_com_flag = 0;       	     /* 操作板开始串口通信的标志: 1开始，0未开始 */

unsigned int usLongDownFlag = 0;     /* 长按下标志: 0未按下，1已按下 */


xdata _T_CHECK_EDGE2 t_FzFwKey = { 
								KEY_IO9_FZ_FW_MASK,
								888, 
								0, 
								0,
								0};

xdata _T_CHECK_EDGE2 t_FzBwKey = { 
								KEY_RED_FZ_BW_MASK,
								888,  
								0, 
								0,
								0};


xdata _T_CHECK_EDGE2 t_PaperFwKey = { //(unsigned short const *)&(unKey.all),
								KEY_IO1_PAPER_FW_MASK,
								888, 
								//40, 
								//500, 
								0, 
								0,
								0};

xdata _T_CHECK_EDGE2 t_PaperBwKey = { 
								KEY_IO2_PAPER_BW_MASK,
								888,  
								0, 
								0,
								0};

xdata _T_CHECK_EDGE2 t_CutKey = { 
								KEY_IO3_CUT_MASK,
								888,  
								0, 
								0,
								0};

xdata _T_CHECK_EDGE2 t_PaperDownKey = { 
								KEY_IO4_PAPER_DOWN_MASK,
								888,  
								0, 
								0,
								0};

xdata _T_CHECK_EDGE2 t_RollKey = { 
								KEY_IO5_ROLL_MASK,
								888,  
								0, 
								0,
								0};

#if 0
//pwm
void Timer0Init(void)
{
    AUXR |= 0x80;                 	//定时器0为1T模式
//  AUXR &= 0x7f;                	//定时器0为12T模式

    TMOD = 0x00;                    //设置定时器为模式0(16位自动重装载)

	//TL0 = 0xfd;                     //初始化计时值
    TL0 = ZL_PWM_FREQ;                        /* 初始化为直流电机的PWM频率248 by james */
    
    TH0 = 0xff;
    TR0 = 1;                        //定时器0开始计时
 //   ET0 = 1;                        //使能定时器0中断
}

void Timer1Init(void)
{
    AUXR |= 0x40;                //定时器1为1T模式
	TMOD &= 0x0f;		//设置定时器模式
	TL1 = 0xCB;		//设置定时初值
	TH1 = 0xD4;	    //设置定时初值
	TF1 = 0;		//清除TF0标志
    IP |= 0x08;
//	TR1 = 1;		//定时器0开始计时
}

void Timer2Init(void)
{
    AUXR |= 0x04;
    T2L = 0xCB;                     //初始化计时值
    T2H = 0xD4;
    AUXR |= 0x10;                   //定时器2开始计时
}


void sys_init(void)		   //系统初始化函数
{
  P2M1 = 0x01;
  P2M0 = 0xC1; //11000001
  
  P3M1 = 0x00;
  P3M0 = 0x6B; //01101011

  P1M1 = 0x00;
  P1M0 = 0x04; //00000100

  
  P0 = 0xff;             //io口初始化
  P1 = 0xff;
  P2 = 0xff;
  P3 = 0x9f;             //步进电机初始化为不使能 
  
  //Timer0Init();  // pwm
  //Timer2Init();  // 1ms主逻辑中断
  
  //pwm_init();

  //Timer1Init();  // 步进脉冲翻转
  
}

#endif


void Timer0Init(void)
{
    AUXR |= 0x80;                 	//定时器0为1T模式
//  AUXR &= 0x7f;                	//定时器0为12T模式

    TMOD = 0x00;                    //设置定时器为模式0(16位自动重装载)
    TL0 = T1MS;                     //初始化计时值
    TH0 = T1MS >> 8;
    TR0 = 1;                        //定时器0开始计时
    ET0 = 1;                        //使能定时器0中断
    EA = 1;
}

void sys_init(void)		   //系统初始化函数
{
	P2M1 = 0x00;
	P2M0 = 0x20;

	P0 = 0xff;             //io口初始化
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff; 

	Timer0Init();
	uart_init();
	
#ifndef LED_DISP
	uart2_init(); 
#endif	
  
}


void GlobalInit()
{
	memset(&public_val, 0, sizeof(public_val));	

	speed_mode = 1;  // 高速模式
}




///踏板长按识别

#if 1
void CheckKeyClick(_T_CHECK_EDGE2 *p)
{
	switch(p->us_logStep)
	{
	case 0:
		
		p->us_finishFlag = 0;
		if( !(unKey.all & p->us_inMask) )										
		{
			p->us_logStep = 10;
			p->us_DownCnt = 0;
		}
		
		break;
		
	case 10:

		if( !(unKey.all & p->us_inMask) )										
		{
			p->us_DownCnt++;
			if(p->us_DownCnt > KEY_JIT_DELAY)			///<按下						
			{
				//p->us_finishFlag = FOOT_DOWN_FLAG;   

				p->us_logStep = 20;   
			}
		}
		else
		{
			p->us_DownCnt = 0;
			p->us_logStep = 0;
		}
		
		break;	
		
	case 20:
		
		p->us_DownCnt++;
		
		if (unKey.all & p->us_inMask)   
		{
			p->us_logStep = 30;
			p->us_UpCnt = 0;
		}

		if (p->us_DownCnt > KEY_LONG_DELAY)
		{
			p->us_finishFlag = KEY_LONG_DOWN_FLAG;   /* 判为长按按下 */   

			//p->us_logStep = 30;
		}
		
		break;

	case 30:

		p->us_DownCnt++;

		if (unKey.all & p->us_inMask)	 
		{
			p->us_UpCnt++;
			if(p->us_UpCnt > KEY_JIT_DELAY)   /* 弹起 */									
			{
				//if (p->us_DownCnt > KEY_LONG_DELAY)     
				if (p->us_finishFlag == KEY_LONG_DOWN_FLAG)
				{
					p->us_finishFlag = KEY_LONG_UP_FLAG;   /* 判为长按抬起 */   
				}
				else
				{
					p->us_finishFlag = KEY_SHORT_UP_FLAG;  /* 判为短按 */
				}

				p->us_DownCnt = 0;				
				p->us_logStep = 888;				
			}
		}
		else
		{
			p->us_UpCnt = 0;
			p->us_logStep = 20;
		}
		
		break;
		
	}
	
}

#else

void CheckKeyClick(_T_CHECK_EDGE2 *p)
{
	switch(p->us_logStep)
	{
	case 0:
		
		p->us_finishFlag = 0;
		if( !(unKey.all & p->us_inMask) )										
		{
			p->us_logStep = 10;
			p->us_DownCnt = 0;
		}
		
		break;
		
	case 10:

		if( !(unKey.all & p->us_inMask) )										
		{
			p->us_DownCnt++;
			if(p->us_DownCnt > KEY_JIT_DELAY)			///<按下						
			{
				p->us_finishFlag = KEY_LONG_DOWN_FLAG;   /* 判为长按按下 */     

				p->us_DownCnt = 0;
				p->us_logStep = 20;   
			}
		}
		else
		{
			p->us_DownCnt = 0;
			p->us_logStep = 0;
		}
		
		break;	
		
	case 20:		
		
		if (unKey.all & p->us_inMask)   
		{
			p->us_logStep = 30;
			p->us_UpCnt = 0;
		}
		
		break;

	case 30:

		if (unKey.all & p->us_inMask)	 
		{
			p->us_UpCnt++;
			if(p->us_UpCnt > KEY_JIT_DELAY)   /* 弹起 */									
			{
				p->us_finishFlag = KEY_LONG_UP_FLAG;   /* 判为长按抬起 */   

				p->us_UpCnt = 0;				
				p->us_logStep = 888;				
			}
		}
		else
		{
			p->us_UpCnt = 0;
			p->us_logStep = 20;
		}
		
		break;
		
	}
	
}

#endif


void key_detect()
{	
	CheckKeyClick(&t_PaperFwKey);
	CheckKeyClick(&t_PaperBwKey);
	//CheckKeyClick(&t_CutKey);	
	CheckKeyClick(&t_PaperDownKey);	
	CheckKeyClick(&t_RollKey);	
	CheckKeyClick(&t_FzFwKey);		
	CheckKeyClick(&t_FzBwKey);		
}

void start_key_detect()
{
	t_PaperFwKey.us_logStep = 0;
	t_PaperBwKey.us_logStep = 0;
	//t_CutKey.us_logStep = 0;
	t_PaperDownKey.us_logStep = 0;
	t_RollKey.us_logStep = 0;	
	t_FzFwKey.us_logStep = 0;
	t_FzBwKey.us_logStep = 0;	
}

void judge_key()
{
	// 发纸
	if (t_FzFwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO9;
		
		if (t_FzFwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_FzFwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_FzFwKey.us_finishFlag = 0;
			t_FzFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_FzFwKey.us_finishFlag = 0;
			t_FzFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}

	// 收纸
	if (t_FzBwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_RED;
		
		if (t_FzBwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_FzBwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_FzBwKey.us_finishFlag = 0;
			t_FzBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_FzBwKey.us_finishFlag = 0;
			t_FzBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}


	
	/* 进纸 */
	if (t_PaperFwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO1;
		
		if (t_PaperFwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_PaperFwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_PaperFwKey.us_finishFlag = 0;
			t_PaperFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_PaperFwKey.us_finishFlag = 0;
			t_PaperFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}

	/* 退纸 */	
	if (t_PaperBwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO2;
		
		if (t_PaperBwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_PaperBwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_PaperBwKey.us_finishFlag = 0;
			t_PaperBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_PaperBwKey.us_finishFlag = 0;
			t_PaperBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}

	/* 切纸 */	
#if 0
	if (t_CutKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO3;
		
		if (t_CutKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_CutKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_CutKey.us_finishFlag = 0;
			t_CutKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_CutKey.us_finishFlag = 0;
			t_CutKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}
#endif

	/* 下料 */	
	if (t_PaperDownKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO4;
		
		if (t_PaperDownKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_PaperDownKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_PaperDownKey.us_finishFlag = 0;
			t_PaperDownKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_PaperDownKey.us_finishFlag = 0;
			t_PaperDownKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}

	/* 搓条 */	
	if (t_RollKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO5;
		
		if (t_RollKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* 长按 */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* 有按键，需要写按键到主板 */
			}
		}
		else if (t_RollKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* 短按 */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_RollKey.us_finishFlag = 0;
			t_RollKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}
		else                                                       /* 长按抬起 */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_RollKey.us_finishFlag = 0;
			t_RollKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* 清长按标志 */

			write_key_flag = 1;   /* 有按键，需要写按键到主板 */			
		}	
	}
	
}

/*************定时器0中断***************
函数原型：void tm0_isr(void) interrupt 1 using 1
入口参数：无
出口参数：无
备注：
*******************************/
void tm0_isr() interrupt 1 using 1
{
	public_val.ms_timer++;
	
	check_uart_rec();

	key_detect();
	
}



void main()
{
	eeprom_en = 0;

	/* 硬件初始化 */
	sys_init();

	/* 全局变量初始化 */
	GlobalInit();
		
	InitLEDDisplay();

	cnt_read_state = public_val.ms_timer;	
	
	//add by yq
	GpuSend("DR2\r\n");
	
	while(1)
	{	
		if (start_com_flag == 0)
		{
			if (public_val.ms_timer - cnt_read_state > 500)  /* 开机延迟500ms才开始通信 */
			//if (public_val.ms_timer - cnt_read_state > 1500)   //for test
			{
				cnt_read_state = public_val.ms_timer;
				cnt_read_param = public_val.ms_timer;

				/* 开始手动模式按键检测 */
				start_key_detect();

				start_com_flag = 1;
			}
		}

		if (start_com_flag == 1)
		{		
			LEDDisplay();
			
			if(public_val.uart_rec_statu == 2)   /* 接收完毕，开始处理 */
			{			
				rec_data_func();
				
				public_val.uart_rec_statu = 3;   /* 置接收处理完毕状态 */
				public_val.uart_rec_count = 0;   /* 清接收字节计数 */
			}
		  	

			send_data_func();

			judge_key();

		}
		
	}
}


