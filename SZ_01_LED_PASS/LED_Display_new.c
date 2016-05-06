/**
 * Copyright (c) 2012, ³¤É³êÏ¹âµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾
 * All rights reserved.
 *
 * FILE:   LED_Display.c
 * LEDÏÔÊ¾³ÌÐò
 *
 * @file LED_Display.h led_driver.h
 * Í·ÎÄ¼þ.
 *
 * @author JWD
 * @date 2012-12-21
 *
 */
#include "eeprom.h"
#include "LED_Display.h"
#include "I2c.h"
#include "reg51.h"
#include "save_cnt.h"
#include "global.h"
#include "sys.h"

#include "uart.h"


#include <stdio.h>


#ifdef DISP_C

#define BELL P25

#define MAIN_ADJ_DELAY 2000

//chang 0405 11:00
//#define KEY_DOU_DELY 50		//old
#define KEY_DOU_DELY 40
//chang 0405 11:00

#define KEY_LONG_DELY 100
#define BELL_ON_DELY 50
#define LONG_KEY_STEP_SWITCH 50
#define DISP_ERR_TIMER 1000 //ÑÓÊ±Êý¾ÝÐÞ¸ÄÊ§°ÜÊ±¼ä ºÍ ´íÎóÏÔÊ¾Ê±¼ä

//#define NO_KEY_RETURN_MAIN 15000 //00000²Ëµ¥ÎÞ°´¼ü·µ»ØÖ÷Ò³ÃæÑÓÊ±
#define NO_KEY_RETURN_MAIN 30000 //00000²Ëµ¥ÎÞ°´¼ü·µ»ØÖ÷Ò³ÃæÑÓÊ±

#define TEST_MODE_CNT    40
//#define LEARN_MODE_CNT   20
#define LEARN_MODE_CNT   30

#define NULL		0


xdata unsigned char cZcKunShuFlag;


extern char eeprom_en;
extern xdata unsigned int disp_buf[5];

//add by 0322
extern xdata unsigned char work_mode;


//ÒÔÏÂ3ÏîÍ¨ÐÅ»ñÈ¡
extern xdata unsigned int  unUpVerNum;
extern xdata unsigned int  unDownVerNum;
extern xdata unsigned int  unIoStatus;

extern xdata unsigned int  unPaterLength;	//ÉèÖÃ»ñÈ¡
extern xdata unsigned int  unBunchNum;		//ÉèÖÃ»ñÈ¡
extern xdata unsigned long  TotalNum;		//int Í¨ÐÅ»ñÈ¡

//add  0403 0854
extern xdata unsigned long glTotalAllCnt;
//add  0403 0854


extern xdata unsigned char cCurDisplayItemNum;

//extern xdata unsigned char  *szMenuDispHz[3];

extern xdata unsigned char  *szDispHz1;
extern xdata unsigned char  *szDispHz2;
extern xdata unsigned char  *szDispHz3;

/*
extern xdata unsigned char  *szDispSM1;
extern xdata unsigned char  *szDispSM2;
extern xdata unsigned char  *szDispSM3;
extern xdata unsigned char  *szDispSM4;
*/


extern xdata unsigned int	unAlarmErrorSn;
extern xdata unsigned char	cAlarmFlag;

extern xdata unsigned int  unRow1Data;
extern xdata unsigned int  unRow2Data;
extern xdata unsigned long  unRow3Data;

extern xdata unsigned char clookVerFlag;
extern xdata unsigned char cMenuEditFlag;

extern xdata unsigned char cFirstDispIoStatusFlag;


extern xdata unsigned char cMainUiDispFlag;

//extern xdata unsigned char cFirstInputPassWordFlag;

extern xdata unsigned char cQzFlag;

extern xdata unsigned char cMainUiFlag;

extern xdata unsigned char cZcOptSelEnterRtnMainFlag;

extern xdata unsigned char *szShowFlashString;

extern xdata unsigned char *cTestKeyStringName;

extern xdata unsigned char cFirstEnterAlarmFlag;

extern xdata char cFlashAlarmFlag;

extern xdata char cKeyOptionFlash;

extern xdata unsigned char cMainNotFlashFlag;


extern xdata unsigned char lcd_disp_buf[50*2];		//lcd_disp_buf[50*2]

extern xdata unsigned char szTotalNum[9];


extern xdata  unsigned char cDisplayBitNum;

extern xdata unsigned char   cHandOptionFlag;


//extern xdata unsigned char cFirstDispPassword;

//extern xdata char*  lcd_disp_str_pBuf;
//extern xdata char   lcd_x_def;


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 |                                º¯ÊýÉùÃ÷                                    |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//void InitLEDDrivers(void);
void LEDShowFlashString(const char  * buff,char x);
//void LEDShowString(char  * buff,char x);
void LEDShowCharS(const char * buff,char num,char x);
void LEDClearChar(char x,char n);
void LEDClearCharS(char num,char x);
void LED_ShowNumber(char * buff,char x);
//void LED_ShowData(char * buff,char x,char nPoint);

static char DataToStr(void * const pData,char size,char nBit,char nPoint,char *string,char nFlag,char nType);
static long StrToData(char *string,char len,char nType);

void MenuEditLook(unsigned char cDisplayItemNum);

void DispHandOperation();


static xdata unsigned char  cMenuSn;


//extern void ResetDoorForTest();


//sbit P17 = P1^7;
//sbit P37 = P3^7;
sbit P25 = P2^5;
//sbit P21 = P2^1;
//sbit P22 = P2^2;

//#define RED P17
//#define YELLOW P54
//#define BLUE P55




//char xdata DIS_OPTIONING;											///<¡¾²»ÔÊÐí²åÒý¹¤×÷±êÖ¾¡£0£ºÔÊÐí²åÒý£»1£º²»ÔÊÐí²åÒý¡¿
char xdata DIS_ERR_FLGA = 0;

//ÄÚ²¿±äÁ¿¶¨Òå
//static xdata unsigned int  l_state;								//ÏÔÊ¾»­Ãæ×´Ì¬

//static xdata unsigned long  l_input_value;						//²ÎÊýÐÞ¸Ä»­ÃæÊäÈëÖµ
static xdata char  l_stop_poll;									//Í£Ö¹Ñ­»·ÏÔÊ¾

//static xdata char DisplayStateOld;								//¼ÇÒä»­Ãæ×´Ì¬

//static xdata char  DisplayMainID;									//Ö÷»­ÃæÐòºÅ
//static xdata char  DisplayMenuID;									//²Ëµ¥»­ÃæÐòºÅ

//static xdata char  DisplayMainTotal;								//Ö÷»­Ãæ×ÜÊý
//static xdata char  MenuTotals;									//Ö÷²Ëµ¥»­Ãæ×ÜÊý


//static xdata char  DisplayMenu0Total;								//²Ëµ¥»­Ãæ×ÜÊý
//static xdata char  DisplayMenu1Total;								//²Ëµ¥»­Ãæ×ÜÊý
static xdata char  DisplayMenuTotal;								//²Ëµ¥»­Ãæ×ÜÊý

//static xdata char  DisplayItemNum0;								//²Ëµ¥0ÏÔÊ¾ÏîÄ¿ÐòºÅ
//static xdata char  DisplayItemNum1;								//²Ëµ¥1ÏÔÊ¾ÏîÄ¿ÐòºÅ
static xdata char  DisplayItemNum;								//ÏÔÊ¾ÏîÄ¿ÐòºÅ
static xdata char  DisplayBitNum;									//ÏÔÊ¾Î»ºÅ

//static xdata char  MenuUpdateState;								//²Ëµ¥ÏÔÊ¾¸üÐÂ×´Ì¬
static xdata char  PasswordBuff[5];								//ÃÜÂëÊý¾Ý»º³åÇø
static xdata char  DisplayBuff[10];

int  DisplayState;											//»­Ãæ×´Ì¬


static xdata const MAIN_SHOW * pMain;								//Ö÷»­ÃæÊý¾ÝÖ¸Õë
static xdata const MENU_SHOW * pMenu;								//²Ëµ¥Ö¸Õë
static xdata const MENU_ITEM * pItem;								//²Ëµ¥ÏîÄ¿Ö¸Õë

//static xdata const MENU_SHOW * pCode;
static xdata const MENU_SHOW * pDataValue;

//static xdata const TYPE_CODE * disp_code;

//static xdata char  UpdateDataCount;
static xdata char  CycShowCount;
static xdata char  InputDataLen;
static xdata long  TempInputData;
static xdata char  DataType;


xdata public_stru public_val;
xdata MENU_CHANG now_menu;

xdata char cPauseBell = 0; 

xdata char test_mode = 0;   /* ²âÊÔÄ£Ê½±êÖ¾: 0Õý³£¹¤×÷Ä£Ê½£¬1²âÊÔÄ£Ê½ */


xdata char uart_send_buf[10 + (MENU_ID_00000_MAX_ITEM << 1)];




//²Ëµ¥ÅäÖÃ½çÃæ
//²ÎÊý×é¶¨Òå
//#define DEBUG_MENU_GS_VAL 30
//#define USE_MENU_NUM 16

static xdata MENU_SHOW Menu00000[MENU_ID_00000_MAX_ITEM];

xdata char Menu_Number[MENU_NUM] = {MENU_ID_00000_MAX_ITEM};   //»úÆ÷²ÎÊý¸öÊý  ×î´ó30
xdata char Menu0_Number = 10, Menu1_Number = 20;		//

extern xdata char qz_key_cnt;		//ÇÐÖ½¼ü¼ÆÊý




/* ²ÎÊýÈ¡Öµ·¶Î§ */
static xdata STRU_PARAM_RANGE Default_Param_Range[MENU_ID_00000_MAX_ITEM] = 
{		
	{0, 999},  /* ²½½øÆô¶¯ËÙ¶È */	
	{0, 999},  /* ²½½øÍ£Ö¹»¬ÐÐ¾àÀë */			
	{0, 999},    /* ²âÊÔÄ£Ê½ */	

};

static xdata int iDefaultParm[MENU_ID_00000_MAX_ITEM] = 
{
	100,   /* ²½½øÆô¶¯ËÙ¶È */
	100,  /* ²½½øÍ£Ö¹»¬ÐÐ¾àÀë£¬µ¥Î»Âö³å */	
	1,    /* ²âÊÔÄ£Ê½ */
};

/*--------------²Ëµ¥ÉèÖÃ»­Ãæ Êý¾ÝÅäÖÃ-------------*/
code MENU_ITEM MenuItem[]={
  //²Ëµ¥ÐòºÅ,²Ëµ¥µØÖ·,ÃÜÂë Êý¾Ý´æ´¢Ê×µØÖ· ×î´óÊý¾Ý¸öÊý
	{MENU_ID_00000,
		Menu00000,
		"00000",
		0,
		MENU_ID_00000_MAX_ITEM,
		0,
		&Menu_Number[0]}, 

	{0,0,0}
};

code MAIN_SHOW MainItem[]={
    //ÏÔÊ¾µÄ×Ö·û´®,Êý¾ÝÏÔÊ¾ÀàÐÍ,Êý¾ÝµØÖ·,º¯ÊýÃû
    {"",TY_D_A,0,&(public_val.main_disp_val),INT32_SIZE,5,0,99999,0},		
    {0,0,0}
};

/*-------------------------Ö÷»­ÃæÊý¾ÝÅäÖÃ,È«¾Ö±äÁ¿----------------------------*/
xdata const MAIN_PIC main_menu_cfg[]={
    {4,2,MainItem,MenuItem},
};

/*
xdata const MAIN_PIC main_menu_cfg[]={
    {4,2,MainItem,MenuItem},
		};		//4:Ã¿µãÏÔÊ¾´ÎÊý,10:²Ëµ¥Ë¢ÐÂÆµÂÊ(Ã²ËÆÃ»ÓÃµ½) 
*/

static code const char LED_ASCII[128]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //0x0
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //0x1
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
				            0x00,0xFE,0xB0,0xED,0xF9,0xB3,0xDB,0xDF, //0x2
				            0xF0,0xFF,0xFB,0x00,0x00,0x40,0x80,0x00, 
				            0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07, // ÆðÊ¼48:0-7
				            0x7f,0x6F,0x77,0x1F,0x4E,0x3D,0x4F,0x47, //     56
				            0xFB,0x77,0x1F,0x4E,0x3D,0x79,0x71,0xFB, //     64
				            0x37,0x06,0x0E,0x07,0x38,0x6A,0x76,0x1D, //     72
				            0x73,0x67,0x05,0x5B,0x31,0x3E,0x1C,0x5C, //     80
				            0x31,0x3B,0xED,0x00,0x00,0x00,0x62,0x08,
				            0x00,0x77,0x1F,0x4E,0x3D,0x4F,0x47,0xFB, //0x6
				            0x37,0x06,0x38,0x07,0x0E,0x6A,0x15,0x1D, 
				            0x67,0x73,0x50,0x5B,0x78,0x3E,0x1C,0x5C, //0x7
				            0x31,0x3B,0xED,0x00,0x00,0x00,0x00,0x00, 
                           };

//unsigned short usParamNum = 30;   /* Ö÷°åµÄÅäÖÃ²ÎÊý¸öÊý£¬×î´óÎª30¸ö */

xdata int gsKeyVal = 0;    /* °´¼üÖµ */
xdata char gcKeyType = 0;  /* °´¼üÀàÐÍ: 0¶Ì°´, 1³¤°´ÏÂ£¬2³¤°´Ì§Æð */

//xdata char gcKeyLongUpFlag = 0;    /* ³¤°´Ì§Æð±êÖ¾:0Î´Ì§Æð£¬1Ì§Æð */
//xdata char gcKeyLongDownFlag = 0;  /* ³¤°´°´ÏÂ±êÖ¾:0Î´°´ÏÂ£¬1°´ÏÂ */

xdata _U_BIT_PARAM unKey;

						   
//static bit 	bDaJiao = 1;					   
 

xdata char speed_mode = 1;   //ËÙ¶ÈÄ£Ê½: 1¸ßËÙ£¬ 0µÍËÙ

xdata char qz_wait = 0;      //µÈ´ýÇÐÖ½È·ÈÏ: 0Î´µÈ´ý£¬1µÈ´ý
						   
#define uchar unsigned char


//add 0331					   
typedef union i_status_16
//¶¨Òå¹²ÓÃÌå
{ 
	unsigned int iostatus_16; 
	struct b_16
	{			uchar b00:1;    //Î»Óò  
				uchar b01:1;
				uchar b02:1;
				uchar b03:1;
				uchar b04:1;
				uchar b05:1;
				uchar b06:1;
				uchar b07:1;
				uchar b08:1;
				uchar b09:1;
				uchar b10:1;  
				uchar b11:1;
				uchar b12:1;
				uchar b13:1;
				uchar b14:1;
				uchar b15:1;  
	}bit_16;  
}uints;

uints io_status;

xdata unsigned char sziostatus[17];


/*++++++++++++++++++++++++++++++++++++++++++++++++
¹¦ÄÜ£ºÏÔÊ¾×Ö·û´®¡£

²ÎÊý£ºbuff:Óû·¢ËÍµÄÊý¾Ý»º³åÇø£»x:×ø±ê

·µ»ØÖµ£ºÎÞ¡£

ËµÃ÷£º
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LEDShowFlashString(const char  * buff,char x) 
{
	/*
	do
	{
		disp_buf[x++] = LED_ASCII[*buff++];			//»ñÈ¡×ÖÐÎÂë
	}while(x<6 && *buff!=0);
	*/
		
	#ifdef LED_DISP
	do
	{
		//add
		if (*buff == " ")
		{
			disp_buf[x++] = 0;
			buff++;
		}
		else
		//add
		{
			disp_buf[x++] = LED_ASCII[*buff++];			//»ñÈ¡×ÖÐÎÂë
		}
	}while(x<6 && *buff!=0);
	#else
	//lcd_x_def = x;
	
	//add 0325
	//szShowFlashString = buff;
	//add
	
	do
	{
		//if (*buff == " ")
		if (*buff == ' ' || *buff == '-')
		{
			//x++;
			//Èç¹û½«' '¸³Öµ¸ødisp_buf,È»ºóÖ±½ÓÏÔÊ¾³öÀ´µÄ»°,' 'Îª0x20, ¶ÔÓ¦ÏÔÊ¾³öÀ´µÄ10½øÖÆasciiÂëÎª32,30.
			//Òò´Ë£¬ÐèÒªÖ±½Ó±£´æ¸ÃÖµ,È»ºó,¸ù¾Ý¸ÃÖµ,Ö±½Ó·¢ËÍÏÔÊ¾¸ÃÖµ.
			//Èç:if (disp_buf[i] == ' ')
			/*
						{
							sprintf(lcd_disp_buf,"DS%d(%d,174,' ',1);\r\n",zt,i * 26 + 10);
							GpuSend(lcd_disp_buf);
						}
			*/
			//ÁíÍâ£¬Èç¹û½«ÕâÐ©asciiÂë(ÐèÒª¸ÄÐ©µÄÃÜÂëºÍ²ÎÊýÀýÍâ),Ö±½Ó´æÔÚÒ»¸ö×Ö·û´®±äÁ¿ÖÐ,
			//×îºóÈ¡³öÊ±£¬Ö±½ÓÒÔ×Ö·û´®'%s'ÐÎÊ½È¡³ö£¬¾Í·½±ã¶àÁË
			disp_buf[x++] = *buff++;   //chang by 0322
		}
		else
		{
			//³ö´í±êÖ¾:"E--"£¬ÕâÑù´¦Àí¿ÉÄÜ²»ºÏÊÊ
			disp_buf[x++] = *buff++ - '0';
			
			/*
			if ( buff > 'a' || buff < 'f')
			{
				disp_buf[x++] = *buff++ - 'a' + 9;			//lcd_disp_str_pBuf
			}
			else if ( buff > '0' || buff < '9')
			{
				disp_buf[x++] = *buff++ - '0';
			}
			else
			{
				disp_buf[x++] = *buff++;
			}
			*/
		}
	}while(x<6 && *buff!=0);
	#endif
	
}


//¸ù¾ÝÐòºÅnum(²Ëµ¥Ìõ±àºÅ)1,2,3,...,»ñÈ¡²Ëµ¥±àºÅ
//»ñÈ¡²Ëµ¥±àºÅ(ascii)
void  LEDShowCaption(char num) 
{
char *buff,x = 0;
switch(num)
	{
		case 0:
			buff = "1-";
			if (cMenuEditFlag == 1)
			{
/*				
				szDispHz2 = "±¨¾¯³ÖÐøÊ±¼ä";		//		//µÚ1ÐÐÏÔÊ¾ºº×Ö,32µãÕóºº×Ö(Ð¡×Ö¿â)ÒÑÉèÖÃºÃ
				
				szDispSM4 = "µ¥Î»S";								//  	//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó				
				szDispSM2 = NULL;
				szDispSM3 = NULL;
*/		
			}
			else
			{
				//szDispHz2 = "Ö½³¤";
			}
			
		break;
		case 1:
			buff = "2-";
			if (cMenuEditFlag == 1)
			{
/*				
				
				szDispHz2 = "ÎÞ¸Ë¼ì²â½Ó½ü´«¸ÐÆ÷³¬Ê±";		//
				szDispSM4 = "´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸Ã";						//   //ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
				szDispSM2 = "Ê±¼ä,ÎÞ¸Ë¼ì²â¸ÐÓ¦Æ÷Î´¸ÐÓ¦µ½";
				szDispSM3 = "Í£»ú±¨¾¯¡£(µ¥Î»0.1s)";
*/		
			}
			else
			{
				//szDispHz2 = "À¦Êý";
			}			
		break;		
		case 2:
			buff = "3-";
/*				
				
			szDispHz2 = "ÎÞ¸Ë¼ì²âÍ¨¶Ï´«¸ÐÆ÷³¬Ê±";
			szDispSM4 = "´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸Ã";				//  // ´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸ÃÊ±¼ä,  //ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "Ê±¼ä,ÎÞ¸ËÍ¨¶Ï¸ÐÓ¦Æ÷Î´¸ÐÓ¦µ½,";
			szDispSM3 = "Í£»ú±¨¾¯¡£(µ¥Î»0.1s)";
*/		
		break;	
		case 3:
			buff = "4-";
/*				
				
		szDispHz2 = "ËÍÖ½²½½øµç»ú·½Ïò";
			szDispSM4 = "ÉèÖÃ¸ÃÖµÈ·¶¨µç»úÕýÈ·µÄÔË";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "¶¯·½Ïò¡£Ò»¾­ÕýÈ·Éè¶¨£¬";
			szDispSM3 = "²»µÃËæÒâ¸ü¸Ä¡£";
*/			  
		break;		
		case 4:
			buff = "5-";
/*				
				
		szDispHz2 = "ËÍÖ½²½½øËÙ¶ÈÔöÒæ";
			szDispSM4 = "¸ÃÖµÔ½´ó£¬ËÍÖ½ËÙ¶ÈÔ½¿ì¡£";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;
			szDispSM3 = NULL;	
*/				
		break;		
		case 5:
			buff = "6-";
/*				
				
		szDispHz2 = "ËÍÖ½²½½øÂö³åµ±Á¿";
			szDispSM4 = "µ¥Î»0.1Âö³å/ºÁÃ×¡£ÓÉ²½½ø";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "Ï¸·Ö¡¢¼õËÙ±È¡¢¹öÖá°ë¾¶¹²Í¬";
			szDispSM3 = "¾ö¶¨¡£Ò»¾­ÕýÈ·Éè¶¨,²»µÃ¸ü¸Ä¡£";
*/		
		break;		
		case 6:
			buff = "7-";
/*				
				
		szDispHz2 = "·¢Ö½ËÙ¶ÈÎ¢µ÷ÏµSHU";			// "Êý"Õâ¸ö×Ö±àÂë³ö´í,ÏÔÊ¾Ê±³ö´íµÄµØ·½»áË¢ÆÁ
			szDispSM4 = "£¨ÎÞ·¢Ö½µç»úÎÞÐ§£©";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "¸ÃÖµ¿ÉÎ¢µ÷·¢Ö½ËÙ¶È,";
			szDispSM3 = "ÖµÔ½´ó·¢Ö½Ô½¿ì¡£";
*/		
		break;		
		case 7:
			buff = "8-";
/*				
				
		szDispHz2 = "·¢Ö½¹öÂÖ°ë¾¶";
			szDispSM4 = "µ¥Î»0.1ºÁÃ×¡£";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "£¨ÎÞ·¢Ö½µç»úÎÞÐ§£©";
			szDispSM3 = "Ò»¾­ÕýÈ·Éè¶¨£¬²»µÃ¸ü¸Ä¡£";
*/		
		break;		
		case 8:
			buff = "9-";
/*				
				
		szDispHz2 = "·¢Ö½ËÙ¶Èµ÷½ÚÆµÂÊ";
			szDispSM4 = "£¨ÎÞ·¢Ö½µç»úÎÞÐ§£©";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "¸ÃÖµÔ½´ó·¢Ö½ËÙ¶Èµ÷½Ú";
			szDispSM3 = "ÆµÂÊÔ½¸ß¡£";
*/		
		break;		
		case 9:
			buff = "10";
/*				
				
		szDispHz2 = "·¢Ö½µç»úÆô¶¯ÏµSHU";				//Êý   // "Êý"Õâ¸ö×Ö±àÂë³ö´í,ÏÔÊ¾Ê±³ö´íµÄµØ·½»áË¢ÆÁ
			szDispSM4 = "£¨ÎÞ·¢Ö½µç»úÎÞÐ§£©";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "Ò»¾­ÕýÈ·Éè¶¨,²»µÃËæÒâ¸ü¸Ä¡£";
			szDispSM3 = NULL;
*/		
		break;		
		case 10:
			buff = "11";
/*				
				
		szDispHz2 = "ÇÐÖ½µç»ú¹öÂÖ¼õËÙ±È";
			szDispSM4 = "Ò»¾­ÕýÈ·Éè¶¨,²»µÃËæÒâ¸ü¸Ä¡£";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;
			szDispSM3 = NULL;
*/		
		break;		
		case 11:
			buff = "12";
/*				
				
		szDispHz2 = "ÇÐÖ½µç»úÄ¿±êËÙ¶È";
			szDispSM4 = "µ¥Î»0.1khzÂö³åÆµÂÊ";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "¸ÃÖµÔ½´óÔòÇÐÖ½Ô½¿ì¡£";
			szDispSM3 = NULL;
*/		
		break;		
		case 12:
			buff = "13";
/*				
				
		szDispHz2 = "·¢Ö½ËÙ¶È×î¸ßµÈ¼¶";
			szDispSM4 = "£¨ÎÞ·¢Ö½µç»úÎÞÐ§£©";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "·¢Ö½µç»úËÙ¶Èµ÷½ÚµÄ×î¸ßËÙ¶È¡£";
			szDispSM3 = "¸ÃÖµÔ½´óÔò·¢Ö½µç»úËÙ¶ÈÔ½¿ì¡£";
*/		
		break;	
		case 13:
			buff = "14";
/*				
				
		szDispHz2 = "·¢Ö½ËÙ¶È×îµÍµÈ¼¶";
			szDispSM4 = "£¨ÎÞ·¢Ö½µç»úÎÞÐ§£©";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "·¢Ö½µç»úËÙ¶Èµ÷½ÚµÄ×îµÍËÙ¶È¡£";
			szDispSM3 = "¸ÃÖµÔ½Ð¡Ôò·¢Ö½µç»úËÙ¶ÈÔ½Âý¡£";
*/		
		break;	
		case 14:
			buff = "15";
/*				
				
		szDispHz2 = "ËÍÖ½Æô¶¯Ê±ÇÐµ¶¸ß¶È";
			szDispSM4 = "µ¥Î»0.1kÂö³å";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "¸ÃÖµÔ½´óËÍÖ½Æô¶¯Ê±ÇÐµ¶Ì§ÆðÔ½¸ß¡£";
			szDispSM3 = NULL;
*/		
		break;	
		case 15:
			buff = "16";
/*				
					
			szDispHz2 = "´ò½ºµç»úÔË¶¯ËÙ¶È";
			szDispSM4 = "µ¥Î»°Ù·ÖÊý";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = "¸ÃÖµÔ½´óÔË¶¯ËÙ¶ÈÔ½¿ì¡£";
			szDispSM3 = NULL;
*/		
		break;	
		case 16:
			buff = "17";
/*				
				
			szDispHz2 = "ÇÐÖ½µç»úÆô¶¯ËÙ¶È";	
			szDispSM4 = "¸ÃÖµÔ½´óÆô¶¯ËÙ¶ÈÔ½¿ì¡£";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;
			szDispSM3 = NULL;
*/		
		break;
		case 17:
			buff = "18";

/*				
						
			szDispSM4 = NULL;						//´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸ÃÊ±¼ä  ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;				//"ÎÞ¸Ë¸ÐÓ¦Æ÷Î´¸ÐÓ¦µ½,ÔòÍ£»ú±¨¾¯";
			szDispSM3 = NULL;	
*/		
		break;	
		case 18:
			buff = "19";

/*				
						
			szDispSM4 = NULL;		//"´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸ÃÊ±¼ä";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;		//"ÎÞ¸Ë¸ÐÓ¦Æ÷Î´¸ÐÓ¦µ½,ÔòÍ£»ú±¨¾¯";
			szDispSM3 = NULL;	
*/		
		break;	
		case 19:
			buff = "20";

/*				
						
			szDispSM4 = NULL;		//"´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸ÃÊ±¼ä";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;		//"ÎÞ¸Ë¸ÐÓ¦Æ÷Î´¸ÐÓ¦µ½,ÔòÍ£»ú±¨¾¯";
			szDispSM3 = NULL;	
*/			
		break;			
		case 20:
			buff = "21";

/*				
						
			szDispSM4 = NULL;		//"´êÌõÖÜÆÚ¸ÐÓ¦Æ÷¸ÐÓ¦µ½,³¬¹ý¸ÃÊ±¼ä";		//ËµÃ÷ÎÄ×Ö,Ê¹ÓÃ24µãÕó
			szDispSM2 = NULL;		//"ÎÞ¸Ë¸ÐÓ¦Æ÷Î´¸ÐÓ¦µ½,ÔòÍ£»ú±¨¾¯";
			szDispSM3 = NULL;
*/		
		break;		
		case 21:
			buff = "22";
		
		break;		
		case 22:
			buff = "23";
		
		break;	
		case 23:
			buff = "24";
		
		break;	
		case 24:
			buff = "25";
		
		break;	
		case 25:
			buff = "26";
		
		break;	
		case 26:
			buff = "27";
		
		break;
		case 27:
			buff = "28";
		
		break;	
		case 28:
			buff = "29";
		
		break;	
		case 29:
			buff = "30";
		
		break;	
		case 80:
			buff = "   A1";
		break;
		case 81:
			buff = "   A2";
		break;
		case 82:
			buff = "   A3";
		break;
		case 83:
			buff = "   A4";
		break;
		case 84:
			buff = "   A5";
		break;
		case 85:
			buff = "   A6";
		break;
		case 86:
			buff = "   A7";
		break;
		case 87:
			buff = "   A8";
		break;
		case 88:
			buff = "   A9";
		break;
		case 89:
			buff = "  A10";
		break;
	}
	
	#ifdef LED_DISP	
	do
	{
		//add
		if (*buff == ' ')
		{
			disp_buf[x++] = 0;
			buff++;
		}
		else
		//add
		{
			disp_buf[x++] = LED_ASCII[*buff++];
		}
	}while(x<6 && *buff!=0);
	#else
	//lcd_x_def = x;
	
	/*
	*szDispHz1 = *buff;			//¸øÖ¸Õë¸³Öµ(µØÖ·)	
	*szDispHz2 = *szMenuDispHz[num];		//
	*/
	szDispHz1 = buff;			//¸øÖ¸Õë¸³Öµ(µØÖ·)
	
	/*
  //if (num < 30)
	if (num < 3)
	{
		//szDispHz2 = *(&szMenuDispHz[num]);		//
			szDispHz2 = "½Ó½ü´«¸ÐÆ÷³¬Ê±";
	}
	else
	{
		szDispHz2 = "ËÍÖ½²½½øµç»ú·½Ïò";
	}
	*/
	
	
	do
	{
		
		//if (*buff == " " || *buff == "-")			//chang by 0322 yi
		if (*buff == ' ' )
		{
			//x++;
			//buff++;			
			disp_buf[x++] = ' ';
			buff++;
		}
		else
		{
			disp_buf[x++] = *buff++ - '0';	
		}
	}while(x<6 && *buff != 0);
	#endif
}

/*++++++++++++++++++++++++++++++++++++++++++++++++
¹¦ÄÜ£ºÏÔÊ¾×Ö·û´®¡£

²ÎÊý£ºbuff:Óû·¢ËÍµÄÊý¾Ý»º³åÇø£»x:×ø±ê

·µ»ØÖµ£ºÎÞ¡£

ËµÃ÷£º
++++++++++++++++++++++++++++++++++++++++++++++++*/
/*void  LEDShowString(char  * buff,char x) 
{
	do
	{
		disp_buf[x++] = LED_ASCII[*buff++];
	
	}while(x<6 && *buff!=0);
}*/

/*void  LED_ShowData(char * buff,char x,char nPoint)
{
    do
	{
        if(nPoint !=0 && (8-nPoint)==x)
        	disp_buf[x++] = LED_ASCII[*buff++] | 0x80;		///<¡¾Î»7ÖÃ1¡¿
		else
			disp_buf[x++] = LED_ASCII[*buff++];
	}while(x<6 && *buff!=0);
}*/

/*++++++++++++++++++++++++++++++++++++++++++++++++
¹¦ÄÜ£ºÇå³ýÖ¸¶¨ÐÐºÅÁÐºÅµÄ×Ö·û¡£

²ÎÊý£ºx:×ø±ê1-8¡£
x: ½øÐÐÉÁË¸µÄÎ», n£º¼¸Î»(¹Ì¶¨Îª1)

·µ»ØÖµ£ºÎÞ¡£

ËµÃ÷£º ÎÞ¡£
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LEDClearChar(char x,char n)
{
	char i;
	#ifdef LED_DISP
	for(i = x; i< (x+n); i++ )
		{
			disp_buf[i] = 0;		
		}
	#else
	for(i = x; i< (x+n); i++ )
	{
		//disp_buf[i] = " ";		//ÏÔÊ¾¿Õ¸ñ   Êµ¼ÊÏÔÊ¾:12354
		disp_buf[i] = ' ';		//ÏÔÊ¾¿Õ¸ñ   Êµ¼ÊÏÔÊ¾: 32
	}
	#endif	
}


/*++++++++++++++++++++++++++++++++++++++++++++++++
¹¦ÄÜ£ºÏÔÊ¾Êý×Ö¡£
²ÎÊý£ºbuff:Óû·¢ËÍµÄÊý¾Ý»º³åÇø£»x:×ø±ê
·µ»ØÖµ£ºÎÞ¡£
ËµÃ÷£º
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LED_ShowNumber(char * buff,char x) 
{
	#ifdef LED_DISP
	char TempFlag=0;
	do
	{
		if(*(buff+1)!='.' || TempFlag==1)		//ÅÐÏÂÒ»¸öÊýÊÇ·ñÎªÐ¡Êýµã¡£
		{
			disp_buf[x++] = LED_ASCII[*buff++];
			//SendLed(x++,LED_ASCII[*buff++]);
		}
		/* ×¢: ÔÚledÏÔÊ¾ÖÐ, Îª¿Õ¸ñµÄ,»ñÈ¡µÄÎ»¶ÎÂëÎª0,ËùÒÔ²»ÏÔÊ¾
		//add		
		else if (*buff == ' ')
		{
			disp_buf[x++] = 0;
			buff++;
		}
		//add 0321 by yi
		*/
		else
		{
			disp_buf[x++] = LED_ASCII[*buff++] | 0x80;		//½«×ÖÐÎÂëµÄÐ¡ÊýµãÎ»ÖÃÎ»
			//SendLed(x++,(LED_ASCII[*buff++] | 0x80));
			TempFlag=1;
			buff++;					//Ìø¹ýÏÂÒ»¸öÐ¡Êýµã
		}
	}while(x<6);
	#else
	//lcd_x_def = x;
	do
	{
		/*
		if (*buff == ' ')
		{
			//x++;
			disp_buf[x++] == ' ';
			buff++;
		}
		else
		{
			disp_buf[x++] = *buff++ - '0';
		}
		*/
		
		//del by 0325 19:45
		/*
		if (*buff == '.')		//' ' == 0x20 :: =32
		{
			//x++;
			disp_buf[x++] = ' ';	//'.'
			buff++;
		}		
		//add
		else if (*buff == ' ')
		{
			disp_buf[x++] = ' ';
			buff++;
		}
		else if (*buff == '-')
		{
			disp_buf[x++] = ' ';		//'-'
			buff++;
		}
		//add 0321 by yi		
		else
		{
			disp_buf[x++] = *buff++ - '0';
		}
		*/
		
		//0325 19:43  chang
		if (disp_buf[x] == '.')		//' ' == 0x20 :: =32
		{
			x++;
		}
		//add
		else if (*buff == ' ')
		{
			disp_buf[x++] = ' ';
			buff++;
		}
		//add 0321 by yi
		else
		{
			disp_buf[x++] = *buff++ - '0';
		}
		//0325 19:43  chang

	}while(x<6);
	#endif		
}


//²Ëµ¥Êý¾ÝÖ¸Ïò
void menu_cfg(void)
{
	char i;
	for(i = 0; i < MENU_ID_00000_MAX_ITEM ; i++)
	{
		Menu00000[i].m_data_vale = &public_val.menu_parm[i];		//ÉèÖÃ²ÎÊý
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Ãû³Æ:   read_menu_data(char menu_id)
¹¦ÄÜ£º  ¶ÁÈ¡²Ëµ¥ÅäÖÃÊý¾Ý ºÍ ²Ëµ¥Êý¾Ý (ÅäÖÃ²ÎÊý)
²ÎÊý£º  ÎÞ
·µ»ØÖµ£ºÎÞ
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


void read_menu_data(char menu_id)
{
	unsigned char *progbyte,read_byte;
	unsigned int progadd;

	int i;

	progadd = MenuItem[menu_id].eeprom_cfg_add;
	read_byte = IapReadByte(progadd++);
	*((char*)(MenuItem[menu_id].com_max_item)) = MENU_ID_00000_MAX_ITEM;
	
	//¶Á²ÎÊý·¶Î§
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			MenuItem[menu_id].m_pcfg[i].m_max = Default_Param_Range[i].max;		//²ÎÊý·¶Î§
			MenuItem[menu_id].m_pcfg[i].m_min = Default_Param_Range[i].min;
			MenuItem[menu_id].m_pcfg[i].ext_property = 0; //read_byte;
		}

	
	switch(menu_id)		//Èç¹ûmenu_id == MENU_ID_00000
		{
			case MENU_ID_00000:
				progbyte = (unsigned char *)&public_val.menu_parm[0];
			break;			
		}
		
	//¶ÁÈ¡²ÎÊý(¸ù¾Ý×î´ó²Ëµ¥¸öÊý¶ÁÈ¡²ÎÊý)  ²ÎÊý´æ´¢ÔÚeeprom_addµØÖ·
	//¶ÁÈ¡µ½µÄ²ÎÊý´æ´¢ÔÚ&public_val.menu_parm[0]Àï,  £¨xdata public_stru public_val;£©
	progadd = MenuItem[menu_id].eeprom_add;
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			*progbyte = IapReadByte(progadd++);
			progbyte++;
			*progbyte = IapReadByte(progadd++);
			progbyte++;
		}
		
		/*
		unPaterLength =  *(int *)public_val.menu_parm[0];		//300
		unBunchNum =  *(int *)public_val.menu_parm[1];			//350
		//TotalNum = 100000;	
		*/
}


//¸ù¾Ý²Ù×÷ÊÖ²á:
//¿ª»ú½çÃæ: ÏÔÊ¾ÒÑËÍÖ½ÕÅÊý
//È»ºó£¬Èç¹û°´"-"2Ãë¿É½«ÒÑËÍÖ½ÕÅÊýÇå0£»
//Èç¹ûÔÚ¿ª»ú½çÃæ,°´"È·ÈÏ¡°¼ü,¿ÉÏÔÊ¾Èí¼þ°æ±¾ºÅºÍ´«¸ÐÆ÷×´Ì¬,ÏÈÏÔÊ¾5Î»°æ±¾ºÅ,ºóÏÔÊ¾4Î»´«¸ÐÆ÷×´Ì¬.
//ÆäÓà½çÃæ,°´Ñ¡Ôñ·µ»Ø¿ª»ú½çÃæ.
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Ãû³Æ:   void InitLEDDisplay(void)

¹¦ÄÜ£º  ÏÔÊ¾³õÊ¼»¯³ÌÐò

²ÎÊý£º  ÎÞ
      
·µ»ØÖµ£ºÎÞ
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void InitLEDDisplay(void)
{
    char TempB;
    //unsigned int temp_int;
		
   	BELL = 0;
	
//	  LEDShowFlashString("Lt-2 ",0);
//		LEDShowFlashString("51231",0);    /* ÏÔÊ¾²Ù×÷°å°æ±¾ºÅ */	
//		TM1637_Write();

#ifdef LED_DISP
		LEDShowFlashString("Lt-2 ",0);
		LEDShowFlashString("88236",0);    // ÏÔÊ¾²Ù×÷°å°æ±¾ºÅ  51231   ÊÇ±¾»ú×Ô¼º¶¨ÒåµÄ.
		TM1637_Write();
#else
		GpuSend("CLS(0);\r\n");		
		DELAY_US(UART2_DELAY);
		//add 0325
		 
		//add
	  //GpuSend("DS48(100,100,'88236',1);\r\n");		//51231


/*
		//GpuSend("W8DF(4,3,'116622663333');\r\n");	
		GpuSend("W8DF(5,3,'111662226633333');\r\n");	
		//GpuSend("W8DF(4,3,'116622663344');\r\n");	
		DELAY_US(UART2_DELAY);						
		//GpuSend("W8MU(10,10,95,70,1,2);\r\n");		//4  //2  \r\n
		
		//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
		//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
		GpuSend("W8MU(0,0,95,79,1,2);\r\n");
		
		DELAY_US(UART2_DELAY);	
		*/
		
		
		//Ö§³ÖºáÅÌµ¹Á¢µÄ»°£¬Ðè¼ÓÌõÖ¸Áî:DR2;
		#ifdef HP_DL
		GpuSend("DR2;\r\n");
		#endif
		///////////////////////////////////
		
		
		#ifdef LCD_9COL
		GpuSend("SPG(26);\r\n");
		#else
		GpuSend("SPG(52);\r\n");		
		#endif
		
		DELAY_US(UART2_DELAY);		
		DELAY_US(UART1s_DELAY);
		DELAY_US(UART1s_DELAY);
		
		unUpVerNum = 88236;		//²Ù×÷°å°æ±¾ºÅ
		//unDownVerNum = 16305;
		//unIoStatus = 0;

		//unPaterLength = 300;		//300
		//unBunchNum = 350;			//350
		//TotalNum = 100000;
		
		//new
		szDispHz1 = "Ö½³¤";
		szDispHz2 = "À¦Êý";
		szDispHz3 = "×ÜÊý";
		
		//unRow1Data = unPaterLength;
		//unRow2Data = unBunchNum;		//¸³³õÖµ,ÉèÖÃºó,Ö»ÄÜÓÃÉèÖÃºóµÄÖµ,²»ÄÜÔÙ¸³Öµ
		//unRow3Data = TotalNum;
		
		cTestKeyStringName = NULL;		//""	//ËÍÖ½»ú
		
		cMainUiFlag = 1;
		
		qz_key_cnt = 0;
		
		//add 0327 06:32
		cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
		//add 0327 06:32
		
		//TM1637_Write();
		

#endif

		cZcKunShuFlag = 0;
		
	///¡¾Í³¼ÆÖ÷½çÃæÐèÒªÏÔÊ¾µÄÏîÊý¡¿
	pMain = main_menu_cfg[0].m_main;
 	for( TempB = 0; pMain[TempB].m_str; TempB++ ) ///<¡¾¿Õ´®²»Îª0£¬¼´²»Îª¼Ù¡¿
		{
			;
		}			
	//DisplayMainTotal = TempB;

	///¡¾Í³¼ÆÏÂ¼¶×Ó²Ëµ¥ÊýÁ¿¡¿
	pItem = main_menu_cfg[0].m_menu;
	pMenu = pItem[0].m_pcfg;

	read_menu_data(MENU_ID_00000);
	
	menu_cfg();
		
	pDataValue=&pMenu[0];
	unPaterLength = *(int *)pDataValue->m_data_vale;			//TempInputData = *(int *)pDataValue->m_data_vale;
	//unPaterLength =	pDataValue->m_data_vale;
		
	pDataValue=&pMenu[1];
	unBunchNum = *(int *)pDataValue->m_data_vale;
	
	
	unRow1Data = unPaterLength;
	unRow2Data = unBunchNum;		//¸³³õÖµ,ÉèÖÃºó,Ö»ÄÜÓÃÉèÖÃºóµÄÖµ,²»ÄÜÔÙ¸³Öµ
	
	
	//add 0403
	//unUpVerNum = 85235;				//Ö÷°æ±¾ºÅ£¬ÊÇÍ¨¹ý´®¿Ú·µ»ØµÄ¡£
	unDownVerNum = 16305;
	unIoStatus = 0;					//IO¿Ú×´Ì¬£¬Ò²ÊÇÍ¨¹ý´®¿Ú·µ»ØµÄ.
	//add 0403	
		
	CycShowCount = 0;
	DisplayItemNum = 0;
	DisplayState = ST_MAIN;									//½øÈëÖ÷»­Ãæ
	l_stop_poll = 0;										//Ä¬ÈÏ¶¨¸ñÏÔÊ¾
	
	//±êÖ¾Î»³õÊ¼»¯
	public_val.Work_Pause_Flag = 0;	
	public_val.Err_Flag = 0;
	public_val.Bell_Flag = 0;

	public_val.cnt_after_powered = 0;
	
	//¶ÁÈ¡À¦Êý
	read_xt_num();
	
	//add 0403
	unRow3Data = TotalNum;
	//add 0403
	
	/* added by james */
    public_val.ms_timer = 0;
	public_val.io_read = 0;		//ÉèÖÃio¿Ú×´Ì¬£¬³õÖµÎª0.

	unKey.all = 0xFFFF;
	
	
	//³õÊ¼»¯Ïà¹ØÖ¸Õë
	szShowFlashString = NULL;
	//cTestKeyStringName = NULL;
	
	/*
	szDispSM1 = NULL;
	szDispSM2 = NULL;
	szDispSM3 = NULL;
	szDispSM4 = NULL;
	*/
	//////////////////////////////
	
	cFirstEnterAlarmFlag = 0;	
	cHandOptionFlag = 0;
	
	//cHandOptionFlag = 0;
	//cFirstEnterAlarmFlag = 0;
	cMainUiFlag = 1;

	unAlarmErrorSn = 0;		//0xff;	 //»áÏÔÊ¾E255.	//³õÊ¼Öµ
	
	//add 0402
	//cFirstEnterAlarmFlag = 0;
	//add 0402
	
	cFlashAlarmFlag = 0;
	
	cKeyOptionFlash = 0;
	
	cMenuSn = 0;
	
	//add 0403
	qz_wait = 0;
	cQzFlag = 0;
	//Main_disp_other = 0;
	clookVerFlag = 0;
	//add 0403
	
}


//Íê³É¶Ô²Ëµ¥Êý¾ÝµÄ´æ´¢
//±£´æËùÓÐ±à¼­²ÎÊý()
void func_menu_update(int menu_id)
{
	unsigned char *progbyte,i,readbyte;
	unsigned int progadd;
	
	eeprom_en = 0xaa;
	IapEraseSector(MenuItem[menu_id].eeprom_add);
	eeprom_en = 0;
	switch(menu_id)
		{
			case MENU_ID_00000:
				progbyte = (unsigned char *)&public_val.menu_parm[0];
			break;			
		}
	progadd = MenuItem[menu_id].eeprom_add;
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			//SendData(*(progbyte));
			eeprom_en = 0xaa;
			IapProgramByte(progadd,*(progbyte));
			readbyte = IapReadByte(progadd);
			if(readbyte != *progbyte)
				public_val.Err_Flag |= ERR_NUM_EEP_ERR; //Ð´EERPOM´íÎó
			progbyte++;
			progadd++;
			//SendData(*(progbyte));
			IapProgramByte(progadd,*(progbyte));
			readbyte = IapReadByte(progadd);
			if(readbyte != *progbyte)
				public_val.Err_Flag |= ERR_NUM_EEP_ERR;
			progbyte++;
			progadd++;
			eeprom_en = 0;
		}
	
	write_param_flag = 1;    // ¸üÐÂÏÂÎ»»ú²ÎÊý
			
}

#if 0
//Íê³É¶Ô°æ±¾ºÅµÄ´æ´¢
void func_ver_update(void)
{
	char progbyte;
	eeprom_en = 0xaa;
	IapEraseSector(EEPROM_DRIVE_TYPE);
	eeprom_en = 0;
	progbyte = (unsigned char)(public_val.drive_type >> 8);
	eeprom_en = 0xaa;
	IapProgramByte(EEPROM_DRIVE_TYPE,progbyte);		//´æ´¢×°ÖÃ°æ±¾ºÅ
	progbyte = (unsigned char)(public_val.drive_type & 0xff);
	IapProgramByte(EEPROM_DRIVE_TYPE + 1,progbyte);
	progbyte = public_val.drive_indx;
	
	IapProgramByte(EEPROM_DRIVE_INDX,progbyte);    //´æ´¢°æ±¾Ë÷ÒýºÅ
	eeprom_en = 0;
}

//Íê³É¶Ô²Ëµ¥ÅäÖÃµÄ´æ´¢
void func_cfg_update(int menu_id)
{
unsigned char i,progbyte;
unsigned int progadd;
	eeprom_en = 0xaa;
	IapEraseSector(MenuItem[menu_id].eeprom_cfg_add);
	progadd = MenuItem[menu_id].eeprom_cfg_add;
	progbyte = (unsigned char)(*(MenuItem[menu_id].com_max_item));
	IapProgramByte(progadd++,progbyte);
	
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			progbyte = (unsigned char)(MenuItem[menu_id].m_pcfg[i].m_max >> 8);
			IapProgramByte(progadd++,progbyte);
			progbyte = (unsigned char)(MenuItem[menu_id].m_pcfg[i].m_max & 0xff);
			IapProgramByte(progadd++,progbyte);
			progbyte = (unsigned char)(MenuItem[menu_id].m_pcfg[i].m_min >> 8);
			IapProgramByte(progadd++,progbyte);
			progbyte = (unsigned char)(MenuItem[menu_id].m_pcfg[i].m_min & 0xff);
			IapProgramByte(progadd++,progbyte);
			progbyte = (unsigned char)(MenuItem[menu_id].m_pcfg[i].ext_property);
			IapProgramByte(progadd++,progbyte);
		}
	eeprom_en = 0;
}

#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Ãû³Æ:   void LEDDisplay(char Key,char UpdateState)
¹¦ÄÜ£º  ÏÔÊ¾ÔËÐÐ³ÌÐò
²ÎÊý£º  ÎÞ
·µ»ØÖµ£ºÎÞ
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/* ²âÊÔÄ£Ê½ÄÚÈÝÏÔÊ¾ */
void test_set_display(char test_mode)
{
	switch (test_mode)
	{
		case 0:
			LEDShowFlashString(" F--0",0);			
			break;
		case 1:
			LEDShowFlashString(" F--1",0);
			break;
		case 2:
			LEDShowFlashString(" F--2",0);
			break;			
		case 3:
			LEDShowFlashString(" F--3",0);
			break;	
		case 4:
			LEDShowFlashString(" F--4",0);
			break;				
		case 5:
			LEDShowFlashString(" F--5",0);
			break;				
		default:
			break;
	}	
}

void LEDDisplay(void)
{
	char TempDataB,TempValue;
    static xdata char Bell_flag = 0,zc_key_flag = 0;
    static xdata unsigned int UpdateCountB,TempCount,flash_flag = 0;	
	static xdata long DisplayValueTemp;
	//static xdata unsigned char cMenuLookCnt = 31;
	
	//static xdata char Key_Val = 0,Pr_Key_Val = 0;
	//static xdata char old_key = 0;
	static xdata int Key_Val = 0,Pr_Key_Val = 0, old_key = 0;
	static bit Key_Dajiao_Val = 1, Pr_Key_Dajiao_Val = 1, old_Dajiao_key = 1;
	

	static xdata char UpdateState = 0;

	static xdata int Read_key_dely = 0,Read_key_dely_2 = 0,long_count = 0,long_input_flag = 0;
	static xdata int Read_key_dely_3 = 0,No_key_delay = 0;
	static xdata int Main_adj_key_delay = 0,Main_disp_other = 0;
	static xdata int edit_menu_id = 0;
	static xdata int bell_delay;
	static xdata int long_key_count = 0,long_key_step = 1,pr_long_input_flag;
	static xdata char war_bell_step = 0;
	static xdata int war_bell_delay = 0;

	static xdata char test_key_cnt = 0;   /* Ö÷½çÃæÏÂÑ¡Ôñ¼üÁ¬°´¼ÆÊý added by james for v19407 */
	//static xdata char learn_key_cnt = 0; 

	int temp_max;	
	int iBellOnTime, iBellOffTime;


//É¨ÃèÏà¹Ø¶Ë¿Ú
	unKey.u_bit.K1 = P24;
	unKey.u_bit.K2 = P23;
	unKey.u_bit.K3 = P22;
	unKey.u_bit.K4 = P21;
	
	unKey.u_bit.IO1 = P20;
	unKey.u_bit.IO2 = P35;
	unKey.u_bit.IO3 = P33;
	unKey.u_bit.IO4 = P32;
	unKey.u_bit.IO5 = P26;
	unKey.u_bit.IO6 = P27;
	unKey.u_bit.IO7 = P12;
	unKey.u_bit.IO8 = P13;
	unKey.u_bit.IO9 = P16;		//¶ÔÓ¦Ïà¹ØÒý½ÅµÄÖµ¸³¸ø¸ÃÎ»±äÁ¿

	unKey.u_bit.RED = P17;
	unKey.u_bit.GREEN = P54;
	unKey.u_bit.BLUE = P55;
	

	Key_Val = (int)unKey.all; 
	
	
	//ADD 0329 10:24
	Key_Dajiao_Val = P14;
	//ADD 0329 10:24
	
	
	//Key_Val = (P2 >> 1) & 0x0f;
	//Key_Val |= 0xf0;

	
	//chang 0329 0910
	if(Key_Val != old_key || Key_Dajiao_Val != old_Dajiao_key)
	//chang 0329 0910
	{
		No_key_delay = public_val.ms_timer;
		
		if(!long_input_flag)
		{
			bell_delay = public_val.ms_timer;
			Bell_flag |= 0x01;
		}
		else
		{
			Bell_flag &= 0xfe;		//¹Ø·äÃùÆ÷
		}
		
		if( (public_val.ms_timer - Read_key_dely_2) > KEY_DOU_DELY)
		{
			//add 0329 09:14
			if (Key_Dajiao_Val != old_Dajiao_key)
			{
				old_Dajiao_key = Key_Dajiao_Val;
			}
			else
			{
			//add 0329 09:14
				old_key = Key_Val;
			}
			Read_key_dely = public_val.ms_timer;
			long_count = 0;
			long_input_flag = 0;
			long_key_step = 1;
		}
		else
		{
			Key_Val = 0;		//¶Ì°´¼ü£¬Î´´ïµ½Ò»¶¨ÑÓÊ±£¬²»×÷ÎªÓÐÐ§¼ü
			//add
			Key_Dajiao_Val = 1;
			//add
		}		
	}
	else		//ÎÞ¼ü°´ÏÂ£¬°üÀ¨;³¤°´¼üÒ»Ö±°´ÏÂ
	{
		if( (public_val.ms_timer - bell_delay) > BELL_ON_DELY)
		{
			Bell_flag &= 0xfe;
		}

		//ÎÞ°´¼üÊ±,ÑÓÊ±Ê±¼äµ½·µ»ØÖ÷½çÃæ(ÏÔÊ¾io¿Ú×´Ì¬ºó,ÑÓÊ±·µ»ØÖ÷½çÃæ)
		//ms_timer: ºÁÃë¼ÆÊýÆ÷
		//ÎÞ¼ü°´ÏÂ£¬ÑÓÊ±Ê±¼äµ½
		if( (public_val.ms_timer - No_key_delay) > NO_KEY_RETURN_MAIN)   //NO_KEY_RETURN_MAIN:²Ëµ¥ÎÞ°´¼ü·µ»ØÖ÷Ò³ÃæÑÓÊ±
		{
			No_key_delay = public_val.ms_timer;
			
			
			if ((edit_menu_id == MENU_ID_00000) && (DisplayState != ST_LEARN))
			{
				
				DisplayState = ST_MAIN;
				Main_disp_other = 0;    /* added by james for v19407 */
				
//#if 1				
#ifndef LED_DISP		
				//¼ÓÉÏÕâÀï£¬ÒòÎªÓÐ¸öÏÔÊ¾Ñ­»·£¬ËùÒÔ£¬»áÓÐ½Ï³¤µÄÑÓÊ±£¬²Å»á½øÈë
				//add
				
	//new
	
	//¼ÓÉÏÕâ¸öifÓï¾äºÜºÃ£¬²»»á³öÏÖË¢ÆÁµÄÏÖÏó
				if (DisplayState == ST_MENU || DisplayState == ST_DATA_INPUT || DisplayState == ST_PASSWORD || clookVerFlag == 1 || cZcKunShuFlag == 1 || cQzFlag == 1 || cHandOptionFlag == 1)
				{	
						//add 0327 06:17
						//chang 0402
						//if (cMainNotFlashFlag == 0)
						if (cMainNotFlashFlag == 0 || cFirstEnterAlarmFlag == 0)
						{
						//chang 0402
						//add 0327 06:17	
							GpuSend("CLS(0);\r\n");		
							DELAY_US(UART2_DELAY);
						//add 0327 06:17
						}
						//add 0327 06:17				
						
						/*
						//GpuSend("W8DF(4,3,'116622663333');\r\n");	
						GpuSend("W8DF(5,3,'111662226633333');\r\n");	
						//GpuSend("W8DF(4,3,'116622663344');\r\n");		
						DELAY_US(UART2_DELAY);					
						//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
						//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
						GpuSend("W8MU(0,0,95,79,1,2);\r\n");
						DELAY_US(UART2_DELAY);
						*/
						
						if (clookVerFlag == 1)
							clookVerFlag = 0;
						
						if (cFirstDispIoStatusFlag)
							cFirstDispIoStatusFlag = 0;
						
						if (cHandOptionFlag == 1)
						{
							cHandOptionFlag = 0;
							//add 0405
							cKeyOptionFlash = 1;
							UpdateState = 0;
							//add 0405
						}
						
						if (cQzFlag == 1)
						{
							cQzFlag = 0;
							
							//add 0405
							cKeyOptionFlash = 1;
							UpdateState = 0;
							//add 0405
						}
						
						//add 0402
						if (cAlarmFlag)
						{
							szDispHz1 = "E--";
							szDispHz2 = NULL;
							szDispHz3 = NULL;
							//add 0322 by yi
							
							unRow1Data = 0;
							unRow2Data = 0;
							unRow3Data = 0;			

							cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
							//del end
							
							if(!war_bell_step) 
								war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
							
							//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
							
							//add 0322 by yi
							cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
							
							if (cFirstEnterAlarmFlag == 0)
							{
								GpuSend("BPIC(1,4,3,10);\r\n");
								DELAY_US(UART2_DELAY);
								DELAY_US(UART1s_DELAY);
								DELAY_US(UART1s_DELAY);
								
								sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
								GpuSend(lcd_disp_buf);
								DELAY_US(UART2_DELAY);
								
								cFirstEnterAlarmFlag = 1;
							}
						}									
						else
						{
						//add 0402				
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");					
							#endif
							DELAY_US(UART2_DELAY);
							
							DELAY_US(UART1s_DELAY);				
							

							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							cMainUiFlag = 1;
							
							//add
							//TM1637_Write();		 	 	//ÕâÌõÓï¾ä²»ÄÜÉÙ,·ñÔò£¬¿ª»úºóºÚÆÁ
						}
				}
#endif			
//#endif
					
					
			}
		}
		
		
		//µÚ¶þ´ÎÉ¨ÃèÊ±£¬Èç¹û³¤°´¼üÒ»Ö±°´ÏÂ£¬Î´´ïµ½Ò»¶¨ÑÓÊ±£¬ÒòÎªÊÇÎÞÐ§¼ü
		if( (public_val.ms_timer - Read_key_dely) > KEY_DOU_DELY)
		{
			Read_key_dely_2 = public_val.ms_timer;
			
			//´ïµ½Ò»¶¨ÑÓÊ±,Èç¹ûÓÐ¼ü°´ÏÂ£¬ÇÒ²»ÊÇ³¤°´¼ü
			if( ( (Key_Val&0xffff) != 0xffff) && (!long_input_flag) || (Key_Dajiao_Val != 1) && (!long_input_flag))
			{
				long_count++;
				
				//ÅÐ¶Ï³¤°´µÄÊ±¼ä£¬long_count´óÓÚ100´Î£¬¼´ÒòÎªÊÇ³¤°´¼ü
				if(long_count > 10)
				//if(long_count > 100)   		//100 * 50 = 5000ms(5s)
				{							
					long_input_flag = 1;		//³¤°´¿ªÊ¼Ê±,²½³¤Îª1
					long_key_step = 1;
					long_key_count = 0;
					Read_key_dely_3 = public_val.ms_timer; 
					long_count = 10;
				}
			}					
		}
		else
		{
			Key_Val = 0;		//·Ç³¤°´¼ü
			
			//add 0329 09:28
			Key_Dajiao_Val = 1;
			//add
		}
	}
	
	
	if(long_input_flag)  /* ³¤°´ */
	{
		if(public_val.ms_timer - Read_key_dely_3 > KEY_LONG_DELY)	//KEY_LONG_DELY:100
		{
			long_key_count ++;
			if(long_key_count > LONG_KEY_STEP_SWITCH)		//50 
			{
				long_key_step = 10;						//³¤°´¼ü´ïµ½Ò»¶¨ÑÓÊ±ºó¿ªÊ¼¼ÓËÙ
			}
			if(long_key_count)
				Read_key_dely_3 = public_val.ms_timer;
				Pr_Key_Val = 0xFF;		//¸øPr_Key_Val¸³Öµ£¬ÈÃÃ¿´Î³¤°´ÅÐ¶ÏÊ±£¬ÏÂÃæµÄÅÐ¶ÏÓï¾ä£¬Key_Val != Pr_Key_Val¡£ Ö»ÓÐ³¤°´¼üÌ§ÆðÊ±£¬Key_Val == Pr_Key_Val (Key_Val==0xff)
			
			//add 0329 10:03
			Pr_Key_Dajiao_Val = 1;
			//add
			
		}
	}
	
	if(Key_Val || Key_Dajiao_Val != 1)
	{
		if((Key_Val != Pr_Key_Val) || (Key_Dajiao_Val != Pr_Key_Dajiao_Val))
		{
			if (Key_Val != Pr_Key_Val)
			{
				Pr_Key_Val = Key_Val;			
			}
			else
			{
				Pr_Key_Dajiao_Val = Key_Dajiao_Val;
			}
		}
		else
		{
			if(Key_Val == Pr_Key_Val)
			{
				Key_Val = 0;			//³¤°´¼üÌ§ÆðÊ±£¬ÊÓÎªÎÞÐ§¼ü
			}
			else
			{
				Pr_Key_Dajiao_Val = 1;
			}		
		}		
	}


	/* ÓÐ°´¼üÔòÏÂ·¢°´¼üÖµºÍ°´¼üÀàÐÍ */
		
	if ((Key_Val == KEY_IO6) || (Key_Val == KEY_IO7) 
		|| (Key_Val == KEY_IO8))
	{
		write_key_flag = 1;   /* ÓÐ°´¼ü£¬ÐèÒªÐ´°´¼üµ½Ö÷°å */
		
		gsKeyVal = Key_Val;
		gcKeyType = KEY_SHORT_UP_FLAG;
	}
	
	
	if (Key_Val == 0)
	{
		if (Key_Dajiao_Val == 0)
		{
			Key_Val = KEY_DAJIAO_FAULT;			//Key_Dajiao_Val;		//´ò½º¼ü°´ÏÂ
		}
	}
	
	
//ÓÐ±¨¾¯ÐÅºÅ£¬²»ÄÜÆô¶¯(Æô¶¯°´¼ü²»Æð×÷ÓÃ),µ«ÓÒ±ßµÄ10¸ö°´¼ü£¬»¹ÊÇ¿ÉÒÔ²Ù×÷
//µ±È»£¬Í£Ö¹¼ü£¬Í£Ö½¼ü»¹ÊÇÄÜÆð×÷ÓÃ¡£	
//ÔÚÖ÷½çÃæ,°´ÏÂ"Ñ¡Ôñ"¼ü,»áÏÔÊ¾"000000"ÊäÈëÃÜÂë½çÃæ
  switch(DisplayState)
	{
	    case ST_MAIN:   /* Ö÷½çÃæ */
	    {
	        switch(Key_Val)
		    {

		        case KEY_SEL:

					test_key_cnt = 0;
					//learn_key_cnt = 0;

					if(Main_disp_other == 0)
					{
			            PasswordBuff[0] = '0';
			            PasswordBuff[1] = '0';
			            PasswordBuff[2] = '0';
			            PasswordBuff[3] = '0';  
			            PasswordBuff[4] = '0';
			            DisplayBitNum = 4;

						LEDShowFlashString(&PasswordBuff[0],0);
			            DisplayState = ST_PASSWORD;
										
					
						//add 0323
						GpuSend("CLS(0);\r\n");
						DELAY_US(UART2_DELAY);
						
						//add 0407
						DELAY_US(UART1s_DELAY);
						cAlarmFlag = 0;						
						//add 0407

						GpuSend("SPG(27);\r\n");
						//test
						//GpuSend("SPG(64);\r\n");
						//test
						DELAY_US(UART2_DELAY);		
						
						
						szDispHz1 = "";
						szDispHz2 = "ÇëÊäÈëÃÜÂë:";
						szDispHz3 = "";

						unRow1Data = 0; 
						unRow2Data = 0;
						unRow3Data = 0;	
						
						cMainUiFlag = 0;	

						//add 0402
						GpuSend("DS48(20,174,'00000',0);\r\n");
						DELAY_US(UART2_DELAY);
						return;		
						//add 0402						
					}
					else
					{
						if (qz_wait == 1)
						{
							qz_wait = 0;   // ÇåµÈ´ý×´Ì¬
							
							//add
							if (cQzFlag == 1)
							{
								cQzFlag = 0;
							}
							//add 0324

							war_bell_step = 0;  // Çå³ý·äÃùÆ÷ÏìÉù
							cPauseBell = 0;							
						}
						
						
						if (clookVerFlag)    //²é¿´°æ±¾
						clookVerFlag = 0;
						
						
						if (cFirstDispIoStatusFlag)
							cFirstDispIoStatusFlag = 0;
						
						if (cMenuEditFlag)
							cMenuEditFlag = 0;						
						
						
					//	GpuSend("CLS(0);\r\n");		
					//	DELAY_US(UART2_DELAY);
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
					/*
						//GpuSend("W8DF(4,3,'116622663333');\r\n");	
						GpuSend("W8DF(5,3,'111662226633333');\r\n");		
						//GpuSend("W8DF(4,3,'116622663344');\r\n");		
						DELAY_US(UART2_DELAY);						
						//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
						//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
						GpuSend("W8MU(0,0,95,79,1,2);\r\n");
						DELAY_US(UART2_DELAY);	
						*/
						
						//add 0402
						if (cAlarmFlag)
						{
							szDispHz1 = "E--";
							szDispHz2 = NULL;
							szDispHz3 = NULL;
							//add 0322 by yi
							
							unRow1Data = 0;
							unRow2Data = 0;
							unRow3Data = 0;			

							cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
							//del end
							
							//add 0405
							Main_disp_other = 0;
							//add 0405
							
							if(!war_bell_step) 
								war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
							
							//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
							
							//add 0322 by yi
							cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
							
							GpuSend("BPIC(1,4,3,10);\r\n");
							DELAY_US(UART2_DELAY);
							DELAY_US(UART1s_DELAY);															
							
							sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
							GpuSend(lcd_disp_buf);
							DELAY_US(UART2_DELAY);
							
							cFirstEnterAlarmFlag = 1;
							
							//add 0405
							cKeyOptionFlash = 1;
							UpdateState = 0;
							//add 0405
							return;
						}									
						else
						{
						//add 0402
							
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							//add 0327 06:32
							
							//TM1637_Write();
						}
			
					}										
					
		      break;	
					
		      case KEY_REDUCE:	 //¼õºÅ
		        
					test_key_cnt = 0;
					//learn_key_cnt = 0;
			  
			  //add 0327 07:12
					//cTestKeyStringName = "¼õºÅ";		//-
			  //add 0327 07:12
					
					//ÔÚÖ÷½çÃæÏÂ£¬³¤°´"-",¿É½«´êÌõ¸ùÊýÇå0.
					if (long_input_flag && (Main_disp_other == 0))
					{
						if (public_val.main_disp_val != 0)
						{
							public_val.main_disp_val = 0;				//xdata public_stru public_val;
							public_val.cnt_before_powered = 0;	//Ö÷°å¶ÁÈ¡µÄ¿ª»úÇ°ÒÑ±£´æÀ¦Êý
							public_val.cnt_after_powered = 0;		//Ö÷°å·´À¡¿ª»úºóµÄÀ¦Êý

							//add 0327 07:05
							TotalNum = 0;
							//add 0327 07:05
							
							eeprom_en = 0xaa;
							IapEraseSector(EEPROM_LOCK_VAL);  //0x4400
							IapEraseSector(0x4600);
							IapEraseSector(0x4800);
							IapEraseSector(0x4a00);
							IapEraseSector(0x4c00);
							eeprom_en = 0x0; 

							write_clear_cnt_flag = 1;    /* ·¢ËÍÃüÁîÖÁÖ÷°åÇå³ý¿ª»úºó¼ÆÊý */
							main_borad_finish_clear = 0; /* ÖÃÎªÖ÷°åÎ´Íê³ÉÇå³ý×´Ì¬ */
						}
					}					

					//°´"-"¼ü,ÖÃÓÉ¸ßËÙÄ£Ê½ÇÐ»»µ½µÍËÙÄ£Ê½
					if (speed_mode == 1)  //¸ßËÙ
					{
						write_speed_mode_flag = 1;  
						speed_mode = 0;   //ÇÐµ½µÍËÙ
					}
				
					break;	
					
					//°´"+"¼ü,ÖÃÓÉµÍËÙÄ£Ê½ÇÐ»»µ½¸ßËÙÄ£Ê½, ÔÚÖ÷½çÃæÏÂ, +, -Îª¸ßµÍËÙÇÐ»»¼ü, °´"+"¼ÓËÙ,°´"-"¼õËÙ,²¢ÇÒ£¬³¤°´-¿É½«¸ùÊýÇå0.
		      case KEY_INCREASE:  	 //¼ÓºÅ

					test_key_cnt = 0;
			  
					 //add 0327 07:12
					//cTestKeyStringName = "¼ÓºÅ";		//+
					//add 0327 07:12
					
					Main_adj_key_delay = public_val.ms_timer;					

					if (speed_mode == 0)  //µÍËÙ
					{
						write_speed_mode_flag = 1;  
						speed_mode = 1;  //ÇÐµ½¸ßËÙ
					}
#if 0
					if (Main_disp_other == 0)
					{
						/* ½øÈë²ÎÊý²Ëµ¥ */
					    pMenu = pItem[0].m_pcfg;	// Ö¸ÏòÊý¾Ý				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
		                DisplayItemNum  = 0;

						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//½øÈëÏàÓ¦²Ëµ¥  
	                    return;
					}
#endif						
										
					break;	

		        case KEY_GREEN:  	 //Ö½³¤

					test_key_cnt = 0;
					
					//chang 0407
					//if (Main_disp_other == 0)
					if (Main_disp_other == 0 || cAlarmFlag ==1 )
					//chang 0407
					{
						//add 0407
						if (cAlarmFlag)
							cAlarmFlag = 0;						
						//add 0407
						/* ½øÈë²ÎÊý²Ëµ¥ */
						
						//add
					//	work_mode = DEV_PARA_SET;						
						//add  by 0322						
						
						pMenu = pItem[0].m_pcfg;	// Ö¸ÏòÊý¾Ý				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
						DisplayItemNum  = 0;
						zc_key_flag = 1;								//ÒòÎª£¬ÊÇÀûÓÃzc_key_flag = 1£¬ÑÓÊ±ºó£¬Ö±½Ó½øÈëInPutData. ËùÒÔ,ÔÚST_MENU×´Ì¬£¬Êµ¼ÊÉÏÊÇÃ»ÓÐ²Ëµ¥Ñ¡Ôñ
						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//½øÈëÏàÓ¦²Ëµ¥
						
						
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						//chang 0404 13:46
						//GpuSend("SPG(29);\r\n");
						GpuSend("SPG(58);\r\n");
						//chang 0404 13:46
						DELAY_US(UART2_DELAY);
						
						
						szDispHz1 = "1-";			//¸øÖ¸Õë¸³Öµ(µØÖ·)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "Ö½³¤";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
						
						cMenuEditFlag = 0;						
						cZcKunShuFlag = 1; 		//ÖÃÖ½³¤±êÖ¾
												
						cMainUiFlag = 0;
						
						//add 0402
						if (clookVerFlag)
							clookVerFlag = 0;
						
						if (cFirstDispIoStatusFlag)
							cFirstDispIoStatusFlag = 0;
						//add 0402
						
						
					//	TM1637_Write();
						//add 0323							

				
				//×¢: ÕâÀï²»ÄÜ¼Ó·µ»Ø!!!!,ºÜ¶àÏÔÊ¾ÇåÆÁ²»Õý³£µÄÎÊÌâ,¶àÊÇÇåÆÁ²»³¹µ×(ÉÙÐ´ÁËÒ»ÌìÇåÆÁÓï¾ä),»òÕß,Ö±½Ó¼ÓÁË·µ»ØÔì³ÉµÄ
	      //    return;
						
					}
										
					break;	

		        case KEY_BLUE:  	 //À¦Êý

					test_key_cnt = 0;

					//chang 0407
					//if (Main_disp_other == 0)
					if (Main_disp_other == 0 || cAlarmFlag ==1 )
					//chang 0407
					{
						//add 0407
						if (cAlarmFlag)
							cAlarmFlag = 0;						
						//add 0407
						
						//add
					//	work_mode = DEV_PARA_SET;						
						//add  by 0322
						
						/* ½øÈë²ÎÊý²Ëµ¥ */
						pMenu = pItem[0].m_pcfg;	// Ö¸ÏòÊý¾Ý				
						now_menu.menu_id = 0;
						now_menu.par_id = 1;
						now_menu.event_flag = 1;
						DisplayItemNum  = 1;			
						zc_key_flag = 1;
						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//½øÈëÏàÓ¦²Ëµ¥ 

						//add
				//		GpuSend("CLS(0);\r\n");		
				//		DELAY_US(UART2_DELAY);
						//¶à¼ÓÒ»ÌõÇåÆÁÓï¾ä
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						DELAY_US(UART1s_DELAY);
						
						//chang 0404 13:46
						//GpuSend("SPG(30);\r\n");
						GpuSend("SPG(59);\r\n");
						//chang 0404 13:46
						
						DELAY_US(UART2_DELAY);
						
						
						szDispHz1 = "2-";			//¸øÖ¸Õë¸³Öµ(µØÖ·)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "À¦Êý";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
						
						cMenuEditFlag = 0;
						cZcKunShuFlag = 1; 		//ÖÃÀ¦Êý±êÖ¾
						
						cMainUiFlag = 0;
						
						//add 0402
						if (clookVerFlag)
							clookVerFlag = 0;
						
						if (cFirstDispIoStatusFlag)
							cFirstDispIoStatusFlag = 0;
						//add 0402
						
					//	TM1637_Write();
						
						//add 0323
						
					}
										
					break;	

					
		        case KEY_ENT:   //²é¿´³ÌÐò°æ±¾
							//ÔÚ°´ÏÂÇÐÖ½¼üÊ±, qz_wait ÖÃÎª 1.
				
							cFirstDispIoStatusFlag = 0;
							
							//chang 0403
							if (qz_wait == 0)							
							//if ((qz_wait == 0) && (cQzFlag == 0))  //²»ÊÇÇÐÖ½µÈ´ý×´Ì¬
							//chang 0403
							{
								//learn_key_cnt = 0;
								
								//add 0330 11:57
								//chang 0403 								
								if (clookVerFlag == 1)
								//if ((clookVerFlag == 1) && (Main_disp_other == 2))
								//chang 0403 
								{
								
									clookVerFlag = 0;									
									Main_disp_other = 0;  // ·µ»ØÏÔÊ¾Í²Êý   ·µ»ØÏÔÊ¾´êÌõ¸ùÊý
																	
									//add
									GpuSend("CLS(0);\r\n");		
									DELAY_US(UART2_DELAY);

									//add 0402
									if (cAlarmFlag)
									{
										szDispHz1 = "E--";
										szDispHz2 = NULL;
										szDispHz3 = NULL;
										//add 0322 by yi
										
										unRow1Data = 0;
										unRow2Data = 0;
										unRow3Data = 0;			

										//add 0406
										cHandOptionFlag = 0;
										//add 0406
										
										cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
										
										//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
										
										//add 0322 by yi
										cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
										
										GpuSend("BPIC(1,4,3,10);\r\n");
										DELAY_US(UART2_DELAY);
										DELAY_US(UART1s_DELAY);
										
										cFirstEnterAlarmFlag = 1;

										
										
										sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);
										return;
									}									
									else
									{
									//add 0402
									
										#ifdef LCD_9COL
										GpuSend("SPG(26);\r\n");
										#else
										GpuSend("SPG(52);\r\n");
										#endif		
										DELAY_US(UART2_DELAY);
										
										DELAY_US(UART1s_DELAY);

										szDispHz1 = "Ö½³¤";
										szDispHz2 = "À¦Êý";
										szDispHz3 = "×ÜÊý";
									
										unRow1Data = unPaterLength;
										unRow2Data = unBunchNum;
										unRow3Data = TotalNum;		//10000;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
										//add 0327 06:32

									//	TM1637_Write();
										//add 0324								

										war_bell_step = 0;  // Çå³ý·äÃùÆ÷ÏìÉù
										cPauseBell = 0;
										
										
										//add 0402				//¼ÓÉÏÏÔÊ¾Êý¾Ý,±ÜÃâÏÔÊ¾Êý¾ÝÊ±£¬²»ÍêÈ«Ë¢ÐÂµÄÇé¿ö
										GpuSend("W8UE(1);\r\n");
										DELAY_US(UART2_DELAY);
										sprintf(lcd_disp_buf,"DS64(140,7,'%d',15);\r\n",unRow1Data);
										GpuSend(lcd_disp_buf);				
										DELAY_US(UART2_DELAY);
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);
										
										GpuSend("W8UE(3);\r\n");
										DELAY_US(UART2_DELAY);
										sprintf(lcd_disp_buf,"DS64(140,5,'%d',15);\r\n",unRow2Data);
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);
										
										
										szTotalNum[0] = unRow3Data/10000000 + '0';
										
										unRow3Data = unRow3Data % 10000000;
										szTotalNum[1] = unRow3Data/1000000 + '0';
										
										unRow3Data = unRow3Data % 1000000;
										szTotalNum[2] = unRow3Data/100000 + '0';
										
										unRow3Data = unRow3Data%100000;
										szTotalNum[3] = unRow3Data/10000 + '0';
										
										unRow3Data = unRow3Data%10000;
										szTotalNum[4] = unRow3Data/1000 + '0';
										
										unRow3Data = unRow3Data%1000;
										szTotalNum[5] = unRow3Data/100 + '0';
										
										unRow3Data = unRow3Data%100;
										szTotalNum[6] = unRow3Data/10 + '0';
										
										unRow3Data = unRow3Data%10;
										szTotalNum[7] = unRow3Data + '0';
										
										szTotalNum[8] = 0;
					
										unRow3Data = TotalNum;			
										
										
										GpuSend("W8UE(4);\r\n");
										DELAY_US(UART2_DELAY);					
										
										//sprintf(lcd_disp_buf,"DS64(136,7,'%d',15);\r\n",(int*)(&szTotalNum[0]));
										
										sprintf(lcd_disp_buf,"DS64(136,7,'%s',15);\r\n",(int*)(&szTotalNum[0]));					
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);						
										
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);
										
										return;
										//add 0402
										
									}
										  
								}
								else
								{
									//add 0330 11:57
									if (long_input_flag)
								    {						
										if (test_key_cnt < TEST_MODE_CNT)		//²âÊÔÄ£Ê½¼ÆÊý  //40
										{
											test_key_cnt++;
										}
									}
									else
									{
										test_key_cnt = 0;
									}
											
									Main_adj_key_delay = public_val.ms_timer;		//ms¼ÆÊýÆ÷
									Main_disp_other = 2;					//ÔÚ°´ÏÂ²éÑ¯°æ±¾ºÅÊ±,¸ø Main_disp_other ¸³ÖµÎª2. ¸ù¾Ý´Ë¿ÉÅÐ¶Ï
									
									//ÖÃ²é¿´°æ±¾ºÅ±êÖ¾
									clookVerFlag = 1;
									
									//add 0402
								//	cFirstDispIoStatusFlag = 0;
									//add 0402
									
									//add
									GpuSend("CLS(0);\r\n");
									DELAY_US(UART2_DELAY);										
									
									//chang 0404 13:50
									//GpuSend("SPG(28);\r\n");
									GpuSend("SPG(57);\r\n");
									//chang 0404 13:50
									DELAY_US(UART2_DELAY);
									
									DELAY_US(UART1s_DELAY);
										
									
									szDispHz1 = "ÉÏ²ã";
									szDispHz2 = "µ×²ã";
									szDispHz3 = "×ÜÊý";				//"×´Ì¬";
									
									
									//chang 0326 16:36
									unRow1Data = unUpVerNum;		//88236;
									if (unRow1Data == 0)
									{
										unRow1Data = 88236;
										unUpVerNum = 88236;
									}
									
									unRow2Data = unDownVerNum;		//16305;
									
									//chang 0403
									unRow3Data = unIoStatus;			//23456;
								//	unRow3Data = glTotalAllCnt;			//ÏÔÊ¾×Ü×ÜÊý
									//chang 0403									
									
									cMainUiFlag = 0;									
									
									//add 0330 17:09
									//chang 0404 13:50
									//sprintf(lcd_disp_buf,"DS48(180,1,'%d',15);\r\n",unRow1Data);		//ÉÏ²ã
									//GpuSend(lcd_disp_buf);
									//DELAY_US(UART2_DELAY);
									
									GpuSend("W8UE(1);\r\n");
									DELAY_US(UART2_DELAY);									
									GpuSend("DS64(180,7,'88236',15);\r\n");		//,unRow1Data
									DELAY_US(UART2_DELAY);
									GpuSend("SXY(0,0);\r\n");
									DELAY_US(UART2_DELAY);																		
									//chang 0404 13:50
									
									
									//chang 0404 13:50
									//sprintf(lcd_disp_buf,"DS48(180,86,'%d',15);\r\n",unRow2Data);		//µ×²ã
									//GpuSend(lcd_disp_buf);
									//DELAY_US(UART2_DELAY);
									
									GpuSend("W8UE(2);\r\n");
									DELAY_US(UART2_DELAY);
									sprintf(lcd_disp_buf,"DS64(180,7,'%d',15);\r\n",unRow2Data);
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									GpuSend("SXY(0,0);\r\n");
									DELAY_US(UART2_DELAY);
									//chang 0404 13:50
									
									
								
								/*
									//chang 0331 07:07
									//////////////////////  //test 0405
									io_status.iostatus_16 = unRow3Data;
					
									sziostatus[0] = io_status.bit_16.b00 + '0';
									sziostatus[1] = io_status.bit_16.b01 + '0';
									sziostatus[2] = io_status.bit_16.b02 + '0';
									sziostatus[3] = io_status.bit_16.b03 + '0';
									sziostatus[4] = io_status.bit_16.b04 + '0';
									sziostatus[5] = io_status.bit_16.b05 + '0';
									sziostatus[6] = io_status.bit_16.b06 + '0';
									sziostatus[7] = io_status.bit_16.b07 + '0';
									sziostatus[8] = io_status.bit_16.b08 + '0';
									sziostatus[9] = io_status.bit_16.b09 + '0';
									sziostatus[10] = io_status.bit_16.b10 + '0';
									sziostatus[11] = io_status.bit_16.b11 + '0';
									sziostatus[12] = io_status.bit_16.b12 + '0';
									sziostatus[13] = io_status.bit_16.b13 + '0';
									sziostatus[14] = io_status.bit_16.b14 + '0';
									sziostatus[15] = io_status.bit_16.b15 + '0';
									sziostatus[16] = 0;
					
					
									//sprintf(lcd_disp_buf,"DS32(136,184,'%s',15);\r\n",(int*)(&sziostatus[0]));		//×´Ì¬  
									//GpuSend(lcd_disp_buf);									
									//DELAY_US(UART2_DELAY);
									
									GpuSend("W8UE(3);\r\n");
									DELAY_US(UART2_DELAY);
									
									//add test  0405
									GpuSend("DS64(1,7,'×´Ì¬',15);\r\n");
									DELAY_US(UART2_DELAY);
									//add test  0405
									
									sprintf(lcd_disp_buf,"DS32(136,24,'%s',15);\r\n",(int*)(&sziostatus[0]));									
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									GpuSend("SXY(0,0);\r\n");
									DELAY_US(UART2_DELAY);
									////////////////////////		//test 0405
									*/
					
					
									//if ((TotalNum > 10000000) && (TotalNum < 100000000))
									//{
										szTotalNum[0] = unRow3Data/10000000 + '0';				//¸ÄÎª×ÜÊý
										
										unRow3Data = unRow3Data % 10000000;
										szTotalNum[1] = unRow3Data/1000000 + '0';
										
										unRow3Data = unRow3Data % 1000000;
										szTotalNum[2] = unRow3Data/100000 + '0';
										
										unRow3Data = unRow3Data%100000;
										szTotalNum[3] = unRow3Data/10000 + '0';
										
										unRow3Data = unRow3Data%10000;
										szTotalNum[4] = unRow3Data/1000 + '0';
										
										unRow3Data = unRow3Data%1000;
										szTotalNum[5] = unRow3Data/100 + '0';
										
										unRow3Data = unRow3Data%100;
										szTotalNum[6] = unRow3Data/10 + '0';
										
										unRow3Data = unRow3Data%10;
										szTotalNum[7] = unRow3Data + '0';
										
										szTotalNum[8] = 0;
									//}
									
									unRow3Data = glTotalAllCnt;				
									
									//chang 0404 13:50
									//sprintf(lcd_disp_buf,"DS48(136,184,'%s',15);\r\n",(int*)(&szTotalNum[0]));					
									//GpuSend(lcd_disp_buf);
									//DELAY_US(UART2_DELAY);
									
									GpuSend("W8UE(3);\r\n");
									DELAY_US(UART2_DELAY);
									
									//sprintf(lcd_disp_buf,"DS48(136,7,'%s',15);\r\n",(int*)(&szTotalNum[0]));
									sprintf(lcd_disp_buf,"DS64(136,7,'%s',15);\r\n",(int*)(&szTotalNum[0]));
									
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									GpuSend("SXY(0,0);\r\n");
									DELAY_US(UART2_DELAY);									
									//chang 0404 13:50									
											//test 0405
									//add 0402
									return;
									//add 0402
							
									/*
									sprintf(lcd_disp_buf,"DS48(180,174,'%d',15);\r\n",unRow3Data);	//×´Ì¬  
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									*/
									//add 0330 17:09
									//chang 0331 07:07
										
										
									//TM1637_Write();
									
								//add 0330 11:57
								}
								//add 0330 11:57										
							
							} 
							else   // ÇÐÖ½µÈ´ý×´Ì¬
							{
								write_key_flag = 1;   /* ÓÐ°´¼ü£¬ÐèÒªÐ´°´¼üµ½Ö÷°å */

								// ÏÂ·¢ÇÐÖ½ÃüÁî
								gsKeyVal = KEY_IO3;
								gcKeyType = KEY_SHORT_UP_FLAG; 
								
								Main_disp_other = 0;  // ·µ»ØÏÔÊ¾Í²Êý   ·µ»ØÏÔÊ¾´êÌõ¸ùÊý
								qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
								cQzFlag = 0;
																
								//add
								GpuSend("CLS(0);\r\n");		
								DELAY_US(UART2_DELAY);	

								//DELAY_US(UART1s_DELAY);
							
/*								
								//GpuSend("W8DF(4,3,'116622663333');\r\n");	
								GpuSend("W8DF(5,3,'111662226633333');\r\n");
								//GpuSend("W8DF(4,3,'116622663344');\r\n");
								DELAY_US(UART2_DELAY);						
								//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
								//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
								GpuSend("W8MU(0,0,95,79,1,2);\r\n");
								DELAY_US(UART2_DELAY);		
*/				
								//add 0402
								if (cAlarmFlag)
								{
									szDispHz1 = "E--";
									szDispHz2 = NULL;
									szDispHz3 = NULL;
									//add 0322 by yi
									
									unRow1Data = 0;
									unRow2Data = 0;
									unRow3Data = 0;			

									//add 0406
									cHandOptionFlag = 0;
									//add 0406
									
									cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
									//del end
									
									if(!war_bell_step) 
										war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
									
									//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
									
									//add 0322 by yi
									cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
									
									GpuSend("BPIC(1,4,3,10);\r\n");
									DELAY_US(UART2_DELAY);
									DELAY_US(UART1s_DELAY);
									
									cFirstEnterAlarmFlag = 1;								
									
									sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									return;
								}									
								else
								{
								//add 0402
									
									#ifdef LCD_9COL
									GpuSend("SPG(26);\r\n");
									#else
									GpuSend("SPG(52);\r\n");
									#endif	
									DELAY_US(UART2_DELAY);
									
									DELAY_US(UART1s_DELAY);


									szDispHz1 = "Ö½³¤";
									szDispHz2 = "À¦Êý";
									szDispHz3 = "×ÜÊý";
								
									unRow1Data = unPaterLength;
									unRow2Data = unBunchNum;
									unRow3Data = TotalNum;		//10000;
									
									cMainUiFlag = 1;
									
									//add 0327 06:32
									cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
									//add 0327 06:32

								//	TM1637_Write();
									//add 0324								

									war_bell_step = 0;  // Çå³ý·äÃùÆ÷ÏìÉù
									cPauseBell = 0;
								}
								
								//return;
							}						
					break;		

				case KEY_IO3:   // ÇÐÖ½
				
				//add 0404 15:41
				//LEDShowFlashString("--Q2",0);				
				
				//chang 0406 11:58
				//cTestKeyStringName = "ÇÐÖ½";		//²Ù×÷
				cTestKeyStringName = NULL;
			//	GpuSend("SPG(62);\r\n");
			//	DELAY_US(UART2_DELAY);
				//chang 0406 11:58
				//add 0404 15:41

				//add 0330 12:04
				if (clookVerFlag == 1)	
					clookVerFlag = 0;
				//add 0330 12:04
				
				if (cFirstDispIoStatusFlag == 1)
				{
					cFirstDispIoStatusFlag = 0;				
				}
				
				qz_key_cnt = 0;
				qz_wait = 0;
				
				
				//add test
				if (qz_key_cnt == 0)
				{
					//add test  0326
					if (qz_wait == 0)  //Î´µÈ´ýÇÐÖ½
					{
						write_warning_flag = 1;   // ÏÂ·¢±¨¾¯ÏûÏ¢
						
						//½øÈëÇÐÖ½µÈ´ý
						//°´ÏÂÇÐÖ½¼ü£¬½«ÏÂÃæÕâÌõÓï¾ä: Main_disp_other = 4;ÆÁ±Îµô£¬»òÕß½« Main_disp_other = 0; ÇÐÖ½¼üµÄ²Ù×÷£¬¾ÍÓëÆäËû°´¼üÃ»Ê²Ã´²»Í¬¡£
						
						Main_disp_other = 4;	//ÇÐÖ½²Ù×÷,°´"È·¶¨¼ü"È·ÈÏ²Ù×÷£¬°´ÆäËûÈÎÒâ¼üÈ¡Ïû²Ù×÷
						qz_wait = 1; 
						
						//add 0404 0902					
						//cQzFlag = 1;								
						//add 0404 09:24
						
						//add 0406
						//cMainUiFlag = 0;		//cMainUiFlag
						//add 0406
						
					}			//Ô­: ²»ÊÇÇÐÖ½µÈ´ý,²»ÏìÓ¦						
				}
				
				//Èç¹ûÆÁ±ÎÉÏÃæµÄMain_disp_other = 4;,ÄÇÃ´ÔÚÏÂÃæÐè¼ÓÉÏÏÂÃæµÄÅÐ¶ÏÓï¾ä£¬"ÇÐÖ½¼ü"µÄ²Ù×÷£¬¾ÍÓëÆäËû°´¼üÃ»Ê²Ã´²»Í¬¡£
				//ÇÐÖ½¼ü²Ù×÷£¬ÏÔÊ¾¹Ì¶¨µÄÌáÊ¾½çÃæ,ÆÁ±ÎÏÂÃæµÄÓï¾ä
				//if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1)
				//{
				//	Main_disp_other = 0;
				//}
				
				//ÇÐÖ½²Ù×÷,°´"È·¶¨¼ü"È·ÈÏ²Ù×÷£¬°´ÆäËûÈÎÒâ¼üÈ¡Ïû²Ù×÷
				//ÇÐÖ½¼ü²Ù×÷£¬ÏÔÊ¾¹Ì¶¨µÄÌáÊ¾½çÃæ,ÆÁ±ÎÏÂÃæµÄÓï¾ä
				//DispHandOperation();
				
				/*
				if (cAlarmFlag)
				{
					if (cHandOptionFlag == 0)
					{
						cHandOptionFlag = 1;
					}
				//add 0404 09:24
						
					if (cMainUiFlag == 0)
					{
						//add 0403
						#ifdef LCD_9COL
						GpuSend("SPG(26);\r\n");
						#else
						GpuSend("SPG(52);\r\n");
						#endif		
						DELAY_US(UART2_DELAY);		

						DELAY_US(UART1s_DELAY);
						
						
						szDispHz1 = "Ö½³¤";
						szDispHz2 = "À¦Êý";
						szDispHz3 = "×ÜÊý";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;
						
						//del 0406 09:24
						//Main_disp_other = 0;		//ÆÁ±ÎµôÕâÌõÓï¾ä£¬Ê¼ÖÕ±£³Ö Main_disp_other = 4¡£ Ò»Ö±ÏÔÊ¾"ÇÐÖ½"µÄÌáÊ¾£¬Ö±µ½: ÆÁÄ»Ë¢ÐÂÊ±¼äµ½»ò°´ÏÂ"È·¶¨"¼ü£¬ÏÔÊ¾°æ±¾ºÅºÍ×´Ì¬£¬½«Main_disp_other¸³ÖµÎª0.
						//del 0406 09:24
						
						cMainUiFlag = 1;
						
						//add 0327 06:32
						cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
						
						cFirstEnterAlarmFlag = 0;
						//add 0403
					}
				}
				//add 0405
				else
				{
					if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
					{
						clookVerFlag = 0;
						cFirstDispIoStatusFlag = 0;
						
						cHandOptionFlag = 1;
					
						#ifdef LCD_9COL
						GpuSend("SPG(26);\r\n");
						#else
						GpuSend("SPG(52);\r\n");
						#endif		
						DELAY_US(UART2_DELAY);		

						DELAY_US(UART1s_DELAY);
						
						
						szDispHz1 = "Ö½³¤";
						szDispHz2 = "À¦Êý";
						szDispHz3 = "×ÜÊý";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;
						
						//del 0406 09:24
						//Main_disp_other = 0;
						//del 0406 09:24
						
						cMainUiFlag = 1;
						
						
						cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
						
						cFirstEnterAlarmFlag = 0;
					}
				}
				*/
				//add 0405
				//add 0404 0902
					
				
				//add 0406 12:45				
				//del 0404 09:02
				
				//chang 0407
				//if(cQzFlag == 0 )
				if(cQzFlag == 0 || cAlarmFlag == 1)
				//chang 0407
				{
					cQzFlag = 1;
					qz_wait = 1;
					
					if (cAlarmFlag)
						cAlarmFlag = 0;
					
					GpuSend("CLS(0);\r\n");		
					  DELAY_US(UART2_DELAY);
					  DELAY_US(UART1s_DELAY);
				}
				
				
				unRow1Data = 0;
				unRow2Data = 0;
				unRow3Data = 0;
					
				szDispHz1 = NULL;
				szDispHz2 = "ÇÐÖ½,Çë×¢Òâ!!!";
				//szDispHz3 = "Q2---";
				szDispHz3 = NULL;
			
				cMainUiFlag = 0;
				
				//chang 0406 12:05
				//sprintf(lcd_disp_buf,"DS48(75,90,'ÇÐÖ½,Çë×¢Òâ!!!',15);\r\n");		//Ô­Îªzt: 48µãÕó,ÏÖ¸ÄÎª:32µãÕó
				//GpuSend(lcd_disp_buf);
				//DELAY_US(UART2_DELAY);
				
				GpuSend("SPG(62);\r\n");
				DELAY_US(UART2_DELAY);
				DELAY_US(UART1s_DELAY);		//Õâ¸öÑÓÊ±ºÜÖØÒª
				DELAY_US(UART1s_DELAY);
				
				cKeyOptionFlash = 1;
				UpdateState = 0;
				
				//chang 0406 12:05				
				return;
				//del 0404 0902
				//add 0406 12:45
					
				
					
					/*
					//new add
					if (qz_key_cnt == 0)	//µÚ1´Î°´ÏÂ,±£³ÖÔ­À´,
					{
						qz_key_cnt = 1;
					}
					else if (qz_key_cnt == 1)  //µÚ2´Î°´ÏÂ,±¨¾¯¿ªÊ¼
					{
						qz_key_cnt = 2;
						
						//add by 0322 yi
						cAlarmFlag = 1;
						//cErrorAlarmValue = public_val.Err_Flag;
						//add by 0322 yi						
						//add
						
						cFirstEnterAlarmFlag = 0;						
						
						
						//ÖÃ´íÎó±êÖ¾,»¹ÊÇÔÚÑ­»·ÖÐ£¬ÅÐ´íÎó±êÖ¾
						public_val.Err_Flag = 6;			//ADD TEST
						
					} 
					else if (qz_key_cnt == 2)  //µÚ3´Î°´ÏÂ,½â³ý±¨¾¯, »Ø¸´Ô­À´
					{
						qz_key_cnt = 0;						
						
						//½â³ý±¨¾¯±êÖ¾
						cAlarmFlag = 0;
						public_val.Err_Flag = 0;	//»Ö¸´´íÎó±êÖ¾Îª0: Õý³£
						//cMainUiFlag = 1;		//»Ö¸´µ½Ö÷½çÃæ
						
						cFirstEnterAlarmFlag = 0;
						
						Main_disp_other = 0;  // ·µ»ØÏÔÊ¾Í²Êý   ·µ»ØÏÔÊ¾´êÌõ¸ùÊý
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
														
						//add
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
					
						
//						//GpuSend("W8DF(4,3,'116622663333');\r\n");	
//						GpuSend("W8DF(5,3,'111662226633333');\r\n");
//						//GpuSend("W8DF(4,3,'116622663344');\r\n");
//						DELAY_US(UART2_DELAY);						
//						//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
//						//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
//						GpuSend("W8MU(0,0,95,79,1,2);\r\n");
//						DELAY_US(UART2_DELAY);	


						#ifdef LCD_9COL
						GpuSend("SPG(26);\r\n");
						#else
						GpuSend("SPG(52);\r\n");
						#endif		
						DELAY_US(UART2_DELAY);
						
						DELAY_US(UART1s_DELAY);

						szDispHz1 = "Ö½³¤";
						szDispHz2 = "À¦Êý";
						szDispHz3 = "×ÜÊý";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;		//10000;
						
						cMainUiFlag = 1;
						
						//add 0327 06:32
						cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
						//add 0327 06:32

					//	TM1637_Write();
					}
					*/					
					//add 0326
					break;					
					
					
					//add 0325 21:42					
					case KEY_IO7:  // Æô¶¯
					LEDShowFlashString("   11",0);
				//add 0325
					//cTestKeyStringName = "Æô¶¯²Ù×÷";
					cTestKeyStringName = "Æô¶¯";		//²Ù×÷
					
					//if (qz_wait == 1 || cQzFlag == 1 )	//¼ÓÉÏÅÐ¶ÏÕâÌõÓï¾ä,°´ÏÂÇÐÖ½¼üµÄ²Ù×÷,¾Í»áÓëÆäËûÊÖ¶¯¼üÃ»Ê²Ã´²»Í¬,µ«°´ÏÂÇÐÖ½¼üÊ±£¬Ã»ÉèÖÃMain_disp_other = 4;¾Í²»ÓÃ¼ÓÉÏ¸ÃÓï¾ä
					//{
					//	Main_disp_other = 0;		
					//}
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 ||  qz_wait == 1 || cQzFlag == 1 )
					{
						Main_disp_other = 0;
					}
					DispHandOperation();
					
					/*
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )		//ÊÇ·ñ¿ÉÒÔÕâÑù¶¨Òå: ÇÐÖ½¼ü°´ÏÂÊ±£¬Î´°´È·¶¨¼ü£¬ÆäËüÊÖ¶¯¼ü²»ÄÜ²Ù×÷
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
								
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					*/
					//add 0405
				//add 0325
					break;

				case KEY_IO8:  // Í£Ö¹
					LEDShowFlashString("   12",0);
				  //add 0325
					cTestKeyStringName = "Í£Ö¹";		
				//add 0325
				
				if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1 )
				{
					Main_disp_other = 0;
				}
				DispHandOperation();
				
				/*
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
								
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					
					break;

				case KEY_RED:  // ÊÕÖ½
					LEDShowFlashString("   13",0);
					//add 0325
					cTestKeyStringName = "ÊÕÖ½";
				
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}				

					DispHandOperation();				
					
					/*
				
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
							
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO9:  // ·¢Ö½
					LEDShowFlashString("   14",0);
					//add 0325
					cTestKeyStringName = "·¢Ö½";

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					DispHandOperation();

					/*
				
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
					
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO2:  // ÍËÖ½
					LEDShowFlashString("   15",0);
					//add 0325
					cTestKeyStringName = "ÍËÖ½";
				//add 0325

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					DispHandOperation();

					/*
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52				
				
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO1:  // ½øÖ½
					LEDShowFlashString("   16",0);
					//add 0325
					cTestKeyStringName = "½øÖ½";

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}	
					DispHandOperation();

					
					
					
					/*
				
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
				
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO4:  // ÏÂÁÏ
					LEDShowFlashString("   17",0);
					//add 0325
					cTestKeyStringName = "ÏÂÁÏ";
				//add 0325
				
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}	

					DispHandOperation();				
					
					/*
				
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
				
				
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					
					break;

				case KEY_IO5:  // ´êÌõ
					LEDShowFlashString("   18",0);
					//add 0325
					cTestKeyStringName = "´êÌõ";
				//add 0325
				
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					
					DispHandOperation();				
						

					/*
				
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52
				
				//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO6:  // Í£Ö¹ËÍÖ½
					LEDShowFlashString("   19",0);
					//add 0325
					//cTestKeyStringName = "Í£Ö¹ËÍÖ½";
					cTestKeyStringName = "Í£Ö½";
				//add 0325
				
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					
					DispHandOperation();

					/*
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0404 09:52				
				
				//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
				
					break;
				
				
				//0329 10:07
//´ò½º
/*				
write_key_flag = 1;  

// ÏÂ·¢´ò½ºÃüÁî
gsKeyVal = KEY_IO3;
gcKeyType = KEY_SHORT_UP_FLAG; 

Main_disp_other = 0;
qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
cQzFlag = 0;
*/
				case KEY_DAJIAO_FAULT:		//DAJIAO_KEY_FAULT:		//KEY_DAJIAO:
					
					//GpuSend("DS32(1,7,'´ò½º',15);\r\n");				
					//DELAY_US(UART2_DELAY);
					LEDShowFlashString("   20",0);
					cTestKeyStringName = "´ò½º";		//¶Ì°´ÎªÊÖ¶¯´ò½ºÒ»´Î¡£
				
				/*
					//add 0403
					write_key_flag = 1;  
					// ÏÂ·¢´ò½ºÃüÁî
					gsKeyVal = KEY_IO3;		//×¢: ·¢KEY_IO3¼üÖµÊÇÇÐÖ½
					gcKeyType = KEY_SHORT_UP_FLAG; 
				*/

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					
					DispHandOperation();

					/*
				
					//Main_disp_other = 0;				
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
						cQzFlag = 0;
					}
					//add 0403
				
					//add 0404 09:24
					if (cAlarmFlag)
					{
						if (cHandOptionFlag == 0)
						{
							cHandOptionFlag = 1;
						}
					//add 0404 09:24
							
						if (cMainUiFlag == 0)
						{
							
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
							//add 0403
						}
					}
					//add 0405
					else
					{
						if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1))
						{
							clookVerFlag = 0;
							cFirstDispIoStatusFlag = 0;
							
							cHandOptionFlag = 1;
						
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;
				//0329 10:07				
			
				
/*
				case KEY_IO3:  // ÇÐÖ½
					LEDShowFlashString("   20",0);
					//add 0325
					cTestKeyStringName = "ÇÐÖ½²Ù×÷";
				//add 0325
					break;
					
				case KEY_GREEN:  // Ö½³¤
					LEDShowFlashString("   21",0);
					//add 0325
					cTestKeyStringName = "Ö½³¤ÉèÖÃ";
				//add 0325
					break;

				case KEY_BLUE:  // À¦Êý
					LEDShowFlashString("   22",0);	
					//add 0325
					cTestKeyStringName = "À¦ÊýÉèÖÃ";
				//add 0325
					break;
				*/					
					//add 0325
					
		    }
       
				
			if(Main_disp_other == 0)  /* ·µ»ØÖ÷½çÃæ */
			{
				TempCount = main_menu_cfg[0].m_rate;  
				TempDataB = DisplayItemNum;
			
				if(!public_val.Err_Flag)				//ÎÞ±¨´í   //Éè±¸ÊÇ·ñ³ö²î£¬¸ù¾Ý³ö´í±êÖ¾:public_val.Err_FlagÊÇ·ñÎª1.
				{
					//add by 0322
					//½â³ý±¨¾¯±êÖ¾
					//del 0407			//ÒòÎª£¬ÏÂÃæ²¢Ã»ÓÐ½«½çÃæ»Ö¸´µ½Ö÷½çÃæ
					//Õâ¸öÎ»ÖÃ£¬²»ÄÜËæÒâÖÃÎ»»òÇå0,·ñÔò£¬»á³öÏÖ×´Ì¬²»È·¶¨µÄÏÖÏó¡£(¼´:ÔÚÏÂ´ÎÓÐ¹ÊÕÏÊ±,cAlarmFlagÎª1,¶øcMainUiFlagÒ²Îª1µÄÏÖÏó)
					//»òÕß£¬²»ÆÁ±ÎÏÂÃæ2ÌõÓï¾ä£¬ÔÚÃ»ÓÐÈÎºÎ¹ÊÕÏµÄÇé¿öÏÂ£¬´ÓÕâÀï×Ô¶¯»Ö¸´µ½Ö÷½çÃæ£¬Ê×ÏÈ£¬Ðè½«;cFirstEnterAlarmFlag = 0, È»ºó£¬
					//¼ÓÉÏ»Ö¸´µ½Ö÷½çÃæµÄÄÇÐ©Óï¾ä£¬È»ºó£¬»¹ÐèÒªÔö¼ÓÒ»¸ö±äÁ¿: cFirstEnterNormalFlag.(»òÕß£¬Ê¹ÓÃcMainUiFlag±êÖ¾£¬ÅÐcMainUiFlag±êÖ¾,
					//Èç¹û:cMainUiFlag==0,ÔòÈÏÎªÊÇµÚÒ»´Î½øÐÐÕý³£Ö÷½çÃæ£¬ÏÔÊ¾¸ÃÖ÷½çÃæ£»·ñÔò£¬²»ÔÚ´ËË¢ÆÁÖ÷½çÃæ)
					//cAlarmFlag = 0;				
					//cMainUiFlag = 1;		//»Ö¸´µ½Ö÷½çÃæ	
					//del 0407
					//add by 0322 yi
					
					/*
					//add 0407 10:08
					//ÉÏÊöÉè¼ÆµÄ¾ßÌå³ÌÐòÈçÏÂ:
					///////////////////////////////////////////////////////////////////////
					 cAlarmFlag = 0;
					 cFirstEnterAlarmFlag = 0;		//ÎªµÚ¶þ´Î¹ÊÕÏÀ´ÁÙÊ±£¬Í¨¹ýÅÐ¶Ï:cFirstEnterAlarmFlag == 0,½øÈë¹ÊÕÏ½çÃæ×ö×¼±¸¡£
					 if (cMainUiFlag == 0)			//ÊÇµÚÒ»´ÎÓÐ¹ÊÕÏ×´Ì¬ÇÐ»»µ½Õý³£×´Ì¬
					 {
							cMainUiFlag = 1;
						 
							//add 0403
							#ifdef LCD_9COL
							GpuSend("SPG(26);\r\n");
							#else
							GpuSend("SPG(52);\r\n");
							#endif		
							DELAY_US(UART2_DELAY);		

							DELAY_US(UART1s_DELAY);
							
							
							szDispHz1 = "Ö½³¤";
							szDispHz2 = "À¦Êý";
							szDispHz3 = "×ÜÊý";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
							
							//cFirstEnterAlarmFlag = 0;
							//add 0403					 
					 }
					 //add 0407 10:08
					 ///////////////////////////////////////////////////////////////////////
					 */
					
					//unAlarmErrorSn = 0xff;		//³õÊ¼Öµ
					
					
					if(!public_val.Work_Pause_Flag)     /* ÎÞÔÝÍ££¬Ë¢ÐÂÏÔÊ¾µ±Ç°ÒÑÏ´±ýÊý */
					{
						if(public_val.Bell_Flag == YES)   /* ·äÃùÆ÷³¤ÔÝÍ£Ïì added for v19416 */
						{
							cPauseBell = 1;
							if(!war_bell_step) war_bell_step = 1;
						}
						else
						{
							Bell_flag &= 0xfd;
						}
						
						//ÎÞ±¨´í,ÎÞÔÝÍ£,Ë¢ÐÂÏÔÊ¾µ±Ç°´êÌõ¸ùÊý.
						DisplayValueTemp = (long)public_val.main_disp_val; 				//Ö÷ÏÔÊ¾Öµ£¬¾ÍÊÇ´êÌõ¸ùÊý
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[0],0);						
						
						//¸üÐÂ´êÌõ¸ùÊýµÄÏÔÊ¾Öµ
						unRow3Data = DisplayValueTemp;    //add 0324
						TotalNum = unRow3Data;				
					
						
					}
					else
					{
						LEDShowFlashString("P--",0);						//ÏÔÊ¾ÏîÄ¿Ãû³ÆÄÚÈÝ
						DisplayValueTemp = public_val.Work_Pause_Flag & 0x7F;
						if(public_val.Work_Pause_Flag & 0x80)
							{
								if(!war_bell_step) war_bell_step = 1;
							}
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[3],3);	

						cPauseBell = 1;

					}
				}
				else			//³ö´í
				{
			
					if ((public_val.Err_Flag == P_FINISH_LEARN1)
						|| (public_val.Err_Flag == P_FINISH_LEARN2)) //ÊÇÑ§Ï°Íê³É³ö´í£¬»¹ÊÇÕý³£±¨´í
					{
						//Ñ§Ï°Íê³ÉÒ²ÏÔÊ¾ËÍÖ½¼ÆÊý
						if(public_val.Bell_Flag == YES)   /* ·äÃùÆ÷³¤ÔÝÍ£Ïì added for v19416 */
						{
							cPauseBell = 1;
							if(!war_bell_step) war_bell_step = 1;
						}
						else
						{
							Bell_flag &= 0xfd;
						}
						
						DisplayValueTemp = (long)public_val.main_disp_val; 
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[0],0);
						
						//unAlarmErrorSn = 0xff;		//³õÊ¼Öµ
						
						
					}
					else		 // Õý³£±¨´í			(±¨¾¯´¦ÀíÔÚÕâÀï)
					{
						
						//ÏÔÊ¾´íÎó±êÖ¾
					//	LEDShowFlashString("E--",0);		/* modified by james at 2014-4-12 */
						
						
						//ÎªÁË²»ÈÃÕý³£±¨´íµÄÑ­»·£¬Ó°Ïì³ÌÐòµÄÕý³£ÔËÐÐ£¬ÏÈÔÝÊ±ÆÁ±ÎÕâ¶Î  0325 17:13
						//ÖÃ±¨¾¯±êÖ¾
						//add by 0322 yi
						cAlarmFlag = 1;
						
						unAlarmErrorSn = public_val.Err_Flag;
						
#if 1
						//ÇÐÖ½¼ü²âÊÔ¹Ø±Õ
						//Ê¹ÓÃÇÐÖ½¼ü²âÊÔ
						//add 0324 19:11
						//del start
						//del 0402		//
						//add 0403
						if (cHandOptionFlag == 1)		//ÒâË¼Îª£¬ÔÚÓÐÊÖ¶¯°´¼ü²Ù×÷Ê±£¬²»ÄÜË¢ÆÁµ½¹ÊÕÏ½çÃæ
						{
							
							/*
							//add 0404 16:17							
							if (qz_wait == 1)
							{
								if(!war_bell_step) 
									war_bell_step = 1;			//ÖÃÏì·äÃùÆ÷±êÖ¾			

									cPauseBell = 1;
									cQzFlag = 1;
									cMainUiFlag = 1;
									//del 0405
									//cTestKeyStringName = "ÇÐÖ½";
									//del 0405
							}							
							//add 0404 16:17
							*/
						}
						else
						{
						//add 0403
							if (cFirstEnterAlarmFlag == 0)							
							//if ((cFirstEnterAlarmFlag == 0) || (unAlarmErrorSn == 0xff))
							{
								//add 0327 14:17
								if (cMainUiFlag == 1 )	//°´È·¶¨¼ü£¬½øÈë¹ýÖ÷½çÃæ,²ÅÇåÆÁ£¬·ñÔò£¬²»ÇåÆÁ
								//if (cMainUiFlag == 1 || unAlarmErrorSn == 0xff)
								{
									unAlarmErrorSn = public_val.Err_Flag;		//ÊÇµÚÒ»´Î½øÈë
									//add 0406
									cMainUiFlag = 0;
									//add 0406
									
									//add 0327 14:17								
									//Èç¹ûÃ»ÓÐ½øÈëÖ÷½çÃæÄ£Ê½,Á¬ÐøµÚ2´Î°´ÏÂ"ÇÐÖ½"¼ü£¬²»ÇåÆÁ. Ö»ÓÐ°´ÏÂ"È·¶¨"¼ü,½øÈëÖ÷½çÃæºóµÚ2´Î°´"ÇÐÖ½"¼ü,²ÅÇåÆÁ
									GpuSend("CLS(0,0);\r\n");
									DELAY_US(UART2_DELAY);
									//DELAY_US(UART1s_DELAY);
									
									
									//GpuSend("CLS(0);\r\n");		
										//DELAY_US(UART2_DELAY);
									
									//add 0402									
									GpuSend("BPIC(1,4,3,10);\r\n");
									DELAY_US(UART2_DELAY);	
									DELAY_US(UART1s_DELAY);
									
									cFirstEnterAlarmFlag = 1;
									//cMainUiFlag = 0;
									
									DELAY_US(UART1s_DELAY);								
									
									war_bell_step = 1;
									cPauseBell = 0;								
									
									//chang 0406
									//sprintf(lcd_disp_buf,"PS%d(1,175,100,'E%d',15,399);\r\n",unAlarmErrorSn);	//100: 115
									sprintf(lcd_disp_buf,"PS48(1,175,100,'E%d',15,399);\r\n",unAlarmErrorSn);
									//chang 0406
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									
									
									//add 0405
									cKeyOptionFlash = 1;
									UpdateState = 0;
									//add 0405
									//return;
									//add 0402
									
									//add 0406
									szDispHz1 = "E--";
									szDispHz2 = NULL;
									szDispHz3 = NULL;
									//add 0322 by yi
									
									unRow1Data = 0;
									unRow2Data = 0;
									unRow3Data = 0;			

									//cMainUiFlag = 0;
									//add 0406
								}							
							}
							//add 0407
							else
							{
							//	cMainUiFlag = 0;
							//	unAlarmErrorSn = public_val.Err_Flag;

								//add 0405
							//	cKeyOptionFlash = 1;
							//	UpdateState = 0;
								//add 0405								
							}
							//add 0407
						}
						//del 0402
						//ÇÐÖ½¼ü²âÊÔ¹Ø±Õ
#endif					
						/*
						//add 0403
						if (cHandOptionFlag == 0)
						{
						//add 0403
							szDispHz1 = "E--";
							szDispHz2 = NULL;
							szDispHz3 = NULL;
							//add 0322 by yi
							
							unRow1Data = 0;
							unRow2Data = 0;
							unRow3Data = 0;			

							cMainUiFlag = 0;
						}
						*/
						//del end
						
						if(!war_bell_step) 
							war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
						
						//add 0322 by yi
						//±£´æ±¨¾¯³ö´íÐòºÅ:
						//unAlarmErrorSn = public_val.Err_Flag;		//ÒÆµ½Ç°Ãæ
						//add 0322 by yi
						
						//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
						//DisplayValueTemp = public_val.Err_Flag;
						
						/*
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[3],3);		//ÏÔÊ¾³ö´íÐòºÅ:2Î».
						//´íÎóÐòºÅ,¼´: disp_bufµÄ3,4Î», Ó¦ÓëszDispHz1·ÅÔÚÒ»Æð(ÔÚµÚÒ»ÐÐ).
						*/
						
						cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì						
						
					}
				}

				//DisplayState: Îª¶ÔÓ¦Ö÷²Ù×÷½çÃæ×´Ì¬
				//Main_disp_other:ÎªDisplayStateÏÂµÄ×Ó×´Ì¬				
				
				if(l_stop_poll)												//Ñ­»·ÏÔÊ¾
				if(++CycShowCount > main_menu_cfg[0].m_time)				//---------------ÖÜÆÚ¼ÆÊýÆ÷ÅÐ¶Ï     
				{
				    CycShowCount=0;
					DisplayItemNum++;
					TempCount = 0; 
				}
			}
			//ÔÚÖ÷½çÃæ,°´ÏÂenter¼üÊ±,ÉèÖÃ:Main_adj_key_delay = public_val.ms_timer; Main_disp_other = 2;
			else if(Main_disp_other == 2)    /* ÏÔÊ¾°æ±¾ºÅ */
			{
				//DisplayValueTemp = public_val.version;
//add 0321				
				unDownVerNum = public_val.version;
				//add by 0321
				
				//add 0402
				unRow1Data = unUpVerNum;			//×îÉÏÎª²Ù×÷°å°æ±¾ºÅ
				
				unRow2Data = unDownVerNum;			//ÎªÖ÷¿Ø°å°æ±¾ºÅ
				unIoStatus = public_val.io_read;
				//chang 0403
				//unRow3Data = unIoStatus;	
				unRow3Data = glTotalAllCnt;			//ÏÔÊ¾×Ü×ÜÊý
				//chang 0403
				
				if (clookVerFlag == 0)
					clookVerFlag = 1;
				
				cFirstDispIoStatusFlag = 0;
				
				//add 0402
				
				//³ÌÐò°æ±¾ºÅ£¬ÔÚÕâÀïÑ­»·ÏÔÊ¾¸üOK.
				/*
				//³ÌÐò°æ±¾ºÅÔÚ°´¼üÒÑÏÔÊ¾£¬ÇÒ°æ±¾ºÅÖµ²»ÐèÒªË¢ÐÂ
				sprintf(lcd_disp_buf,"DS48(180,1,'%d',15);\r\n",unRow1Data);		//ÉÏ²ã								
													GpuSend(lcd_disp_buf);
													//add
													DELAY_US(UART2_DELAY);


				sprintf(lcd_disp_buf,"DS48(180,86,'%d',15);\r\n",unRow2Data);		//µ×²ã		
								GpuSend(lcd_disp_buf);
								DELAY_US(UART2_DELAY);
				*/

				/*
				//add 0402
				sprintf(lcd_disp_buf,"DS32(136,184,'%s',15);\r\n",(int*)(&sziostatus[0]));		//×´Ì¬  
						//chang
				GpuSend(lcd_disp_buf);				
				DELAY_US(UART2_DELAY);
				//add 0402
				*/

				
				/*
				//°æ±¾ºÅ£¬²»Ê¹ÓÃ:DisplayBuff½øÐÐÏÔÊ¾.	¶øÊÇÒÔÕûÊýÐÎÊ½ÏÔÊ¾		
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);
				*/
				//²Ëµ¥¸üÐÂµÄÑÓÊ±Ê±¼ä: ÔÚÖ÷½çÃæÏÂ,°´"È·¶¨"¼ü,½øÈëÏÔÊ¾°æ±¾ºÅ,¾­¹ýÒ»¶¨ÑÓÊ±ºó,ÏÔÊ¾io¿Ú×´Ì¬.
				//ÑÓÊ±Ê±¼äµ½,ÏÔÊ¾io¿Ú×´Ì¬
				//test 0405
				if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY + 2)		//2000ms
				//if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY * 15 + 2)		//2000ms
				//test 0405
				{
					//ÑÓÊ±Ê±¼äµ½£¬×Ô¶¯ÏÔÊ¾io¿Ú×´Ì¬
					//test 0405
					Main_disp_other = 3;		//ÉèÎªÏÔÊ¾io¿Ú×´Ì¬
					//test 0405
					
					cFirstDispIoStatusFlag = 0;
				}
				//return;		//Ö±½Ó·µ»Ø£¬·äÃùÆ÷»áÒ»Ö±Ïì
			}			
			else if(Main_disp_other == 3)    /* ÏÔÊ¾´«¸ÐÆ÷µÈÊäÈëIOÖµ */
			{
				//DisplayValueTemp = (long)public_val.io_read;		//ÏÔÊ¾io×´Ì¬
				////DisplayValueTemp &= 0xff;
				//DisplayValueTemp &= 0xffff;
				
				//add 0322				
				unIoStatus = public_val.io_read;		//(unsigned int)DisplayValueTemp;				
				//add 0322 by yi
				
				//add 0402
				unRow3Data = unIoStatus;
				//add 0402
				
				//ÏÈÇåÆÁ
				//µÚÒ»´Îµ÷ÓÃ,ÏÈÇåÆÁ£¬ÔÙÏÔÊ¾. ÏÔÊ¾°æ±¾ºÅ£¬¾­¹ýÒ»¶¨ÑÓÊ±ºó£¬½øÈë×´Ì¬ÏÔÊ¾
				//¼ÓÒ»×´Ì¬ÏÔÊ¾±êÖ¾
				
				if (cFirstDispIoStatusFlag == 0)
				{
					cFirstDispIoStatusFlag = 1;
					
					GpuSend("CLS(0);\r\n");		//µÚÒ»´Î½øÈëµ÷ÓÃ
					  DELAY_US(UART2_DELAY);			
					//µ÷ÓÃÅú±àºÅÏÔÊ¾
					#if 0
					GpuSend("SPG(56);\r\n");	//60: ÏÔÊ¾ÎÄ×Ö
					#endif
					
					#if 1
					GpuSend("SPG(60);\r\n");	//60: ÏÔÊ¾ÎÄ×Ö
					#endif
					
					DELAY_US(UART2_DELAY);
					
					DELAY_US(UART1s_DELAY);
					DELAY_US(UART1s_DELAY);		//±ØÐë¼ÓÉÏÕâÌõÓï¾ä,·ñÔò£¬S11£¬S12ÎÞ·¨ÏÔÊ¾
					
					return;
				}
				
				cFirstEnterAlarmFlag = 0;
				
				/*/
				//·ÅÔÚÕâÀïµÄÈ±µãÊÇË¢ÆÁºÜ¿ì¡£
				//²»ÄÜ·ÅÔÚÕâÀïË¢ÆÁ,·ñÔò£¬·äÃùÆ÷»áÒ»Ö±Ïì				
				GpuSend("W8UE(1);");		//\r\n
				GpuSend("DS32(1,7,'S1',15);");
				if (sziostatus[0] - '0')
				{
					GpuSend("CIRF(65,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(65,40,25,2);");
				}
				GpuSend("DS32(100,7,'S2',15);");
				if (sziostatus[1] - '0')
				{
					GpuSend("CIRF(165,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(165,40,25,2);");
				}				
				GpuSend("SXY(0,0);");
				GpuSend("W8UE(2);");
				GpuSend("DS32(1,7,'S3',15);");			
				//CIRF(65,40,25,1);
				if (sziostatus[2] - '0')
				{
					GpuSend("CIRF(65,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(65,40,25,2);");
				}
				GpuSend("DS32(100,7,'S4',15);");
				//CIRF(165,40,25,1);
				if (sziostatus[3] - '0')
				{
					GpuSend("CIRF(165,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(165,40,25,2);");
				}
				GpuSend("SXY(0,0);");
				GpuSend("W8UE(3);");
				GpuSend("DS32(1,5,'S5',15);");
				//CIRF(65,40,25,1);
				if (sziostatus[4] - '0')
				{
					GpuSend("CIRF(65,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(65,40,25,2);");
				}
				GpuSend("DS32(100,5,'S6',15);");
				//CIRF(165,40,25,1);				
				if (sziostatus[5] - '0')
				{
					GpuSend("CIRF(165,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(165,40,25,2);");
				}
				GpuSend("SXY(0,0);");
				GpuSend("W8UE(4);");
				GpuSend("DS32(1,5,'S7',15);");
				//CIRF(65,40,25,1);
				if (sziostatus[6] - '0')
				{
					GpuSend("CIRF(65,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(65,40,25,2);");
				}
				GpuSend("DS32(100,5,'S8',15);");
				//CIRF(165,40,25,1);
				if (sziostatus[7] - '0')
				{
					GpuSend("CIRF(165,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(165,40,25,2);");
				}
				GpuSend("SXY(0,0);");
				GpuSend("W8UE(5);");
				GpuSend("DS32(1,7,'S9',15);");
				//CIRF(65,40,25,1);
				if (sziostatus[8] - '0')
				{
					GpuSend("CIRF(65,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(65,40,25,2);");
				}
				GpuSend("DS24(100,7,'S10',15);");
				//CIRF(165,40,25,1);
				if (sziostatus[9] - '0')
				{
					GpuSend("CIRF(165,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(165,40,25,2);");
				}
				GpuSend("SXY(0,0);");
				GpuSend("W8UE(6);");
				GpuSend("DS24(1,7,'S11',15);");
				//CIRF(65,35,25,1);
				if (sziostatus[10] - '0')
				{
					GpuSend("CIRF(65,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(65,40,25,2);");
				}
				GpuSend("DS24(100,7,'S12',15);");
				//CIRF(165,35,25,1);
				if (sziostatus[11] - '0')
				{
					GpuSend("CIRF(165,40,25,1);");
				}
				else
				{
					GpuSend("CIRF(165,40,25,2);");
				}
				GpuSend("SXY(0,0);\r\n");
				*/
				//return;
				
				/*
				//del 0402
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);			//Ô­À´ÒÔ5Î»ÕûÊýÐÎÊ½ÏÔÊ¾£¬²¢Ã»ÓÐ¶ÔÕâÐ©×´Ì¬Î»½øÐÐ½âÎö
				*/
				
				
			}
			else if (Main_disp_other == 4)    /* ÇÐÖ½µÈ´ý */
			{
					//LEDShowFlashString("Q2---",0);

					if(!war_bell_step) 
						war_bell_step = 1;			//ÖÃÏì·äÃùÆ÷±êÖ¾			

					cPauseBell = 1;
					
					//if (cQzFlag == 0)
					//{
					//	cQzFlag = 1;
					//	qz_wait = 1;
					//}
					
					//add 0404 09:19
					//cTestKeyStringName = "ÇÐÖ½";		//²Ù×÷	

					if (cHandOptionFlag == 0)
					{
						cHandOptionFlag = 1;
					}
					//add 0404 09:19
					
					//add 0404 09:07
					if (cHandOptionFlag == 1)
					{
						if (cMainUiFlag == 0)
						{
							cMainUiFlag = 1;		//ÏÂÃæÕâÌõÓï¾ä²»ÄÜ¼Ó,·ñÔò£¬±¨¾¯×´Ì¬ÏÂ£¬°´ÏÂ¸Ã¼ü£¬ÓëÆäËû¼ü»á²»Ò»Ñù
						}
					}
					/*
					else
					{
					//add 0404 09:07
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
							
						szDispHz1 = "";
						szDispHz2 = "ÇÐÖ½,Çë×¢Òâ!!!";		//
						//szDispHz3 = "Q2---";
						szDispHz3 = "";
					
						cMainUiFlag = 0;
					}
					*/
					
					
					/*
					//del 0404 15:47
					szDispHz1 = "Ö½³¤";
					szDispHz2 = "À¦Êý";
					szDispHz3 = "×ÜÊý";
				
					unRow1Data = unPaterLength;
					unRow2Data = unBunchNum;
					unRow3Data = TotalNum;
					
					Main_disp_other = 0;
					
					cMainUiFlag = 1;
					//del 0404 15:47					
					
					
					//add 0327 06:32
					cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
					//add 0327 06:32
					
					cFirstEnterAlarmFlag = 0;
					*/
					
					/*
					//ÒòÎª£¬ÔÚÖ÷Ñ­»·ÖÐ,ËùÒÔ£¬ÕâÀï²»ÄÜÖ±½Óµ÷ÓÃÏÔÊ¾£¬·ñÔò£¬Ë¢ÆÁÌ«¿ì
					//chang 0402
					//TM1637_Write();					
					sprintf(lcd_disp_buf,"DS32(75,86,'ÇÐÖ½,Çë×¢Òâ!!!',15);\r\n");		//Ô­Îªzt: 48µãÕó,ÏÖ¸ÄÎª:32µãÕó
					GpuSend(lcd_disp_buf);
					DELAY_US(UART2_DELAY);
					return;
					//chang 0402
					//return;		
					//add 0324
					*/
			}
	    }

		
		if (test_key_cnt == TEST_MODE_CNT)  /* ³¤°´¼ÆÊýºó½øÈë²âÊÔ²ÎÊýÉèÖÃ½çÃæ added by james for v19407 */
		{
			test_key_cnt = 0;
			DisplayState = ST_TEST;
			//ÖÃÎª²âÊÔÏÔÊ¾,ÉèÎªÇÐÖ½µÈ´ý
			Main_disp_other = 4;
			
			test_set_display(test_mode);
		}		

	#if 0
		if (learn_key_cnt == LEARN_MODE_CNT)  /* ³¤°´¼ÆÊýºó½øÈë²âÊÔ²ÎÊýÉèÖÃ½çÃæ added by james for v19407 */
		{
			learn_key_cnt = 0;
			DisplayState = ST_LEARN;
			Main_disp_other = 0;    
			
			LEDShowFlashString("P-- 0",0);

			public_val.Err_Flag = 0;   

			write_learn_mode_flag = 1;  
		}	
	#endif
		
	    break;


		case ST_PASSWORD://---------------------------------------------ÃÜÂëÊäÈë½çÃæ

	        switch(Key_Val)
		    {
		        case KEY_SEL:
		            DisplayItemNum = 0;			
							//DisplayItemNum0 = 0;
							//DisplayItemNum1 = 0;
		            DisplayState = ST_MAIN;						
						
						//add
								GpuSend("CLS(0);\r\n");		
								DELAY_US(UART2_DELAY);
						
				/*
								//GpuSend("W8DF(4,3,'116622663333');\r\n");	
								GpuSend("W8DF(5,3,'111662226633333');\r\n");
								//GpuSend("W8DF(4,3,'116622663344');\r\n");		
								DELAY_US(UART2_DELAY);						
								//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
								//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
								GpuSend("W8MU(0,0,95,79,1,2);\r\n");
								DELAY_US(UART2_DELAY);
				*/
				
				//add 0402
								if (cAlarmFlag)
								{
									szDispHz1 = "E--";
									szDispHz2 = NULL;
									szDispHz3 = NULL;
									//add 0322 by yi
									
									unRow1Data = 0;
									unRow2Data = 0;
									unRow3Data = 0;			

									cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
									//del end
									
									if(!war_bell_step) 
										war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
									
									//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
									
									//add 0322 by yi
									cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
									
									GpuSend("BPIC(1,4,3,10);\r\n");
									DELAY_US(UART2_DELAY);										
									cFirstEnterAlarmFlag = 1;								
									
									sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
									GpuSend(lcd_disp_buf);
									DELAY_US(UART2_DELAY);
									
									//add 0405
									cKeyOptionFlash = 1;
									UpdateState = 0;
										//add 0405
									return;
								}									
								else
								{
								//add 0402
									//GpuSend("SPG(26);\r\n");
									#ifdef LCD_9COL
									GpuSend("SPG(26);\r\n");
									#else
									GpuSend("SPG(52);\r\n");
									#endif
									DELAY_US(UART2_DELAY);
									
									DELAY_US(UART1s_DELAY);

									szDispHz1 = "Ö½³¤";
									szDispHz2 = "À¦Êý";
									szDispHz3 = "×ÜÊý";
								
									unRow1Data = unPaterLength;		//unPaterLength;
									unRow2Data = unBunchNum;		//unBunchNum;
									unRow3Data = TotalNum;	//TotalNum;
							
									cMainUiFlag = 1;
									
									//add 0327 06:32
									cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
									//add 0327 06:32
						
							//	    TM1637_Write();
								}
						//add by 0322 yi					
						
		         //   return;  
		            break;	
					
		        case KEY_REDUCE:
		        	if(--DisplayBitNum < 0)
		        	    DisplayBitNum=4;  

					//add 0329 19:32
					//	cDisplayBitNum = DisplayBitNum;
					//add
							    
					//	DELAY_US(UART2_DELAY);
		            break;	
		        case KEY_INCREASE:
		        	if( ++PasswordBuff[DisplayBitNum] > '9')
		        	    PasswordBuff[DisplayBitNum]='0'; 
									
		            break;	
		        case KEY_ENT:

				#if 0
	                if(PasswordBuff[0]=='0')//
	                if(PasswordBuff[1]=='0')//
	                if(PasswordBuff[2]=='0')//
	                if(PasswordBuff[3]=='0')//
	                if(PasswordBuff[4]=='0')//ÃÜÂë·ûºÏÒªÇó
	                {                
						/* ½øÈë²ÎÊý²Ëµ¥ */
					    pMenu = pItem[0].m_pcfg;	// Ö¸ÏòÊý¾Ý				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
						DisplayItemNum  = 0;

						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//½øÈëÏàÓ¦²Ëµ¥  
						
	                    return;
	                }
				#endif
				
					
						if(PasswordBuff[0]=='1')//
						if(PasswordBuff[1]=='0')//
						if(PasswordBuff[2]=='1')//
						if(PasswordBuff[3]=='0')//
						if(PasswordBuff[4]=='1')//ÃÜÂë·ûºÏÒªÇó
						{                   
						/* ½øÈë²ÎÊý²Ëµ¥ */
					  pMenu = pItem[0].m_pcfg;	// Ö¸ÏòÊý¾Ý				
						now_menu.menu_id = 1;			//==1,ÃÜÂë
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
										
						DisplayItemNum  = Menu0_Number;

						DisplayMenuTotal = Menu_Number[0];
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//½øÈëÏàÓ¦²Ëµ¥
						
						//ÖÃ²Ëµ¥±à¼­±êÖ¾
						//add by 0321
						cMenuEditFlag = 1;

						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						//MenuEditLook(DisplayItemNum);
						cMenuSn = 10;
						MenuEditLook(cMenuSn);
						
						/*
						sprintf(lcd_disp_buf,"SPG(%d);\r\n",(int)(DisplayItemNum + 21));
						GpuSend(lcd_disp_buf);
						DELAY_US(UART2_DELAY);						
						
						DELAY_US(UART1s_DELAY);
						*/						
						
						szDispHz1 = "1-";			//¸øÖ¸Õë¸³Öµ(µØÖ·)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "±¨¾¯³ÖÐøÊ±¼ä";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;					

						cMainUiFlag = 0;
						//add						
					
						//TM1637_Write();
					
	       }								            

				#ifndef SZ_03
	                if(PasswordBuff[0]=='1')//
	                if(PasswordBuff[1]=='1')//
	                if(PasswordBuff[2]=='1')//
	                if(PasswordBuff[3]=='1')//
	                if(PasswordBuff[4]=='1')//ÃÜÂë·ûºÏÒªÇó
	                {                   
						//learn_key_cnt = 0;
						DisplayState = ST_LEARN;
						Main_disp_other = 0;    
						
						LEDShowFlashString("P-- 0",0);

						public_val.Err_Flag = 0;   

						write_learn_mode_flag = 1;  								
						
										
	                    return;
	                }

				#endif
					
									//del 032511:42 //°´ÏÂ°´¼ü£¬´ÓÕâÀï·µ»Ø,ÎÞ·¨Ë¢ÆÁ
					//return;		
									//del 032511:42
					
		            break;			
		    }
			
			//ÃÜÂëÊäÈëÊ±£¬ÀûÓÃÕâÀïµÄ
	        if( flash_flag  && !long_input_flag  )												//ÉÁË¸¹¦ÄÜ
	        {	
	            LEDClearChar(DisplayBitNum, 1);		//½«¶ÔÓÚÎ»ÖÃÏÔÊ¾»º³åÇå0 (µ±Ç°ÊäÈëÎ»)
	        }
	        else
	        {
	            LEDShowFlashString(&PasswordBuff[0], 0);     //ÏÔÊ¾¸ÃÃÜÂëÊäÈë»º³åÇøµÄÊý¾Ý       
	        }	      
					
					//add
			TM1637_Write();		//ÀûÓÃºóÃæµÄË¢ÆÁ    //del 0325 11:44
					//add 0323
			    
			break;


		case ST_LEARN:

			switch(Key_Val) 
		    {
		        case KEY_SEL:  // Ñ¡Ôñ¼ü·µ»Ø
		        	Main_disp_other = 0;
					DisplayBitNum = 4;
					CycShowCount  = 0;
					now_menu.menu_id = 888;
					now_menu.event_flag = 1;
		            DisplayItemNum = 0;	
		            DisplayState   = ST_MAIN;											//·µ»ØÖ÷½çÃæ
		            
					war_bell_step = 0;  // Çå³ý·äÃùÆ÷ÏìÉù
					cPauseBell = 0;

					return;
					
					break;	
					
				default:
					break;
			}

			if ((public_val.Err_Flag == P_FINISH_LEARN1)
				|| (public_val.Err_Flag == P_FINISH_LEARN2))
			{
				if (public_val.Err_Flag == P_FINISH_LEARN1)    // Ñ§Ï°Íê³É
				{
					LEDShowFlashString("P-- 1",0);

					if(!war_bell_step) 
						war_bell_step = 1;						

					cPauseBell = 1;						
				}
				else if (public_val.Err_Flag == P_FINISH_LEARN2)    // Ñ§Ï°Íê³É
				{
					LEDShowFlashString("P-- 2",0);

					if(!war_bell_step) 
						war_bell_step = 1;						

					cPauseBell = 1;						
				}
			}	
			else
			{
				LEDShowFlashString("P-- 0",0);
				war_bell_step = 0;
				cPauseBell = 0;	
			}

			break;

			
		case ST_TEST:        /* ²âÊÔ²ÎÊýÉèÖÃ½çÃæÏìÓ¦ added by james for v19407 */
			switch(Key_Val)
		    {
		        case KEY_SEL:
		        	Main_disp_other = 0;
					DisplayBitNum = 4;
					CycShowCount  = 0;
					now_menu.menu_id = 888;
					now_menu.event_flag = 1;
		            DisplayItemNum = 0;	
		            DisplayState   = ST_MAIN;											//·µ»ØÖ÷½çÃæ
		            break;	

				case KEY_INCREASE:
					if (test_mode < 1)
					{
						test_mode++;

						write_test_mode_flag = 1;  /* Ð´²âÊÔÄ£Ê½µ½Ö÷°å */
					}
					test_set_display(test_mode);
					
					if (test_mode > 0)  /* ²âÊÔÄ£Ê½ */
					{
						//ResetDoorForTest();
					}					
					break;

				case KEY_REDUCE:
					if (test_mode > 0)
					{
						test_mode--;

						write_test_mode_flag = 1;  /* Ð´²âÊÔÄ£Ê½µ½Ö÷°å */						
					}
					test_set_display(test_mode);					
					break;

				case KEY_IO7:  // Æô¶¯
					LEDShowFlashString("   11",0);
				//add 0325
					cTestKeyStringName = "Æô¶¯";
				//add 0325
					break;

				case KEY_IO8:  // Í£Ö¹
					LEDShowFlashString("   12",0);
				  //add 0325
					cTestKeyStringName = "Í£Ö¹";
				//add 0325
					break;

				case KEY_RED:  // ÊÕÖ½
					LEDShowFlashString("   13",0);
					//add 0325
					cTestKeyStringName = "ÊÕÖ½";
				//add 0325
					break;

				case KEY_IO9:  // ·¢Ö½
					LEDShowFlashString("   14",0);
					//add 0325
					cTestKeyStringName = "·¢Ö½";
				//add 0325
					break;

				case KEY_IO2:  // ÍËÖ½
					LEDShowFlashString("   15",0);
					//add 0325
					cTestKeyStringName = "ÍËÖ½";
				//add 0325
					break;

				case KEY_IO1:  // ½øÖ½
					LEDShowFlashString("   16",0);
					//add 0325
					cTestKeyStringName = "½øÖ½";
				//add 0325
					break;

				case KEY_IO4:  // ÏÂÁÏ
					LEDShowFlashString("   17",0);
					//add 0325
					cTestKeyStringName = "ÏÂÁÏ";
				//add 0325
					break;

				case KEY_IO5:  // ´êÌõ
					LEDShowFlashString("   18",0);
					//add 0325
					cTestKeyStringName = "´êÌõ";
				//add 0325
					break;

				case KEY_IO6:  // Í£Ö¹ËÍÖ½
					LEDShowFlashString("   19",0);
					//add 0325
					cTestKeyStringName = "Í£Ö½";
				//add 0325
					break;

				case KEY_IO3:  // ÇÐÖ½
					LEDShowFlashString("   20",0);
					//add 0325
					cTestKeyStringName = "ÇÐÖ½";
				//add 0325
					break;
					
				case KEY_GREEN:  // Ö½³¤
					LEDShowFlashString("   21",0);
					//add 0325
					cTestKeyStringName = "Ö½³¤";
				//add 0325
					break;

				case KEY_BLUE:  // À¦Êý
					LEDShowFlashString("   22",0);	
					//add 0325
					cTestKeyStringName = "À¦Êý";		//ÉèÖÃ
				//add 0325
					break;
					
				default:
					break;
			}

		break;
			
	    case ST_MENU:			/* ²Ëµ¥Êý¾Ý²é¿´ */
	    { 
	        switch(Key_Val)
		    {
		        case KEY_SEL:
		        	Main_disp_other = 0;
							DisplayBitNum = 4;
							CycShowCount  = 0;
							now_menu.menu_id = 888;
							now_menu.event_flag = 1;
							DisplayItemNum = 0;	
							DisplayState   = ST_MAIN;											//·µ»ØÖ÷½çÃæ
					//DIS_OPTIONING=0;
						
						//Ö±½Ó·µ»ØÖ÷½çÃæ
									GpuSend("CLS(0);\r\n");		
									DELAY_US(UART2_DELAY);
						
				/*
									//GpuSend("W8DF(4,3,'116622663333');\r\n");	
									GpuSend("W8DF(5,3,'111662226633333');\r\n");	
									//GpuSend("W8DF(4,3,'116622663344');\r\n");	
									DELAY_US(UART2_DELAY);						
									//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
									//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
									GpuSend("W8MU(0,0,95,79,1,2);\r\n");
									DELAY_US(UART2_DELAY);
									*/
									
									//add 0402
									if (cAlarmFlag)
									{
										szDispHz1 = "E--";
										szDispHz2 = NULL;
										szDispHz3 = NULL;
										//add 0322 by yi
										
										unRow1Data = 0;
										unRow2Data = 0;
										unRow3Data = 0;			

										cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
										
										//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
										
										//add 0322 by yi
										cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
										
										GpuSend("BPIC(1,4,3,10);\r\n");
										DELAY_US(UART2_DELAY);										
										cFirstEnterAlarmFlag = 1;								
										
										sprintf(lcd_disp_buf,"PS48(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);
										return;
									}									
									else
									{
									//add 0402
										#ifdef LCD_9COL
										GpuSend("SPG(26);\r\n");
										#else
										GpuSend("SPG(52);\r\n");
										#endif		
										DELAY_US(UART2_DELAY);
										
										DELAY_US(UART1s_DELAY);
										
										szDispHz1 = "Ö½³¤";
										szDispHz2 = "À¦Êý";
										szDispHz3 = "×ÜÊý";
									
										unRow1Data = unPaterLength;
										unRow2Data = unBunchNum;
										unRow3Data = TotalNum;
										
										Main_disp_other = 0;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
										//add 0327 06:32
										
										cFirstEnterAlarmFlag = 0;
									
									//del 0325 11:58
									//	TM1637_Write();		//Ð´ÏÔÊ¾
										//del 0325 11:58
									}
						
		            break;	
						
		      case KEY_REDUCE:   //ÉÏ¼ü,²é¿´ÉÏÒ»²Ëµ¥Êý¾Ý
						
					if (now_menu.menu_id == 0)
					{
						if (DisplayItemNum-- == 0)			//DisplayItemNum: ²Ëµ¥Ïî¼ÆÊý
					        DisplayItemNum = DisplayMenuTotal-1; 
						//if	(cMenuLookCnt-- == 31)
						//	cMenuLookCnt = 50;
					}

					if (now_menu.menu_id == 1)
					{
			    	    if (DisplayItemNum-- == Menu0_Number)
			    	        DisplayItemNum = DisplayMenuTotal-1;
						//if	(cMenuLookCnt-- == 31)
						//	cMenuLookCnt = 50;
					}		
					
					
					now_menu.par_id = DisplayItemNum;
					now_menu.event_flag = 1;
					
					//add
					GpuSend("CLS(0);\r\n");
					DELAY_US(UART2_DELAY);
					
					if (cMenuSn-- <= 10)
						cMenuSn = 29;
					
					MenuEditLook(cMenuSn);
					//MenuEditLook(DisplayItemNum);
					/*
					sprintf(lcd_disp_buf,"SPG(%d);\r\n",(int)(DisplayItemNum + 21));
					GpuSend(lcd_disp_buf);
					DELAY_US(UART2_DELAY);
					
					DELAY_US(UART1s_DELAY);
					*/
					
					
					//GpuSend("SPG(cMenuLookCnt);\r\n");
					//GpuSend("SPG(DisplayItemNum + 31);\r\n");					
				//	sprintf(lcd_disp_buf,"DS24(1,150,'%d',15);\r\n",(int)DisplayItemNum);
				//	GpuSend(lcd_disp_buf);
				//	DELAY_US(UART2_DELAY);
					//DELAY_US(UART1s_DELAY);
					
					//add
		      
					break;	
					
		        case KEY_INCREASE:		//ÉÏ¼ü,²é¿´ÉÏÒ»²Ëµ¥Êý¾Ý

					if (now_menu.menu_id == 0)
					{
						if (++DisplayItemNum >= DisplayMenuTotal) 	//DisplayItemNum:²Ëµ¥Ïî¼ÆÊý¼Ó1
			                DisplayItemNum = 0;	
						//if	(++cMenuLookCnt >= 50)
						//	cMenuLookCnt = 0;
					}

					if (now_menu.menu_id == 1)
					{
			            if (++DisplayItemNum >= DisplayMenuTotal) 
			                DisplayItemNum = Menu0_Number;
						
						//if	(++cMenuLookCnt >= 50)
						//	cMenuLookCnt = 0;
					}					

					now_menu.par_id = DisplayItemNum;
					now_menu.event_flag = 1;
					
					//add
					GpuSend("CLS(0);\r\n");
					DELAY_US(UART2_DELAY);		
					
					if (cMenuSn++ >= 29)
						cMenuSn = 10;
					
					//MenuEditLook(DisplayItemNum);
					MenuEditLook(cMenuSn);
					/*
					sprintf(lcd_disp_buf,"SPG(%d);\r\n",(int)(DisplayItemNum + 21));
					GpuSend(lcd_disp_buf);
					DELAY_US(UART2_DELAY);
					
					DELAY_US(UART1s_DELAY);			
					*/
					
					
				//	sprintf(lcd_disp_buf,"DS24(1,150,'%d',15);\r\n",(int)DisplayItemNum);
				//	GpuSend(lcd_disp_buf);
				//	DELAY_US(UART2_DELAY);

					
					//GpuSend("SPG(cMenuLookCnt);\r\n");
					//GpuSend("SPG(DisplayItemNum + 31);\r\n");
					
					//sprintf(lcd_disp_buf,"SPG(%d);\r\n",DisplayItemNum + 31);
					//GpuSend(lcd_disp_buf);
					//DELAY_US(UART1s_DELAY);
					
					//add
		            break;	
					
		      case KEY_ENT:	
						
					zc_key_flag = 5;		//²Ëµ¥²é¿´È·¶¨¼ü£¬½øÈëÐÞ¸Ä					
					    
		       break;			
		    }        

			//µ±°´ÏÂÖ½³¤¼ü, »á½øÈëST_MENUÄ£Ê½,ÔÚ¸ÃÄ£Ê½,»á¶Ôzc_key_flag×Ô¶¯¼ÆÊý, µ±zc_key_flag>5Ê±£¬»áÄ¬ÈÏ°´ÏÂÈ·¶¨¼ü£¬¶ø×Ô¶¯½øÈë±à¼­½çÃæ(Ïàµ±ÓÚ°´ÏÂenter¼ü)
			if (now_menu.menu_id == 0 && zc_key_flag)
				{
					zc_key_flag ++;
				}

			if(zc_key_flag == 5)
				{
						zc_key_flag = 0;
					TempDataB = DisplayItemNum;				//²Ëµ¥ÐòºÅ£¬ÔÚÕâÀï¸³Öµ£¬È»ºó,Ê¹ÓÃ: pDataValue=&pMenu[TempDataB];, ÔÙ»ñÈ¡:pDataValue->m_data_valeµÄÖµ
					if (TY_D == TY_B  || 
						TY_D == TY_D  ||  
						TY_D == TY_H)									//ÊýÖµÀà
					{
						DisplayBitNum = 4;
						DataType=TY_D;
						TempInputData = *(int *)pDataValue->m_data_vale;
						
						//TempInputDataµÄÖµ£¬ÓëÏÂÃæµÄ:pDataValue->m_data_valeÊÇ·ñÎªÍ¬Ò»ÖµÄØ
						
						DisplayState=ST_DATA_INPUT;										//½øÈë²ÎÊýÊäÈë½çÃæ

						pDataValue=&pMenu[TempDataB];			//¸øµ±Ç°²Ëµ¥Ö¸Õë¸³Ö¸Õë³õÖµ
						DataType=TY_D;
						TempValue=DataToStr(pDataValue->m_data_vale,
					                        INT16_SIZE,
										    DEFAUT_POSE,
										    DEFAUT_POT,
										    DisplayBuff,0,DataType);				
						
						//InputDataLen: ¸Ã²ÎÊýµÄÄ¬ÈÏ³¤¶È (ËüÓÐ3Î»,2Î»»òÕß1Î»)
						InputDataLen = DEFAUT_POSE+DEFAUT_POT;    //3 + 0.  ËùÒÔ: InputDataLen == 3
						//DisplayBitNum = InputDataLen - 1; 
						PasswordBuff[0] = '0';
						PasswordBuff[1] = '0';
						//Êµ¼ÊÉÏ£¬ÔÚ·µ»ØµÄ´¦ÀíÊ±,²¢Ã»ÓÐÅÐ¶Ï¿Õ¸ñÊý,ËùÒÔ,¿ÉÈÏÎªÊÇ:return 0;
						//add by 0321
						//·µ»ØµÄ¿Õ¸ñÊýÎª: TempValue
						//TempValue = 0;		//Êµ¼ÊÉÏ·µ»ØÊ¼ÖÕÎª0.
						//add by 0321
						switch(InputDataLen)
						{	/*ÓÃÓÚ¸´ÖÆÏÔÊ¾»º³åÇøÊý¾Ý*/				//¸´ÖÆÏÔÊ¾»º³åÇøÊý¾Ýµ½ PasswordBuff(×Ö·û´®´æ´¢). ÔÚ²ÎÊý±à¼­Ê±È¡³ö    
						case 3:    //Êý¾Ý³¤¶ÈÎª3Î»
							PasswordBuff[2]=DisplayBuff[TempValue];      //
						//Èç¹ûÊÇ3Î»Êý¾Ý,µÚ3Î»(×î¸ßÎ»)Îª¿Õ¸ñ,ÓÃ'0'´úÌæ. ÏÔÊ¾»º³å×î×ó±ß´æ´¢: PasswordBuff[0]Êý¾Ý
							if(PasswordBuff[2] == ' ') PasswordBuff[2] = '0';
							PasswordBuff[3]=DisplayBuff[TempValue+1];
						 //µÚ4Î»Îª¿Õ¸ñ,ÓÃ'0'´úÌæ
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];		//×îºóÒ»Î»£¬²»¿ÉÄÜÎª¿Õ¸ñ		
						    break;			
						case 2:    //Êý¾Ý³¤¶ÈÎª2Î»
							PasswordBuff[2]=' ';    //½«µÚ3Î»ÉèÎª¿Õ¸ñ
							PasswordBuff[3]=DisplayBuff[TempValue+1];		//×î¶àÖ»ÓÐÒ»Î»¿Õ¸ñ
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];				
						    break;		
						case 1:    //Êý¾Ý³¤¶ÈÎª1Î», ¸ß2Î»ÉèÎª¿Õ¸ñ
							PasswordBuff[2]=' ';
							PasswordBuff[3]=' ';
							PasswordBuff[4]=DisplayBuff[TempValue+2];
							break;				
						}
						pr_long_input_flag = 0;
						DisplayState=ST_DATA_INPUT;										//½øÈë²ÎÊýÊäÈë½çÃæ
					}	
				}
				
			if(TempCount > 0)  	    
			{	
				TempCount-- ;
				break;          
			}
			
			//»ñÈ¡²Ëµ¥Ë¢ÐÂÆµÂÊ
			TempCount = main_menu_cfg[0].m_rate;  
			
			  //»ñÈ¡µ±Ç°ÏÔÊ¾²Ëµ¥Ìõ  ²Ëµ¥Ö¸Õë
			pDataValue=&pMenu[DisplayItemNum];	//pMenu: MENU_SHOW * pMenu;								//²Ëµ¥Ö¸Õë
		    TempInputData = *(int *)pDataValue->m_data_vale;   //»ñÈ¡²Ëµ¥ÌõÖ¸ÕëÊýÖµ       //Ê¹ÓÃlcdÊ±,Ò²¿ÉÖ±½ÓÏÔÊ¾¸ÃÊýÖµ

			//ÏÔÊ¾²ÎÊýÌõÄ¿ÐòºÅ,²¢ÏÔÊ¾¶ÔÓ¦ÐòºÅµÄ²ÎÊý
			if (now_menu.menu_id == 0)
			{
				LEDShowCaption(DisplayItemNum);		//»ñÈ¡¶ÔÓ¦²Ëµ¥±àºÅµÄ×ÖÐÎÂë
				
			}
			if (now_menu.menu_id == 1)
			{
				LEDShowCaption(DisplayItemNum - Menu0_Number);
			}
			
			//½«²Ëµ¥Ö¸ÕëÊýÖµ:TempInputData,×ª»»Îª×Ö·û´®.  ÏÔÊ¾»º³åÇøÊý¾Ý
			//ÕâÀïÊµ¼ÊÉÏÎª²Ëµ¥²é¿´Ê±µÄÏÔÊ¾Öµ
			TempValue=DataToStr(&TempInputData,   //pDataValue->m_data_vale
			                    INT32_SIZE,      //pDataValue->m_size,
								DEFAUT_POSE,
								DEFAUT_POT,
								DisplayBuff,0,TY_D );		
			//´Ó²»°üÀ¨¿Õ¸ñµÄDisplayBuff,·ÅÔÚÏÔÊ¾»º³åÇø2¿ªÊ¼µÄÎ»ÖÃ(×î¶à3Î»)
			LED_ShowNumber(&DisplayBuff[TempValue],2);    //TempValue:·µ»Ø¸ßÎ»¿Õ¸ñÊý, 2:±íÊ¾´ÓµÚ¶þÎ»¿ªÊ¼ÏÔÊ¾, 0,1ÎªÏÔÊ¾²Ëµ¥ÐòºÅ
	    }
			
			if (Key_Val != KEY_SEL)			
			{
				unRow1Data = 0;
				unRow2Data = 0;
				unRow3Data = 0;	
			}
			
	    break;

	    case ST_DATA_INPUT: /* Êý¾ÝÊäÈë»­Ãæ */
	    {
	        switch(Key_Val)
		    {
		        case KEY_SEL:   
					if (now_menu.menu_id == 0)
						{
							Main_disp_other = 0;
							DisplayBitNum = 4;
							CycShowCount  = 0;
							now_menu.menu_id = 888;
							now_menu.event_flag = 1;
							DisplayItemNum = 0;	
							DisplayState   = ST_MAIN;											//·µ»ØÖ÷½çÃæ
							
													
							//add 0325
							if (cZcKunShuFlag)
							{
								cZcKunShuFlag = 0;
								//cZcOptSelEnterRtnMainFlag = 1;
							}
							//add 0325							
					
							//Ö±½Ó·µ»ØÖ÷½çÃæ
							GpuSend("CLS(0);\r\n");		
							DELAY_US(UART2_DELAY);
					
							/*
							//GpuSend("W8DF(4,3,'116622663333');\r\n");	
							GpuSend("W8DF(5,3,'111662226633333');\r\n");		
							//GpuSend("W8DF(4,3,'116622663344');\r\n");		
							DELAY_US(UART2_DELAY);						
							//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
							//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
							GpuSend("W8MU(0,0,95,79,1,2);\r\n");
							DELAY_US(UART2_DELAY);
*/

							//add 0402
							if (cAlarmFlag)
							{
								szDispHz1 = "E--";
								szDispHz2 = NULL;
								szDispHz3 = NULL;
								//add 0322 by yi
								
								unRow1Data = 0;
								unRow2Data = 0;
								unRow3Data = 0;			

								cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
								//del end
								
								if(!war_bell_step) 
									war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
								
								//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
								
								//add 0322 by yi
								cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
								
								GpuSend("BPIC(1,4,3,10);\r\n");
								DELAY_US(UART2_DELAY);
								DELAY_US(UART1s_DELAY);
								cFirstEnterAlarmFlag = 1;								
								
								sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
								GpuSend(lcd_disp_buf);
								DELAY_US(UART2_DELAY);
								
								//add 0405
								cKeyOptionFlash = 1;
								UpdateState = 0;
								//add 0405
								return;
							}									
							else
							{
							//add 0402
								#ifdef LCD_9COL
								GpuSend("SPG(26);\r\n");
								#else
								GpuSend("SPG(52);\r\n");
								#endif		
								DELAY_US(UART2_DELAY);
								
								DELAY_US(UART1s_DELAY);

								szDispHz1 = "Ö½³¤";
								szDispHz2 = "À¦Êý";
								szDispHz3 = "×ÜÊý";
							
								unRow1Data = unPaterLength;
								unRow2Data = unBunchNum;
								unRow3Data = TotalNum;
								
								Main_disp_other = 0;
								
								cMainUiFlag = 1;
								
								//add 0327 06:32
								cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
								//add 0327 06:32
							
								//	TM1637_Write();		//Ð´ÏÔÊ
							}							
						}
					else
		        		DisplayState = ST_MENU;			
								
					
		            break;	
		        case KEY_REDUCE: //¼õÉÙ
		        	if(pMenu[DisplayItemNum].ext_property & 0x01)
		        		{
		        			;
		        		}
		        	else
		        		{
							TempInputData-=long_key_step;
							if(TempInputData<pDataValue->m_min) //ÅÐÊÇ·ñ³¬·¶Î§
								TempInputData = pDataValue->m_min; //pDataValue->m_min;   //ÏÞÖÆ³¬·¶Î§
						}
						
						//add
					//	GpuSend("CLS(0);\r\n");
					//	DELAY_US(UART2_DELAY);
					//add
						
		            break;	
		        case KEY_INCREASE: //Ôö¼Ó
		        	if(pMenu[DisplayItemNum].ext_property & 0x01)
		        		{
							;
		        		}
		        	else
		        		{
							TempInputData+=long_key_step;
		   					temp_max = pDataValue->m_max;
							if(TempInputData>temp_max) // temp_max
								TempInputData=temp_max;
							
						//add
					//	GpuSend("CLS(0);\r\n");
					//	DELAY_US(UART2_DELAY);
					//add
						}
		            break;	
						
						
		        case KEY_ENT:
		        	if(!long_input_flag)
	        		{			
								TempInputData=StrToData(PasswordBuff,5,DataType);		//´ÓPasswordBuffÖÐ»ñÈ¡Êý¾Ýµ½ :TempInputData
								//ÅÐÊÇ·ñ³¬³ö×î´ó»ò×îÐ¡·¶Î§
						if(TempInputData <= pDataValue->m_max && TempInputData >= pDataValue->m_min)    //pDataValueÔÚ½øÈëST_DATA_INPUTÖ®Ç°£¬°´È·¶¨¼üÊ±,¾ÍÒÑ¸³µ±Ç°²Ëµ¥µÄÖ¸Õë³õÖµ
						{
					        switch(INT16_SIZE)
						    {
						    case INT32_SIZE:
						        *(long *)pDataValue->m_data_vale = TempInputData;			//Ëù±à¼­²ÎÊý£¬´æÈë²Ëµ¥Êý¾Ý»º´æÇø
						        break;
						    case INT16_SIZE:
						        *(int *)pDataValue->m_data_vale = (int)TempInputData;
						        break;			
						    case INT8_SIZE:
						        *(char *)pDataValue->m_data_vale = (char)TempInputData;
						        break;			
							}	
							if(PasswordBuff[2] == '0')    //Ç°ÃæÊÇÎª¿Õ¸ñ,±äÎª0; ÕâÀïÎªÊÇ·ñ±äÎª¿Õ¸ñ
								{
									PasswordBuff[2] = ' ';
									if(PasswordBuff[3] == '0') PasswordBuff[3] = ' ';
								}
								//ÔÚledÖÐ,»ñÈ¡¿Õ¸ñ(0x20)µÄÎ»¶ÎÂëÎª0,ËùÒÔ²»ÏÔÊ¾.
								//µ«ÔÚlcdÖÐ¾Í²»Í¬,0x20µÄascÂëÖµÎª32.
							LED_ShowNumber(&PasswordBuff[2],2);

							//²Ëµ¥±à¼­½çÃæ,°´"È·¶¨"¼ü,´æ´¢ËùÓÐ²ÎÊý
							func_menu_update(0);		//Íê³É¶Ô²Ëµ¥Êý¾ÝµÄ´æ´¢£¬½«ËùÉèÖÃµÄ²ÎÊý£¬±£´æµ½eeprom
							
						}
	        		}

					  //if (now_menu.menu_id == 0)
						 if (now_menu.menu_id == 0 )
						  {
								Main_disp_other = 0;
								DisplayBitNum = 4;
								CycShowCount  = 0;
								now_menu.menu_id = 888;
								now_menu.event_flag = 1;
											DisplayItemNum = 0;	
								//Êµ¼ÊÉÏÔÚÕâÀï£¬Ëü»á×Ô¶¯·µ»ØÖ÷½çÃæ(µ«ÐèÒªÑÓÊ±)
											DisplayState   = ST_MAIN;											//·µ»ØÖ÷½çÃæ
								
								//add
								//if (cZcKunShuFlag == 1)
								//{
								
										//Çå²Ëµ¥±à¼­±êÖ¾
										//add by 0321
										cMenuEditFlag = 0;
										//add		
									
										//add 0325
									if (cZcKunShuFlag == 1)
									{
										cZcKunShuFlag = 0;
										//cZcOptSelEnterRtnMainFlag = 1;
										//add 0325
									}									
										//Ö±½Ó·µ»ØÖ÷½çÃæ
										GpuSend("CLS(0);\r\n");		
										DELAY_US(UART2_DELAY);
									
									
									/*
										//GpuSend("W8DF(4,3,'116622663333');\r\n");	
										GpuSend("W8DF(5,3,'111662226633333');\r\n");
										//GpuSend("W8DF(4,3,'116622663344');\r\n");	
										DELAY_US(UART2_DELAY);						
										//GpuSend("W8MU(1,1,95,70,1,2);\r\n");		//4  //2  \r\n
										//GpuSend("W8MU(1,1,79,79,1,2);\r\n");		//4  //2  \r\n
										GpuSend("W8MU(0,0,95,79,1,2);\r\n");
										DELAY_US(UART2_DELAY);
									*/
									
									//add 0402
									if (cAlarmFlag)
									{
										szDispHz1 = "E--";
										szDispHz2 = NULL;
										szDispHz3 = NULL;
										//add 0322 by yi
										
										unRow1Data = 0;
										unRow2Data = 0;
										unRow3Data = 0;			

										cMainUiFlag = 0;		//´úÌæ£º	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//ÖÃÏì·äÃùÆ÷³õÊ¼Öµ
										
										//¶Ô³ö´í±êÖ¾´¦Àí,ÏÔÊ¾³ö´íÐòºÅ
										
										//add 0322 by yi
										cPauseBell = 0;		//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
										
										GpuSend("BPIC(1,4,3,10);\r\n");
										DELAY_US(UART2_DELAY);
										DELAY_US(UART1s_DELAY);
										
										cFirstEnterAlarmFlag = 1;								
										
										sprintf(lcd_disp_buf,"PS%d(1,175,115,'E%d',15,399);\r\n",unAlarmErrorSn);
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);
										
										//add 0405
										cKeyOptionFlash = 1;
										UpdateState = 0;
										//add 0405
										return;
									}									
									else
									{
									//add 0402
										//GpuSend("SPG(26);\r\n");
										#ifdef LCD_9COL
										GpuSend("SPG(26);\r\n");
										#else
										GpuSend("SPG(52);\r\n");
										#endif
										DELAY_US(UART2_DELAY);
									
										DELAY_US(UART1s_DELAY);										
										

										szDispHz1 = "Ö½³¤";
										szDispHz2 = "À¦Êý";
										szDispHz3 = "×ÜÊý";
									
										//unRow1Data = 300;
										//unRow2Data = 350;	
										//unRow3Data = 10000;								
										pDataValue=&pMenu[0];
										unPaterLength = *(int *)pDataValue->m_data_vale;
										unRow1Data = unPaterLength;
										
										pDataValue=&pMenu[1];
										unBunchNum= *(int *)pDataValue->m_data_vale;
										unRow2Data = unBunchNum;
										
										unRow3Data = TotalNum;
										
										Main_disp_other = 0;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
										//add 0327 06:32
										
										//add 0404 11:54		//Ö±½Ó·µ»Ø
										
										
										if (cMainNotFlashFlag == 0 )
										{
											cMainNotFlashFlag = 1;
										}
										
										GpuSend("W8UE(1);\r\n");
										DELAY_US(UART2_DELAY);
										sprintf(lcd_disp_buf,"DS64(140,7,'%d',15);\r\n",unRow1Data);
										GpuSend(lcd_disp_buf);				
										DELAY_US(UART2_DELAY);
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);
										
										
										GpuSend("W8UE(3);\r\n");
										DELAY_US(UART2_DELAY);
										sprintf(lcd_disp_buf,"DS64(140,5,'%d',15);\r\n",unRow2Data);
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);
										
										
										//unRow3Data = TotalNum;				//ÔÚÖ÷Ñ­»·ÖÐ£¬²é¿´×ÜÊý±ä»¯Ö»ÄÜÕâÑù¸³Öµ
									
										szTotalNum[0] = unRow3Data/10000000 + '0';
										
										unRow3Data = unRow3Data % 10000000;
										szTotalNum[1] = unRow3Data/1000000 + '0';
										
										unRow3Data = unRow3Data % 1000000;
										szTotalNum[2] = unRow3Data/100000 + '0';
										
										unRow3Data = unRow3Data%100000;
										szTotalNum[3] = unRow3Data/10000 + '0';
										
										unRow3Data = unRow3Data%10000;
										szTotalNum[4] = unRow3Data/1000 + '0';
										
										unRow3Data = unRow3Data%1000;
										szTotalNum[5] = unRow3Data/100 + '0';
										
										unRow3Data = unRow3Data%100;
										szTotalNum[6] = unRow3Data/10 + '0';
										
										unRow3Data = unRow3Data%10;
										szTotalNum[7] = unRow3Data + '0';
										
										szTotalNum[8] = 0;
										
										unRow3Data = TotalNum;
										
										GpuSend("W8UE(4);\r\n");
										DELAY_US(UART2_DELAY);					
										
										//sprintf(lcd_disp_buf,"DS64(136,7,'%d',15);\r\n",(int*)(&szTotalNum[0]));
										
										sprintf(lcd_disp_buf,"DS64(136,7,'%s',15);\r\n",(int*)(&szTotalNum[0]));					
										GpuSend(lcd_disp_buf);
										DELAY_US(UART2_DELAY);						
										
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);						
										
										
										GpuSend("W8UE(2);\r\n");	
										DELAY_US(UART2_DELAY);
										
										if (cTestKeyStringName == NULL)
										{
										
										}
										else
										{
											sprintf(lcd_disp_buf,"DS48(30,65,'%s',15);\r\n",(int*)cTestKeyStringName);
											GpuSend(lcd_disp_buf);
											DELAY_US(UART2_DELAY);				
										}					
										GpuSend("SXY(0,0);\r\n");
										DELAY_US(UART2_DELAY);
										DELAY_US(UART1s_DELAY);
										
										//add 0404 12:07
										cKeyOptionFlash = 1;
										UpdateState = 0;
										//add 0404 12:07
										return;								
										//add 0404 11:54
								
										//TM1637_Write();		//Ð´ÏÔÊ¾                //¼ÓÉÏÕâÌõÓï¾ä£¬²Ù×÷ÇøÎÞ·¨¶¯Ì¬ÏÔÊ¾
									}
										
										//add 0324
					//					cMainUiDispFlag = 1;
										//add 0324
									
										//return;						//±ØÐë¼Ó·µ»Ø				
								//	}
							
							//}
							//add 0323
						}
					else
		        		DisplayState = ST_MENU;	
					
		            break;			
		    }
				
				
				//ÉÁË¸»òÏÔÊ¾ÏÂÒ»¸ö²ÎÊýµÄÐòºÅºÍÊý¾Ý
	        if( flash_flag && !long_input_flag )  //ÉÁË¸¹¦ÄÜ
	        {	
	            LEDClearChar(DisplayBitNum,1);
	        }
	        else			
	        {
						if (now_menu.menu_id == 0)
						{
							LEDShowCaption(DisplayItemNum);
						}
						if (now_menu.menu_id == 1)
						{
							LEDShowCaption(DisplayItemNum - Menu0_Number);
						}
						TempValue=DataToStr(&TempInputData,
														INT32_SIZE,
										5,
										0,
										DisplayBuff,0,TY_D );	

						PasswordBuff[2]=DisplayBuff[TempValue];
						if(PasswordBuff[2] == ' ') PasswordBuff[2] = '0';
						PasswordBuff[3]=DisplayBuff[TempValue+1];
						if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
						PasswordBuff[4]=DisplayBuff[TempValue+2];						
						LED_ShowNumber(&PasswordBuff[2],2);				//¸²¸ÇË¢ÐÂÏÔÊ¾µÄÊý¾Ý
						
						
						//add 0325 12:42
						if ((Key_Val == KEY_SEL) || (Key_Val == KEY_ENT) && (cZcKunShuFlag == 1))
						{
						
						}
						else	
						{		//add 0325 12:42
							unRow1Data = 0;
							unRow2Data = 0;
							unRow3Data = 0;
						}						
	        }
					
					//add 0325
		//			TM1637_Write();		//Ð´ÏÔÊ¾
					//add 0325
					
	    }	
	    break;	
		
		case ST_DISP_ERR:
		{
			if(public_val.ms_timer - now_menu.err_dsip_delay > DISP_ERR_TIMER )
			{
				DisplayState = now_menu.chang_next_step;
			}
			else
			{
				LEDShowFlashString("  Err",0);
			}
		}
		break;
		
		default :
	        DisplayState = ST_MAIN;
			//DIS_OPTIONING=0;
	    break;    				
		
	}


	if(!UpdateState)
	{
		UpdateState = 1;
		UpdateCountB = public_val.ms_timer;
	}
	else
	{
		if(public_val.ms_timer - UpdateCountB > 50)		//100
		{
			flash_flag = !flash_flag;		//ÉÁË¸±êÖ¾
			UpdateState = 0;
			
			//add 0326 20:21
			cFlashAlarmFlag = !cFlashAlarmFlag;
			//add 0326 20:21
			
			//0330 14:25
			cKeyOptionFlash = !cKeyOptionFlash;
			//0330 14:25
		}		
	}


	//¸ù¾Ýwar_bell_stepÎª1,Æô¶¯·äÃùÆ÷ÏìÍ£¼ä¸ô
	switch(war_bell_step)  /* ·äÃùÆ÷±¨¾¯´¦Àí */   //¶¨Ê±·äÃùÆ÷ÏìµÄÊ±¼äºÍ²»ÏìµÄÊ±¼ä
	{
		case 1:
			war_bell_delay = public_val.ms_timer;
			Bell_flag |= 0x02;
			war_bell_step = 10;
		break;
		case 10:
			iBellOnTime = (cPauseBell == 0) ? 500 : 50;		//cPauseBell: 1£¬·äÃùÆ÷³¤ÔÝÍ£Ïì
			//iBellOnTime = (cPauseBell == 0) ? 2000 : 50;
			if(public_val.ms_timer - war_bell_delay > iBellOnTime) //ÏìµÄÊ±¼ä
				{
					war_bell_delay = public_val.ms_timer;
					Bell_flag &= 0xfd;
					war_bell_step = 20;
				}
		break;
		case 20:
			iBellOffTime = (cPauseBell == 0) ? 2000 : 2000;			//·äÃùÆ÷³¤ÔÝÍ£²»Ïì
			//iBellOffTime = (cPauseBell == 0) ? 200 : 2000;			
			if(public_val.ms_timer - war_bell_delay > iBellOffTime) //²»ÏìµÄÊ±¼ä
				{
					war_bell_delay = public_val.ms_timer;
					war_bell_step = 30;
				}			
		break;
		case 30:
			if((!public_val.Err_Flag) 
			   && (!(public_val.Work_Pause_Flag & 0x80))
			   && (!public_val.Bell_Flag))  /* added for v19416 */
				{
					war_bell_step = 0;   /* ÎÞ±¨´í */
				}
			else
				{
					war_bell_step = 1;		//¸ù¾Ý´íÎó±êÖ¾public_val.Err_Flag£¬ÖØÖÃ·äÃùÆ÷Ïì³õÖµ
				}
		break;
	}
	
//#define BELL P25	
	
	if(Bell_flag)		//·äÃùÆ÷Ïì±êÖ¾  1,2 Ïì  0£º²»Ïì¡£(1:°´¼üÏì, 2: ±¨¾¯Ïì)
	{
		BELL = 1;
	}
	else
	{
		BELL = 0;
	}
	
	if (work_mode == DEV_PARA_SET)
	{
		cCurDisplayItemNum = DisplayItemNum;
	}

	//add 0404 14:28
	if (clookVerFlag == 1)		//°æ±¾²é¿´Ê±,io×´Ì¬ÓÐ¿ÉÄÜÎª0
	{	
		//add 0402
		if (cFirstDispIoStatusFlag == 1)
		{
			//add 0404 14:02
			//unIoStatus = public_val.io_read;			
			unRow3Data = unIoStatus;							
			io_status.iostatus_16 = unRow3Data;
			
			sziostatus[0] = io_status.bit_16.b00 + '0';
			sziostatus[1] = io_status.bit_16.b01 + '0';
			sziostatus[2] = io_status.bit_16.b02 + '0';
			sziostatus[3] = io_status.bit_16.b03 + '0';
			sziostatus[4] = io_status.bit_16.b04 + '0';
			sziostatus[5] = io_status.bit_16.b05 + '0';
			sziostatus[6] = io_status.bit_16.b06 + '0';
			sziostatus[7] = io_status.bit_16.b07 + '0';
			sziostatus[8] = io_status.bit_16.b08 + '0';
			sziostatus[9] = io_status.bit_16.b09 + '0';
			sziostatus[10] = io_status.bit_16.b10 + '0';
			sziostatus[11] = io_status.bit_16.b11 + '0';
			sziostatus[12] = io_status.bit_16.b12 + '0';
			sziostatus[13] = io_status.bit_16.b13 + '0';
			sziostatus[14] = io_status.bit_16.b14 + '0';
			sziostatus[15] = io_status.bit_16.b15 + '0';
			sziostatus[16] = 0;
			//add 0404 14:02
		}
	}
	//add 0404 14:28
	

	TM1637_Write();		//Ð´ÏÔÊ¾
	
}
		

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
¹¦ÄÜ£ºÕûÐÎ±ä×Ö·ûÐÍ

²ÎÊý£ºnBit:ÕûÊýÎ»¸öÊý£¬nPoint:Ð¡ÊýÎ»¸öÊý  ,nFlag==1ÕûÊýÎ»Ç°µÄ0ÒÔ¿Õ¸ñ²¹³ä

·µ»ØÖµ£º¸ßÎ»¿Õ¸ñÊý
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static char DataToStr(void * const pData,char size,char nBit,char nPoint,char *string,char nFlag,char nType)
{	
	long DataS32Bit;
	char spaces;
	char Temp,module;
	char SignZero;//¸ºÊý±êÖ¾Î»,  >0: 0,  <0: 1
    	
	if(size==0 || size>4)//ÅÐ¶ÏsizeµÄºÏÀíÐÔ
	    return 0;
	size--;    //1
//	if( ( *((char *)(pData)+size) ) & 0x80 )//ÅÐ¶Ï×î¸ßÎ»ÊÇ·ñÎª1
//	    SignZero=1;
//	else
	    SignZero=0;	//ÕýÊý
	    
    DataS32Bit=0;  //¸³³õÖµÎª0
	if(size==3)//LONGÐÍÊý¾Ý
	{			    
		if(SignZero)
		    *((long *)&DataS32Bit) = 0 - *((long *)pData);
		else
		    *((long *)&DataS32Bit) = *((long *)pData);
	}	
	else if(size==1)//INTÐÍÊý¾Ý   (ÔÚÕâÀïÊµ¼ÊÎªÕûÐÍ)
	{
	    if(SignZero)
		    *((int *)&DataS32Bit) = 0 - *((int *)pData);						
		else
		    *((int *)&DataS32Bit) = *((int *)pData);	//ÎªÕûÊý ---------   »ñÈ¡pDataµÄÖµµ½DataS32Bit
	}	
	else if(size==0)//charÐÍÊý¾Ý
	{
	    if(SignZero)
		    *((char *)&DataS32Bit) = 0 - *((char *)pData);
		else
		    *((char *)&DataS32Bit) = *((char *)pData);
	}
	else
		return 0;	
	
	switch(nType){
	case TY_B:
		module=2;
		break;
	case TY_D:
	case TY_RD:
	case TY_D_A:
	case TY_D_L:
        module=10;		//ÔÚÕâÀï,nType == TY_D (10½øÖÆ), ËùÒÔ, module=10;
		break;
	case TY_H:
        module=16;
		break;
	}

    spaces = 3-(nPoint+nBit);     //»ñÈ¡ spacesµÄÖµ(Îª0). ÔÚÕâÀïnBit==3, nPoint = 0
	Temp = nPoint+nBit;     //Temp = 3
	if(Temp<3)
	{
	    nBit += (3-Temp);
		Temp=3;
	}
		
	  do
    {
        if(Temp != nBit || nFlag)  // Temp == nBit,  nFlag==0
        {
					  //µÚ¶þ´ÎÑ­»·¼°ÒÔºó, È¡DataS32BitµÄÖµ¸østring[Temp].
            string[Temp]='0'+(char)(DataS32Bit%(module));
            DataS32Bit/=module;
        }
        else 
				{
					if(nPoint != 0)   //nPoint==0
							string[Temp]='.';
					else
						string[Temp]=' ';  //µÚÒ»´ÎÑ­»·(Temp==3): string[3] = ' '.
				}
        
    }while( Temp-- >0 );
        
	if(nFlag)//nFlag==1ÕûÊýÎ»Ç°µÄ0ÒÔ¿Õ¸ñ²¹³ä
	{
	    if(SignZero && spaces<3)
		    string[spaces]='-';
		return 0;
  }
	
	
	Temp=0;
	spaces=0;
	do
	{
		 //ÅÐ¶Ï»ñÈ¡µ½string[Temp],ÆäÖÐÎª0µÄÊý.
		 if(string[Temp]=='0' && (Temp + 1 < nBit)) 
		 {
			 string[Temp]=' ';     //string[0]´æ´¢µÄÊÇ×î¸ßÎ»µÄÖµ£¬string[2]´æ´¢×îµÍÎ»µÄÖµ,ÏÈ´Ó×î¸ßÎ»¿ªÊ¼ÅÐ¶Ï
			 spaces++;
		 }
		 else
		 {
					 break;
		 }
		 Temp++;
   }while(1);
	
	if(SignZero)
	{
	    string[--spaces]='-';   //Èç¹ûÊÇ¸ºÊý,¼ÓÉÏ'-'
	}
	
	Temp=nPoint+nBit-spaces;
	
	if(Temp<3)
	    spaces -= (3-Temp);
	return spaces;        
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++
¹¦ÄÜ£ºÐèÒªÒ»¸öº¯Êý½«×Ö·û»¯ÕûÐÎ

²ÎÊý£ºlen Îª×Ö·û³¤¶È

·µ»ØÖµ£ºÎÞ
++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static long StrToData(char *string,char len,char nType)
{	
    unsigned long x,bi;     
    char i,module;

    switch(nType){
	case TY_B:
		module=2;
		break;
	case TY_D:
        module=10;
		break;
	case TY_H:
        module=16;
		break;
	}
	x=0;
	bi=1;
	for(i=len;i>0;)
    {
        i--;
		if(string[i] >= '0' && string[i]<=0x3F)
		{
		   x+=(string[i] - '0')*bi;
		   bi*=module;
        }
	}
	if(string[0]=='-' || string[1]=='-' || string[2]=='-' )//¸ºÊý
	    return (0-x);
	else
	    return x;

}



void MenuEditLook(unsigned char cDisplayItemNum)
{
	
	//unsigned char DisplayItemNum;
	switch(cDisplayItemNum)
	{
		case 10:
			GpuSend("SPG(31);\r\n");		
		break;
	
		case 11:
			GpuSend("SPG(32);\r\n");		
		break;
		
		case 12:
			GpuSend("SPG(33);\r\n");		
		break;
		
		case 13:
			GpuSend("SPG(34);\r\n");		
		break;
		
		case 14:
			GpuSend("SPG(35);\r\n");		
		break;
		
		case 15:
			GpuSend("SPG(36);\r\n");		
		break;
		
		case 16:
			GpuSend("SPG(37);\r\n");		
		break;
		
		case 17:
			GpuSend("SPG(38);\r\n");		
		break;
		
		case 18:
			GpuSend("SPG(39);\r\n");		
		break;
		
		case 19:
			GpuSend("SPG(40);\r\n");		
		break;
		
		
		case 20:
			GpuSend("SPG(41);\r\n");		
		break;
		
		
		case 21:
			GpuSend("SPG(42);\r\n");		
		break;
		
		case 22:
			GpuSend("SPG(43);\r\n");		
		break;
		
		case 23:
			GpuSend("SPG(44);\r\n");		
		break;
		
		case 24:
			GpuSend("SPG(45);\r\n");		
		break;
		
		case 25:
			GpuSend("SPG(46);\r\n");		
		break;
		
		case 26:
			GpuSend("SPG(47);\r\n");		
		break;
		
		case 27:
			GpuSend("SPG(48);\r\n");		
		break;
		
		case 28:
			GpuSend("SPG(49);\r\n");		
		break;
		
		case 29:
			GpuSend("SPG(50);\r\n");		
		break;		
	
	}
	
	DELAY_US(UART1s_DELAY);
}



					
void DispHandOperation()
{
	/*
	if (qz_wait == 1 || cQzFlag == 1 )		//ÊÇ·ñ¿ÉÒÔÕâÑù¶¨Òå: ÇÐÖ½¼ü°´ÏÂÊ±£¬Î´°´È·¶¨¼ü£¬ÆäËüÊÖ¶¯¼ü²»ÄÜ²Ù×÷
	{
		qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
		cQzFlag = 0;
	}
	*/
	//add 0404 09:52
				
	//add 0404 09:24
	if (cAlarmFlag)
	{
		if (cHandOptionFlag == 0)
		{
			cHandOptionFlag = 1;
		}
	//add 0404 09:24
			
		if (cMainUiFlag == 0)
		{
			
			//GpuSend("CLS(0);\r\n");		
			//DELAY_US(UART2_DELAY);
			
			//add 0403
			#ifdef LCD_9COL
			GpuSend("SPG(26);\r\n");
			#else
			GpuSend("SPG(52);\r\n");
			#endif		
			DELAY_US(UART2_DELAY);		

			DELAY_US(UART1s_DELAY);
			
			
			szDispHz1 = "Ö½³¤";
			szDispHz2 = "À¦Êý";
			szDispHz3 = "×ÜÊý";
		
			unRow1Data = unPaterLength;
			unRow2Data = unBunchNum;
			unRow3Data = TotalNum;
			
			//Main_disp_other = 0;
			
			cMainUiFlag = 1;
			
			//add 0327 06:32
			cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
			
			cFirstEnterAlarmFlag = 0;
			//add 0403
		}
	}
	//add 0405
	else
	{
		if ((clookVerFlag == 1) || (cFirstDispIoStatusFlag == 1)  || (qz_wait == 1) || (cQzFlag == 1))
		{
			//GpuSend("CLS(0);\r\n");		
			//DELAY_US(UART2_DELAY);
			
			clookVerFlag = 0;
			cFirstDispIoStatusFlag = 0;
			
			cHandOptionFlag = 1;
		
			#ifdef LCD_9COL
			GpuSend("SPG(26);\r\n");
			#else
			GpuSend("SPG(52);\r\n");
			#endif		
			DELAY_US(UART2_DELAY);		

			DELAY_US(UART1s_DELAY);
			
			
			szDispHz1 = "Ö½³¤";
			szDispHz2 = "À¦Êý";
			szDispHz3 = "×ÜÊý";
		
			unRow1Data = unPaterLength;
			unRow2Data = unBunchNum;
			unRow3Data = TotalNum;
			
			//Main_disp_other = 0;
			
			cMainUiFlag = 1;
			
			
			cMainNotFlashFlag = 0;		//µÚÒ»´Î½øÈë, È«²¿ÏÔÊ¾
			
			cFirstEnterAlarmFlag = 0;
		}
	}
	
	if (qz_wait == 1 || cQzFlag == 1 )		//ÊÇ·ñ¿ÉÒÔÕâÑù¶¨Òå: ÇÐÖ½¼ü°´ÏÂÊ±£¬Î´°´È·¶¨¼ü£¬ÆäËüÊÖ¶¯¼ü²»ÄÜ²Ù×÷
	{
		qz_wait = 0;   // ÇåµÈ´ý×´Ì¬								
		cQzFlag = 0;
	}
}

#endif

