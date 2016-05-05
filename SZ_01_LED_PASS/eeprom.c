/*#include <reg51.h>

#include <intrins.h>

#define uint unsigned int
#define uchar unsigned char
sfr IAP_DATA   = 0xC2;         //IAPÊý¾Ý¼Ä´æÆ÷
sfr IAP_ADDRH  = 0xC3;		   //IAPµØÖ·¼Ä´æÆ÷¸ßÎ»
sfr IAP_ADDRL  = 0xC4;		   //IAPµØÖ·¼Ä´æÆ÷µÍÎ»
sfr IAP_CMD    = 0xC5;		   //IAPÃüÁî¼Ä´æÆ÷
sfr IAP_TRIG   = 0xC6;		   //IAPÃüÁî´¥·¢¼Ä´æÆ÷
sfr IAP_CONTR  = 0xC7;		   //IAP¿ØÖÆ¼Ä´æÆ÷				  

#define CMD_IDLE  0     // ¿ÕÏÐÄ£Ê½
#define CMD_READ  1     // IAP×Ö½Ú¶ÁÃüÁî
#define CMD_PROGRAM  2     //IAP×Ö½Ú±à³ÌÃüÁî
#define CMD_ERASE  3     //	IAPÉÈÇø²Á³ýÃüÁî
#define ENABLE_IAP   0X82	   */
#include"eeprom.h"
#include"led_display.h"
extern xdata public_stru public_val;
extern char eeprom_en;
#define EEPROM_EN_WRITE
/**************¹Ø±ÕIAP********************
**************************************/
void iapidle()
{
	IAP_CONTR  =0;
	IAP_CMD    =0;
	IAP_TRIG   =0;
	IAP_ADDRH  =0X80;
	IAP_ADDRL  =0;
}

/*****************¶ÁIAPº¯Êý**************************
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

/*****************Ð´IAPº¯Êý**************************
ÓÐÐ§EEPROMµØÖ·´Ó0xf000-0xf3ff
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

/*****************IAPÒ³²Á³ýº¯Êý**************************
ÓÐÐ§EEPROMµØÖ·´Ó0x0000-0xf3ff
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



