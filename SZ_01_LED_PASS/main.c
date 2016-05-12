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

#define T1MS (65536-FOSC/12/1000) //12Tģʽ


extern xdata public_stru public_val;

char eeprom_en = 0;
int iInitCnt = 0;

/* ���з��͵�֡���� */
//char send_frame_type = 0xff;
char send_frame_type = UP_REPLY_DONE;

unsigned int cnt_read_state = 0;
unsigned int cnt_read_param = 0;
unsigned int cnt_read_param_range = 0;

char already_read_param = 0;   		 /* �Ƿ��Ѷ������ò���:1�Ѷ�����0δ���� */
//char send_read_param_flag = 0;       /* �Ƿ��ѷ��� */

char already_read_param_range = 0;   /* �Ƿ��Ѷ������ò�����Χ:1�Ѷ�����0δ���� */

char write_param_flag = 0;           /* ��ǰ�Ƿ�Ҫд���ò���������:1��Ҫ��0����Ҫ */

char write_test_mode_flag = 0;       /* ��ǰ�Ƿ�Ҫд����ģʽ������:1��Ҫ��0����Ҫ */

char write_learn_mode_flag = 0;      /* ��ǰ�Ƿ�Ҫдѧϰģʽ������:1��Ҫ��0����Ҫ */

char write_speed_mode_flag = 0;      /* ��ǰ�Ƿ�Ҫд�ٶ�ģʽ������:1��Ҫ��0����Ҫ */

char write_warning_flag = 0;         /* ��ǰ�Ƿ�Ҫд������Ϣ������:1��Ҫ��0����Ҫ */

char write_clear_cnt_flag = 0;       /* ��ǰ�Ƿ�Ҫд����ѳ�Ͳ��������:1��Ҫ��0����Ҫ */
char main_borad_finish_clear = 1;    /* �����յ�������������ɼ�������:1����ɣ�0δ��� */

char write_key_flag = 0;             /* ��ǰ�Ƿ�Ҫд����������:1��Ҫ��0����Ҫ */

char start_com_flag = 0;       	     /* �����忪ʼ����ͨ�ŵı�־: 1��ʼ��0δ��ʼ */

unsigned int usLongDownFlag = 0;     /* �����±�־: 0δ���£�1�Ѱ��� */


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
    AUXR |= 0x80;                 	//��ʱ��0Ϊ1Tģʽ
//  AUXR &= 0x7f;                	//��ʱ��0Ϊ12Tģʽ

    TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)

	//TL0 = 0xfd;                     //��ʼ����ʱֵ
    TL0 = ZL_PWM_FREQ;                        /* ��ʼ��Ϊֱ�������PWMƵ��248 by james */
    
    TH0 = 0xff;
    TR0 = 1;                        //��ʱ��0��ʼ��ʱ
 //   ET0 = 1;                        //ʹ�ܶ�ʱ��0�ж�
}

void Timer1Init(void)
{
    AUXR |= 0x40;                //��ʱ��1Ϊ1Tģʽ
	TMOD &= 0x0f;		//���ö�ʱ��ģʽ
	TL1 = 0xCB;		//���ö�ʱ��ֵ
	TH1 = 0xD4;	    //���ö�ʱ��ֵ
	TF1 = 0;		//���TF0��־
    IP |= 0x08;
//	TR1 = 1;		//��ʱ��0��ʼ��ʱ
}

void Timer2Init(void)
{
    AUXR |= 0x04;
    T2L = 0xCB;                     //��ʼ����ʱֵ
    T2H = 0xD4;
    AUXR |= 0x10;                   //��ʱ��2��ʼ��ʱ
}


void sys_init(void)		   //ϵͳ��ʼ������
{
  P2M1 = 0x01;
  P2M0 = 0xC1; //11000001
  
  P3M1 = 0x00;
  P3M0 = 0x6B; //01101011

  P1M1 = 0x00;
  P1M0 = 0x04; //00000100

  
  P0 = 0xff;             //io�ڳ�ʼ��
  P1 = 0xff;
  P2 = 0xff;
  P3 = 0x9f;             //���������ʼ��Ϊ��ʹ�� 
  
  //Timer0Init();  // pwm
  //Timer2Init();  // 1ms���߼��ж�
  
  //pwm_init();

  //Timer1Init();  // �������巭ת
  
}

#endif


void Timer0Init(void)
{
    AUXR |= 0x80;                 	//��ʱ��0Ϊ1Tģʽ
//  AUXR &= 0x7f;                	//��ʱ��0Ϊ12Tģʽ

    TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
    TL0 = T1MS;                     //��ʼ����ʱֵ
    TH0 = T1MS >> 8;
    TR0 = 1;                        //��ʱ��0��ʼ��ʱ
    ET0 = 1;                        //ʹ�ܶ�ʱ��0�ж�
    EA = 1;
}

void sys_init(void)		   //ϵͳ��ʼ������
{
	P2M1 = 0x00;
	P2M0 = 0x20;

	P0 = 0xff;             //io�ڳ�ʼ��
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

	speed_mode = 1;  // ����ģʽ
}




///̤�峤��ʶ��

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
			if(p->us_DownCnt > KEY_JIT_DELAY)			///<����						
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
			p->us_finishFlag = KEY_LONG_DOWN_FLAG;   /* ��Ϊ�������� */   

			//p->us_logStep = 30;
		}
		
		break;

	case 30:

		p->us_DownCnt++;

		if (unKey.all & p->us_inMask)	 
		{
			p->us_UpCnt++;
			if(p->us_UpCnt > KEY_JIT_DELAY)   /* ���� */									
			{
				//if (p->us_DownCnt > KEY_LONG_DELAY)     
				if (p->us_finishFlag == KEY_LONG_DOWN_FLAG)
				{
					p->us_finishFlag = KEY_LONG_UP_FLAG;   /* ��Ϊ����̧�� */   
				}
				else
				{
					p->us_finishFlag = KEY_SHORT_UP_FLAG;  /* ��Ϊ�̰� */
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
			if(p->us_DownCnt > KEY_JIT_DELAY)			///<����						
			{
				p->us_finishFlag = KEY_LONG_DOWN_FLAG;   /* ��Ϊ�������� */     

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
			if(p->us_UpCnt > KEY_JIT_DELAY)   /* ���� */									
			{
				p->us_finishFlag = KEY_LONG_UP_FLAG;   /* ��Ϊ����̧�� */   

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
	// ��ֽ
	if (t_FzFwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO9;
		
		if (t_FzFwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_FzFwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_FzFwKey.us_finishFlag = 0;
			t_FzFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_FzFwKey.us_finishFlag = 0;
			t_FzFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}

	// ��ֽ
	if (t_FzBwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_RED;
		
		if (t_FzBwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_FzBwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_FzBwKey.us_finishFlag = 0;
			t_FzBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_FzBwKey.us_finishFlag = 0;
			t_FzBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}


	
	/* ��ֽ */
	if (t_PaperFwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO1;
		
		if (t_PaperFwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_PaperFwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_PaperFwKey.us_finishFlag = 0;
			t_PaperFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_PaperFwKey.us_finishFlag = 0;
			t_PaperFwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}

	/* ��ֽ */	
	if (t_PaperBwKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO2;
		
		if (t_PaperBwKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_PaperBwKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_PaperBwKey.us_finishFlag = 0;
			t_PaperBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_PaperBwKey.us_finishFlag = 0;
			t_PaperBwKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}

	/* ��ֽ */	
#if 0
	if (t_CutKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO3;
		
		if (t_CutKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_CutKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_CutKey.us_finishFlag = 0;
			t_CutKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_CutKey.us_finishFlag = 0;
			t_CutKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}
#endif

	/* ���� */	
	if (t_PaperDownKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO4;
		
		if (t_PaperDownKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_PaperDownKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_PaperDownKey.us_finishFlag = 0;
			t_PaperDownKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_PaperDownKey.us_finishFlag = 0;
			t_PaperDownKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}

	/* ���� */	
	if (t_RollKey.us_finishFlag != 0) 
	{
		gsKeyVal = KEY_IO5;
		
		if (t_RollKey.us_finishFlag == KEY_LONG_DOWN_FLAG)     /* ���� */
		{			
			gcKeyType = KEY_LONG_DOWN_FLAG;  

			if (usLongDownFlag == 0)
			{
				usLongDownFlag = 1;

				write_key_flag = 1;   /* �а�������Ҫд���������� */
			}
		}
		else if (t_RollKey.us_finishFlag == KEY_SHORT_UP_FLAG) /* �̰� */
		{
			gcKeyType = KEY_SHORT_UP_FLAG;   

			t_RollKey.us_finishFlag = 0;
			t_RollKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}
		else                                                       /* ����̧�� */
		{
			gcKeyType = KEY_LONG_UP_FLAG;   

			t_RollKey.us_finishFlag = 0;
			t_RollKey.us_logStep = 0;			

			usLongDownFlag = 0;   /* �峤����־ */

			write_key_flag = 1;   /* �а�������Ҫд���������� */			
		}	
	}
	
}

/*************��ʱ��0�ж�***************
����ԭ�ͣ�void tm0_isr(void) interrupt 1 using 1
��ڲ�������
���ڲ�������
��ע��
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

	/* Ӳ����ʼ�� */
	sys_init();

	/* ȫ�ֱ�����ʼ�� */
	GlobalInit();
		
	InitLEDDisplay();

	cnt_read_state = public_val.ms_timer;	
	
	//add by yq
	GpuSend("DR2\r\n");
	
	while(1)
	{	
		if (start_com_flag == 0)
		{
			if (public_val.ms_timer - cnt_read_state > 500)  /* �����ӳ�500ms�ſ�ʼͨ�� */
			//if (public_val.ms_timer - cnt_read_state > 1500)   //for test
			{
				cnt_read_state = public_val.ms_timer;
				cnt_read_param = public_val.ms_timer;

				/* ��ʼ�ֶ�ģʽ������� */
				start_key_detect();

				start_com_flag = 1;
			}
		}

		if (start_com_flag == 1)
		{		
			LEDDisplay();
			
			if(public_val.uart_rec_statu == 2)   /* ������ϣ���ʼ���� */
			{			
				rec_data_func();
				
				public_val.uart_rec_statu = 3;   /* �ý��մ������״̬ */
				public_val.uart_rec_count = 0;   /* ������ֽڼ��� */
			}
		  	

			send_data_func();

			judge_key();

		}
		
	}
}


