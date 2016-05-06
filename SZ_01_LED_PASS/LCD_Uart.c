#include "sys.h"
#include "reg51.h"
#include "LCD_Uart.h"


typedef unsigned char BYTE;

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

bit busy2;  /* 1正在发送，0发送完成 */

#define  T9_6KHZ  65536-(FOSC/BAUD2/4)		//定义为1t模式时,不要除12



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

void GpuSend(char *s)
{
//进行下载时，需屏蔽
#ifndef	LCD_UART_DOWN
//#if 0
	while (*s)                  //检测字符串结束标志
    {
        Uasrt2SendData(*s++);         //发送当前字符
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
