/**
 * Copyright (c) 2012, 长沙晗光电子科技有限公司
 * All rights reserved.
 *
 * FILE:   LED_Display.c
 * LED显示程序
 *
 * @file LED_Display.h led_driver.h
 * 头文件.
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
#define DISP_ERR_TIMER 1000 //延时数据修改失败时间 和 错误显示时间

//#define NO_KEY_RETURN_MAIN 15000 //00000菜单无按键返回主页面延时
#define NO_KEY_RETURN_MAIN 30000 //00000菜单无按键返回主页面延时

#define TEST_MODE_CNT    40
//#define LEARN_MODE_CNT   20
#define LEARN_MODE_CNT   30

#define NULL		0


xdata unsigned char cZcKunShuFlag;


extern char eeprom_en;
extern xdata unsigned int disp_buf[5];

//add by 0322
extern xdata unsigned char work_mode;


//以下3项通信获取
extern xdata unsigned int  unUpVerNum;
extern xdata unsigned int  unDownVerNum;
extern xdata unsigned int  unIoStatus;

extern xdata unsigned int  unPaterLength;	//设置获取
extern xdata unsigned int  unBunchNum;		//设置获取
extern xdata unsigned long  TotalNum;		//int 通信获取

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
 |                                函数声明                                    |
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




//char xdata DIS_OPTIONING;											///<【不允许插引工作标志。0：允许插引；1：不允许插引】
char xdata DIS_ERR_FLGA = 0;

//内部变量定义
//static xdata unsigned int  l_state;								//显示画面状态

//static xdata unsigned long  l_input_value;						//参数修改画面输入值
static xdata char  l_stop_poll;									//停止循环显示

//static xdata char DisplayStateOld;								//记忆画面状态

//static xdata char  DisplayMainID;									//主画面序号
//static xdata char  DisplayMenuID;									//菜单画面序号

//static xdata char  DisplayMainTotal;								//主画面总数
//static xdata char  MenuTotals;									//主菜单画面总数


//static xdata char  DisplayMenu0Total;								//菜单画面总数
//static xdata char  DisplayMenu1Total;								//菜单画面总数
static xdata char  DisplayMenuTotal;								//菜单画面总数

//static xdata char  DisplayItemNum0;								//菜单0显示项目序号
//static xdata char  DisplayItemNum1;								//菜单1显示项目序号
static xdata char  DisplayItemNum;								//显示项目序号
static xdata char  DisplayBitNum;									//显示位号

//static xdata char  MenuUpdateState;								//菜单显示更新状态
static xdata char  PasswordBuff[5];								//密码数据缓冲区
static xdata char  DisplayBuff[10];

int  DisplayState;											//画面状态


static xdata const MAIN_SHOW * pMain;								//主画面数据指针
static xdata const MENU_SHOW * pMenu;								//菜单指针
static xdata const MENU_ITEM * pItem;								//菜单项目指针

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

xdata char test_mode = 0;   /* 测试模式标志: 0正常工作模式，1测试模式 */


xdata char uart_send_buf[10 + (MENU_ID_00000_MAX_ITEM << 1)];




//菜单配置界面
//参数组定义
//#define DEBUG_MENU_GS_VAL 30
//#define USE_MENU_NUM 16

static xdata MENU_SHOW Menu00000[MENU_ID_00000_MAX_ITEM];

xdata char Menu_Number[MENU_NUM] = {MENU_ID_00000_MAX_ITEM};   //机器参数个数  最大30
xdata char Menu0_Number = 10, Menu1_Number = 20;		//

extern xdata char qz_key_cnt;		//切纸键计数




/* 参数取值范围 */
static xdata STRU_PARAM_RANGE Default_Param_Range[MENU_ID_00000_MAX_ITEM] = 
{		
	{0, 999},  /* 步进启动速度 */	
	{0, 999},  /* 步进停止滑行距离 */			
	{0, 999},    /* 测试模式 */	

};

static xdata int iDefaultParm[MENU_ID_00000_MAX_ITEM] = 
{
	100,   /* 步进启动速度 */
	100,  /* 步进停止滑行距离，单位脉冲 */	
	1,    /* 测试模式 */
};

/*--------------菜单设置画面 数据配置-------------*/
code MENU_ITEM MenuItem[]={
  //菜单序号,菜单地址,密码 数据存储首地址 最大数据个数
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
    //显示的字符串,数据显示类型,数据地址,函数名
    {"",TY_D_A,0,&(public_val.main_disp_val),INT32_SIZE,5,0,99999,0},		
    {0,0,0}
};

/*-------------------------主画面数据配置,全局变量----------------------------*/
xdata const MAIN_PIC main_menu_cfg[]={
    {4,2,MainItem,MenuItem},
};

/*
xdata const MAIN_PIC main_menu_cfg[]={
    {4,2,MainItem,MenuItem},
		};		//4:每点显示次数,10:菜单刷新频率(貌似没用到) 
*/

static code const char LED_ASCII[128]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //0x0
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //0x1
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
				            0x00,0xFE,0xB0,0xED,0xF9,0xB3,0xDB,0xDF, //0x2
				            0xF0,0xFF,0xFB,0x00,0x00,0x40,0x80,0x00, 
				            0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07, // 起始48:0-7
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

//unsigned short usParamNum = 30;   /* 主板的配置参数个数，最大为30个 */

xdata int gsKeyVal = 0;    /* 按键值 */
xdata char gcKeyType = 0;  /* 按键类型: 0短按, 1长按下，2长按抬起 */

//xdata char gcKeyLongUpFlag = 0;    /* 长按抬起标志:0未抬起，1抬起 */
//xdata char gcKeyLongDownFlag = 0;  /* 长按按下标志:0未按下，1按下 */

xdata _U_BIT_PARAM unKey;

						   
//static bit 	bDaJiao = 1;					   
 

xdata char speed_mode = 1;   //速度模式: 1高速， 0低速

xdata char qz_wait = 0;      //等待切纸确认: 0未等待，1等待
						   
#define uchar unsigned char


//add 0331					   
typedef union i_status_16
//定义共用体
{ 
	unsigned int iostatus_16; 
	struct b_16
	{			uchar b00:1;    //位域  
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
功能：显示字符串。

参数：buff:欲发送的数据缓冲区；x:坐标

返回值：无。

说明：
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LEDShowFlashString(const char  * buff,char x) 
{
	/*
	do
	{
		disp_buf[x++] = LED_ASCII[*buff++];			//获取字形码
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
			disp_buf[x++] = LED_ASCII[*buff++];			//获取字形码
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
			//如果将' '赋值给disp_buf,然后直接显示出来的话,' '为0x20, 对应显示出来的10进制ascii码为32,30.
			//因此，需要直接保存该值,然后,根据该值,直接发送显示该值.
			//如:if (disp_buf[i] == ' ')
			/*
						{
							sprintf(lcd_disp_buf,"DS%d(%d,174,' ',1);\r\n",zt,i * 26 + 10);
							GpuSend(lcd_disp_buf);
						}
			*/
			//另外，如果将这些ascii码(需要改些的密码和参数例外),直接存在一个字符串变量中,
			//最后取出时，直接以字符串'%s'形式取出，就方便多了
			disp_buf[x++] = *buff++;   //chang by 0322
		}
		else
		{
			//出错标志:"E--"，这样处理可能不合适
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


//根据序号num(菜单条编号)1,2,3,...,获取菜单编号
//获取菜单编号(ascii)
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
				szDispHz2 = "报警持续时间";		//		//第1行显示汉字,32点阵汉字(小字库)已设置好
				
				szDispSM4 = "单位S";								//  	//说明文字,使用24点阵				
				szDispSM2 = NULL;
				szDispSM3 = NULL;
*/		
			}
			else
			{
				//szDispHz2 = "纸长";
			}
			
		break;
		case 1:
			buff = "2-";
			if (cMenuEditFlag == 1)
			{
/*				
				
				szDispHz2 = "无杆检测接近传感器超时";		//
				szDispSM4 = "搓条周期感应器感应到,超过该";						//   //说明文字,使用24点阵
				szDispSM2 = "时间,无杆检测感应器未感应到";
				szDispSM3 = "停机报警。(单位0.1s)";
*/		
			}
			else
			{
				//szDispHz2 = "捆数";
			}			
		break;		
		case 2:
			buff = "3-";
/*				
				
			szDispHz2 = "无杆检测通断传感器超时";
			szDispSM4 = "搓条周期感应器感应到,超过该";				//  // 搓条周期感应器感应到,超过该时间,  //说明文字,使用24点阵
			szDispSM2 = "时间,无杆通断感应器未感应到,";
			szDispSM3 = "停机报警。(单位0.1s)";
*/		
		break;	
		case 3:
			buff = "4-";
/*				
				
		szDispHz2 = "送纸步进电机方向";
			szDispSM4 = "设置该值确定电机正确的运";		//说明文字,使用24点阵
			szDispSM2 = "动方向。一经正确设定，";
			szDispSM3 = "不得随意更改。";
*/			  
		break;		
		case 4:
			buff = "5-";
/*				
				
		szDispHz2 = "送纸步进速度增益";
			szDispSM4 = "该值越大，送纸速度越快。";		//说明文字,使用24点阵
			szDispSM2 = NULL;
			szDispSM3 = NULL;	
*/				
		break;		
		case 5:
			buff = "6-";
/*				
				
		szDispHz2 = "送纸步进脉冲当量";
			szDispSM4 = "单位0.1脉冲/毫米。由步进";		//说明文字,使用24点阵
			szDispSM2 = "细分、减速比、滚轴半径共同";
			szDispSM3 = "决定。一经正确设定,不得更改。";
*/		
		break;		
		case 6:
			buff = "7-";
/*				
				
		szDispHz2 = "发纸速度微调系SHU";			// "数"这个字编码出错,显示时出错的地方会刷屏
			szDispSM4 = "（无发纸电机无效）";		//说明文字,使用24点阵
			szDispSM2 = "该值可微调发纸速度,";
			szDispSM3 = "值越大发纸越快。";
*/		
		break;		
		case 7:
			buff = "8-";
/*				
				
		szDispHz2 = "发纸滚轮半径";
			szDispSM4 = "单位0.1毫米。";		//说明文字,使用24点阵
			szDispSM2 = "（无发纸电机无效）";
			szDispSM3 = "一经正确设定，不得更改。";
*/		
		break;		
		case 8:
			buff = "9-";
/*				
				
		szDispHz2 = "发纸速度调节频率";
			szDispSM4 = "（无发纸电机无效）";		//说明文字,使用24点阵
			szDispSM2 = "该值越大发纸速度调节";
			szDispSM3 = "频率越高。";
*/		
		break;		
		case 9:
			buff = "10";
/*				
				
		szDispHz2 = "发纸电机启动系SHU";				//数   // "数"这个字编码出错,显示时出错的地方会刷屏
			szDispSM4 = "（无发纸电机无效）";		//说明文字,使用24点阵
			szDispSM2 = "一经正确设定,不得随意更改。";
			szDispSM3 = NULL;
*/		
		break;		
		case 10:
			buff = "11";
/*				
				
		szDispHz2 = "切纸电机滚轮减速比";
			szDispSM4 = "一经正确设定,不得随意更改。";		//说明文字,使用24点阵
			szDispSM2 = NULL;
			szDispSM3 = NULL;
*/		
		break;		
		case 11:
			buff = "12";
/*				
				
		szDispHz2 = "切纸电机目标速度";
			szDispSM4 = "单位0.1khz脉冲频率";		//说明文字,使用24点阵
			szDispSM2 = "该值越大则切纸越快。";
			szDispSM3 = NULL;
*/		
		break;		
		case 12:
			buff = "13";
/*				
				
		szDispHz2 = "发纸速度最高等级";
			szDispSM4 = "（无发纸电机无效）";		//说明文字,使用24点阵
			szDispSM2 = "发纸电机速度调节的最高速度。";
			szDispSM3 = "该值越大则发纸电机速度越快。";
*/		
		break;	
		case 13:
			buff = "14";
/*				
				
		szDispHz2 = "发纸速度最低等级";
			szDispSM4 = "（无发纸电机无效）";		//说明文字,使用24点阵
			szDispSM2 = "发纸电机速度调节的最低速度。";
			szDispSM3 = "该值越小则发纸电机速度越慢。";
*/		
		break;	
		case 14:
			buff = "15";
/*				
				
		szDispHz2 = "送纸启动时切刀高度";
			szDispSM4 = "单位0.1k脉冲";		//说明文字,使用24点阵
			szDispSM2 = "该值越大送纸启动时切刀抬起越高。";
			szDispSM3 = NULL;
*/		
		break;	
		case 15:
			buff = "16";
/*				
					
			szDispHz2 = "打胶电机运动速度";
			szDispSM4 = "单位百分数";		//说明文字,使用24点阵
			szDispSM2 = "该值越大运动速度越快。";
			szDispSM3 = NULL;
*/		
		break;	
		case 16:
			buff = "17";
/*				
				
			szDispHz2 = "切纸电机启动速度";	
			szDispSM4 = "该值越大启动速度越快。";		//说明文字,使用24点阵
			szDispSM2 = NULL;
			szDispSM3 = NULL;
*/		
		break;
		case 17:
			buff = "18";

/*				
						
			szDispSM4 = NULL;						//搓条周期感应器感应到,超过该时间  说明文字,使用24点阵
			szDispSM2 = NULL;				//"无杆感应器未感应到,则停机报警";
			szDispSM3 = NULL;	
*/		
		break;	
		case 18:
			buff = "19";

/*				
						
			szDispSM4 = NULL;		//"搓条周期感应器感应到,超过该时间";		//说明文字,使用24点阵
			szDispSM2 = NULL;		//"无杆感应器未感应到,则停机报警";
			szDispSM3 = NULL;	
*/		
		break;	
		case 19:
			buff = "20";

/*				
						
			szDispSM4 = NULL;		//"搓条周期感应器感应到,超过该时间";		//说明文字,使用24点阵
			szDispSM2 = NULL;		//"无杆感应器未感应到,则停机报警";
			szDispSM3 = NULL;	
*/			
		break;			
		case 20:
			buff = "21";

/*				
						
			szDispSM4 = NULL;		//"搓条周期感应器感应到,超过该时间";		//说明文字,使用24点阵
			szDispSM2 = NULL;		//"无杆感应器未感应到,则停机报警";
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
	*szDispHz1 = *buff;			//给指针赋值(地址)	
	*szDispHz2 = *szMenuDispHz[num];		//
	*/
	szDispHz1 = buff;			//给指针赋值(地址)
	
	/*
  //if (num < 30)
	if (num < 3)
	{
		//szDispHz2 = *(&szMenuDispHz[num]);		//
			szDispHz2 = "接近传感器超时";
	}
	else
	{
		szDispHz2 = "送纸步进电机方向";
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
功能：显示字符串。

参数：buff:欲发送的数据缓冲区；x:坐标

返回值：无。

说明：
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
        	disp_buf[x++] = LED_ASCII[*buff++] | 0x80;		///<【位7置1】
		else
			disp_buf[x++] = LED_ASCII[*buff++];
	}while(x<6 && *buff!=0);
}*/

/*++++++++++++++++++++++++++++++++++++++++++++++++
功能：清除指定行号列号的字符。

参数：x:坐标1-8。
x: 进行闪烁的位, n：几位(固定为1)

返回值：无。

说明： 无。
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
		//disp_buf[i] = " ";		//显示空格   实际显示:12354
		disp_buf[i] = ' ';		//显示空格   实际显示: 32
	}
	#endif	
}


/*++++++++++++++++++++++++++++++++++++++++++++++++
功能：显示数字。
参数：buff:欲发送的数据缓冲区；x:坐标
返回值：无。
说明：
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LED_ShowNumber(char * buff,char x) 
{
	#ifdef LED_DISP
	char TempFlag=0;
	do
	{
		if(*(buff+1)!='.' || TempFlag==1)		//判下一个数是否为小数点。
		{
			disp_buf[x++] = LED_ASCII[*buff++];
			//SendLed(x++,LED_ASCII[*buff++]);
		}
		/* 注: 在led显示中, 为空格的,获取的位段码为0,所以不显示
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
			disp_buf[x++] = LED_ASCII[*buff++] | 0x80;		//将字形码的小数点位置位
			//SendLed(x++,(LED_ASCII[*buff++] | 0x80));
			TempFlag=1;
			buff++;					//跳过下一个小数点
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


//菜单数据指向
void menu_cfg(void)
{
	char i;
	for(i = 0; i < MENU_ID_00000_MAX_ITEM ; i++)
	{
		Menu00000[i].m_data_vale = &public_val.menu_parm[i];		//设置参数
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   read_menu_data(char menu_id)
功能：  读取菜单配置数据 和 菜单数据 (配置参数)
参数：  无
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


void read_menu_data(char menu_id)
{
	unsigned char *progbyte,read_byte;
	unsigned int progadd;

	int i;

	progadd = MenuItem[menu_id].eeprom_cfg_add;
	read_byte = IapReadByte(progadd++);
	*((char*)(MenuItem[menu_id].com_max_item)) = MENU_ID_00000_MAX_ITEM;
	
	//读参数范围
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			MenuItem[menu_id].m_pcfg[i].m_max = Default_Param_Range[i].max;		//参数范围
			MenuItem[menu_id].m_pcfg[i].m_min = Default_Param_Range[i].min;
			MenuItem[menu_id].m_pcfg[i].ext_property = 0; //read_byte;
		}

	
	switch(menu_id)		//如果menu_id == MENU_ID_00000
		{
			case MENU_ID_00000:
				progbyte = (unsigned char *)&public_val.menu_parm[0];
			break;			
		}
		
	//读取参数(根据最大菜单个数读取参数)  参数存储在eeprom_add地址
	//读取到的参数存储在&public_val.menu_parm[0]里,  （xdata public_stru public_val;）
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


//根据操作手册:
//开机界面: 显示已送纸张数
//然后，如果按"-"2秒可将已送纸张数清0；
//如果在开机界面,按"确认“键,可显示软件版本号和传感器状态,先显示5位版本号,后显示4位传感器状态.
//其余界面,按选择返回开机界面.
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
名称:   void InitLEDDisplay(void)

功能：  显示初始化程序

参数：  无
      
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void InitLEDDisplay(void)
{
    char TempB;
    //unsigned int temp_int;
		
   	BELL = 0;
	
//	  LEDShowFlashString("Lt-2 ",0);
//		LEDShowFlashString("51231",0);    /* 显示操作板版本号 */	
//		TM1637_Write();

#ifdef LED_DISP
		LEDShowFlashString("Lt-2 ",0);
		LEDShowFlashString("88236",0);    // 显示操作板版本号  51231   是本机自己定义的.
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
		
		
		//支持横盘倒立的话，需加条指令:DR2;
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
		
		unUpVerNum = 88236;		//操作板版本号
		//unDownVerNum = 16305;
		//unIoStatus = 0;

		//unPaterLength = 300;		//300
		//unBunchNum = 350;			//350
		//TotalNum = 100000;
		
		//new
		szDispHz1 = "纸长";
		szDispHz2 = "捆数";
		szDispHz3 = "总数";
		
		//unRow1Data = unPaterLength;
		//unRow2Data = unBunchNum;		//赋初值,设置后,只能用设置后的值,不能再赋值
		//unRow3Data = TotalNum;
		
		cTestKeyStringName = NULL;		//""	//送纸机
		
		cMainUiFlag = 1;
		
		qz_key_cnt = 0;
		
		//add 0327 06:32
		cMainNotFlashFlag = 0;		//第一次进入, 全部显示
		//add 0327 06:32
		
		//TM1637_Write();
		

#endif

		cZcKunShuFlag = 0;
		
	///【统计主界面需要显示的项数】
	pMain = main_menu_cfg[0].m_main;
 	for( TempB = 0; pMain[TempB].m_str; TempB++ ) ///<【空串不为0，即不为假】
		{
			;
		}			
	//DisplayMainTotal = TempB;

	///【统计下级子菜单数量】
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
	unRow2Data = unBunchNum;		//赋初值,设置后,只能用设置后的值,不能再赋值
	
	
	//add 0403
	//unUpVerNum = 85235;				//主版本号，是通过串口返回的。
	unDownVerNum = 16305;
	unIoStatus = 0;					//IO口状态，也是通过串口返回的.
	//add 0403	
		
	CycShowCount = 0;
	DisplayItemNum = 0;
	DisplayState = ST_MAIN;									//进入主画面
	l_stop_poll = 0;										//默认定格显示
	
	//标志位初始化
	public_val.Work_Pause_Flag = 0;	
	public_val.Err_Flag = 0;
	public_val.Bell_Flag = 0;

	public_val.cnt_after_powered = 0;
	
	//读取捆数
	read_xt_num();
	
	//add 0403
	unRow3Data = TotalNum;
	//add 0403
	
	/* added by james */
    public_val.ms_timer = 0;
	public_val.io_read = 0;		//设置io口状态，初值为0.

	unKey.all = 0xFFFF;
	
	
	//初始化相关指针
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

	unAlarmErrorSn = 0;		//0xff;	 //会显示E255.	//初始值
	
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


//完成对菜单数据的存储
//保存所有编辑参数()
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
				public_val.Err_Flag |= ERR_NUM_EEP_ERR; //写EERPOM错误
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
	
	write_param_flag = 1;    // 更新下位机参数
			
}

#if 0
//完成对版本号的存储
void func_ver_update(void)
{
	char progbyte;
	eeprom_en = 0xaa;
	IapEraseSector(EEPROM_DRIVE_TYPE);
	eeprom_en = 0;
	progbyte = (unsigned char)(public_val.drive_type >> 8);
	eeprom_en = 0xaa;
	IapProgramByte(EEPROM_DRIVE_TYPE,progbyte);		//存储装置版本号
	progbyte = (unsigned char)(public_val.drive_type & 0xff);
	IapProgramByte(EEPROM_DRIVE_TYPE + 1,progbyte);
	progbyte = public_val.drive_indx;
	
	IapProgramByte(EEPROM_DRIVE_INDX,progbyte);    //存储版本索引号
	eeprom_en = 0;
}

//完成对菜单配置的存储
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
名称:   void LEDDisplay(char Key,char UpdateState)
功能：  显示运行程序
参数：  无
返回值：无
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/* 测试模式内容显示 */
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

	static xdata char test_key_cnt = 0;   /* 主界面下选择键连按计数 added by james for v19407 */
	//static xdata char learn_key_cnt = 0; 

	int temp_max;	
	int iBellOnTime, iBellOffTime;


//扫描相关端口
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
	unKey.u_bit.IO9 = P16;		//对应相关引脚的值赋给该位变量

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
			Bell_flag &= 0xfe;		//关蜂鸣器
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
			Key_Val = 0;		//短按键，未达到一定延时，不作为有效键
			//add
			Key_Dajiao_Val = 1;
			//add
		}		
	}
	else		//无键按下，包括;长按键一直按下
	{
		if( (public_val.ms_timer - bell_delay) > BELL_ON_DELY)
		{
			Bell_flag &= 0xfe;
		}

		//无按键时,延时时间到返回主界面(显示io口状态后,延时返回主界面)
		//ms_timer: 毫秒计数器
		//无键按下，延时时间到
		if( (public_val.ms_timer - No_key_delay) > NO_KEY_RETURN_MAIN)   //NO_KEY_RETURN_MAIN:菜单无按键返回主页面延时
		{
			No_key_delay = public_val.ms_timer;
			
			
			if ((edit_menu_id == MENU_ID_00000) && (DisplayState != ST_LEARN))
			{
				
				DisplayState = ST_MAIN;
				Main_disp_other = 0;    /* added by james for v19407 */
				
//#if 1				
#ifndef LED_DISP		
				//加上这里，因为有个显示循环，所以，会有较长的延时，才会进入
				//add
				
	//new
	
	//加上这个if语句很好，不会出现刷屏的现象
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

							cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
							//del end
							
							if(!war_bell_step) 
								war_bell_step = 1;		//置响蜂鸣器初始值
							
							//对出错标志处理,显示出错序号
							
							//add 0322 by yi
							cPauseBell = 0;		//蜂鸣器长暂停不响
							
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
							

							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							cMainUiFlag = 1;
							
							//add
							//TM1637_Write();		 	 	//这条语句不能少,否则，开机后黑屏
						}
				}
#endif			
//#endif
					
					
			}
		}
		
		
		//第二次扫描时，如果长按键一直按下，未达到一定延时，因为是无效键
		if( (public_val.ms_timer - Read_key_dely) > KEY_DOU_DELY)
		{
			Read_key_dely_2 = public_val.ms_timer;
			
			//达到一定延时,如果有键按下，且不是长按键
			if( ( (Key_Val&0xffff) != 0xffff) && (!long_input_flag) || (Key_Dajiao_Val != 1) && (!long_input_flag))
			{
				long_count++;
				
				//判断长按的时间，long_count大于100次，即因为是长按键
				if(long_count > 10)
				//if(long_count > 100)   		//100 * 50 = 5000ms(5s)
				{							
					long_input_flag = 1;		//长按开始时,步长为1
					long_key_step = 1;
					long_key_count = 0;
					Read_key_dely_3 = public_val.ms_timer; 
					long_count = 10;
				}
			}					
		}
		else
		{
			Key_Val = 0;		//非长按键
			
			//add 0329 09:28
			Key_Dajiao_Val = 1;
			//add
		}
	}
	
	
	if(long_input_flag)  /* 长按 */
	{
		if(public_val.ms_timer - Read_key_dely_3 > KEY_LONG_DELY)	//KEY_LONG_DELY:100
		{
			long_key_count ++;
			if(long_key_count > LONG_KEY_STEP_SWITCH)		//50 
			{
				long_key_step = 10;						//长按键达到一定延时后开始加速
			}
			if(long_key_count)
				Read_key_dely_3 = public_val.ms_timer;
				Pr_Key_Val = 0xFF;		//给Pr_Key_Val赋值，让每次长按判断时，下面的判断语句，Key_Val != Pr_Key_Val。 只有长按键抬起时，Key_Val == Pr_Key_Val (Key_Val==0xff)
			
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
				Key_Val = 0;			//长按键抬起时，视为无效键
			}
			else
			{
				Pr_Key_Dajiao_Val = 1;
			}		
		}		
	}


	/* 有按键则下发按键值和按键类型 */
		
	if ((Key_Val == KEY_IO6) || (Key_Val == KEY_IO7) 
		|| (Key_Val == KEY_IO8))
	{
		write_key_flag = 1;   /* 有按键，需要写按键到主板 */
		
		gsKeyVal = Key_Val;
		gcKeyType = KEY_SHORT_UP_FLAG;
	}
	
	
	if (Key_Val == 0)
	{
		if (Key_Dajiao_Val == 0)
		{
			Key_Val = KEY_DAJIAO_FAULT;			//Key_Dajiao_Val;		//打胶键按下
		}
	}
	
	
//有报警信号，不能启动(启动按键不起作用),但右边的10个按键，还是可以操作
//当然，停止键，停纸键还是能起作用。	
//在主界面,按下"选择"键,会显示"000000"输入密码界面
  switch(DisplayState)
	{
	    case ST_MAIN:   /* 主界面 */
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
						szDispHz2 = "请输入密码:";
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
							qz_wait = 0;   // 清等待状态
							
							//add
							if (cQzFlag == 1)
							{
								cQzFlag = 0;
							}
							//add 0324

							war_bell_step = 0;  // 清除蜂鸣器响声
							cPauseBell = 0;							
						}
						
						
						if (clookVerFlag)    //查看版本
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

							cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
							//del end
							
							//add 0405
							Main_disp_other = 0;
							//add 0405
							
							if(!war_bell_step) 
								war_bell_step = 1;		//置响蜂鸣器初始值
							
							//对出错标志处理,显示出错序号
							
							//add 0322 by yi
							cPauseBell = 0;		//蜂鸣器长暂停不响
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							//add 0327 06:32
							
							//TM1637_Write();
						}
			
					}										
					
		      break;	
					
		      case KEY_REDUCE:	 //减号
		        
					test_key_cnt = 0;
					//learn_key_cnt = 0;
			  
			  //add 0327 07:12
					//cTestKeyStringName = "减号";		//-
			  //add 0327 07:12
					
					//在主界面下，长按"-",可将搓条根数清0.
					if (long_input_flag && (Main_disp_other == 0))
					{
						if (public_val.main_disp_val != 0)
						{
							public_val.main_disp_val = 0;				//xdata public_stru public_val;
							public_val.cnt_before_powered = 0;	//主板读取的开机前已保存捆数
							public_val.cnt_after_powered = 0;		//主板反馈开机后的捆数

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

							write_clear_cnt_flag = 1;    /* 发送命令至主板清除开机后计数 */
							main_borad_finish_clear = 0; /* 置为主板未完成清除状态 */
						}
					}					

					//按"-"键,置由高速模式切换到低速模式
					if (speed_mode == 1)  //高速
					{
						write_speed_mode_flag = 1;  
						speed_mode = 0;   //切到低速
					}
				
					break;	
					
					//按"+"键,置由低速模式切换到高速模式, 在主界面下, +, -为高低速切换键, 按"+"加速,按"-"减速,并且，长按-可将根数清0.
		      case KEY_INCREASE:  	 //加号

					test_key_cnt = 0;
			  
					 //add 0327 07:12
					//cTestKeyStringName = "加号";		//+
					//add 0327 07:12
					
					Main_adj_key_delay = public_val.ms_timer;					

					if (speed_mode == 0)  //低速
					{
						write_speed_mode_flag = 1;  
						speed_mode = 1;  //切到高速
					}
#if 0
					if (Main_disp_other == 0)
					{
						/* 进入参数菜单 */
					    pMenu = pItem[0].m_pcfg;	// 指向数据				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
		                DisplayItemNum  = 0;

						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//进入相应菜单  
	                    return;
					}
#endif						
										
					break;	

		        case KEY_GREEN:  	 //纸长

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
						/* 进入参数菜单 */
						
						//add
					//	work_mode = DEV_PARA_SET;						
						//add  by 0322						
						
						pMenu = pItem[0].m_pcfg;	// 指向数据				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
						DisplayItemNum  = 0;
						zc_key_flag = 1;								//因为，是利用zc_key_flag = 1，延时后，直接进入InPutData. 所以,在ST_MENU状态，实际上是没有菜单选择
						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//进入相应菜单
						
						
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						//chang 0404 13:46
						//GpuSend("SPG(29);\r\n");
						GpuSend("SPG(58);\r\n");
						//chang 0404 13:46
						DELAY_US(UART2_DELAY);
						
						
						szDispHz1 = "1-";			//给指针赋值(地址)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "纸长";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
						
						cMenuEditFlag = 0;						
						cZcKunShuFlag = 1; 		//置纸长标志
												
						cMainUiFlag = 0;
						
						//add 0402
						if (clookVerFlag)
							clookVerFlag = 0;
						
						if (cFirstDispIoStatusFlag)
							cFirstDispIoStatusFlag = 0;
						//add 0402
						
						
					//	TM1637_Write();
						//add 0323							

				
				//注: 这里不能加返回!!!!,很多显示清屏不正常的问题,多是清屏不彻底(少写了一天清屏语句),或者,直接加了返回造成的
	      //    return;
						
					}
										
					break;	

		        case KEY_BLUE:  	 //捆数

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
						
						/* 进入参数菜单 */
						pMenu = pItem[0].m_pcfg;	// 指向数据				
						now_menu.menu_id = 0;
						now_menu.par_id = 1;
						now_menu.event_flag = 1;
						DisplayItemNum  = 1;			
						zc_key_flag = 1;
						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//进入相应菜单 

						//add
				//		GpuSend("CLS(0);\r\n");		
				//		DELAY_US(UART2_DELAY);
						//多加一条清屏语句
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						DELAY_US(UART1s_DELAY);
						
						//chang 0404 13:46
						//GpuSend("SPG(30);\r\n");
						GpuSend("SPG(59);\r\n");
						//chang 0404 13:46
						
						DELAY_US(UART2_DELAY);
						
						
						szDispHz1 = "2-";			//给指针赋值(地址)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "捆数";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
						
						cMenuEditFlag = 0;
						cZcKunShuFlag = 1; 		//置捆数标志
						
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

					
		        case KEY_ENT:   //查看程序版本
							//在按下切纸键时, qz_wait 置为 1.
				
							cFirstDispIoStatusFlag = 0;
							
							//chang 0403
							if (qz_wait == 0)							
							//if ((qz_wait == 0) && (cQzFlag == 0))  //不是切纸等待状态
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
									Main_disp_other = 0;  // 返回显示筒数   返回显示搓条根数
																	
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
										
										cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//置响蜂鸣器初始值
										
										//对出错标志处理,显示出错序号
										
										//add 0322 by yi
										cPauseBell = 0;		//蜂鸣器长暂停不响
										
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

										szDispHz1 = "纸长";
										szDispHz2 = "捆数";
										szDispHz3 = "总数";
									
										unRow1Data = unPaterLength;
										unRow2Data = unBunchNum;
										unRow3Data = TotalNum;		//10000;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//第一次进入, 全部显示
										//add 0327 06:32

									//	TM1637_Write();
										//add 0324								

										war_bell_step = 0;  // 清除蜂鸣器响声
										cPauseBell = 0;
										
										
										//add 0402				//加上显示数据,避免显示数据时，不完全刷新的情况
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
										if (test_key_cnt < TEST_MODE_CNT)		//测试模式计数  //40
										{
											test_key_cnt++;
										}
									}
									else
									{
										test_key_cnt = 0;
									}
											
									Main_adj_key_delay = public_val.ms_timer;		//ms计数器
									Main_disp_other = 2;					//在按下查询版本号时,给 Main_disp_other 赋值为2. 根据此可判断
									
									//置查看版本号标志
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
										
									
									szDispHz1 = "上层";
									szDispHz2 = "底层";
									szDispHz3 = "总数";				//"状态";
									
									
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
								//	unRow3Data = glTotalAllCnt;			//显示总总数
									//chang 0403									
									
									cMainUiFlag = 0;									
									
									//add 0330 17:09
									//chang 0404 13:50
									//sprintf(lcd_disp_buf,"DS48(180,1,'%d',15);\r\n",unRow1Data);		//上层
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
									//sprintf(lcd_disp_buf,"DS48(180,86,'%d',15);\r\n",unRow2Data);		//底层
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
					
					
									//sprintf(lcd_disp_buf,"DS32(136,184,'%s',15);\r\n",(int*)(&sziostatus[0]));		//状态  
									//GpuSend(lcd_disp_buf);									
									//DELAY_US(UART2_DELAY);
									
									GpuSend("W8UE(3);\r\n");
									DELAY_US(UART2_DELAY);
									
									//add test  0405
									GpuSend("DS64(1,7,'状态',15);\r\n");
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
										szTotalNum[0] = unRow3Data/10000000 + '0';				//改为总数
										
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
									sprintf(lcd_disp_buf,"DS48(180,174,'%d',15);\r\n",unRow3Data);	//状态  
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
							else   // 切纸等待状态
							{
								write_key_flag = 1;   /* 有按键，需要写按键到主板 */

								// 下发切纸命令
								gsKeyVal = KEY_IO3;
								gcKeyType = KEY_SHORT_UP_FLAG; 
								
								Main_disp_other = 0;  // 返回显示筒数   返回显示搓条根数
								qz_wait = 0;   // 清等待状态								
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
									
									cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
									//del end
									
									if(!war_bell_step) 
										war_bell_step = 1;		//置响蜂鸣器初始值
									
									//对出错标志处理,显示出错序号
									
									//add 0322 by yi
									cPauseBell = 0;		//蜂鸣器长暂停不响
									
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


									szDispHz1 = "纸长";
									szDispHz2 = "捆数";
									szDispHz3 = "总数";
								
									unRow1Data = unPaterLength;
									unRow2Data = unBunchNum;
									unRow3Data = TotalNum;		//10000;
									
									cMainUiFlag = 1;
									
									//add 0327 06:32
									cMainNotFlashFlag = 0;		//第一次进入, 全部显示
									//add 0327 06:32

								//	TM1637_Write();
									//add 0324								

									war_bell_step = 0;  // 清除蜂鸣器响声
									cPauseBell = 0;
								}
								
								//return;
							}						
					break;		

				case KEY_IO3:   // 切纸
				
				//add 0404 15:41
				//LEDShowFlashString("--Q2",0);				
				
				//chang 0406 11:58
				//cTestKeyStringName = "切纸";		//操作
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
					if (qz_wait == 0)  //未等待切纸
					{
						write_warning_flag = 1;   // 下发报警消息
						
						//进入切纸等待
						//按下切纸键，将下面这条语句: Main_disp_other = 4;屏蔽掉，或者将 Main_disp_other = 0; 切纸键的操作，就与其他按键没什么不同。
						
						Main_disp_other = 4;	//切纸操作,按"确定键"确认操作，按其他任意键取消操作
						qz_wait = 1; 
						
						//add 0404 0902					
						//cQzFlag = 1;								
						//add 0404 09:24
						
						//add 0406
						//cMainUiFlag = 0;		//cMainUiFlag
						//add 0406
						
					}			//原: 不是切纸等待,不响应						
				}
				
				//如果屏蔽上面的Main_disp_other = 4;,那么在下面需加上下面的判断语句，"切纸键"的操作，就与其他按键没什么不同。
				//切纸键操作，显示固定的提示界面,屏蔽下面的语句
				//if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1)
				//{
				//	Main_disp_other = 0;
				//}
				
				//切纸操作,按"确定键"确认操作，按其他任意键取消操作
				//切纸键操作，显示固定的提示界面,屏蔽下面的语句
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
						
						
						szDispHz1 = "纸长";
						szDispHz2 = "捆数";
						szDispHz3 = "总数";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;
						
						//del 0406 09:24
						//Main_disp_other = 0;		//屏蔽掉这条语句，始终保持 Main_disp_other = 4。 一直显示"切纸"的提示，直到: 屏幕刷新时间到或按下"确定"键，显示版本号和状态，将Main_disp_other赋值为0.
						//del 0406 09:24
						
						cMainUiFlag = 1;
						
						//add 0327 06:32
						cMainNotFlashFlag = 0;		//第一次进入, 全部显示
						
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
						
						
						szDispHz1 = "纸长";
						szDispHz2 = "捆数";
						szDispHz3 = "总数";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;
						
						//del 0406 09:24
						//Main_disp_other = 0;
						//del 0406 09:24
						
						cMainUiFlag = 1;
						
						
						cMainNotFlashFlag = 0;		//第一次进入, 全部显示
						
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
				szDispHz2 = "切纸,请注意!!!";
				//szDispHz3 = "Q2---";
				szDispHz3 = NULL;
			
				cMainUiFlag = 0;
				
				//chang 0406 12:05
				//sprintf(lcd_disp_buf,"DS48(75,90,'切纸,请注意!!!',15);\r\n");		//原为zt: 48点阵,现改为:32点阵
				//GpuSend(lcd_disp_buf);
				//DELAY_US(UART2_DELAY);
				
				GpuSend("SPG(62);\r\n");
				DELAY_US(UART2_DELAY);
				DELAY_US(UART1s_DELAY);		//这个延时很重要
				DELAY_US(UART1s_DELAY);
				
				cKeyOptionFlash = 1;
				UpdateState = 0;
				
				//chang 0406 12:05				
				return;
				//del 0404 0902
				//add 0406 12:45
					
				
					
					/*
					//new add
					if (qz_key_cnt == 0)	//第1次按下,保持原来,
					{
						qz_key_cnt = 1;
					}
					else if (qz_key_cnt == 1)  //第2次按下,报警开始
					{
						qz_key_cnt = 2;
						
						//add by 0322 yi
						cAlarmFlag = 1;
						//cErrorAlarmValue = public_val.Err_Flag;
						//add by 0322 yi						
						//add
						
						cFirstEnterAlarmFlag = 0;						
						
						
						//置错误标志,还是在循环中，判错误标志
						public_val.Err_Flag = 6;			//ADD TEST
						
					} 
					else if (qz_key_cnt == 2)  //第3次按下,解除报警, 回复原来
					{
						qz_key_cnt = 0;						
						
						//解除报警标志
						cAlarmFlag = 0;
						public_val.Err_Flag = 0;	//恢复错误标志为0: 正常
						//cMainUiFlag = 1;		//恢复到主界面
						
						cFirstEnterAlarmFlag = 0;
						
						Main_disp_other = 0;  // 返回显示筒数   返回显示搓条根数
						qz_wait = 0;   // 清等待状态								
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

						szDispHz1 = "纸长";
						szDispHz2 = "捆数";
						szDispHz3 = "总数";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;		//10000;
						
						cMainUiFlag = 1;
						
						//add 0327 06:32
						cMainNotFlashFlag = 0;		//第一次进入, 全部显示
						//add 0327 06:32

					//	TM1637_Write();
					}
					*/					
					//add 0326
					break;					
					
					
					//add 0325 21:42					
					case KEY_IO7:  // 启动
					LEDShowFlashString("   11",0);
				//add 0325
					//cTestKeyStringName = "启动操作";
					cTestKeyStringName = "启动";		//操作
					
					//if (qz_wait == 1 || cQzFlag == 1 )	//加上判断这条语句,按下切纸键的操作,就会与其他手动键没什么不同,但按下切纸键时，没设置Main_disp_other = 4;就不用加上该语句
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
					if (qz_wait == 1 || cQzFlag == 1 )		//是否可以这样定义: 切纸键按下时，未按确定键，其它手动键不能操作
					{
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					*/
					//add 0405
				//add 0325
					break;

				case KEY_IO8:  // 停止
					LEDShowFlashString("   12",0);
				  //add 0325
					cTestKeyStringName = "停止";		
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
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					
					break;

				case KEY_RED:  // 收纸
					LEDShowFlashString("   13",0);
					//add 0325
					cTestKeyStringName = "收纸";
				
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}				

					DispHandOperation();				
					
					/*
				
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO9:  // 发纸
					LEDShowFlashString("   14",0);
					//add 0325
					cTestKeyStringName = "发纸";

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					DispHandOperation();

					/*
				
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO2:  // 退纸
					LEDShowFlashString("   15",0);
					//add 0325
					cTestKeyStringName = "退纸";
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
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO1:  // 进纸
					LEDShowFlashString("   16",0);
					//add 0325
					cTestKeyStringName = "进纸";

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}	
					DispHandOperation();

					
					
					
					/*
				
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO4:  // 下料
					LEDShowFlashString("   17",0);
					//add 0325
					cTestKeyStringName = "下料";
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
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					
					break;

				case KEY_IO5:  // 搓条
					LEDShowFlashString("   18",0);
					//add 0325
					cTestKeyStringName = "搓条";
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
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO6:  // 停止送纸
					LEDShowFlashString("   19",0);
					//add 0325
					//cTestKeyStringName = "停止送纸";
					cTestKeyStringName = "停纸";
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
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
				
					break;
				
				
				//0329 10:07
//打胶
/*				
write_key_flag = 1;  

// 下发打胶命令
gsKeyVal = KEY_IO3;
gcKeyType = KEY_SHORT_UP_FLAG; 

Main_disp_other = 0;
qz_wait = 0;   // 清等待状态								
cQzFlag = 0;
*/
				case KEY_DAJIAO_FAULT:		//DAJIAO_KEY_FAULT:		//KEY_DAJIAO:
					
					//GpuSend("DS32(1,7,'打胶',15);\r\n");				
					//DELAY_US(UART2_DELAY);
					LEDShowFlashString("   20",0);
					cTestKeyStringName = "打胶";		//短按为手动打胶一次。
				
				/*
					//add 0403
					write_key_flag = 1;  
					// 下发打胶命令
					gsKeyVal = KEY_IO3;		//注: 发KEY_IO3键值是切纸
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
						qz_wait = 0;   // 清等待状态								
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;
				//0329 10:07				
			
				
/*
				case KEY_IO3:  // 切纸
					LEDShowFlashString("   20",0);
					//add 0325
					cTestKeyStringName = "切纸操作";
				//add 0325
					break;
					
				case KEY_GREEN:  // 纸长
					LEDShowFlashString("   21",0);
					//add 0325
					cTestKeyStringName = "纸长设置";
				//add 0325
					break;

				case KEY_BLUE:  // 捆数
					LEDShowFlashString("   22",0);	
					//add 0325
					cTestKeyStringName = "捆数设置";
				//add 0325
					break;
				*/					
					//add 0325
					
		    }
       
				
			if(Main_disp_other == 0)  /* 返回主界面 */
			{
				TempCount = main_menu_cfg[0].m_rate;  
				TempDataB = DisplayItemNum;
			
				if(!public_val.Err_Flag)				//无报错   //设备是否出差，根据出错标志:public_val.Err_Flag是否为1.
				{
					//add by 0322
					//解除报警标志
					//del 0407			//因为，下面并没有将界面恢复到主界面
					//这个位置，不能随意置位或清0,否则，会出现状态不确定的现象。(即:在下次有故障时,cAlarmFlag为1,而cMainUiFlag也为1的现象)
					//或者，不屏蔽下面2条语句，在没有任何故障的情况下，从这里自动恢复到主界面，首先，需将;cFirstEnterAlarmFlag = 0, 然后，
					//加上恢复到主界面的那些语句，然后，还需要增加一个变量: cFirstEnterNormalFlag.(或者，使用cMainUiFlag标志，判cMainUiFlag标志,
					//如果:cMainUiFlag==0,则认为是第一次进行正常主界面，显示该主界面；否则，不在此刷屏主界面)
					//cAlarmFlag = 0;				
					//cMainUiFlag = 1;		//恢复到主界面	
					//del 0407
					//add by 0322 yi
					
					/*
					//add 0407 10:08
					//上述设计的具体程序如下:
					///////////////////////////////////////////////////////////////////////
					 cAlarmFlag = 0;
					 cFirstEnterAlarmFlag = 0;		//为第二次故障来临时，通过判断:cFirstEnterAlarmFlag == 0,进入故障界面做准备。
					 if (cMainUiFlag == 0)			//是第一次有故障状态切换到正常状态
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
							
							
							szDispHz1 = "纸长";
							szDispHz2 = "捆数";
							szDispHz3 = "总数";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//第一次进入, 全部显示
							
							//cFirstEnterAlarmFlag = 0;
							//add 0403					 
					 }
					 //add 0407 10:08
					 ///////////////////////////////////////////////////////////////////////
					 */
					
					//unAlarmErrorSn = 0xff;		//初始值
					
					
					if(!public_val.Work_Pause_Flag)     /* 无暂停，刷新显示当前已洗饼数 */
					{
						if(public_val.Bell_Flag == YES)   /* 蜂鸣器长暂停响 added for v19416 */
						{
							cPauseBell = 1;
							if(!war_bell_step) war_bell_step = 1;
						}
						else
						{
							Bell_flag &= 0xfd;
						}
						
						//无报错,无暂停,刷新显示当前搓条根数.
						DisplayValueTemp = (long)public_val.main_disp_val; 				//主显示值，就是搓条根数
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[0],0);						
						
						//更新搓条根数的显示值
						unRow3Data = DisplayValueTemp;    //add 0324
						TotalNum = unRow3Data;				
					
						
					}
					else
					{
						LEDShowFlashString("P--",0);						//显示项目名称内容
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
				else			//出错
				{
			
					if ((public_val.Err_Flag == P_FINISH_LEARN1)
						|| (public_val.Err_Flag == P_FINISH_LEARN2)) //是学习完成出错，还是正常报错
					{
						//学习完成也显示送纸计数
						if(public_val.Bell_Flag == YES)   /* 蜂鸣器长暂停响 added for v19416 */
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
						
						//unAlarmErrorSn = 0xff;		//初始值
						
						
					}
					else		 // 正常报错			(报警处理在这里)
					{
						
						//显示错误标志
					//	LEDShowFlashString("E--",0);		/* modified by james at 2014-4-12 */
						
						
						//为了不让正常报错的循环，影响程序的正常运行，先暂时屏蔽这段  0325 17:13
						//置报警标志
						//add by 0322 yi
						cAlarmFlag = 1;
						
						unAlarmErrorSn = public_val.Err_Flag;
						
#if 1
						//切纸键测试关闭
						//使用切纸键测试
						//add 0324 19:11
						//del start
						//del 0402		//
						//add 0403
						if (cHandOptionFlag == 1)		//意思为，在有手动按键操作时，不能刷屏到故障界面
						{
							
							/*
							//add 0404 16:17							
							if (qz_wait == 1)
							{
								if(!war_bell_step) 
									war_bell_step = 1;			//置响蜂鸣器标志			

									cPauseBell = 1;
									cQzFlag = 1;
									cMainUiFlag = 1;
									//del 0405
									//cTestKeyStringName = "切纸";
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
								if (cMainUiFlag == 1 )	//按确定键，进入过主界面,才清屏，否则，不清屏
								//if (cMainUiFlag == 1 || unAlarmErrorSn == 0xff)
								{
									unAlarmErrorSn = public_val.Err_Flag;		//是第一次进入
									//add 0406
									cMainUiFlag = 0;
									//add 0406
									
									//add 0327 14:17								
									//如果没有进入主界面模式,连续第2次按下"切纸"键，不清屏. 只有按下"确定"键,进入主界面后第2次按"切纸"键,才清屏
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
						//切纸键测试关闭
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
							war_bell_step = 1;		//置响蜂鸣器初始值
						
						//add 0322 by yi
						//保存报警出错序号:
						//unAlarmErrorSn = public_val.Err_Flag;		//移到前面
						//add 0322 by yi
						
						//对出错标志处理,显示出错序号
						//DisplayValueTemp = public_val.Err_Flag;
						
						/*
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[3],3);		//显示出错序号:2位.
						//错误序号,即: disp_buf的3,4位, 应与szDispHz1放在一起(在第一行).
						*/
						
						cPauseBell = 0;		//蜂鸣器长暂停不响						
						
					}
				}

				//DisplayState: 为对应主操作界面状态
				//Main_disp_other:为DisplayState下的子状态				
				
				if(l_stop_poll)												//循环显示
				if(++CycShowCount > main_menu_cfg[0].m_time)				//---------------周期计数器判断     
				{
				    CycShowCount=0;
					DisplayItemNum++;
					TempCount = 0; 
				}
			}
			//在主界面,按下enter键时,设置:Main_adj_key_delay = public_val.ms_timer; Main_disp_other = 2;
			else if(Main_disp_other == 2)    /* 显示版本号 */
			{
				//DisplayValueTemp = public_val.version;
//add 0321				
				unDownVerNum = public_val.version;
				//add by 0321
				
				//add 0402
				unRow1Data = unUpVerNum;			//最上为操作板版本号
				
				unRow2Data = unDownVerNum;			//为主控板版本号
				unIoStatus = public_val.io_read;
				//chang 0403
				//unRow3Data = unIoStatus;	
				unRow3Data = glTotalAllCnt;			//显示总总数
				//chang 0403
				
				if (clookVerFlag == 0)
					clookVerFlag = 1;
				
				cFirstDispIoStatusFlag = 0;
				
				//add 0402
				
				//程序版本号，在这里循环显示更OK.
				/*
				//程序版本号在按键已显示，且版本号值不需要刷新
				sprintf(lcd_disp_buf,"DS48(180,1,'%d',15);\r\n",unRow1Data);		//上层								
													GpuSend(lcd_disp_buf);
													//add
													DELAY_US(UART2_DELAY);


				sprintf(lcd_disp_buf,"DS48(180,86,'%d',15);\r\n",unRow2Data);		//底层		
								GpuSend(lcd_disp_buf);
								DELAY_US(UART2_DELAY);
				*/

				/*
				//add 0402
				sprintf(lcd_disp_buf,"DS32(136,184,'%s',15);\r\n",(int*)(&sziostatus[0]));		//状态  
						//chang
				GpuSend(lcd_disp_buf);				
				DELAY_US(UART2_DELAY);
				//add 0402
				*/

				
				/*
				//版本号，不使用:DisplayBuff进行显示.	而是以整数形式显示		
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);
				*/
				//菜单更新的延时时间: 在主界面下,按"确定"键,进入显示版本号,经过一定延时后,显示io口状态.
				//延时时间到,显示io口状态
				//test 0405
				if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY + 2)		//2000ms
				//if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY * 15 + 2)		//2000ms
				//test 0405
				{
					//延时时间到，自动显示io口状态
					//test 0405
					Main_disp_other = 3;		//设为显示io口状态
					//test 0405
					
					cFirstDispIoStatusFlag = 0;
				}
				//return;		//直接返回，蜂鸣器会一直响
			}			
			else if(Main_disp_other == 3)    /* 显示传感器等输入IO值 */
			{
				//DisplayValueTemp = (long)public_val.io_read;		//显示io状态
				////DisplayValueTemp &= 0xff;
				//DisplayValueTemp &= 0xffff;
				
				//add 0322				
				unIoStatus = public_val.io_read;		//(unsigned int)DisplayValueTemp;				
				//add 0322 by yi
				
				//add 0402
				unRow3Data = unIoStatus;
				//add 0402
				
				//先清屏
				//第一次调用,先清屏，再显示. 显示版本号，经过一定延时后，进入状态显示
				//加一状态显示标志
				
				if (cFirstDispIoStatusFlag == 0)
				{
					cFirstDispIoStatusFlag = 1;
					
					GpuSend("CLS(0);\r\n");		//第一次进入调用
					  DELAY_US(UART2_DELAY);			
					//调用批编号显示
					#if 0
					GpuSend("SPG(56);\r\n");	//60: 显示文字
					#endif
					
					#if 1
					GpuSend("SPG(60);\r\n");	//60: 显示文字
					#endif
					
					DELAY_US(UART2_DELAY);
					
					DELAY_US(UART1s_DELAY);
					DELAY_US(UART1s_DELAY);		//必须加上这条语句,否则，S11，S12无法显示
					
					return;
				}
				
				cFirstEnterAlarmFlag = 0;
				
				/*/
				//放在这里的缺点是刷屏很快。
				//不能放在这里刷屏,否则，蜂鸣器会一直响				
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
				LED_ShowNumber(&DisplayBuff[0],0);			//原来以5位整数形式显示，并没有对这些状态位进行解析
				*/
				
				
			}
			else if (Main_disp_other == 4)    /* 切纸等待 */
			{
					//LEDShowFlashString("Q2---",0);

					if(!war_bell_step) 
						war_bell_step = 1;			//置响蜂鸣器标志			

					cPauseBell = 1;
					
					//if (cQzFlag == 0)
					//{
					//	cQzFlag = 1;
					//	qz_wait = 1;
					//}
					
					//add 0404 09:19
					//cTestKeyStringName = "切纸";		//操作	

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
							cMainUiFlag = 1;		//下面这条语句不能加,否则，报警状态下，按下该键，与其他键会不一样
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
						szDispHz2 = "切纸,请注意!!!";		//
						//szDispHz3 = "Q2---";
						szDispHz3 = "";
					
						cMainUiFlag = 0;
					}
					*/
					
					
					/*
					//del 0404 15:47
					szDispHz1 = "纸长";
					szDispHz2 = "捆数";
					szDispHz3 = "总数";
				
					unRow1Data = unPaterLength;
					unRow2Data = unBunchNum;
					unRow3Data = TotalNum;
					
					Main_disp_other = 0;
					
					cMainUiFlag = 1;
					//del 0404 15:47					
					
					
					//add 0327 06:32
					cMainNotFlashFlag = 0;		//第一次进入, 全部显示
					//add 0327 06:32
					
					cFirstEnterAlarmFlag = 0;
					*/
					
					/*
					//因为，在主循环中,所以，这里不能直接调用显示，否则，刷屏太快
					//chang 0402
					//TM1637_Write();					
					sprintf(lcd_disp_buf,"DS32(75,86,'切纸,请注意!!!',15);\r\n");		//原为zt: 48点阵,现改为:32点阵
					GpuSend(lcd_disp_buf);
					DELAY_US(UART2_DELAY);
					return;
					//chang 0402
					//return;		
					//add 0324
					*/
			}
	    }

		
		if (test_key_cnt == TEST_MODE_CNT)  /* 长按计数后进入测试参数设置界面 added by james for v19407 */
		{
			test_key_cnt = 0;
			DisplayState = ST_TEST;
			//置为测试显示,设为切纸等待
			Main_disp_other = 4;
			
			test_set_display(test_mode);
		}		

	#if 0
		if (learn_key_cnt == LEARN_MODE_CNT)  /* 长按计数后进入测试参数设置界面 added by james for v19407 */
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


		case ST_PASSWORD://---------------------------------------------密码输入界面

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

									cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
									//del end
									
									if(!war_bell_step) 
										war_bell_step = 1;		//置响蜂鸣器初始值
									
									//对出错标志处理,显示出错序号
									
									//add 0322 by yi
									cPauseBell = 0;		//蜂鸣器长暂停不响
									
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

									szDispHz1 = "纸长";
									szDispHz2 = "捆数";
									szDispHz3 = "总数";
								
									unRow1Data = unPaterLength;		//unPaterLength;
									unRow2Data = unBunchNum;		//unBunchNum;
									unRow3Data = TotalNum;	//TotalNum;
							
									cMainUiFlag = 1;
									
									//add 0327 06:32
									cMainNotFlashFlag = 0;		//第一次进入, 全部显示
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
	                if(PasswordBuff[4]=='0')//密码符合要求
	                {                
						/* 进入参数菜单 */
					    pMenu = pItem[0].m_pcfg;	// 指向数据				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
						DisplayItemNum  = 0;

						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//进入相应菜单  
						
	                    return;
	                }
				#endif
				
					
						if(PasswordBuff[0]=='1')//
						if(PasswordBuff[1]=='0')//
						if(PasswordBuff[2]=='1')//
						if(PasswordBuff[3]=='0')//
						if(PasswordBuff[4]=='1')//密码符合要求
						{                   
						/* 进入参数菜单 */
					  pMenu = pItem[0].m_pcfg;	// 指向数据				
						now_menu.menu_id = 1;			//==1,密码
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
										
						DisplayItemNum  = Menu0_Number;

						DisplayMenuTotal = Menu_Number[0];
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//进入相应菜单
						
						//置菜单编辑标志
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
						
						szDispHz1 = "1-";			//给指针赋值(地址)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "报警持续时间";
						
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
	                if(PasswordBuff[4]=='1')//密码符合要求
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
					
									//del 032511:42 //按下按键，从这里返回,无法刷屏
					//return;		
									//del 032511:42
					
		            break;			
		    }
			
			//密码输入时，利用这里的
	        if( flash_flag  && !long_input_flag  )												//闪烁功能
	        {	
	            LEDClearChar(DisplayBitNum, 1);		//将对于位置显示缓冲清0 (当前输入位)
	        }
	        else
	        {
	            LEDShowFlashString(&PasswordBuff[0], 0);     //显示该密码输入缓冲区的数据       
	        }	      
					
					//add
			TM1637_Write();		//利用后面的刷屏    //del 0325 11:44
					//add 0323
			    
			break;


		case ST_LEARN:

			switch(Key_Val) 
		    {
		        case KEY_SEL:  // 选择键返回
		        	Main_disp_other = 0;
					DisplayBitNum = 4;
					CycShowCount  = 0;
					now_menu.menu_id = 888;
					now_menu.event_flag = 1;
		            DisplayItemNum = 0;	
		            DisplayState   = ST_MAIN;											//返回主界面
		            
					war_bell_step = 0;  // 清除蜂鸣器响声
					cPauseBell = 0;

					return;
					
					break;	
					
				default:
					break;
			}

			if ((public_val.Err_Flag == P_FINISH_LEARN1)
				|| (public_val.Err_Flag == P_FINISH_LEARN2))
			{
				if (public_val.Err_Flag == P_FINISH_LEARN1)    // 学习完成
				{
					LEDShowFlashString("P-- 1",0);

					if(!war_bell_step) 
						war_bell_step = 1;						

					cPauseBell = 1;						
				}
				else if (public_val.Err_Flag == P_FINISH_LEARN2)    // 学习完成
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

			
		case ST_TEST:        /* 测试参数设置界面响应 added by james for v19407 */
			switch(Key_Val)
		    {
		        case KEY_SEL:
		        	Main_disp_other = 0;
					DisplayBitNum = 4;
					CycShowCount  = 0;
					now_menu.menu_id = 888;
					now_menu.event_flag = 1;
		            DisplayItemNum = 0;	
		            DisplayState   = ST_MAIN;											//返回主界面
		            break;	

				case KEY_INCREASE:
					if (test_mode < 1)
					{
						test_mode++;

						write_test_mode_flag = 1;  /* 写测试模式到主板 */
					}
					test_set_display(test_mode);
					
					if (test_mode > 0)  /* 测试模式 */
					{
						//ResetDoorForTest();
					}					
					break;

				case KEY_REDUCE:
					if (test_mode > 0)
					{
						test_mode--;

						write_test_mode_flag = 1;  /* 写测试模式到主板 */						
					}
					test_set_display(test_mode);					
					break;

				case KEY_IO7:  // 启动
					LEDShowFlashString("   11",0);
				//add 0325
					cTestKeyStringName = "启动";
				//add 0325
					break;

				case KEY_IO8:  // 停止
					LEDShowFlashString("   12",0);
				  //add 0325
					cTestKeyStringName = "停止";
				//add 0325
					break;

				case KEY_RED:  // 收纸
					LEDShowFlashString("   13",0);
					//add 0325
					cTestKeyStringName = "收纸";
				//add 0325
					break;

				case KEY_IO9:  // 发纸
					LEDShowFlashString("   14",0);
					//add 0325
					cTestKeyStringName = "发纸";
				//add 0325
					break;

				case KEY_IO2:  // 退纸
					LEDShowFlashString("   15",0);
					//add 0325
					cTestKeyStringName = "退纸";
				//add 0325
					break;

				case KEY_IO1:  // 进纸
					LEDShowFlashString("   16",0);
					//add 0325
					cTestKeyStringName = "进纸";
				//add 0325
					break;

				case KEY_IO4:  // 下料
					LEDShowFlashString("   17",0);
					//add 0325
					cTestKeyStringName = "下料";
				//add 0325
					break;

				case KEY_IO5:  // 搓条
					LEDShowFlashString("   18",0);
					//add 0325
					cTestKeyStringName = "搓条";
				//add 0325
					break;

				case KEY_IO6:  // 停止送纸
					LEDShowFlashString("   19",0);
					//add 0325
					cTestKeyStringName = "停纸";
				//add 0325
					break;

				case KEY_IO3:  // 切纸
					LEDShowFlashString("   20",0);
					//add 0325
					cTestKeyStringName = "切纸";
				//add 0325
					break;
					
				case KEY_GREEN:  // 纸长
					LEDShowFlashString("   21",0);
					//add 0325
					cTestKeyStringName = "纸长";
				//add 0325
					break;

				case KEY_BLUE:  // 捆数
					LEDShowFlashString("   22",0);	
					//add 0325
					cTestKeyStringName = "捆数";		//设置
				//add 0325
					break;
					
				default:
					break;
			}

		break;
			
	    case ST_MENU:			/* 菜单数据查看 */
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
							DisplayState   = ST_MAIN;											//返回主界面
					//DIS_OPTIONING=0;
						
						//直接返回主界面
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

										cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//置响蜂鸣器初始值
										
										//对出错标志处理,显示出错序号
										
										//add 0322 by yi
										cPauseBell = 0;		//蜂鸣器长暂停不响
										
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
										
										szDispHz1 = "纸长";
										szDispHz2 = "捆数";
										szDispHz3 = "总数";
									
										unRow1Data = unPaterLength;
										unRow2Data = unBunchNum;
										unRow3Data = TotalNum;
										
										Main_disp_other = 0;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//第一次进入, 全部显示
										//add 0327 06:32
										
										cFirstEnterAlarmFlag = 0;
									
									//del 0325 11:58
									//	TM1637_Write();		//写显示
										//del 0325 11:58
									}
						
		            break;	
						
		      case KEY_REDUCE:   //上键,查看上一菜单数据
						
					if (now_menu.menu_id == 0)
					{
						if (DisplayItemNum-- == 0)			//DisplayItemNum: 菜单项计数
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
					
		        case KEY_INCREASE:		//上键,查看上一菜单数据

					if (now_menu.menu_id == 0)
					{
						if (++DisplayItemNum >= DisplayMenuTotal) 	//DisplayItemNum:菜单项计数加1
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
						
					zc_key_flag = 5;		//菜单查看确定键，进入修改					
					    
		       break;			
		    }        

			//当按下纸长键, 会进入ST_MENU模式,在该模式,会对zc_key_flag自动计数, 当zc_key_flag>5时，会默认按下确定键，而自动进入编辑界面(相当于按下enter键)
			if (now_menu.menu_id == 0 && zc_key_flag)
				{
					zc_key_flag ++;
				}

			if(zc_key_flag == 5)
				{
						zc_key_flag = 0;
					TempDataB = DisplayItemNum;				//菜单序号，在这里赋值，然后,使用: pDataValue=&pMenu[TempDataB];, 再获取:pDataValue->m_data_vale的值
					if (TY_D == TY_B  || 
						TY_D == TY_D  ||  
						TY_D == TY_H)									//数值类
					{
						DisplayBitNum = 4;
						DataType=TY_D;
						TempInputData = *(int *)pDataValue->m_data_vale;
						
						//TempInputData的值，与下面的:pDataValue->m_data_vale是否为同一值呢
						
						DisplayState=ST_DATA_INPUT;										//进入参数输入界面

						pDataValue=&pMenu[TempDataB];			//给当前菜单指针赋指针初值
						DataType=TY_D;
						TempValue=DataToStr(pDataValue->m_data_vale,
					                        INT16_SIZE,
										    DEFAUT_POSE,
										    DEFAUT_POT,
										    DisplayBuff,0,DataType);				
						
						//InputDataLen: 该参数的默认长度 (它有3位,2位或者1位)
						InputDataLen = DEFAUT_POSE+DEFAUT_POT;    //3 + 0.  所以: InputDataLen == 3
						//DisplayBitNum = InputDataLen - 1; 
						PasswordBuff[0] = '0';
						PasswordBuff[1] = '0';
						//实际上，在返回的处理时,并没有判断空格数,所以,可认为是:return 0;
						//add by 0321
						//返回的空格数为: TempValue
						//TempValue = 0;		//实际上返回始终为0.
						//add by 0321
						switch(InputDataLen)
						{	/*用于复制显示缓冲区数据*/				//复制显示缓冲区数据到 PasswordBuff(字符串存储). 在参数编辑时取出    
						case 3:    //数据长度为3位
							PasswordBuff[2]=DisplayBuff[TempValue];      //
						//如果是3位数据,第3位(最高位)为空格,用'0'代替. 显示缓冲最左边存储: PasswordBuff[0]数据
							if(PasswordBuff[2] == ' ') PasswordBuff[2] = '0';
							PasswordBuff[3]=DisplayBuff[TempValue+1];
						 //第4位为空格,用'0'代替
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];		//最后一位，不可能为空格		
						    break;			
						case 2:    //数据长度为2位
							PasswordBuff[2]=' ';    //将第3位设为空格
							PasswordBuff[3]=DisplayBuff[TempValue+1];		//最多只有一位空格
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];				
						    break;		
						case 1:    //数据长度为1位, 高2位设为空格
							PasswordBuff[2]=' ';
							PasswordBuff[3]=' ';
							PasswordBuff[4]=DisplayBuff[TempValue+2];
							break;				
						}
						pr_long_input_flag = 0;
						DisplayState=ST_DATA_INPUT;										//进入参数输入界面
					}	
				}
				
			if(TempCount > 0)  	    
			{	
				TempCount-- ;
				break;          
			}
			
			//获取菜单刷新频率
			TempCount = main_menu_cfg[0].m_rate;  
			
			  //获取当前显示菜单条  菜单指针
			pDataValue=&pMenu[DisplayItemNum];	//pMenu: MENU_SHOW * pMenu;								//菜单指针
		    TempInputData = *(int *)pDataValue->m_data_vale;   //获取菜单条指针数值       //使用lcd时,也可直接显示该数值

			//显示参数条目序号,并显示对应序号的参数
			if (now_menu.menu_id == 0)
			{
				LEDShowCaption(DisplayItemNum);		//获取对应菜单编号的字形码
				
			}
			if (now_menu.menu_id == 1)
			{
				LEDShowCaption(DisplayItemNum - Menu0_Number);
			}
			
			//将菜单指针数值:TempInputData,转换为字符串.  显示缓冲区数据
			//这里实际上为菜单查看时的显示值
			TempValue=DataToStr(&TempInputData,   //pDataValue->m_data_vale
			                    INT32_SIZE,      //pDataValue->m_size,
								DEFAUT_POSE,
								DEFAUT_POT,
								DisplayBuff,0,TY_D );		
			//从不包括空格的DisplayBuff,放在显示缓冲区2开始的位置(最多3位)
			LED_ShowNumber(&DisplayBuff[TempValue],2);    //TempValue:返回高位空格数, 2:表示从第二位开始显示, 0,1为显示菜单序号
	    }
			
			if (Key_Val != KEY_SEL)			
			{
				unRow1Data = 0;
				unRow2Data = 0;
				unRow3Data = 0;	
			}
			
	    break;

	    case ST_DATA_INPUT: /* 数据输入画面 */
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
							DisplayState   = ST_MAIN;											//返回主界面
							
													
							//add 0325
							if (cZcKunShuFlag)
							{
								cZcKunShuFlag = 0;
								//cZcOptSelEnterRtnMainFlag = 1;
							}
							//add 0325							
					
							//直接返回主界面
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

								cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
								//del end
								
								if(!war_bell_step) 
									war_bell_step = 1;		//置响蜂鸣器初始值
								
								//对出错标志处理,显示出错序号
								
								//add 0322 by yi
								cPauseBell = 0;		//蜂鸣器长暂停不响
								
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

								szDispHz1 = "纸长";
								szDispHz2 = "捆数";
								szDispHz3 = "总数";
							
								unRow1Data = unPaterLength;
								unRow2Data = unBunchNum;
								unRow3Data = TotalNum;
								
								Main_disp_other = 0;
								
								cMainUiFlag = 1;
								
								//add 0327 06:32
								cMainNotFlashFlag = 0;		//第一次进入, 全部显示
								//add 0327 06:32
							
								//	TM1637_Write();		//写显�
							}							
						}
					else
		        		DisplayState = ST_MENU;			
								
					
		            break;	
		        case KEY_REDUCE: //减少
		        	if(pMenu[DisplayItemNum].ext_property & 0x01)
		        		{
		        			;
		        		}
		        	else
		        		{
							TempInputData-=long_key_step;
							if(TempInputData<pDataValue->m_min) //判是否超范围
								TempInputData = pDataValue->m_min; //pDataValue->m_min;   //限制超范围
						}
						
						//add
					//	GpuSend("CLS(0);\r\n");
					//	DELAY_US(UART2_DELAY);
					//add
						
		            break;	
		        case KEY_INCREASE: //增加
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
								TempInputData=StrToData(PasswordBuff,5,DataType);		//从PasswordBuff中获取数据到 :TempInputData
								//判是否超出最大或最小范围
						if(TempInputData <= pDataValue->m_max && TempInputData >= pDataValue->m_min)    //pDataValue在进入ST_DATA_INPUT之前，按确定键时,就已赋当前菜单的指针初值
						{
					        switch(INT16_SIZE)
						    {
						    case INT32_SIZE:
						        *(long *)pDataValue->m_data_vale = TempInputData;			//所编辑参数，存入菜单数据缓存区
						        break;
						    case INT16_SIZE:
						        *(int *)pDataValue->m_data_vale = (int)TempInputData;
						        break;			
						    case INT8_SIZE:
						        *(char *)pDataValue->m_data_vale = (char)TempInputData;
						        break;			
							}	
							if(PasswordBuff[2] == '0')    //前面是为空格,变为0; 这里为是否变为空格
								{
									PasswordBuff[2] = ' ';
									if(PasswordBuff[3] == '0') PasswordBuff[3] = ' ';
								}
								//在led中,获取空格(0x20)的位段码为0,所以不显示.
								//但在lcd中就不同,0x20的asc码值为32.
							LED_ShowNumber(&PasswordBuff[2],2);

							//菜单编辑界面,按"确定"键,存储所有参数
							func_menu_update(0);		//完成对菜单数据的存储，将所设置的参数，保存到eeprom
							
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
								//实际上在这里，它会自动返回主界面(但需要延时)
											DisplayState   = ST_MAIN;											//返回主界面
								
								//add
								//if (cZcKunShuFlag == 1)
								//{
								
										//清菜单编辑标志
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
										//直接返回主界面
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

										cMainUiFlag = 0;		//代替：	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//置响蜂鸣器初始值
										
										//对出错标志处理,显示出错序号
										
										//add 0322 by yi
										cPauseBell = 0;		//蜂鸣器长暂停不响
										
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
										

										szDispHz1 = "纸长";
										szDispHz2 = "捆数";
										szDispHz3 = "总数";
									
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
										cMainNotFlashFlag = 0;		//第一次进入, 全部显示
										//add 0327 06:32
										
										//add 0404 11:54		//直接返回
										
										
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
										
										
										//unRow3Data = TotalNum;				//在主循环中，查看总数变化只能这样赋值
									
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
								
										//TM1637_Write();		//写显示                //加上这条语句，操作区无法动态显示
									}
										
										//add 0324
					//					cMainUiDispFlag = 1;
										//add 0324
									
										//return;						//必须加返回				
								//	}
							
							//}
							//add 0323
						}
					else
		        		DisplayState = ST_MENU;	
					
		            break;			
		    }
				
				
				//闪烁或显示下一个参数的序号和数据
	        if( flash_flag && !long_input_flag )  //闪烁功能
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
						LED_ShowNumber(&PasswordBuff[2],2);				//覆盖刷新显示的数据
						
						
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
		//			TM1637_Write();		//写显示
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
			flash_flag = !flash_flag;		//闪烁标志
			UpdateState = 0;
			
			//add 0326 20:21
			cFlashAlarmFlag = !cFlashAlarmFlag;
			//add 0326 20:21
			
			//0330 14:25
			cKeyOptionFlash = !cKeyOptionFlash;
			//0330 14:25
		}		
	}


	//根据war_bell_step为1,启动蜂鸣器响停间隔
	switch(war_bell_step)  /* 蜂鸣器报警处理 */   //定时蜂鸣器响的时间和不响的时间
	{
		case 1:
			war_bell_delay = public_val.ms_timer;
			Bell_flag |= 0x02;
			war_bell_step = 10;
		break;
		case 10:
			iBellOnTime = (cPauseBell == 0) ? 500 : 50;		//cPauseBell: 1，蜂鸣器长暂停响
			//iBellOnTime = (cPauseBell == 0) ? 2000 : 50;
			if(public_val.ms_timer - war_bell_delay > iBellOnTime) //响的时间
				{
					war_bell_delay = public_val.ms_timer;
					Bell_flag &= 0xfd;
					war_bell_step = 20;
				}
		break;
		case 20:
			iBellOffTime = (cPauseBell == 0) ? 2000 : 2000;			//蜂鸣器长暂停不响
			//iBellOffTime = (cPauseBell == 0) ? 200 : 2000;			
			if(public_val.ms_timer - war_bell_delay > iBellOffTime) //不响的时间
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
					war_bell_step = 0;   /* 无报错 */
				}
			else
				{
					war_bell_step = 1;		//根据错误标志public_val.Err_Flag，重置蜂鸣器响初值
				}
		break;
	}
	
//#define BELL P25	
	
	if(Bell_flag)		//蜂鸣器响标志  1,2 响  0：不响。(1:按键响, 2: 报警响)
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
	if (clookVerFlag == 1)		//版本查看时,io状态有可能为0
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
	

	TM1637_Write();		//写显示
	
}
		

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
功能：整形变字符型

参数：nBit:整数位个数，nPoint:小数位个数  ,nFlag==1整数位前的0以空格补充

返回值：高位空格数
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static char DataToStr(void * const pData,char size,char nBit,char nPoint,char *string,char nFlag,char nType)
{	
	long DataS32Bit;
	char spaces;
	char Temp,module;
	char SignZero;//负数标志位,  >0: 0,  <0: 1
    	
	if(size==0 || size>4)//判断size的合理性
	    return 0;
	size--;    //1
//	if( ( *((char *)(pData)+size) ) & 0x80 )//判断最高位是否为1
//	    SignZero=1;
//	else
	    SignZero=0;	//正数
	    
    DataS32Bit=0;  //赋初值为0
	if(size==3)//LONG型数据
	{			    
		if(SignZero)
		    *((long *)&DataS32Bit) = 0 - *((long *)pData);
		else
		    *((long *)&DataS32Bit) = *((long *)pData);
	}	
	else if(size==1)//INT型数据   (在这里实际为整型)
	{
	    if(SignZero)
		    *((int *)&DataS32Bit) = 0 - *((int *)pData);						
		else
		    *((int *)&DataS32Bit) = *((int *)pData);	//为整数 ---------   获取pData的值到DataS32Bit
	}	
	else if(size==0)//char型数据
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
        module=10;		//在这里,nType == TY_D (10进制), 所以, module=10;
		break;
	case TY_H:
        module=16;
		break;
	}

    spaces = 3-(nPoint+nBit);     //获取 spaces的值(为0). 在这里nBit==3, nPoint = 0
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
					  //第二次循环及以后, 取DataS32Bit的值给string[Temp].
            string[Temp]='0'+(char)(DataS32Bit%(module));
            DataS32Bit/=module;
        }
        else 
				{
					if(nPoint != 0)   //nPoint==0
							string[Temp]='.';
					else
						string[Temp]=' ';  //第一次循环(Temp==3): string[3] = ' '.
				}
        
    }while( Temp-- >0 );
        
	if(nFlag)//nFlag==1整数位前的0以空格补充
	{
	    if(SignZero && spaces<3)
		    string[spaces]='-';
		return 0;
  }
	
	
	Temp=0;
	spaces=0;
	do
	{
		 //判断获取到string[Temp],其中为0的数.
		 if(string[Temp]=='0' && (Temp + 1 < nBit)) 
		 {
			 string[Temp]=' ';     //string[0]存储的是最高位的值，string[2]存储最低位的值,先从最高位开始判断
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
	    string[--spaces]='-';   //如果是负数,加上'-'
	}
	
	Temp=nPoint+nBit-spaces;
	
	if(Temp<3)
	    spaces -= (3-Temp);
	return spaces;        
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++
功能：需要一个函数将字符化整形

参数：len 为字符长度

返回值：无
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
	if(string[0]=='-' || string[1]=='-' || string[2]=='-' )//负数
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
	if (qz_wait == 1 || cQzFlag == 1 )		//是否可以这样定义: 切纸键按下时，未按确定键，其它手动键不能操作
	{
		qz_wait = 0;   // 清等待状态								
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
			
			
			szDispHz1 = "纸长";
			szDispHz2 = "捆数";
			szDispHz3 = "总数";
		
			unRow1Data = unPaterLength;
			unRow2Data = unBunchNum;
			unRow3Data = TotalNum;
			
			//Main_disp_other = 0;
			
			cMainUiFlag = 1;
			
			//add 0327 06:32
			cMainNotFlashFlag = 0;		//第一次进入, 全部显示
			
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
			
			
			szDispHz1 = "纸长";
			szDispHz2 = "捆数";
			szDispHz3 = "总数";
		
			unRow1Data = unPaterLength;
			unRow2Data = unBunchNum;
			unRow3Data = TotalNum;
			
			//Main_disp_other = 0;
			
			cMainUiFlag = 1;
			
			
			cMainNotFlashFlag = 0;		//第一次进入, 全部显示
			
			cFirstEnterAlarmFlag = 0;
		}
	}
	
	if (qz_wait == 1 || cQzFlag == 1 )		//是否可以这样定义: 切纸键按下时，未按确定键，其它手动键不能操作
	{
		qz_wait = 0;   // 清等待状态								
		cQzFlag = 0;
	}
}

#endif

