/*#include <reg51.h>

#include <intrins.h>

#define uint unsigned int
#define uchar unsigned char
sfr IAP_DATA   = 0xC2;         //IAP���ݼĴ���
sfr IAP_ADDRH  = 0xC3;		   //IAP��ַ�Ĵ�����λ
sfr IAP_ADDRL  = 0xC4;		   //IAP��ַ�Ĵ�����λ
sfr IAP_CMD    = 0xC5;		   //IAP����Ĵ���
sfr IAP_TRIG   = 0xC6;		   //IAP������Ĵ���
sfr IAP_CONTR  = 0xC7;		   //IAP���ƼĴ���				  

#define CMD_IDLE  0     // ����ģʽ
#define CMD_READ  1     // IAP�ֽڶ�����
#define CMD_PROGRAM  2     //IAP�ֽڱ������
#define CMD_ERASE  3     //	IAP������������
#define ENABLE_IAP   0X82	   */
#include"eeprom.h"
#include"led_display.h"
extern xdata public_stru public_val;
extern char eeprom_en;
#define EEPROM_EN_WRITE
/**************�ر�IAP********************
**************************************/
void iapidle()
{
	IAP_CONTR  =0;
	IAP_CMD    =0;
	IAP_TRIG   =0;
	IAP_ADDRH  =0X80;
	IAP_ADDRL  =0;
}

/*****************��IAP����**************************
***********************************************/
uchar  IapReadByte (uint addr)
{
	uchar dat;
	EA = 0;
	IAP_CONTR =ENABLE_IAP;
	IAP_CMD   = CMD_READ;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_TRIG  =0x5a;
	IAP_TRIG  =0xa5;
	_nop_();
	_nop_();
	dat = IAP_DATA;
	iapidle();
	EA = 1;
	return dat;
}

/*****************дIAP����**************************
��ЧEEPROM��ַ��0xf000-0xf3ff
***********************************************/
void  IapProgramByte (uint addr,uchar dat)
{
#ifdef EEPROM_EN_WRITE
	if( (eeprom_en & 0xff) == 0xaa)
		{
			EA = 0;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_CONTR = ENABLE_IAP;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_CMD   = CMD_PROGRAM;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_ADDRL = addr;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_ADDRH = addr >> 8;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_DATA  = dat;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_TRIG  =0x5a;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_TRIG  =0xa5;
			_nop_();  	
			iapidle();
			EA = 1;
		}
#endif	
}

/*****************IAPҳ��������**************************
��ЧEEPROM��ַ��0x0000-0xf3ff
***********************************************/
void IapEraseSector(uint addr)
{
#ifdef EEPROM_EN_WRITE
	if( (eeprom_en & 0xff) == 0xaa)
		{
			EA = 0;
			if( (eeprom_en & 0xff) == 0xaa)
			  	IAP_CONTR =ENABLE_IAP;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_CMD   = CMD_ERASE;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_ADDRL = addr;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_ADDRH = addr >> 8;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_TRIG  =0x5a;
			if( (eeprom_en & 0xff) == 0xaa)
				IAP_TRIG  =0xa5;
			_nop_();  	
			iapidle();
			EA = 1;
		}
#endif	
}



