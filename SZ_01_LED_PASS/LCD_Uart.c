#include "sys.h"
#include "reg51.h"
#include "LCD_Uart.h"


typedef unsigned char BYTE;

#define NONE_PARITY     0       //��У��
#define ODD_PARITY      1       //��У��
#define EVEN_PARITY     2       //żУ��
#define MARK_PARITY     3       //���У��
#define SPACE_PARITY    4       //�հ�У��

bit busy2;  /* 1���ڷ��ͣ�0������� */

#define  T9_6KHZ  65536-(FOSC/BAUD2/4)		//����Ϊ1tģʽʱ,��Ҫ��12



void uart2_init()
{
	//uart2 ��ʼ������
	//  P_SW2 &= ~ S2_S;	//ѡ����p1.0,p1.1��
	
	//����2���ƼĴ���
	//S2CON
	//S2SM0 - S2SM2 S2REN S2TB8 S2RB8 S2TI S2RI
	//����, S2SM0Ϊ0,8λ�ɱ䲨����;Ϊ1,9λ�ɱ䲨����
	//S2SM2����ʽ1���ͨ�ſ���λ
	//����2������
	//S2BUF
	//����2���ж�����λ
	//ES2   (IE2.0)   IE2:����λѰַ
	
 	  AUXR |= 0x04;			//��ʱ��2Ϊ1Tģʽ
	
	  S2CON = 0x50;
	
/*	
#if (PARITYBIT2 == NONE_PARITY)
    S2CON = 0x50;                //8λ�ɱ䲨����,��ʽ1, ʹ�ܴ���2����
		#elif (PARITYBIT2 == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)   //MARK_PARITY:���У��
    S2CON = 0xda;                //9λ�ɱ䲨����,У��λ��ʼΪ1   ͨ�ŷ�ʽ3
#elif (PARITYBIT2 == SPACE_PARITY)  //�հ�У��
	  S2CON = 0xd2;                //9λ�ɱ䲨����,У��λ��ʼΪ0   ͨ�ŷ�ʽ3    (��ʽ0:8λ��λ�Ĵ�����ʽ, ��ʽ2:9λ,�����ʹ̶�)
#endif
*/
		T2L = T9_6KHZ;                     //��ʼ����ʱֵ
		T2H = T9_6KHZ >> 8;					////����2��T2��Ϊ�䲨���ʷ�����,��T2������ģʽ0,�䲨����=T2������ʳ���4.����2ֻ��ѡ��T2.
		
		AUXR |= 0x10;		//����T2����
		
		IE2 |= 0x1; 		//ʹ�ܴ���2�����ж�		 //��Ҫʹ��t2���ж�ET2 (ie2.2)
		
		EA = 1;
}

/*----------------------------
UART �жϷ������
-----------------------------*/

void Uart2RecIsr() interrupt 8 using 1
{
    if (S2CON & S2RI)
    {
		S2CON &= ~S2RI;	//���RIλ
        
		// Uart2_RecBuf = S2BUF; //��ȡ��������
        //public_val.uart_rec_statu = 1;
        //public_val.uart_rec_start_timer = public_val.ms_timer;
		
		//add 0407
		//����1���գ�ת��������2������2�յ��󣬷��ظ�����1��ʾ������OK.
		#ifdef	LCD_UART_DOWN
		SendData(S2BUF);
		#endif
		//add 0407
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;	                 //���TIλ
        busy2 = 0;               //��æ��־
    }
}

void GpuSend(char *s)
{
//��������ʱ��������
#ifndef	LCD_UART_DOWN
//#if 0
	while (*s)                  //����ַ���������־
    {
        Uasrt2SendData(*s++);         //���͵�ǰ�ַ�
    }
#endif	
}


void Uasrt2SendData(BYTE dat)
{
	//del 0407
#ifndef LCD_UART_DOWN
	while (busy2);               //    //����ҳ����Ϣʱ��while (busy2) ���������; //�ڱ�������ͨ��(����PC��ͨ��)ʱ,while (busy2) ��䲻������  //�ȴ�ǰ������ݷ������
#endif	
	//del 0407
    ACC = dat;                  //��ȡУ��λP (PSW.0)

    if (P)                      //����P������У��λ
    {
#if (PARITYBIT2 == ODD_PARITY)
        S2TB8 = 0;                //����У��λΪ0
#elif (PARITYBIT2 == EVEN_PARITY)
        S2TB8 = 1;                //����У��λΪ1
#endif
    }
    else
    {
#if (PARITYBIT2 == ODD_PARITY)
        S2TB8 = 1;                //����У��λΪ1
#elif (PARITYBIT2 == EVEN_PARITY)
        S2TB8 = 0;                //����У��λΪ0
#endif
    }
	
    busy2 = 1;
    S2BUF = ACC;                 //д���ݵ�UART���ݼĴ���
}
