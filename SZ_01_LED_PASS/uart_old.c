
#include "reg51.h"
#include "intrins.h"
#include "led_display.h"
#include "uart.h"
#include "global.h"
#include "save_cnt.h"
#include "sys.h"
#include <string.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 11059200L          //系统频率
#define BAUD 9600             //串口波特率


//#define BAUD2 115200

//#define BAUD2 	57600				//9600		

//#define BAUD2 		38400

//#define BAUD2  19200				


#define BAUD2 9600



#define  T9_6KHZ  65536-(FOSC/BAUD2/4)		//定义为1t模式时,不要除12


#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位

#define PARITYBIT2 NONE_PARITY   //定义校验位

//#define UART_REC_FRAME_DELAY 50
#define UART_REC_FRAME_DELAY 10

extern xdata public_stru public_val;
extern xdata char uart_send_buf[100];
extern code MENU_ITEM MenuItem[];
extern xdata MENU_CHANG now_menu;


extern xdata unsigned int  unUpVerNum;
extern xdata unsigned int  unDownVerNum;
extern xdata unsigned int  unIoStatus;

extern xdata unsigned long  TotalNum;
extern xdata unsigned long  unRow3Data;



//sfr AUXR  = 0x8e;               //辅助寄存器

//sbit P22 = P2^2;

bit busy;  /* 1正在发送，0发送完成 */

//void SendString(char *s);

xdata unsigned char Uart_RecBuf[100];
void * com_data = Uart_RecBuf;

//add
xdata unsigned char Uart2_RecBuf[10 + (MENU_ID_00000_MAX_ITEM << 2)];
void * com2_data = Uart2_RecBuf;

bit busy2;  /* 1正在发送，0发送完成 */
//add

char temp_char;

void SendData(BYTE dat);

//add
void Uasrt2SendData(BYTE dat);

//add

//void GpuSend2(char *s);


//使用t1的16位自动重装载作为串口1的波特率发生器
//pcon.7(SMOD)用串行通信方式1,2,3的波特率加倍,为1加倍，为0不加倍
void uart_init()
{
#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8位可变波特率,方式1,使能接收
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9位可变波特率,校验位初始为1   通信方式3
#elif (PARITYBIT == SPACE_PARITY)
	SCON = 0xd2;                //9位可变波特率,校验位初始为0   通信方式3    (方式0:8位移位寄存器方式, 方式2:9位,波特率固定)
#endif

    AUXR = 0x40;                //定时器1为1T模式
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值 //为1T模式时，这里不要除12  //串口1用T1做为其波特率发生器,且T1工作于模式0,其波特率=T1的溢出率除以4
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //定时器1开始启动
    ES = 1;                     //使能串口中断
    EA = 1;
	
	//设置串口1为高优先级中断    //根据相同优先级的内部查询顺次: usart1高于usart2, t1高于t2.
	//当使用c语言编程时,中断查询顺次号,就是中断号.
	PS = 1;		//add 0331
}


void uart2_init()
{
	//uart2 初始化定义
	//  P_SW2 &= ~ S2_S;	//选择在p1.0,p1.1口
	
	//串口2控制寄存器
	//S2CON
	//S2SM0 - S2SM2 S2REN S2TB8 S2RB8 S2TI S2RI
	//其中, S2SM0为0,8位可变波特率;为1,9位可变波特率
	//S2SM2允许方式1多机通信控制位
	//串口2缓冲区
	//S2BUF
	//串口2的中断允许位
	//ES2   (IE2.0)   IE2:不可位寻址
	
 	  AUXR |= 0x04;			//定时器2为1T模式
	
	  S2CON = 0x50;
	
/*	
#if (PARITYBIT2 == NONE_PARITY)
    S2CON = 0x50;                //8位可变波特率,方式1, 使能串口2接收
		#elif (PARITYBIT2 == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)   //MARK_PARITY:标记校验
    S2CON = 0xda;                //9位可变波特率,校验位初始为1   通信方式3
#elif (PARITYBIT2 == SPACE_PARITY)  //空白校验
	  S2CON = 0xd2;                //9位可变波特率,校验位初始为0   通信方式3    (方式0:8位移位寄存器方式, 方式2:9位,波特率固定)
#endif
*/
		T2L = T9_6KHZ;                     //初始化计时值
		T2H = T9_6KHZ >> 8;					////串口2用T2做为其波特率发生器,且T2工作于模式0,其波特率=T2的溢出率除以4.串口2只能选择T2.
		
		AUXR |= 0x10;		//启动T2运行
		
		IE2 |= 0x1; 		//使能串口2接收中断		 //不要使用t2的中断ET2 (ie2.2)
		
		EA = 1;
}
/*----------------------------
UART 中断服务程序
-----------------------------*/
//当使用c语言编程时,中断查询顺次号,就是中断号.
void UartRecIsr() interrupt 4 using 1
{

//add 0407
#ifdef	LCD_UART_DOWN
	if (RI)
    {
        RI = 0;                 //清除RI位
		
		//test 0407
		//使用串口1，定义LCD_UART_DOWN时，测试OK.
		SendData(SBUF);
		//test 0407		
		
		//在串口1接收，转发给串口2，定义LCD_UART_DOWN时，测试OK.
        //Uasrt2SendData(SBUF);		
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }	
#else	
//add 0407	
    if (RI)
    {
        RI = 0;                 //清除RI位
        Uart_RecBuf[public_val.uart_rec_count++] = SBUF;
        public_val.uart_rec_statu = 1;
        public_val.uart_rec_start_timer = public_val.ms_timer;		
		
		//test 0407
		//使用串口1，无定义LCD_UART_DOWN时，测试OK.
		//SendData(SBUF);
		//test 0407
		
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
#endif

}




/*----------------------------
UART 中断服务程序
-----------------------------*/

void Uart2RecIsr() interrupt 8 using 1
{
    if (S2CON & S2RI)
    {
		S2CON &= ~S2RI;	//清除RI位
        
		// Uart2_RecBuf = S2BUF; //获取串口数据
        //public_val.uart_rec_statu = 1;
        //public_val.uart_rec_start_timer = public_val.ms_timer;
		
		//add 0407
		//串口1接收，转发给串口2，串口2收到后，返回给串口1显示。测试OK.
		#ifdef	LCD_UART_DOWN
		SendData(S2BUF);
		#endif
		//add 0407
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;	                 //清除TI位
        busy2 = 0;               //清忙标志
    }
}






/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   void check_uart_rec(void)
功能：  接收数据帧判断
参数：  无
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void check_uart_rec(void)
{
	if(public_val.uart_rec_statu == 1)
	{
		/* 如果超过50ms未接收到数据，认为当前帧接收完，开始处理 */
		if(public_val.ms_timer - public_val.uart_rec_start_timer > UART_REC_FRAME_DELAY)
		{
			public_val.uart_rec_statu = 2;  /* 置正在处理状态 */
		}
	}
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(BYTE dat)
{
	//del 0407
#ifndef LCD_UART_DOWN
    while (busy);               //等待前面的数据发送完成    //下载页面信息时，while (busy) 语句需屏蔽;  //在本机运行通信(非与PC机通信)时,while (busy) 语句不能屏蔽
#endif
	//del 0407
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //设置校验位为0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   Send_UartBuf(char * buf)
功能：  数据发送 和校验
参数：  无
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void Send_UartBuf(char * buf)
{
	int i;
	static char frame_id = 1;
	char sum_byte = 0;
	
	//buf[0] = (buf[0] & 0x03) | (frame_id << 2);
	for(i = 0; i < buf[1] + 2; i++)
	{
		sum_byte += buf[i];
		SendData(buf[i]);
	}
	SendData(sum_byte);
	frame_id++;
	if(!frame_id) frame_id = 1;
}

/*----------------------------
发送字符串
----------------------------*/
/*void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}*/

/*
void GpuSend2(char *s)
{
//#ifndef	LCD_UART_DOWN
#if 1
	while (*s)                  //检测字符串结束标志
    {
        Uasrt2SendData(*s++);         //发送当前字符
    }
#endif	
}
*/


void GpuSend(char *s)
{
//进行下载时，需屏蔽
#ifndef	LCD_UART_DOWN
//#if 0
	while (*s)                  //检测字符串结束标志
    {
        Uasrt2SendData(*s);         //发送当前字符
		*s='\0';
		s++;
		while(busy2);
//		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}; //等待发送结束
	}
#endif	
}


void Uasrt2SendData(BYTE dat)
{
	//del 0407
#ifndef LCD_UART_DOWN
	while (busy2);               //    //下载页面信息时，while (busy2) 语句需屏蔽; //在本机运行通信(非与PC机通信)时,while (busy2) 语句不能屏蔽  //等待前面的数据发送完成
#endif	
	//del 0407
    ACC = dat;                  //获取校验位P (PSW.0)

    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT2 == ODD_PARITY)
        S2TB8 = 0;                //设置校验位为0
#elif (PARITYBIT2 == EVEN_PARITY)
        S2TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT2 == ODD_PARITY)
        S2TB8 = 1;                //设置校验位为1
#elif (PARITYBIT2 == EVEN_PARITY)
        S2TB8 = 0;                //设置校验位为0
#endif
    }
	
    busy2 = 1;
    S2BUF = ACC;                 //写数据到UART数据寄存器
}


#if 0
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   void frame_par_func(void)
功能：  参数字回帧处理
参数：  无
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void frame_par_func()
{
	if(now_menu.chang_par_flag == 1)
		{
			if( ((rt_updata_menu_stru *)com_data)->Statu == 0x00FF )
				{
					now_menu.chang_par_flag = 2;
				}
			else
				{
					now_menu.chang_par_flag = 3;
				}
		}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   void frame_info_func(void)
功能：  回帧信息字处理
参数：  无
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void frame_info_func(void)
{
	char menu_id,i;
	switch(((get_ver_stru *)com_data)->cmd_type)
		{
			case GET_VER_CMD:
				public_val.drive_type_com = ((get_ver_stru *)com_data)->ver;
				public_val.drive_index_com = ((get_ver_stru *)com_data)->drive_indx;
				if(public_val.drive_type_com != public_val.drive_type)
					{
						public_val.com_log_step = COMD_LOG_STEP_GET_CFG;
					}
				else
					{
						public_val.com_log_step = COMD_LOG_STEP_DOWN;
					}
				public_val.send_frame_idx = 0;
			break;
			case SET_VER_IDX_CMD:
				public_val.drive_indx = ((set_ver_idx_stru *)com_data)->ver_idx;
				public_val.drive_type = 0; //清除版本号,下次上电重新获取配置
				func_ver_update();
			break;
			case GET_MENU_CFG_INFO:
				menu_id = ((rec_menu_cfg_stru *)com_data)->menu_id;
				*((char*)(MenuItem[menu_id].com_max_item)) = ((rec_menu_cfg_stru *)com_data)->menu_count;
				for(i = 0; i < *(MenuItem[menu_id].com_max_item); i++)
					{
						MenuItem[menu_id].m_pcfg[i].m_max = 
							((rec_menu_cfg_stru *)com_data)->val[i].menu_max;
						MenuItem[menu_id].m_pcfg[i].m_min = 
							((rec_menu_cfg_stru *)com_data)->val[i].menu_min;
						MenuItem[menu_id].m_pcfg[i].ext_property = 
							((rec_menu_cfg_stru *)com_data)->val[i].ext_property;
					}
				func_cfg_update(menu_id);
				if(menu_id == MENU_NUM - 1) //完成菜单配置数据传输,保存版本及版本序号
					{
						//debug
						public_val.drive_type = public_val.drive_type_com;
						
						//add 0324 
						unDownVerNum = public_val.drive_type;
						//0324 0324 20:34
						
						public_val.drive_indx = public_val.drive_index_com;
						func_ver_update();
					}
			break;
			case GET_DEFAUT_VAL:
				menu_id = ((rec_menu_cfg_stru *)com_data)->menu_id;
				for(i = 0; i < *(MenuItem[menu_id].com_max_item); i++)
					{
						*((int *)MenuItem[menu_id].m_pcfg[i].m_data_vale) = 
							((rec_defaut_stru *)com_data)->val[i];
					}
				//保存菜单数据值到EEPROM
				func_menu_update(menu_id);
			break;
			case GET_PAR_INFO:
				menu_id = ((rec_menu_cfg_stru *)com_data)->menu_id;
				for(i = 0; i < *(MenuItem[menu_id].com_max_item); i++)
					{
						*((int *)MenuItem[menu_id].m_pcfg[i].m_data_vale) = 
							((rec_defaut_stru *)com_data)->val[i];
					}
				//保存菜单数据值到EEPROM
				func_menu_update(menu_id);
			break;
			case GET_STATU_INFO:
				public_val.Err_Flag &= 0xf0;
				public_val.Err_Flag |= (((rec_statu_stru *)com_data)->err_flag) & 0x0f;
				public_val.Work_Statu = ((rec_statu_stru *)com_data)->statu_num;
				public_val.Work_Pause_Flag = ((rec_statu_stru *)com_data)->pass_flag;
				public_val.Other_Bit_Flag = ((rec_statu_stru *)com_data)->other_bit_flag;
				public_val.main_disp_val = ((rec_statu_stru *)com_data)->disp_count;
				public_val.io_read = ((rec_statu_stru *)com_data)->io_read;
				if(public_val.Work_Statu  == WORK_STATU_WAIT)
					{
						public_val.Init_Over = 0;
						public_val.com_log_step = COMD_LOG_STEP_GET_VER;
					}
				else if(public_val.Work_Statu  >= WORK_STATU_START)
					{
						public_val.Init_Over = 1;
					}
			break;
		}
}

#endif



//与send_data_func函数配合来看
void rec_data_func(void)
{
	char i;
	char sum_byte;
    char *pcTemp = 0;
	unsigned long uiCnt;

	static char ucSaveLearn1 = 0, ucSaveLearn2 = 0;

	/* 对主板反馈的其他帧作校验 */
	sum_byte = Uart_RecBuf[0];
	for(i = 1; i < public_val.uart_rec_count - 1; i++)
	{
		sum_byte += Uart_RecBuf[i];
	}

	/* 校验失败报错返回 */
	if(sum_byte != Uart_RecBuf[public_val.uart_rec_count - 1])
	{
		//public_val.Err_Flag = ERR_NUM_ZK_COM_ERR;		
		public_val.Err_Flag = LED_CHECK_ERROR;       /* 接收到校验错误帧后不发送后续帧 */

		send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */
		
		return; 		
	}


	/* 校验成功，对接收帧进行处理 */	
	switch(Uart_RecBuf[0])
	{
		/* 上行写参数帧 */
		case UP_WRITE_PARAM:

			/* 获取参数个数 */
			//Menu_Number[0] = Uart_RecBuf[1] >> 1;    /* 载荷长度除以2为参数个数 */
			Menu_Number[0] = (Uart_RecBuf[1] + Uart_RecBuf[2]) >> 1;  //所有参数个数
			Menu0_Number = Uart_RecBuf[1] >> 1;  					  //菜单0参数个数

			/* 提取下发的参数 */  //将下发的参数备份一份,并保存到public_val.menu_parm[0]菜单参数缓冲区
			pcTemp = (char *)&public_val.menu_parm[0];
			
			for (i = 0; i < (Menu_Number[0] << 1); i++)
			{
				//pcTemp[i] = Uart_RecBuf[2 + i];
				pcTemp[i] = Uart_RecBuf[3 + i];
			}
      
			//操作板下发设置参数,主控板收到后返回
			already_read_param = 1;  /* 置为已读到配置参数 */

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */
			
		break;

		/* 上行写状态帧 */
		//主控板返回版本号,io口传感器状态,错误标志,搓条根数
		case UP_WRITE_STATE:
			
			public_val.version 
				= ((unsigned int)Uart_RecBuf[3] << 8) | (unsigned int)Uart_RecBuf[2];

			//add 
			unDownVerNum = public_val.version;		//主版本号		//主控板版本号
		    if (unDownVerNum == 0)
				unDownVerNum = 12345;
			//add 0323
		
			
		  //主控板返回io口状态
			public_val.io_read
			 	= ((unsigned int)Uart_RecBuf[5] << 8) | (unsigned int)Uart_RecBuf[4];  //传感器状态(可以返回16bit对应16个传感器状态)
		  //add 0320
		  //public_val.io_read = 23456;
			unIoStatus = public_val.io_read;
			//if (unIoStatus == 0)
				//unIoStatus = 1;
			//add 0320
		
			//if ((Uart_RecBuf[7] != 0) || (Uart_RecBuf[6] != 0))
			//{
			public_val.Err_Flag 
				= ((unsigned int)Uart_RecBuf[7] << 8) | (unsigned int)Uart_RecBuf[6];
			//}

			//public_val.disp_cnt_upload
		//上行写搓条根数,在这里更新: public_val.cnt_after_powered  (开机后的搓条根数),
		//并将开机后的搓条根数与开机前的搓条根数相加，保存到eeprom. call save_xt_num().
			uiCnt
				= ((unsigned long)Uart_RecBuf[9] << 8) | (unsigned long)Uart_RecBuf[8]
				| ((unsigned long)Uart_RecBuf[11] << 24) | ((unsigned long)Uart_RecBuf[10] << 16);		//返回搓条根数
				
			
			//add
				TotalNum = uiCnt;				//其余，可直接使用结构public_val的成员赋值,如: public_val.io_read,  public_val.version
				//这里给unRow3Data赋值,有可能引起其他的查看数值变化
				//unRow3Data = uiCnt;
					//TotalNum = 1;
				//add 0324 20:33

			if (main_borad_finish_clear != 0)  /* 不需要清除筒数时才保存更新 */
			{
				if (uiCnt != public_val.cnt_after_powered)  /* 如果筒数有更新，则保存更新 */
				{
					public_val.cnt_after_powered = uiCnt;

					save_xt_num();
				}
			}
			
			

			if ((public_val.version == 11111) && (ucSaveLearn1 == 0))
			{
				public_val.menu_parm[PARAM_FZ_START_PWM_SCALE] = (int)(uiCnt & 0xFFFF);
				//public_val.menu_parm[PARAM_QZ_START_PWM_SCALE] = (int)((uiCnt >> 16) & 0xFFFF);
				
				func_menu_update(0);

				ucSaveLearn1 = 1;
			}

			if ((public_val.version == 22222) && (ucSaveLearn2 == 0))
			{
				public_val.menu_parm[PARAM_FZ_START_PWM_SCALE] = (int)(uiCnt & 0xFFFF);
				//public_val.menu_parm[PARAM_QZ_START_PWM_SCALE] = (int)((uiCnt >> 16) & 0xFFFF);
				
				func_menu_update(0);

				ucSaveLearn2 = 1;
			}
			
			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */
			
		break;

		/* 上行写参数范围帧 */    //主控板返回参数范围
		case UP_WRITE_PARAM_RANGE:

		//存放在菜单配置缓冲: 最大值，最小值单元
			for (i = 0; i < (Uart_RecBuf[1] >> 2); i++)  /* 参数个数为载荷长度除以4 */
			{
				MenuItem[MENU_ID_00000].m_pcfg[i].m_min 
					= ((short)Uart_RecBuf[2 + (i << 2)] << 8)    /* 先收低地址字节 */
						| (short)Uart_RecBuf[2 + (i << 2) + 1];	
				
				MenuItem[MENU_ID_00000].m_pcfg[i].m_max 
					= ((short)Uart_RecBuf[2 + (i << 2) + 2] << 8) 
						| (short)Uart_RecBuf[2 + (i << 2) + 3];				
			}

			//Menu_Number[0] = (Uart_RecBuf[1] >> 2);  

			already_read_param_range = 1;  /* 置为已读到配置参数范围 */	

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */	

		break;

		/* 上行写接收参数完成 */
		case UP_WRITE_REC_PARAM_DONE:

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;

		/* 上行写进入测试模式成功 */
		case UP_WRITE_TEST_MODE_DONE:

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;

		/* 上行写进入学习模式成功 */
		case UP_WRITE_LEARN_MODE_DONE:

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;

		/* 上行写切换速度模式成功 */
		case UP_WRITE_SPEED_MODE_DONE:

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;

		/* 上行写报警消息成功 */
		case UP_WRITE_WARNING_MSG_DONE:

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;


		/* 上行写清除已出筒数成功 */
		case UP_WRITE_CLEAR_CNT_DONE:

			main_borad_finish_clear = 1;      /* 主板已完成清除计数 */

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;		

		/* 上行写按键成功 */
		case UP_WRITE_KEY_DONE:

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */

		break;	


		/* 校验失败帧，报错 */
		case UP_WRITE_CHECK_FAIL:  
			
			//public_val.Err_Flag = ERR_NUM_ZK_COM_ERR;	
			public_val.Err_Flag = MAIN_CHECK_ERROR;		  /* 接收到校验错误帧后不发送后续帧 */	

			send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */
			
		break;

		default:
			
			break;
	}
	
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   send_data_func(void)
功能：  通讯处理过程 由主程序调用
参数：  无
返回值：无
说明: 版本不匹配: 获取菜单配置信息->获取默认参数->下发菜单参数
      版本匹配: 下发菜单参数
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void send_data_func(void)
{
	int i;

  char *pcTemp = 0;
	unsigned char sum_byte;
	

	/* 每1s读取一次主板状态 */   //即io口的传感器状态
#if 1
	if(public_val.ms_timer - cnt_read_state > READ_STATE_PERIOD)
	{	
		if (write_param_flag == 0)
		{
			if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送读状态帧 */
			{
				send_frame_type = DOWN_READ_STATE;
			}
		}

		cnt_read_state = public_val.ms_timer;
	}
#endif

	/* 已读到主板状态，且未读配置参数，则读取一次配置参数 */
#if 1	
	if ((public_val.version != 0) && (already_read_param == 0))
	{
		//if (public_val.ms_timer - cnt_read_param > 100)
		{		
			if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送读参数帧 */
			{

				//send_frame_type = DOWN_READ_PARAM;				

				// 写默认参数
				Menu_Number[0] = MACHINE_PARAM_NUM;     //机器参数个数  22, Menu_Number[]最大存储30个
				Menu0_Number = USER_PARAM_NUM;					//用户参数个数  2,  Menu0_Number  默认10. Menu1_Number 默认20

				// 最后一个参数不是0x55aa则写一次默认参数
				if (public_val.menu_parm[MENU_ID_00000_MAX_ITEM - 1] != 0x55aa)
				{
					public_val.menu_parm[0] = 310;   //纸长					
					public_val.menu_parm[1] = 300;   /* 设定筒数 */
					
					public_val.menu_parm[2] = 2;     /* 1、报警持续时间:s */
					public_val.menu_parm[3] = 15;    /* 2、堵筒检测时间1, 时基感应到堵筒检测1感应报警的延迟时间:0.1s */
					public_val.menu_parm[4] = 15;    /* 3、堵筒检测时间2, 时基感应到堵筒检测2感应报警的延迟时间:0.1s */
					public_val.menu_parm[5] = 0;     /* 4、步进电机方向 */
					public_val.menu_parm[6] = 0;     // 5、送纸速度增益
					public_val.menu_parm[7] = 320;   /* 6、步进电机脉冲当量:0.1个/mm (4000*2 /(2*3.14*40)) 减速比2 */
					public_val.menu_parm[8] = 89;    /* 7、发纸电机链轮减速比 */
					public_val.menu_parm[9] = 400;   /* 8、发纸压轮半径:0.1mm */
					public_val.menu_parm[10] = 20;   /* 9、发纸电机PID调速的比例系数*100 */
				#ifdef SZ_01    // SZ_01
					public_val.menu_parm[11] = 85;   /* 10、发纸电机启动PWM占空比 */
					public_val.menu_parm[12] = 85;   /* 11、切纸电机启动PWM占空比 */
					public_val.menu_parm[13] = 70;   /* 12、切纸电机目标PWM占空比 */
					public_val.menu_parm[14] = 10;   /* 13、发纸最高速度 */
					public_val.menu_parm[15] = 1;    /* 14、发纸最低速度 */
					public_val.menu_parm[16] = 40;   /* 15、送纸角度 */
					public_val.menu_parm[17] = 3;    /* 16、切刀控制模式 3:七段调速 */
					public_val.menu_parm[18] = 84;   /* 17、切纸停止速度 */
					public_val.menu_parm[19] = 1;    /* 18、备用 */
					public_val.menu_parm[20] = 1;    /* 19、备用 */
					public_val.menu_parm[21] = 60;   /* 20、调速最高速度 */
				#endif
					
				#ifdef SZ_02            // SZ_02
					public_val.menu_parm[11] = 84;   /* 10、发纸电机启动PWM占空比 */				
					public_val.menu_parm[12] = 2;    /* 11、切纸电机链轮减速比 */
					public_val.menu_parm[13] = 120;  /* 12、切纸电机目标速度:0.1khz */
					public_val.menu_parm[14] = 10;   /* 13、发纸最高速度 */
					public_val.menu_parm[15] = 1;    /* 14、发纸最低速度 */
					public_val.menu_parm[16] = 73;   /* 15、送纸角度:0.1k脉冲 */
					public_val.menu_parm[17] = 1;    /* 16、备用 */
					public_val.menu_parm[18] = 1;    /* 17、备用 */
					public_val.menu_parm[19] = 1;    /* 18、备用 */
					public_val.menu_parm[20] = 1;    /* 19、备用 */					
					public_val.menu_parm[21] = 1;    /* 20、备用 */					
				#endif

				#ifdef SZ_03
					public_val.menu_parm[3] = 30;    /* 2、堵筒检测时间1, 时基感应到堵筒检测1感应报警的延迟时间:0.1s */
					public_val.menu_parm[4] = 30;    /* 3、堵筒检测时间2, 时基感应到堵筒检测2感应报警的延迟时间:0.1s */
					public_val.menu_parm[5] = 1;     /* 4、步进电机方向 */
					public_val.menu_parm[6] = 20;    // 5、送纸速度增益					
					public_val.menu_parm[7] = 490;   /* 6、 送纸步进电机脉冲当量:0.1个/mm (4000*(36/12) /(2*3.14*39)) 减速比36/12 */
					public_val.menu_parm[8] = 1;     /* 7、切纸电机链轮减速比 */
					public_val.menu_parm[9] = 150;   /* 8、切纸电机目标速度:0.1khz */
					public_val.menu_parm[10] = 37;   /* 9、送纸角度:0.1k脉冲 */
					public_val.menu_parm[11] = 1;    /* 10、备用 */
					public_val.menu_parm[12] = 1;    /* 11、备用 */
					public_val.menu_parm[13] = 1;    /* 12、备用 */
					public_val.menu_parm[14] = 1;    /* 13、备用 */	
					public_val.menu_parm[15] = 1;    /* 14、备用 */
					public_val.menu_parm[16] = 1;    /* 15、备用 */	
					public_val.menu_parm[17] = 1;    /* 16、备用 */
					public_val.menu_parm[18] = 1;    /* 17、备用 */
					public_val.menu_parm[19] = 1;    /* 18、备用 */
					public_val.menu_parm[20] = 1;    /* 19、备用 */	
					public_val.menu_parm[21] = 1;    /* 20、备用 */						//public_val.menu_parm:默认参数
				#endif


					public_val.menu_parm[MENU_ID_00000_MAX_ITEM - 1] = 0x55aa;					

					func_menu_update(0);            ////更新默认参数
					
				}

				write_param_flag = 1;    // 更新下位机参数

				already_read_param = 1;  /* 置为已读到配置参数 */

				send_frame_type = UP_REPLY_DONE;  /* 可以发送下一帧 */				
				
			}

			//cnt_read_param = public_val.ms_timer;
		}
	}
#endif	

	/* 已读到配置参数，且未读配置参数范围，则读取一次配置参数范围 */
	// 主板已收到操作面板下发的设置参数,但未收到读参数配置范围,置发送帧类型send_frame_type为读参数范围
#if 1	
	if ((already_read_param == 1) && (already_read_param_range == 0))
	{
		//if (public_val.ms_timer - cnt_read_param > 100)
		{		
			if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送读参数帧 */
			{
				send_frame_type = DOWN_READ_PARAM_RANGE;
			}

			//cnt_read_param = public_val.ms_timer;
		}
	}
#endif	


	/* 当前需要保存配置参数到主板 */
#if 1
	if (write_param_flag != 0)   //进行参数修改时,每修改一个参数确定,更新下位机参数  都与主控板进行通信更新参数
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写参数帧 */
		{
			send_frame_type = DOWN_WRITE_PARAM;
		}
	}
#endif


	/* 当前需要写学习模式到主板 */
#if 1
	if (write_learn_mode_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写测试模式帧 */
		{
			send_frame_type = DOWN_WRITE_LEARN_MODE;
		}
	}
#endif

#if 1
	if (write_warning_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写测试模式帧 */
		{
			send_frame_type = DOWN_WRITE_WARNING_MSG;
		}
	}
#endif



	/* 当前需要写测试模式到主板 */
#if 1
	if (write_test_mode_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写测试模式帧 */
		{
			send_frame_type = DOWN_WRITE_TEST_MODE;
		}
	}
#endif

	/* 当前需要写速度模式到主板 */
#if 1
	if (write_speed_mode_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写测试模式帧 */
		{
			send_frame_type = DOWN_WRITE_SPEED_MODE;
		}
	}
#endif


	/* 当前需要写清除已出筒数主板 */
#if 1
	if (write_clear_cnt_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写测试模式帧 */
		{
			send_frame_type = DOWN_WRITE_CLEAR_CNT;
		}
	}
#endif

	/* 当前需要写按键到主板 */
#if 1
	if (write_key_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* 上一帧已收到回复，则发送写按键帧 */
		{
			send_frame_type = DOWN_WRITE_KEY;
		}
	}	
#endif

	/* 发送组帧 */
	switch (send_frame_type)
	{
		case DOWN_READ_PARAM:  /* 下行读参数帧 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));
			
			uart_send_buf[0] = DOWN_READ_PARAM;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 1;

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_READ_PARAM_SENT;   /* 置为读参数帧已发送 */
			
			break;

		//下发读状态,主控板返回相应状态
		case DOWN_READ_STATE:  /* 下行读状态 */   //每间隔1s读取主板状态
			memset(uart_send_buf, 0, sizeof(uart_send_buf));
			
			uart_send_buf[0] = DOWN_READ_STATE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 2;

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_READ_STATE_SENT;   /* 置为读状态帧已发送 */
			
			break;

		case DOWN_READ_PARAM_RANGE:  /* 下行读参数范围 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));

			uart_send_buf[0] = DOWN_READ_PARAM_RANGE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 4;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_READ_PARAM_RANGE_SENT;   /* 置为读参数范围帧已发送 */

			
			break;


		//向主控板发送已配置的参数
		case DOWN_WRITE_PARAM:  /* 下行写配置参数 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_PARAM;
			uart_send_buf[1] = Menu_Number[0] << 1;	

			pcTemp = (char *)&public_val.menu_parm[0];   //将所有参数进行一次更新

			sum_byte = uart_send_buf[0] + uart_send_buf[1];			
			
			for (i = 0; i < (Menu_Number[0] << 1); i++)
			{
				uart_send_buf[2 + i] = pcTemp[i];

				/* 计算校验和 */
				sum_byte += pcTemp[i];				
			}
			
			//sum_byte &= 0xff;
			
			uart_send_buf[2 + (Menu_Number[0] << 1)] = sum_byte;
	
			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_PARAM_SENT;   /* 置为写参数帧已发送 */


			write_param_flag = 0;  /* 已完成一次写配置参数，置为不写状态 */	
			
			break;

		case DOWN_WRITE_TEST_MODE:  /* 下行写进入测试模式 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_TEST_MODE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = test_mode;
			uart_send_buf[3] = 5 + test_mode;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_TEST_MODE_SENT;   /* 置为写测试模式帧已发送 */			

			write_test_mode_flag = 0;  /* 已完成一次写测试模式，置为不写状态 */	

			break;

		case DOWN_WRITE_LEARN_MODE:  /* 下行写进入学习模式 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_LEARN_MODE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;  // no use
			uart_send_buf[3] = 8;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_LEARN_MODE_SENT;   /* 置为写学习模式帧已发送 */			

			write_learn_mode_flag = 0;  /* 已完成一次写学习模式，置为不写状态 */	

			break;

		case DOWN_WRITE_SPEED_MODE:  /* 下行写切换速度模式 */

			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_SPEED_MODE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = speed_mode;  
			uart_send_buf[3] = 9 + speed_mode;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_SPEED_MODE_SENT;   /* 置为写速度模式帧已发送 */			

			write_speed_mode_flag = 0;  /* 已完成一次写速度模式，置为不写状态 */	


			break;

		case DOWN_WRITE_WARNING_MSG:

			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_WARNING_MSG;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;   // no use
			uart_send_buf[3] = 10;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_WARNING_MSG_SENT;   

			write_warning_flag = 0;  

			break;

		case DOWN_WRITE_CLEAR_CNT:  /* 下行写清除已出筒数 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_CLEAR_CNT;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 6;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_CLEAR_CNT_SENT;   			

			write_clear_cnt_flag = 0;  

			break;

		case DOWN_WRITE_KEY:  /* 下行写按键操作 */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_KEY;
			uart_send_buf[1] = 3;			
			uart_send_buf[2] = (char)gsKeyVal;
			uart_send_buf[3] = (char)(gsKeyVal >> 8);
			uart_send_buf[4] = gcKeyType;			
			uart_send_buf[5] = 9 + uart_send_buf[2] + uart_send_buf[3] 
				+ uart_send_buf[4];

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_KEY_SENT;   			

			write_key_flag = 0;  

			break;

		default:
			break;
	}


	//if (send_frame_type != 0x7f)     /* 有帧要发送 */
	//{		
	//	Send_UartBuf(uart_send_buf);  //deleted for test

		//send_frame_type = 0x7f;      /* 发送完成，置为无效帧，无帧发送 */
	//}
	
}



//串口屏操作
//第一次使用串口助手(Commix 1.4)(使用字符串方式)(by enter 是否打钩没什么区别)发送如下命令成功:
//CLS(0);\x0d\x0a
//第二次发送成功
//DS48(0,0,'显示字符串',1);\x0d\x0a

//测试程序
//改写程序,初始化usart2后,发送如上命令:
//CLS(0);\x0d\x0a
//DS48(0,0,'显示字符串',1);\x0d\x0a
//看效果如何

//GpuSend("PL(0,80,320,80,11);\r\n");

//sprintf(buf,"ICON(%d,165,1,11,1,10);DS16(%d,224,' --- ',4);BOXF(%d,31,%d,79,0);BOXF(%d,81,%d,129,0);",54*i,54*i+4,x1,x2,x1,x2);
//				GpuSend(buf);

//GpuSend("CLS(13);BOX(0,0,219,175,15);BOX(1,1,218,174,0);BOXF(2,2,217,17,3);PL(2,18,218,18,0);\r\n");
	

