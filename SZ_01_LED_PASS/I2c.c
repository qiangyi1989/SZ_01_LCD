/**
 * Copyright (c) 2012, ��ɳ�Ϲ���ӿƼ����޹�˾
 * All rights reserved.
 *
 * FILE:   I2c.c
 * ģ��I2C����
 *
 * @file i2c.h
 * ͷ�ļ�.
 *
 * @author JWD
 * @version 1.0     = return 12121
 * @date 2012-12-26
 *
 */

#include "I2c.h"
#include "reg51.h"
#include<intrins.h>
void I2c_Start(void);
void I2c_Stop(void);
void I2c_Gpio_In(void);
void I2c_Gpio_Out(void);

sbit P10 = P1^0;
sbit P11 = P1^1;
sbit P14 = P1^4;
sbit P15 = P1^5;


#define I2C_DELAY 0

#define I2C_SDA_GPIO P11
#define I2C_SCK_GPIO P10
#define I2C_SDA_DIR_GPIO P14
#define I2C_SCK_DIR_GPIO P15

xdata unsigned int disp_buf[5] = {0xFF,0xFF,0xFF,0xFF,0xFF};

void DELAY_US(long us_val)
{
	long i;
	for(i = 0; i < us_val; i++)
		{
			_nop_();
		}
}

void I2c_Start(void)   
{  
//  DELAY_US(I2C_DELAY);   
  I2C_SCK_GPIO = 1; // SCK     
  I2C_SCK_GPIO = 1; // SCK     
  I2C_SDA_GPIO = 1; // SDA   
  I2C_SDA_GPIO = 1; // SDA   
  DELAY_US(I2C_DELAY);   
  I2C_SDA_GPIO = 0; // SDA   
  I2C_SDA_GPIO = 0; // SDA    
}   
   
void I2c_Stop(void)   
{   
/*
  clk = 0; 
  Delay_us(2); 
  dio = 0; 
  Delay_us(2); 
  clk = 1; 
  Delay_us(2); 
  dio = 1; 
*/
//  DELAY_US(I2C_DELAY);   
  I2C_SCK_GPIO = 0; // SCK   
  I2C_SCK_GPIO = 0; // SCK   
  DELAY_US(I2C_DELAY);   
  I2C_SDA_GPIO = 0; // SDA   
  I2C_SDA_GPIO = 0; // SDA   
  DELAY_US(I2C_DELAY);   
  I2C_SCK_GPIO = 1; // SCK   
  I2C_SCK_GPIO = 1; // SCK   
  DELAY_US(I2C_DELAY);   
  I2C_SDA_GPIO = 1; // SDA   
  I2C_SDA_GPIO = 1; // SDA   
}   
   
// Ӧ���־   
void I2c_Ack(void)   
{
/*
clk = 0;        
Delay_us(5);       //�ڵڰ˸�ʱ���½���֮����ʱ 5us����ʼ�ж� ACK �ź� 
while(dio); 
                  clk = 1; 
Delay_us(2); 
                  clk=0; 
*/
//  DELAY_US(I2C_DELAY);   
  I2C_SCK_GPIO = 0; // SCK   
  I2C_SCK_GPIO = 0; // SCK  
  DELAY_US(I2C_DELAY);   
//  while(I2C_SDA_GPIO);
  I2C_SCK_GPIO = 1; // SCK   
  I2C_SCK_GPIO = 1; // SCK  
  DELAY_US(I2C_DELAY);   
  I2C_SCK_GPIO = 0; // SCK   
  I2C_SCK_GPIO = 0; // SCK  
  
}   
   
   
void I2c_Send_Byte(unsigned int oneByte)   
{   
unsigned char i; 
    for(i=0;i<8;i++) 
    {      
		DELAY_US(I2C_DELAY); 
    	I2C_SCK_GPIO = 0; 
    	I2C_SCK_GPIO = 0;
		DELAY_US(I2C_DELAY); 
		if(oneByte&0x01)             //��λ��ǰ 
		{ 
		I2C_SDA_GPIO = 1; 
		I2C_SDA_GPIO = 1; 
		} 
		else 
		{ 
		I2C_SDA_GPIO = 0; 
		I2C_SDA_GPIO = 0; 
		} 
		DELAY_US(I2C_DELAY); 
		oneByte=oneByte>>1; 
		I2C_SCK_GPIO = 1; 
		I2C_SCK_GPIO = 1; 
		DELAY_US(I2C_DELAY); 
    }   
}   
   
  
void TM1637_Write(void)   
{   
	unsigned char i;   
	I2c_Gpio_Out(); 
	
	I2c_Start(); 
	I2c_Send_Byte(0x40); // 40H ��ַ�Զ��� 1 ģʽ,44H �̶���ַģʽ,�������
	I2c_Gpio_In(); 
	I2c_Ack(); 
	I2c_Gpio_Out(); 
	I2c_Stop(); 
	I2c_Start(); 
	I2c_Send_Byte(0xc0); //�����׵�ַ�� 
	I2c_Gpio_In(); 
	I2c_Ack(); 
	I2c_Gpio_Out(); 
	for(i=0;i<5;i++) //��ַ�Լӣ�����ÿ�ζ�д��ַ 
	{ 
	  I2c_Send_Byte(disp_buf[i]); // | 0x80 ������  ��������ҪС������������ȷ����
      I2c_Gpio_In(); 
      I2c_Ack(); 
      I2c_Gpio_Out(); 
	} 
    I2c_Stop(); 
    I2c_Start(); 
    I2c_Send_Byte(0x8C); //����ʾ  ��������� 
	I2c_Gpio_In(); 
	I2c_Ack(); 
	I2c_Gpio_Out(); 
    I2c_Stop(); 
}   

#if 0
   
unsigned char TM1637_Read(void)   
{    
//unsigned char rekey,rkey,i; 
unsigned char rekey,i; 

	I2c_Start(); 
	I2c_Send_Byte(0x42);                                          //���������� 
	I2c_Gpio_In(); 
	I2c_Ack(); 
	I2c_Gpio_Out(); 
	I2C_SDA_GPIO = 1; 
	I2C_SDA_GPIO = 1; 
	
	I2c_Gpio_In(); 
	  for(i=0;i<8;i++)                                         //�ӵ�λ��ʼ�� 
	  {      
		I2C_SCK_GPIO = 0; 
		I2C_SCK_GPIO = 0; 
		rekey=rekey>>1; 
		DELAY_US(I2C_DELAY);
		I2C_SCK_GPIO = 1; 
		I2C_SCK_GPIO = 1; 
		if(I2C_SDA_GPIO) 
		{ 
		    rekey=rekey|0x80; 
		} 
		else 
		{ 
		    rekey=rekey|0x00; 
		} 
		DELAY_US(I2C_DELAY); 
	   }     
	I2c_Gpio_In(); 
	I2c_Ack(); 
	I2c_Gpio_Out(); 
	I2c_Stop(); 
	return (rekey); 	              
}   

#endif
   
// I2C SDA input   
void I2c_Gpio_In(void)   
{   
    I2C_SDA_DIR_GPIO = 0;
    I2C_SCK_DIR_GPIO = 1;
}   
   
// I2C SDA output   
void I2c_Gpio_Out(void)   
{   
    I2C_SDA_DIR_GPIO = 1;
    I2C_SCK_DIR_GPIO = 1;
} 
