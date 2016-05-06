/**
 * Copyright (c) 2012, ��ɳ�Ϲ���ӿƼ����޹�˾
 * All rights reserved.
 *
 * FILE:   LED_Display.c
 * LED��ʾ����
 *
 * @file LED_Display.h led_driver.h
 * ͷ�ļ�.
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
#define DISP_ERR_TIMER 1000 //��ʱ�����޸�ʧ��ʱ�� �� ������ʾʱ��

//#define NO_KEY_RETURN_MAIN 15000 //00000�˵��ް���������ҳ����ʱ
#define NO_KEY_RETURN_MAIN 30000 //00000�˵��ް���������ҳ����ʱ

#define TEST_MODE_CNT    40
//#define LEARN_MODE_CNT   20
#define LEARN_MODE_CNT   30

#define NULL		0


xdata unsigned char cZcKunShuFlag;


extern char eeprom_en;
extern xdata unsigned int disp_buf[5];

//add by 0322
extern xdata unsigned char work_mode;


//����3��ͨ�Ż�ȡ
extern xdata unsigned int  unUpVerNum;
extern xdata unsigned int  unDownVerNum;
extern xdata unsigned int  unIoStatus;

extern xdata unsigned int  unPaterLength;	//���û�ȡ
extern xdata unsigned int  unBunchNum;		//���û�ȡ
extern xdata unsigned long  TotalNum;		//int ͨ�Ż�ȡ

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
 |                                ��������                                    |
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




//char xdata DIS_OPTIONING;											///<�����������������־��0�����������1�������������
char xdata DIS_ERR_FLGA = 0;

//�ڲ���������
//static xdata unsigned int  l_state;								//��ʾ����״̬

//static xdata unsigned long  l_input_value;						//�����޸Ļ�������ֵ
static xdata char  l_stop_poll;									//ֹͣѭ����ʾ

//static xdata char DisplayStateOld;								//���仭��״̬

//static xdata char  DisplayMainID;									//���������
//static xdata char  DisplayMenuID;									//�˵��������

//static xdata char  DisplayMainTotal;								//����������
//static xdata char  MenuTotals;									//���˵���������


//static xdata char  DisplayMenu0Total;								//�˵���������
//static xdata char  DisplayMenu1Total;								//�˵���������
static xdata char  DisplayMenuTotal;								//�˵���������

//static xdata char  DisplayItemNum0;								//�˵�0��ʾ��Ŀ���
//static xdata char  DisplayItemNum1;								//�˵�1��ʾ��Ŀ���
static xdata char  DisplayItemNum;								//��ʾ��Ŀ���
static xdata char  DisplayBitNum;									//��ʾλ��

//static xdata char  MenuUpdateState;								//�˵���ʾ����״̬
static xdata char  PasswordBuff[5];								//�������ݻ�����
static xdata char  DisplayBuff[10];

int  DisplayState;											//����״̬


static xdata const MAIN_SHOW * pMain;								//����������ָ��
static xdata const MENU_SHOW * pMenu;								//�˵�ָ��
static xdata const MENU_ITEM * pItem;								//�˵���Ŀָ��

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

xdata char test_mode = 0;   /* ����ģʽ��־: 0��������ģʽ��1����ģʽ */


xdata char uart_send_buf[10 + (MENU_ID_00000_MAX_ITEM << 1)];




//�˵����ý���
//�����鶨��
//#define DEBUG_MENU_GS_VAL 30
//#define USE_MENU_NUM 16

static xdata MENU_SHOW Menu00000[MENU_ID_00000_MAX_ITEM];

xdata char Menu_Number[MENU_NUM] = {MENU_ID_00000_MAX_ITEM};   //������������  ���30
xdata char Menu0_Number = 10, Menu1_Number = 20;		//

extern xdata char qz_key_cnt;		//��ֽ������




/* ����ȡֵ��Χ */
static xdata STRU_PARAM_RANGE Default_Param_Range[MENU_ID_00000_MAX_ITEM] = 
{		
	{0, 999},  /* ���������ٶ� */	
	{0, 999},  /* ����ֹͣ���о��� */			
	{0, 999},    /* ����ģʽ */	

};

static xdata int iDefaultParm[MENU_ID_00000_MAX_ITEM] = 
{
	100,   /* ���������ٶ� */
	100,  /* ����ֹͣ���о��룬��λ���� */	
	1,    /* ����ģʽ */
};

/*--------------�˵����û��� ��������-------------*/
code MENU_ITEM MenuItem[]={
  //�˵����,�˵���ַ,���� ���ݴ洢�׵�ַ ������ݸ���
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
    //��ʾ���ַ���,������ʾ����,���ݵ�ַ,������
    {"",TY_D_A,0,&(public_val.main_disp_val),INT32_SIZE,5,0,99999,0},		
    {0,0,0}
};

/*-------------------------��������������,ȫ�ֱ���----------------------------*/
xdata const MAIN_PIC main_menu_cfg[]={
    {4,2,MainItem,MenuItem},
};

/*
xdata const MAIN_PIC main_menu_cfg[]={
    {4,2,MainItem,MenuItem},
		};		//4:ÿ����ʾ����,10:�˵�ˢ��Ƶ��(ò��û�õ�) 
*/

static code const char LED_ASCII[128]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //0x0
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //0x1
				            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
				            0x00,0xFE,0xB0,0xED,0xF9,0xB3,0xDB,0xDF, //0x2
				            0xF0,0xFF,0xFB,0x00,0x00,0x40,0x80,0x00, 
				            0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07, // ��ʼ48:0-7
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

//unsigned short usParamNum = 30;   /* ��������ò������������Ϊ30�� */

xdata int gsKeyVal = 0;    /* ����ֵ */
xdata char gcKeyType = 0;  /* ��������: 0�̰�, 1�����£�2����̧�� */

//xdata char gcKeyLongUpFlag = 0;    /* ����̧���־:0δ̧��1̧�� */
//xdata char gcKeyLongDownFlag = 0;  /* �������±�־:0δ���£�1���� */

xdata _U_BIT_PARAM unKey;

						   
//static bit 	bDaJiao = 1;					   
 

xdata char speed_mode = 1;   //�ٶ�ģʽ: 1���٣� 0����

xdata char qz_wait = 0;      //�ȴ���ֽȷ��: 0δ�ȴ���1�ȴ�
						   
#define uchar unsigned char


//add 0331					   
typedef union i_status_16
//���干����
{ 
	unsigned int iostatus_16; 
	struct b_16
	{			uchar b00:1;    //λ��  
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
���ܣ���ʾ�ַ�����

������buff:�����͵����ݻ�������x:����

����ֵ���ޡ�

˵����
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LEDShowFlashString(const char  * buff,char x) 
{
	/*
	do
	{
		disp_buf[x++] = LED_ASCII[*buff++];			//��ȡ������
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
			disp_buf[x++] = LED_ASCII[*buff++];			//��ȡ������
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
			//�����' '��ֵ��disp_buf,Ȼ��ֱ����ʾ�����Ļ�,' 'Ϊ0x20, ��Ӧ��ʾ������10����ascii��Ϊ32,30.
			//��ˣ���Ҫֱ�ӱ����ֵ,Ȼ��,���ݸ�ֵ,ֱ�ӷ�����ʾ��ֵ.
			//��:if (disp_buf[i] == ' ')
			/*
						{
							sprintf(lcd_disp_buf,"DS%d(%d,174,' ',1);\r\n",zt,i * 26 + 10);
							GpuSend(lcd_disp_buf);
						}
			*/
			//���⣬�������Щascii��(��Ҫ��Щ������Ͳ�������),ֱ�Ӵ���һ���ַ���������,
			//���ȡ��ʱ��ֱ�����ַ���'%s'��ʽȡ�����ͷ������
			disp_buf[x++] = *buff++;   //chang by 0322
		}
		else
		{
			//�����־:"E--"������������ܲ�����
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


//�������num(�˵������)1,2,3,...,��ȡ�˵����
//��ȡ�˵����(ascii)
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
				szDispHz2 = "��������ʱ��";		//		//��1����ʾ����,32������(С�ֿ�)�����ú�
				
				szDispSM4 = "��λS";								//  	//˵������,ʹ��24����				
				szDispSM2 = NULL;
				szDispSM3 = NULL;
*/		
			}
			else
			{
				//szDispHz2 = "ֽ��";
			}
			
		break;
		case 1:
			buff = "2-";
			if (cMenuEditFlag == 1)
			{
/*				
				
				szDispHz2 = "�޸˼��ӽ���������ʱ";		//
				szDispSM4 = "�������ڸ�Ӧ����Ӧ��,������";						//   //˵������,ʹ��24����
				szDispSM2 = "ʱ��,�޸˼���Ӧ��δ��Ӧ��";
				szDispSM3 = "ͣ��������(��λ0.1s)";
*/		
			}
			else
			{
				//szDispHz2 = "����";
			}			
		break;		
		case 2:
			buff = "3-";
/*				
				
			szDispHz2 = "�޸˼��ͨ�ϴ�������ʱ";
			szDispSM4 = "�������ڸ�Ӧ����Ӧ��,������";				//  // �������ڸ�Ӧ����Ӧ��,������ʱ��,  //˵������,ʹ��24����
			szDispSM2 = "ʱ��,�޸�ͨ�ϸ�Ӧ��δ��Ӧ��,";
			szDispSM3 = "ͣ��������(��λ0.1s)";
*/		
		break;	
		case 3:
			buff = "4-";
/*				
				
		szDispHz2 = "��ֽ�����������";
			szDispSM4 = "���ø�ֵȷ�������ȷ����";		//˵������,ʹ��24����
			szDispSM2 = "������һ����ȷ�趨��";
			szDispSM3 = "����������ġ�";
*/			  
		break;		
		case 4:
			buff = "5-";
/*				
				
		szDispHz2 = "��ֽ�����ٶ�����";
			szDispSM4 = "��ֵԽ����ֽ�ٶ�Խ�졣";		//˵������,ʹ��24����
			szDispSM2 = NULL;
			szDispSM3 = NULL;	
*/				
		break;		
		case 5:
			buff = "6-";
/*				
				
		szDispHz2 = "��ֽ�������嵱��";
			szDispSM4 = "��λ0.1����/���ס��ɲ���";		//˵������,ʹ��24����
			szDispSM2 = "ϸ�֡����ٱȡ�����뾶��ͬ";
			szDispSM3 = "������һ����ȷ�趨,���ø��ġ�";
*/		
		break;		
		case 6:
			buff = "7-";
/*				
				
		szDispHz2 = "��ֽ�ٶ�΢��ϵSHU";			// "��"����ֱ������,��ʾʱ����ĵط���ˢ��
			szDispSM4 = "���޷�ֽ�����Ч��";		//˵������,ʹ��24����
			szDispSM2 = "��ֵ��΢����ֽ�ٶ�,";
			szDispSM3 = "ֵԽ��ֽԽ�졣";
*/		
		break;		
		case 7:
			buff = "8-";
/*				
				
		szDispHz2 = "��ֽ���ְ뾶";
			szDispSM4 = "��λ0.1���ס�";		//˵������,ʹ��24����
			szDispSM2 = "���޷�ֽ�����Ч��";
			szDispSM3 = "һ����ȷ�趨�����ø��ġ�";
*/		
		break;		
		case 8:
			buff = "9-";
/*				
				
		szDispHz2 = "��ֽ�ٶȵ���Ƶ��";
			szDispSM4 = "���޷�ֽ�����Ч��";		//˵������,ʹ��24����
			szDispSM2 = "��ֵԽ��ֽ�ٶȵ���";
			szDispSM3 = "Ƶ��Խ�ߡ�";
*/		
		break;		
		case 9:
			buff = "10";
/*				
				
		szDispHz2 = "��ֽ�������ϵSHU";				//��   // "��"����ֱ������,��ʾʱ����ĵط���ˢ��
			szDispSM4 = "���޷�ֽ�����Ч��";		//˵������,ʹ��24����
			szDispSM2 = "һ����ȷ�趨,����������ġ�";
			szDispSM3 = NULL;
*/		
		break;		
		case 10:
			buff = "11";
/*				
				
		szDispHz2 = "��ֽ������ּ��ٱ�";
			szDispSM4 = "һ����ȷ�趨,����������ġ�";		//˵������,ʹ��24����
			szDispSM2 = NULL;
			szDispSM3 = NULL;
*/		
		break;		
		case 11:
			buff = "12";
/*				
				
		szDispHz2 = "��ֽ���Ŀ���ٶ�";
			szDispSM4 = "��λ0.1khz����Ƶ��";		//˵������,ʹ��24����
			szDispSM2 = "��ֵԽ������ֽԽ�졣";
			szDispSM3 = NULL;
*/		
		break;		
		case 12:
			buff = "13";
/*				
				
		szDispHz2 = "��ֽ�ٶ���ߵȼ�";
			szDispSM4 = "���޷�ֽ�����Ч��";		//˵������,ʹ��24����
			szDispSM2 = "��ֽ����ٶȵ��ڵ�����ٶȡ�";
			szDispSM3 = "��ֵԽ����ֽ����ٶ�Խ�졣";
*/		
		break;	
		case 13:
			buff = "14";
/*				
				
		szDispHz2 = "��ֽ�ٶ���͵ȼ�";
			szDispSM4 = "���޷�ֽ�����Ч��";		//˵������,ʹ��24����
			szDispSM2 = "��ֽ����ٶȵ��ڵ�����ٶȡ�";
			szDispSM3 = "��ֵԽС��ֽ����ٶ�Խ����";
*/		
		break;	
		case 14:
			buff = "15";
/*				
				
		szDispHz2 = "��ֽ����ʱ�е��߶�";
			szDispSM4 = "��λ0.1k����";		//˵������,ʹ��24����
			szDispSM2 = "��ֵԽ����ֽ����ʱ�е�̧��Խ�ߡ�";
			szDispSM3 = NULL;
*/		
		break;	
		case 15:
			buff = "16";
/*				
					
			szDispHz2 = "�򽺵���˶��ٶ�";
			szDispSM4 = "��λ�ٷ���";		//˵������,ʹ��24����
			szDispSM2 = "��ֵԽ���˶��ٶ�Խ�졣";
			szDispSM3 = NULL;
*/		
		break;	
		case 16:
			buff = "17";
/*				
				
			szDispHz2 = "��ֽ��������ٶ�";	
			szDispSM4 = "��ֵԽ�������ٶ�Խ�졣";		//˵������,ʹ��24����
			szDispSM2 = NULL;
			szDispSM3 = NULL;
*/		
		break;
		case 17:
			buff = "18";

/*				
						
			szDispSM4 = NULL;						//�������ڸ�Ӧ����Ӧ��,������ʱ��  ˵������,ʹ��24����
			szDispSM2 = NULL;				//"�޸˸�Ӧ��δ��Ӧ��,��ͣ������";
			szDispSM3 = NULL;	
*/		
		break;	
		case 18:
			buff = "19";

/*				
						
			szDispSM4 = NULL;		//"�������ڸ�Ӧ����Ӧ��,������ʱ��";		//˵������,ʹ��24����
			szDispSM2 = NULL;		//"�޸˸�Ӧ��δ��Ӧ��,��ͣ������";
			szDispSM3 = NULL;	
*/		
		break;	
		case 19:
			buff = "20";

/*				
						
			szDispSM4 = NULL;		//"�������ڸ�Ӧ����Ӧ��,������ʱ��";		//˵������,ʹ��24����
			szDispSM2 = NULL;		//"�޸˸�Ӧ��δ��Ӧ��,��ͣ������";
			szDispSM3 = NULL;	
*/			
		break;			
		case 20:
			buff = "21";

/*				
						
			szDispSM4 = NULL;		//"�������ڸ�Ӧ����Ӧ��,������ʱ��";		//˵������,ʹ��24����
			szDispSM2 = NULL;		//"�޸˸�Ӧ��δ��Ӧ��,��ͣ������";
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
	*szDispHz1 = *buff;			//��ָ�븳ֵ(��ַ)	
	*szDispHz2 = *szMenuDispHz[num];		//
	*/
	szDispHz1 = buff;			//��ָ�븳ֵ(��ַ)
	
	/*
  //if (num < 30)
	if (num < 3)
	{
		//szDispHz2 = *(&szMenuDispHz[num]);		//
			szDispHz2 = "�ӽ���������ʱ";
	}
	else
	{
		szDispHz2 = "��ֽ�����������";
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
���ܣ���ʾ�ַ�����

������buff:�����͵����ݻ�������x:����

����ֵ���ޡ�

˵����
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
        	disp_buf[x++] = LED_ASCII[*buff++] | 0x80;		///<��λ7��1��
		else
			disp_buf[x++] = LED_ASCII[*buff++];
	}while(x<6 && *buff!=0);
}*/

/*++++++++++++++++++++++++++++++++++++++++++++++++
���ܣ����ָ���к��кŵ��ַ���

������x:����1-8��
x: ������˸��λ, n����λ(�̶�Ϊ1)

����ֵ���ޡ�

˵���� �ޡ�
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
		//disp_buf[i] = " ";		//��ʾ�ո�   ʵ����ʾ:12354
		disp_buf[i] = ' ';		//��ʾ�ո�   ʵ����ʾ: 32
	}
	#endif	
}


/*++++++++++++++++++++++++++++++++++++++++++++++++
���ܣ���ʾ���֡�
������buff:�����͵����ݻ�������x:����
����ֵ���ޡ�
˵����
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LED_ShowNumber(char * buff,char x) 
{
	#ifdef LED_DISP
	char TempFlag=0;
	do
	{
		if(*(buff+1)!='.' || TempFlag==1)		//����һ�����Ƿ�ΪС���㡣
		{
			disp_buf[x++] = LED_ASCII[*buff++];
			//SendLed(x++,LED_ASCII[*buff++]);
		}
		/* ע: ��led��ʾ��, Ϊ�ո��,��ȡ��λ����Ϊ0,���Բ���ʾ
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
			disp_buf[x++] = LED_ASCII[*buff++] | 0x80;		//���������С����λ��λ
			//SendLed(x++,(LED_ASCII[*buff++] | 0x80));
			TempFlag=1;
			buff++;					//������һ��С����
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


//�˵�����ָ��
void menu_cfg(void)
{
	char i;
	for(i = 0; i < MENU_ID_00000_MAX_ITEM ; i++)
	{
		Menu00000[i].m_data_vale = &public_val.menu_parm[i];		//���ò���
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   read_menu_data(char menu_id)
���ܣ�  ��ȡ�˵��������� �� �˵����� (���ò���)
������  ��
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


void read_menu_data(char menu_id)
{
	unsigned char *progbyte,read_byte;
	unsigned int progadd;

	int i;

	progadd = MenuItem[menu_id].eeprom_cfg_add;
	read_byte = IapReadByte(progadd++);
	*((char*)(MenuItem[menu_id].com_max_item)) = MENU_ID_00000_MAX_ITEM;
	
	//��������Χ
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			MenuItem[menu_id].m_pcfg[i].m_max = Default_Param_Range[i].max;		//������Χ
			MenuItem[menu_id].m_pcfg[i].m_min = Default_Param_Range[i].min;
			MenuItem[menu_id].m_pcfg[i].ext_property = 0; //read_byte;
		}

	
	switch(menu_id)		//���menu_id == MENU_ID_00000
		{
			case MENU_ID_00000:
				progbyte = (unsigned char *)&public_val.menu_parm[0];
			break;			
		}
		
	//��ȡ����(�������˵�������ȡ����)  �����洢��eeprom_add��ַ
	//��ȡ���Ĳ����洢��&public_val.menu_parm[0]��,  ��xdata public_stru public_val;��
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


//���ݲ����ֲ�:
//��������: ��ʾ����ֽ����
//Ȼ�������"-"2��ɽ�����ֽ������0��
//����ڿ�������,��"ȷ�ϡ���,����ʾ����汾�źʹ�����״̬,����ʾ5λ�汾��,����ʾ4λ������״̬.
//�������,��ѡ�񷵻ؿ�������.
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   void InitLEDDisplay(void)

���ܣ�  ��ʾ��ʼ������

������  ��
      
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void InitLEDDisplay(void)
{
    char TempB;
    //unsigned int temp_int;
		
   	BELL = 0;
	
//	  LEDShowFlashString("Lt-2 ",0);
//		LEDShowFlashString("51231",0);    /* ��ʾ������汾�� */	
//		TM1637_Write();

#ifdef LED_DISP
		LEDShowFlashString("Lt-2 ",0);
		LEDShowFlashString("88236",0);    // ��ʾ������汾��  51231   �Ǳ����Լ������.
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
		
		
		//֧�ֺ��̵����Ļ��������ָ��:DR2;
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
		
		unUpVerNum = 88236;		//������汾��
		//unDownVerNum = 16305;
		//unIoStatus = 0;

		//unPaterLength = 300;		//300
		//unBunchNum = 350;			//350
		//TotalNum = 100000;
		
		//new
		szDispHz1 = "ֽ��";
		szDispHz2 = "����";
		szDispHz3 = "����";
		
		//unRow1Data = unPaterLength;
		//unRow2Data = unBunchNum;		//����ֵ,���ú�,ֻ�������ú��ֵ,�����ٸ�ֵ
		//unRow3Data = TotalNum;
		
		cTestKeyStringName = NULL;		//""	//��ֽ��
		
		cMainUiFlag = 1;
		
		qz_key_cnt = 0;
		
		//add 0327 06:32
		cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
		//add 0327 06:32
		
		//TM1637_Write();
		

#endif

		cZcKunShuFlag = 0;
		
	///��ͳ����������Ҫ��ʾ��������
	pMain = main_menu_cfg[0].m_main;
 	for( TempB = 0; pMain[TempB].m_str; TempB++ ) ///<���մ���Ϊ0������Ϊ�١�
		{
			;
		}			
	//DisplayMainTotal = TempB;

	///��ͳ���¼��Ӳ˵�������
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
	unRow2Data = unBunchNum;		//����ֵ,���ú�,ֻ�������ú��ֵ,�����ٸ�ֵ
	
	
	//add 0403
	//unUpVerNum = 85235;				//���汾�ţ���ͨ�����ڷ��صġ�
	unDownVerNum = 16305;
	unIoStatus = 0;					//IO��״̬��Ҳ��ͨ�����ڷ��ص�.
	//add 0403	
		
	CycShowCount = 0;
	DisplayItemNum = 0;
	DisplayState = ST_MAIN;									//����������
	l_stop_poll = 0;										//Ĭ�϶�����ʾ
	
	//��־λ��ʼ��
	public_val.Work_Pause_Flag = 0;	
	public_val.Err_Flag = 0;
	public_val.Bell_Flag = 0;

	public_val.cnt_after_powered = 0;
	
	//��ȡ����
	read_xt_num();
	
	//add 0403
	unRow3Data = TotalNum;
	//add 0403
	
	/* added by james */
    public_val.ms_timer = 0;
	public_val.io_read = 0;		//����io��״̬����ֵΪ0.

	unKey.all = 0xFFFF;
	
	
	//��ʼ�����ָ��
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

	unAlarmErrorSn = 0;		//0xff;	 //����ʾE255.	//��ʼֵ
	
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


//��ɶԲ˵����ݵĴ洢
//�������б༭����()
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
				public_val.Err_Flag |= ERR_NUM_EEP_ERR; //дEERPOM����
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
	
	write_param_flag = 1;    // ������λ������
			
}

#if 0
//��ɶ԰汾�ŵĴ洢
void func_ver_update(void)
{
	char progbyte;
	eeprom_en = 0xaa;
	IapEraseSector(EEPROM_DRIVE_TYPE);
	eeprom_en = 0;
	progbyte = (unsigned char)(public_val.drive_type >> 8);
	eeprom_en = 0xaa;
	IapProgramByte(EEPROM_DRIVE_TYPE,progbyte);		//�洢װ�ð汾��
	progbyte = (unsigned char)(public_val.drive_type & 0xff);
	IapProgramByte(EEPROM_DRIVE_TYPE + 1,progbyte);
	progbyte = public_val.drive_indx;
	
	IapProgramByte(EEPROM_DRIVE_INDX,progbyte);    //�洢�汾������
	eeprom_en = 0;
}

//��ɶԲ˵����õĴ洢
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
����:   void LEDDisplay(char Key,char UpdateState)
���ܣ�  ��ʾ���г���
������  ��
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/* ����ģʽ������ʾ */
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

	static xdata char test_key_cnt = 0;   /* ��������ѡ����������� added by james for v19407 */
	//static xdata char learn_key_cnt = 0; 

	int temp_max;	
	int iBellOnTime, iBellOffTime;


//ɨ����ض˿�
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
	unKey.u_bit.IO9 = P16;		//��Ӧ������ŵ�ֵ������λ����

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
			Bell_flag &= 0xfe;		//�ط�����
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
			Key_Val = 0;		//�̰�����δ�ﵽһ����ʱ������Ϊ��Ч��
			//add
			Key_Dajiao_Val = 1;
			//add
		}		
	}
	else		//�޼����£�����;������һֱ����
	{
		if( (public_val.ms_timer - bell_delay) > BELL_ON_DELY)
		{
			Bell_flag &= 0xfe;
		}

		//�ް���ʱ,��ʱʱ�䵽����������(��ʾio��״̬��,��ʱ����������)
		//ms_timer: ���������
		//�޼����£���ʱʱ�䵽
		if( (public_val.ms_timer - No_key_delay) > NO_KEY_RETURN_MAIN)   //NO_KEY_RETURN_MAIN:�˵��ް���������ҳ����ʱ
		{
			No_key_delay = public_val.ms_timer;
			
			
			if ((edit_menu_id == MENU_ID_00000) && (DisplayState != ST_LEARN))
			{
				
				DisplayState = ST_MAIN;
				Main_disp_other = 0;    /* added by james for v19407 */
				
//#if 1				
#ifndef LED_DISP		
				//���������Ϊ�и���ʾѭ�������ԣ����нϳ�����ʱ���Ż����
				//add
				
	//new
	
	//�������if���ܺã��������ˢ��������
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

							cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
							//del end
							
							if(!war_bell_step) 
								war_bell_step = 1;		//�����������ʼֵ
							
							//�Գ����־����,��ʾ�������
							
							//add 0322 by yi
							cPauseBell = 0;		//����������ͣ����
							
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
							

							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							cMainUiFlag = 1;
							
							//add
							//TM1637_Write();		 	 	//������䲻����,���򣬿��������
						}
				}
#endif			
//#endif
					
					
			}
		}
		
		
		//�ڶ���ɨ��ʱ�����������һֱ���£�δ�ﵽһ����ʱ����Ϊ����Ч��
		if( (public_val.ms_timer - Read_key_dely) > KEY_DOU_DELY)
		{
			Read_key_dely_2 = public_val.ms_timer;
			
			//�ﵽһ����ʱ,����м����£��Ҳ��ǳ�����
			if( ( (Key_Val&0xffff) != 0xffff) && (!long_input_flag) || (Key_Dajiao_Val != 1) && (!long_input_flag))
			{
				long_count++;
				
				//�жϳ�����ʱ�䣬long_count����100�Σ�����Ϊ�ǳ�����
				if(long_count > 10)
				//if(long_count > 100)   		//100 * 50 = 5000ms(5s)
				{							
					long_input_flag = 1;		//������ʼʱ,����Ϊ1
					long_key_step = 1;
					long_key_count = 0;
					Read_key_dely_3 = public_val.ms_timer; 
					long_count = 10;
				}
			}					
		}
		else
		{
			Key_Val = 0;		//�ǳ�����
			
			//add 0329 09:28
			Key_Dajiao_Val = 1;
			//add
		}
	}
	
	
	if(long_input_flag)  /* ���� */
	{
		if(public_val.ms_timer - Read_key_dely_3 > KEY_LONG_DELY)	//KEY_LONG_DELY:100
		{
			long_key_count ++;
			if(long_key_count > LONG_KEY_STEP_SWITCH)		//50 
			{
				long_key_step = 10;						//�������ﵽһ����ʱ��ʼ����
			}
			if(long_key_count)
				Read_key_dely_3 = public_val.ms_timer;
				Pr_Key_Val = 0xFF;		//��Pr_Key_Val��ֵ����ÿ�γ����ж�ʱ��������ж���䣬Key_Val != Pr_Key_Val�� ֻ�г�����̧��ʱ��Key_Val == Pr_Key_Val (Key_Val==0xff)
			
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
				Key_Val = 0;			//������̧��ʱ����Ϊ��Ч��
			}
			else
			{
				Pr_Key_Dajiao_Val = 1;
			}		
		}		
	}


	/* �а������·�����ֵ�Ͱ������� */
		
	if ((Key_Val == KEY_IO6) || (Key_Val == KEY_IO7) 
		|| (Key_Val == KEY_IO8))
	{
		write_key_flag = 1;   /* �а�������Ҫд���������� */
		
		gsKeyVal = Key_Val;
		gcKeyType = KEY_SHORT_UP_FLAG;
	}
	
	
	if (Key_Val == 0)
	{
		if (Key_Dajiao_Val == 0)
		{
			Key_Val = KEY_DAJIAO_FAULT;			//Key_Dajiao_Val;		//�򽺼�����
		}
	}
	
	
//�б����źţ���������(����������������),���ұߵ�10�����������ǿ��Բ���
//��Ȼ��ֹͣ����ֽͣ�������������á�	
//��������,����"ѡ��"��,����ʾ"000000"�����������
  switch(DisplayState)
	{
	    case ST_MAIN:   /* ������ */
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
						szDispHz2 = "����������:";
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
							qz_wait = 0;   // ��ȴ�״̬
							
							//add
							if (cQzFlag == 1)
							{
								cQzFlag = 0;
							}
							//add 0324

							war_bell_step = 0;  // �������������
							cPauseBell = 0;							
						}
						
						
						if (clookVerFlag)    //�鿴�汾
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

							cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
							//del end
							
							//add 0405
							Main_disp_other = 0;
							//add 0405
							
							if(!war_bell_step) 
								war_bell_step = 1;		//�����������ʼֵ
							
							//�Գ����־����,��ʾ�������
							
							//add 0322 by yi
							cPauseBell = 0;		//����������ͣ����
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							//add 0327 06:32
							
							//TM1637_Write();
						}
			
					}										
					
		      break;	
					
		      case KEY_REDUCE:	 //����
		        
					test_key_cnt = 0;
					//learn_key_cnt = 0;
			  
			  //add 0327 07:12
					//cTestKeyStringName = "����";		//-
			  //add 0327 07:12
					
					//���������£�����"-",�ɽ�����������0.
					if (long_input_flag && (Main_disp_other == 0))
					{
						if (public_val.main_disp_val != 0)
						{
							public_val.main_disp_val = 0;				//xdata public_stru public_val;
							public_val.cnt_before_powered = 0;	//�����ȡ�Ŀ���ǰ�ѱ�������
							public_val.cnt_after_powered = 0;		//���巴�������������

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

							write_clear_cnt_flag = 1;    /* �������������������������� */
							main_borad_finish_clear = 0; /* ��Ϊ����δ������״̬ */
						}
					}					

					//��"-"��,���ɸ���ģʽ�л�������ģʽ
					if (speed_mode == 1)  //����
					{
						write_speed_mode_flag = 1;  
						speed_mode = 0;   //�е�����
					}
				
					break;	
					
					//��"+"��,���ɵ���ģʽ�л�������ģʽ, ����������, +, -Ϊ�ߵ����л���, ��"+"����,��"-"����,���ң�����-�ɽ�������0.
		      case KEY_INCREASE:  	 //�Ӻ�

					test_key_cnt = 0;
			  
					 //add 0327 07:12
					//cTestKeyStringName = "�Ӻ�";		//+
					//add 0327 07:12
					
					Main_adj_key_delay = public_val.ms_timer;					

					if (speed_mode == 0)  //����
					{
						write_speed_mode_flag = 1;  
						speed_mode = 1;  //�е�����
					}
#if 0
					if (Main_disp_other == 0)
					{
						/* ��������˵� */
					    pMenu = pItem[0].m_pcfg;	// ָ������				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
		                DisplayItemNum  = 0;

						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵�  
	                    return;
					}
#endif						
										
					break;	

		        case KEY_GREEN:  	 //ֽ��

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
						/* ��������˵� */
						
						//add
					//	work_mode = DEV_PARA_SET;						
						//add  by 0322						
						
						pMenu = pItem[0].m_pcfg;	// ָ������				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
						DisplayItemNum  = 0;
						zc_key_flag = 1;								//��Ϊ��������zc_key_flag = 1����ʱ��ֱ�ӽ���InPutData. ����,��ST_MENU״̬��ʵ������û�в˵�ѡ��
						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵�
						
						
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						//chang 0404 13:46
						//GpuSend("SPG(29);\r\n");
						GpuSend("SPG(58);\r\n");
						//chang 0404 13:46
						DELAY_US(UART2_DELAY);
						
						
						szDispHz1 = "1-";			//��ָ�븳ֵ(��ַ)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "ֽ��";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
						
						cMenuEditFlag = 0;						
						cZcKunShuFlag = 1; 		//��ֽ����־
												
						cMainUiFlag = 0;
						
						//add 0402
						if (clookVerFlag)
							clookVerFlag = 0;
						
						if (cFirstDispIoStatusFlag)
							cFirstDispIoStatusFlag = 0;
						//add 0402
						
						
					//	TM1637_Write();
						//add 0323							

				
				//ע: ���ﲻ�ܼӷ���!!!!,�ܶ���ʾ����������������,��������������(��д��һ���������),����,ֱ�Ӽ��˷�����ɵ�
	      //    return;
						
					}
										
					break;	

		        case KEY_BLUE:  	 //����

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
						
						/* ��������˵� */
						pMenu = pItem[0].m_pcfg;	// ָ������				
						now_menu.menu_id = 0;
						now_menu.par_id = 1;
						now_menu.event_flag = 1;
						DisplayItemNum  = 1;			
						zc_key_flag = 1;
						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵� 

						//add
				//		GpuSend("CLS(0);\r\n");		
				//		DELAY_US(UART2_DELAY);
						//���һ���������
						GpuSend("CLS(0);\r\n");		
						DELAY_US(UART2_DELAY);
						
						DELAY_US(UART1s_DELAY);
						
						//chang 0404 13:46
						//GpuSend("SPG(30);\r\n");
						GpuSend("SPG(59);\r\n");
						//chang 0404 13:46
						
						DELAY_US(UART2_DELAY);
						
						
						szDispHz1 = "2-";			//��ָ�븳ֵ(��ַ)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "����";
						
						unRow1Data = 0;
						unRow2Data = 0;
						unRow3Data = 0;
						
						cMenuEditFlag = 0;
						cZcKunShuFlag = 1; 		//��������־
						
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

					
		        case KEY_ENT:   //�鿴����汾
							//�ڰ�����ֽ��ʱ, qz_wait ��Ϊ 1.
				
							cFirstDispIoStatusFlag = 0;
							
							//chang 0403
							if (qz_wait == 0)							
							//if ((qz_wait == 0) && (cQzFlag == 0))  //������ֽ�ȴ�״̬
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
									Main_disp_other = 0;  // ������ʾͲ��   ������ʾ��������
																	
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
										
										cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//�����������ʼֵ
										
										//�Գ����־����,��ʾ�������
										
										//add 0322 by yi
										cPauseBell = 0;		//����������ͣ����
										
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

										szDispHz1 = "ֽ��";
										szDispHz2 = "����";
										szDispHz3 = "����";
									
										unRow1Data = unPaterLength;
										unRow2Data = unBunchNum;
										unRow3Data = TotalNum;		//10000;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
										//add 0327 06:32

									//	TM1637_Write();
										//add 0324								

										war_bell_step = 0;  // �������������
										cPauseBell = 0;
										
										
										//add 0402				//������ʾ����,������ʾ����ʱ������ȫˢ�µ����
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
										if (test_key_cnt < TEST_MODE_CNT)		//����ģʽ����  //40
										{
											test_key_cnt++;
										}
									}
									else
									{
										test_key_cnt = 0;
									}
											
									Main_adj_key_delay = public_val.ms_timer;		//ms������
									Main_disp_other = 2;					//�ڰ��²�ѯ�汾��ʱ,�� Main_disp_other ��ֵΪ2. ���ݴ˿��ж�
									
									//�ò鿴�汾�ű�־
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
										
									
									szDispHz1 = "�ϲ�";
									szDispHz2 = "�ײ�";
									szDispHz3 = "����";				//"״̬";
									
									
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
								//	unRow3Data = glTotalAllCnt;			//��ʾ������
									//chang 0403									
									
									cMainUiFlag = 0;									
									
									//add 0330 17:09
									//chang 0404 13:50
									//sprintf(lcd_disp_buf,"DS48(180,1,'%d',15);\r\n",unRow1Data);		//�ϲ�
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
									//sprintf(lcd_disp_buf,"DS48(180,86,'%d',15);\r\n",unRow2Data);		//�ײ�
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
					
					
									//sprintf(lcd_disp_buf,"DS32(136,184,'%s',15);\r\n",(int*)(&sziostatus[0]));		//״̬  
									//GpuSend(lcd_disp_buf);									
									//DELAY_US(UART2_DELAY);
									
									GpuSend("W8UE(3);\r\n");
									DELAY_US(UART2_DELAY);
									
									//add test  0405
									GpuSend("DS64(1,7,'״̬',15);\r\n");
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
										szTotalNum[0] = unRow3Data/10000000 + '0';				//��Ϊ����
										
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
									sprintf(lcd_disp_buf,"DS48(180,174,'%d',15);\r\n",unRow3Data);	//״̬  
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
							else   // ��ֽ�ȴ�״̬
							{
								write_key_flag = 1;   /* �а�������Ҫд���������� */

								// �·���ֽ����
								gsKeyVal = KEY_IO3;
								gcKeyType = KEY_SHORT_UP_FLAG; 
								
								Main_disp_other = 0;  // ������ʾͲ��   ������ʾ��������
								qz_wait = 0;   // ��ȴ�״̬								
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
									
									cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
									//del end
									
									if(!war_bell_step) 
										war_bell_step = 1;		//�����������ʼֵ
									
									//�Գ����־����,��ʾ�������
									
									//add 0322 by yi
									cPauseBell = 0;		//����������ͣ����
									
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


									szDispHz1 = "ֽ��";
									szDispHz2 = "����";
									szDispHz3 = "����";
								
									unRow1Data = unPaterLength;
									unRow2Data = unBunchNum;
									unRow3Data = TotalNum;		//10000;
									
									cMainUiFlag = 1;
									
									//add 0327 06:32
									cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
									//add 0327 06:32

								//	TM1637_Write();
									//add 0324								

									war_bell_step = 0;  // �������������
									cPauseBell = 0;
								}
								
								//return;
							}						
					break;		

				case KEY_IO3:   // ��ֽ
				
				//add 0404 15:41
				//LEDShowFlashString("--Q2",0);				
				
				//chang 0406 11:58
				//cTestKeyStringName = "��ֽ";		//����
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
					if (qz_wait == 0)  //δ�ȴ���ֽ
					{
						write_warning_flag = 1;   // �·�������Ϣ
						
						//������ֽ�ȴ�
						//������ֽ�����������������: Main_disp_other = 4;���ε������߽� Main_disp_other = 0; ��ֽ���Ĳ�����������������ûʲô��ͬ��
						
						Main_disp_other = 4;	//��ֽ����,��"ȷ����"ȷ�ϲ����������������ȡ������
						qz_wait = 1; 
						
						//add 0404 0902					
						//cQzFlag = 1;								
						//add 0404 09:24
						
						//add 0406
						//cMainUiFlag = 0;		//cMainUiFlag
						//add 0406
						
					}			//ԭ: ������ֽ�ȴ�,����Ӧ						
				}
				
				//������������Main_disp_other = 4;,��ô�����������������ж���䣬"��ֽ��"�Ĳ�����������������ûʲô��ͬ��
				//��ֽ����������ʾ�̶�����ʾ����,������������
				//if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1)
				//{
				//	Main_disp_other = 0;
				//}
				
				//��ֽ����,��"ȷ����"ȷ�ϲ����������������ȡ������
				//��ֽ����������ʾ�̶�����ʾ����,������������
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
						
						
						szDispHz1 = "ֽ��";
						szDispHz2 = "����";
						szDispHz3 = "����";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;
						
						//del 0406 09:24
						//Main_disp_other = 0;		//���ε�������䣬ʼ�ձ��� Main_disp_other = 4�� һֱ��ʾ"��ֽ"����ʾ��ֱ��: ��Ļˢ��ʱ�䵽����"ȷ��"������ʾ�汾�ź�״̬����Main_disp_other��ֵΪ0.
						//del 0406 09:24
						
						cMainUiFlag = 1;
						
						//add 0327 06:32
						cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
						
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
						
						
						szDispHz1 = "ֽ��";
						szDispHz2 = "����";
						szDispHz3 = "����";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;
						
						//del 0406 09:24
						//Main_disp_other = 0;
						//del 0406 09:24
						
						cMainUiFlag = 1;
						
						
						cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
						
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
				szDispHz2 = "��ֽ,��ע��!!!";
				//szDispHz3 = "Q2---";
				szDispHz3 = NULL;
			
				cMainUiFlag = 0;
				
				//chang 0406 12:05
				//sprintf(lcd_disp_buf,"DS48(75,90,'��ֽ,��ע��!!!',15);\r\n");		//ԭΪzt: 48����,�ָ�Ϊ:32����
				//GpuSend(lcd_disp_buf);
				//DELAY_US(UART2_DELAY);
				
				GpuSend("SPG(62);\r\n");
				DELAY_US(UART2_DELAY);
				DELAY_US(UART1s_DELAY);		//�����ʱ����Ҫ
				DELAY_US(UART1s_DELAY);
				
				cKeyOptionFlash = 1;
				UpdateState = 0;
				
				//chang 0406 12:05				
				return;
				//del 0404 0902
				//add 0406 12:45
					
				
					
					/*
					//new add
					if (qz_key_cnt == 0)	//��1�ΰ���,����ԭ��,
					{
						qz_key_cnt = 1;
					}
					else if (qz_key_cnt == 1)  //��2�ΰ���,������ʼ
					{
						qz_key_cnt = 2;
						
						//add by 0322 yi
						cAlarmFlag = 1;
						//cErrorAlarmValue = public_val.Err_Flag;
						//add by 0322 yi						
						//add
						
						cFirstEnterAlarmFlag = 0;						
						
						
						//�ô����־,������ѭ���У��д����־
						public_val.Err_Flag = 6;			//ADD TEST
						
					} 
					else if (qz_key_cnt == 2)  //��3�ΰ���,�������, �ظ�ԭ��
					{
						qz_key_cnt = 0;						
						
						//���������־
						cAlarmFlag = 0;
						public_val.Err_Flag = 0;	//�ָ������־Ϊ0: ����
						//cMainUiFlag = 1;		//�ָ���������
						
						cFirstEnterAlarmFlag = 0;
						
						Main_disp_other = 0;  // ������ʾͲ��   ������ʾ��������
						qz_wait = 0;   // ��ȴ�״̬								
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

						szDispHz1 = "ֽ��";
						szDispHz2 = "����";
						szDispHz3 = "����";
					
						unRow1Data = unPaterLength;
						unRow2Data = unBunchNum;
						unRow3Data = TotalNum;		//10000;
						
						cMainUiFlag = 1;
						
						//add 0327 06:32
						cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
						//add 0327 06:32

					//	TM1637_Write();
					}
					*/					
					//add 0326
					break;					
					
					
					//add 0325 21:42					
					case KEY_IO7:  // ����
					LEDShowFlashString("   11",0);
				//add 0325
					//cTestKeyStringName = "��������";
					cTestKeyStringName = "����";		//����
					
					//if (qz_wait == 1 || cQzFlag == 1 )	//�����ж��������,������ֽ���Ĳ���,�ͻ��������ֶ���ûʲô��ͬ,��������ֽ��ʱ��û����Main_disp_other = 4;�Ͳ��ü��ϸ����
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
					if (qz_wait == 1 || cQzFlag == 1 )		//�Ƿ������������: ��ֽ������ʱ��δ��ȷ�����������ֶ������ܲ���
					{
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					*/
					//add 0405
				//add 0325
					break;

				case KEY_IO8:  // ֹͣ
					LEDShowFlashString("   12",0);
				  //add 0325
					cTestKeyStringName = "ֹͣ";		
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
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					
					break;

				case KEY_RED:  // ��ֽ
					LEDShowFlashString("   13",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
				
					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}				

					DispHandOperation();				
					
					/*
				
				//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO9:  // ��ֽ
					LEDShowFlashString("   14",0);
					//add 0325
					cTestKeyStringName = "��ֽ";

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}
					DispHandOperation();

					/*
				
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO2:  // ��ֽ
					LEDShowFlashString("   15",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
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
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO1:  // ��ֽ
					LEDShowFlashString("   16",0);
					//add 0325
					cTestKeyStringName = "��ֽ";

					if ((cAlarmFlag == 1 && cMainUiFlag == 0) || clookVerFlag == 1 || cFirstDispIoStatusFlag == 1 || qz_wait == 1 || cQzFlag == 1)
					{
						Main_disp_other = 0;
					}	
					DispHandOperation();

					
					
					
					/*
				
					//add 0404 09:52
					if (qz_wait == 1 || cQzFlag == 1 )
					{
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO4:  // ����
					LEDShowFlashString("   17",0);
					//add 0325
					cTestKeyStringName = "����";
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
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					
					break;

				case KEY_IO5:  // ����
					LEDShowFlashString("   18",0);
					//add 0325
					cTestKeyStringName = "����";
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
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;

				case KEY_IO6:  // ֹͣ��ֽ
					LEDShowFlashString("   19",0);
					//add 0325
					//cTestKeyStringName = "ֹͣ��ֽ";
					cTestKeyStringName = "ֽͣ";
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
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
				
					break;
				
				
				//0329 10:07
//��
/*				
write_key_flag = 1;  

// �·�������
gsKeyVal = KEY_IO3;
gcKeyType = KEY_SHORT_UP_FLAG; 

Main_disp_other = 0;
qz_wait = 0;   // ��ȴ�״̬								
cQzFlag = 0;
*/
				case KEY_DAJIAO_FAULT:		//DAJIAO_KEY_FAULT:		//KEY_DAJIAO:
					
					//GpuSend("DS32(1,7,'��',15);\r\n");				
					//DELAY_US(UART2_DELAY);
					LEDShowFlashString("   20",0);
					cTestKeyStringName = "��";		//�̰�Ϊ�ֶ���һ�Ρ�
				
				/*
					//add 0403
					write_key_flag = 1;  
					// �·�������
					gsKeyVal = KEY_IO3;		//ע: ��KEY_IO3��ֵ����ֽ
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
						qz_wait = 0;   // ��ȴ�״̬								
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							cMainUiFlag = 1;
							
							
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							cFirstEnterAlarmFlag = 0;
						}
					}
					//add 0405
					*/
					break;
				//0329 10:07				
			
				
/*
				case KEY_IO3:  // ��ֽ
					LEDShowFlashString("   20",0);
					//add 0325
					cTestKeyStringName = "��ֽ����";
				//add 0325
					break;
					
				case KEY_GREEN:  // ֽ��
					LEDShowFlashString("   21",0);
					//add 0325
					cTestKeyStringName = "ֽ������";
				//add 0325
					break;

				case KEY_BLUE:  // ����
					LEDShowFlashString("   22",0);	
					//add 0325
					cTestKeyStringName = "��������";
				//add 0325
					break;
				*/					
					//add 0325
					
		    }
       
				
			if(Main_disp_other == 0)  /* ���������� */
			{
				TempCount = main_menu_cfg[0].m_rate;  
				TempDataB = DisplayItemNum;
			
				if(!public_val.Err_Flag)				//�ޱ���   //�豸�Ƿ������ݳ����־:public_val.Err_Flag�Ƿ�Ϊ1.
				{
					//add by 0322
					//���������־
					//del 0407			//��Ϊ�����沢û�н�����ָ���������
					//���λ�ã�����������λ����0,���򣬻����״̬��ȷ��������(��:���´��й���ʱ,cAlarmFlagΪ1,��cMainUiFlagҲΪ1������)
					//���ߣ�����������2����䣬��û���κι��ϵ�����£��������Զ��ָ��������棬���ȣ��轫;cFirstEnterAlarmFlag = 0, Ȼ��
					//���ϻָ������������Щ��䣬Ȼ�󣬻���Ҫ����һ������: cFirstEnterNormalFlag.(���ߣ�ʹ��cMainUiFlag��־����cMainUiFlag��־,
					//���:cMainUiFlag==0,����Ϊ�ǵ�һ�ν������������棬��ʾ�������棻���򣬲��ڴ�ˢ��������)
					//cAlarmFlag = 0;				
					//cMainUiFlag = 1;		//�ָ���������	
					//del 0407
					//add by 0322 yi
					
					/*
					//add 0407 10:08
					//������Ƶľ����������:
					///////////////////////////////////////////////////////////////////////
					 cAlarmFlag = 0;
					 cFirstEnterAlarmFlag = 0;		//Ϊ�ڶ��ι�������ʱ��ͨ���ж�:cFirstEnterAlarmFlag == 0,������Ͻ�����׼����
					 if (cMainUiFlag == 0)			//�ǵ�һ���й���״̬�л�������״̬
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
							
							
							szDispHz1 = "ֽ��";
							szDispHz2 = "����";
							szDispHz3 = "����";
						
							unRow1Data = unPaterLength;
							unRow2Data = unBunchNum;
							unRow3Data = TotalNum;
							
							Main_disp_other = 0;
							
							//add 0327 06:32
							cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
							
							//cFirstEnterAlarmFlag = 0;
							//add 0403					 
					 }
					 //add 0407 10:08
					 ///////////////////////////////////////////////////////////////////////
					 */
					
					//unAlarmErrorSn = 0xff;		//��ʼֵ
					
					
					if(!public_val.Work_Pause_Flag)     /* ����ͣ��ˢ����ʾ��ǰ��ϴ���� */
					{
						if(public_val.Bell_Flag == YES)   /* ����������ͣ�� added for v19416 */
						{
							cPauseBell = 1;
							if(!war_bell_step) war_bell_step = 1;
						}
						else
						{
							Bell_flag &= 0xfd;
						}
						
						//�ޱ���,����ͣ,ˢ����ʾ��ǰ��������.
						DisplayValueTemp = (long)public_val.main_disp_val; 				//����ʾֵ�����Ǵ�������
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[0],0);						
						
						//���´�����������ʾֵ
						unRow3Data = DisplayValueTemp;    //add 0324
						TotalNum = unRow3Data;				
					
						
					}
					else
					{
						LEDShowFlashString("P--",0);						//��ʾ��Ŀ��������
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
				else			//����
				{
			
					if ((public_val.Err_Flag == P_FINISH_LEARN1)
						|| (public_val.Err_Flag == P_FINISH_LEARN2)) //��ѧϰ��ɳ���������������
					{
						//ѧϰ���Ҳ��ʾ��ֽ����
						if(public_val.Bell_Flag == YES)   /* ����������ͣ�� added for v19416 */
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
						
						//unAlarmErrorSn = 0xff;		//��ʼֵ
						
						
					}
					else		 // ��������			(��������������)
					{
						
						//��ʾ�����־
					//	LEDShowFlashString("E--",0);		/* modified by james at 2014-4-12 */
						
						
						//Ϊ�˲������������ѭ����Ӱ�������������У�����ʱ�������  0325 17:13
						//�ñ�����־
						//add by 0322 yi
						cAlarmFlag = 1;
						
						unAlarmErrorSn = public_val.Err_Flag;
						
#if 1
						//��ֽ�����Թر�
						//ʹ����ֽ������
						//add 0324 19:11
						//del start
						//del 0402		//
						//add 0403
						if (cHandOptionFlag == 1)		//��˼Ϊ�������ֶ���������ʱ������ˢ�������Ͻ���
						{
							
							/*
							//add 0404 16:17							
							if (qz_wait == 1)
							{
								if(!war_bell_step) 
									war_bell_step = 1;			//�����������־			

									cPauseBell = 1;
									cQzFlag = 1;
									cMainUiFlag = 1;
									//del 0405
									//cTestKeyStringName = "��ֽ";
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
								if (cMainUiFlag == 1 )	//��ȷ�����������������,�����������򣬲�����
								//if (cMainUiFlag == 1 || unAlarmErrorSn == 0xff)
								{
									unAlarmErrorSn = public_val.Err_Flag;		//�ǵ�һ�ν���
									//add 0406
									cMainUiFlag = 0;
									//add 0406
									
									//add 0327 14:17								
									//���û�н���������ģʽ,������2�ΰ���"��ֽ"����������. ֻ�а���"ȷ��"��,������������2�ΰ�"��ֽ"��,������
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
						//��ֽ�����Թر�
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
							war_bell_step = 1;		//�����������ʼֵ
						
						//add 0322 by yi
						//���汨���������:
						//unAlarmErrorSn = public_val.Err_Flag;		//�Ƶ�ǰ��
						//add 0322 by yi
						
						//�Գ����־����,��ʾ�������
						//DisplayValueTemp = public_val.Err_Flag;
						
						/*
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[3],3);		//��ʾ�������:2λ.
						//�������,��: disp_buf��3,4λ, Ӧ��szDispHz1����һ��(�ڵ�һ��).
						*/
						
						cPauseBell = 0;		//����������ͣ����						
						
					}
				}

				//DisplayState: Ϊ��Ӧ����������״̬
				//Main_disp_other:ΪDisplayState�µ���״̬				
				
				if(l_stop_poll)												//ѭ����ʾ
				if(++CycShowCount > main_menu_cfg[0].m_time)				//---------------���ڼ������ж�     
				{
				    CycShowCount=0;
					DisplayItemNum++;
					TempCount = 0; 
				}
			}
			//��������,����enter��ʱ,����:Main_adj_key_delay = public_val.ms_timer; Main_disp_other = 2;
			else if(Main_disp_other == 2)    /* ��ʾ�汾�� */
			{
				//DisplayValueTemp = public_val.version;
//add 0321				
				unDownVerNum = public_val.version;
				//add by 0321
				
				//add 0402
				unRow1Data = unUpVerNum;			//����Ϊ������汾��
				
				unRow2Data = unDownVerNum;			//Ϊ���ذ�汾��
				unIoStatus = public_val.io_read;
				//chang 0403
				//unRow3Data = unIoStatus;	
				unRow3Data = glTotalAllCnt;			//��ʾ������
				//chang 0403
				
				if (clookVerFlag == 0)
					clookVerFlag = 1;
				
				cFirstDispIoStatusFlag = 0;
				
				//add 0402
				
				//����汾�ţ�������ѭ����ʾ��OK.
				/*
				//����汾���ڰ�������ʾ���Ұ汾��ֵ����Ҫˢ��
				sprintf(lcd_disp_buf,"DS48(180,1,'%d',15);\r\n",unRow1Data);		//�ϲ�								
													GpuSend(lcd_disp_buf);
													//add
													DELAY_US(UART2_DELAY);


				sprintf(lcd_disp_buf,"DS48(180,86,'%d',15);\r\n",unRow2Data);		//�ײ�		
								GpuSend(lcd_disp_buf);
								DELAY_US(UART2_DELAY);
				*/

				/*
				//add 0402
				sprintf(lcd_disp_buf,"DS32(136,184,'%s',15);\r\n",(int*)(&sziostatus[0]));		//״̬  
						//chang
				GpuSend(lcd_disp_buf);				
				DELAY_US(UART2_DELAY);
				//add 0402
				*/

				
				/*
				//�汾�ţ���ʹ��:DisplayBuff������ʾ.	������������ʽ��ʾ		
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);
				*/
				//�˵����µ���ʱʱ��: ����������,��"ȷ��"��,������ʾ�汾��,����һ����ʱ��,��ʾio��״̬.
				//��ʱʱ�䵽,��ʾio��״̬
				//test 0405
				if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY + 2)		//2000ms
				//if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY * 15 + 2)		//2000ms
				//test 0405
				{
					//��ʱʱ�䵽���Զ���ʾio��״̬
					//test 0405
					Main_disp_other = 3;		//��Ϊ��ʾio��״̬
					//test 0405
					
					cFirstDispIoStatusFlag = 0;
				}
				//return;		//ֱ�ӷ��أ���������һֱ��
			}			
			else if(Main_disp_other == 3)    /* ��ʾ������������IOֵ */
			{
				//DisplayValueTemp = (long)public_val.io_read;		//��ʾio״̬
				////DisplayValueTemp &= 0xff;
				//DisplayValueTemp &= 0xffff;
				
				//add 0322				
				unIoStatus = public_val.io_read;		//(unsigned int)DisplayValueTemp;				
				//add 0322 by yi
				
				//add 0402
				unRow3Data = unIoStatus;
				//add 0402
				
				//������
				//��һ�ε���,������������ʾ. ��ʾ�汾�ţ�����һ����ʱ�󣬽���״̬��ʾ
				//��һ״̬��ʾ��־
				
				if (cFirstDispIoStatusFlag == 0)
				{
					cFirstDispIoStatusFlag = 1;
					
					GpuSend("CLS(0);\r\n");		//��һ�ν������
					  DELAY_US(UART2_DELAY);			
					//�����������ʾ
					#if 0
					GpuSend("SPG(56);\r\n");	//60: ��ʾ����
					#endif
					
					#if 1
					GpuSend("SPG(60);\r\n");	//60: ��ʾ����
					#endif
					
					DELAY_US(UART2_DELAY);
					
					DELAY_US(UART1s_DELAY);
					DELAY_US(UART1s_DELAY);		//��������������,����S11��S12�޷���ʾ
					
					return;
				}
				
				cFirstEnterAlarmFlag = 0;
				
				/*/
				//���������ȱ����ˢ���ܿ졣
				//���ܷ�������ˢ��,���򣬷�������һֱ��				
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
				LED_ShowNumber(&DisplayBuff[0],0);			//ԭ����5λ������ʽ��ʾ����û�ж���Щ״̬λ���н���
				*/
				
				
			}
			else if (Main_disp_other == 4)    /* ��ֽ�ȴ� */
			{
					//LEDShowFlashString("Q2---",0);

					if(!war_bell_step) 
						war_bell_step = 1;			//�����������־			

					cPauseBell = 1;
					
					//if (cQzFlag == 0)
					//{
					//	cQzFlag = 1;
					//	qz_wait = 1;
					//}
					
					//add 0404 09:19
					//cTestKeyStringName = "��ֽ";		//����	

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
							cMainUiFlag = 1;		//����������䲻�ܼ�,���򣬱���״̬�£����¸ü������������᲻һ��
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
						szDispHz2 = "��ֽ,��ע��!!!";		//
						//szDispHz3 = "Q2---";
						szDispHz3 = "";
					
						cMainUiFlag = 0;
					}
					*/
					
					
					/*
					//del 0404 15:47
					szDispHz1 = "ֽ��";
					szDispHz2 = "����";
					szDispHz3 = "����";
				
					unRow1Data = unPaterLength;
					unRow2Data = unBunchNum;
					unRow3Data = TotalNum;
					
					Main_disp_other = 0;
					
					cMainUiFlag = 1;
					//del 0404 15:47					
					
					
					//add 0327 06:32
					cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
					//add 0327 06:32
					
					cFirstEnterAlarmFlag = 0;
					*/
					
					/*
					//��Ϊ������ѭ����,���ԣ����ﲻ��ֱ�ӵ�����ʾ������ˢ��̫��
					//chang 0402
					//TM1637_Write();					
					sprintf(lcd_disp_buf,"DS32(75,86,'��ֽ,��ע��!!!',15);\r\n");		//ԭΪzt: 48����,�ָ�Ϊ:32����
					GpuSend(lcd_disp_buf);
					DELAY_US(UART2_DELAY);
					return;
					//chang 0402
					//return;		
					//add 0324
					*/
			}
	    }

		
		if (test_key_cnt == TEST_MODE_CNT)  /* ���������������Բ������ý��� added by james for v19407 */
		{
			test_key_cnt = 0;
			DisplayState = ST_TEST;
			//��Ϊ������ʾ,��Ϊ��ֽ�ȴ�
			Main_disp_other = 4;
			
			test_set_display(test_mode);
		}		

	#if 0
		if (learn_key_cnt == LEARN_MODE_CNT)  /* ���������������Բ������ý��� added by james for v19407 */
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


		case ST_PASSWORD://---------------------------------------------�����������

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

									cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
									//del end
									
									if(!war_bell_step) 
										war_bell_step = 1;		//�����������ʼֵ
									
									//�Գ����־����,��ʾ�������
									
									//add 0322 by yi
									cPauseBell = 0;		//����������ͣ����
									
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

									szDispHz1 = "ֽ��";
									szDispHz2 = "����";
									szDispHz3 = "����";
								
									unRow1Data = unPaterLength;		//unPaterLength;
									unRow2Data = unBunchNum;		//unBunchNum;
									unRow3Data = TotalNum;	//TotalNum;
							
									cMainUiFlag = 1;
									
									//add 0327 06:32
									cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
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
	                if(PasswordBuff[4]=='0')//�������Ҫ��
	                {                
						/* ��������˵� */
					    pMenu = pItem[0].m_pcfg;	// ָ������				
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
						DisplayItemNum  = 0;

						//DisplayMenuTotal = Menu_Number[0];
						DisplayMenuTotal = Menu0_Number;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵�  
						
	                    return;
	                }
				#endif
				
					
						if(PasswordBuff[0]=='1')//
						if(PasswordBuff[1]=='0')//
						if(PasswordBuff[2]=='1')//
						if(PasswordBuff[3]=='0')//
						if(PasswordBuff[4]=='1')//�������Ҫ��
						{                   
						/* ��������˵� */
					  pMenu = pItem[0].m_pcfg;	// ָ������				
						now_menu.menu_id = 1;			//==1,����
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
										
						DisplayItemNum  = Menu0_Number;

						DisplayMenuTotal = Menu_Number[0];
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵�
						
						//�ò˵��༭��־
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
						
						szDispHz1 = "1-";			//��ָ�븳ֵ(��ַ)
						//szDispHz2 = *(&szMenuDispHz[0]);
						szDispHz2 = "��������ʱ��";
						
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
	                if(PasswordBuff[4]=='1')//�������Ҫ��
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
					
									//del 032511:42 //���°����������ﷵ��,�޷�ˢ��
					//return;		
									//del 032511:42
					
		            break;			
		    }
			
			//��������ʱ�����������
	        if( flash_flag  && !long_input_flag  )												//��˸����
	        {	
	            LEDClearChar(DisplayBitNum, 1);		//������λ����ʾ������0 (��ǰ����λ)
	        }
	        else
	        {
	            LEDShowFlashString(&PasswordBuff[0], 0);     //��ʾ���������뻺����������       
	        }	      
					
					//add
			TM1637_Write();		//���ú����ˢ��    //del 0325 11:44
					//add 0323
			    
			break;


		case ST_LEARN:

			switch(Key_Val) 
		    {
		        case KEY_SEL:  // ѡ�������
		        	Main_disp_other = 0;
					DisplayBitNum = 4;
					CycShowCount  = 0;
					now_menu.menu_id = 888;
					now_menu.event_flag = 1;
		            DisplayItemNum = 0;	
		            DisplayState   = ST_MAIN;											//����������
		            
					war_bell_step = 0;  // �������������
					cPauseBell = 0;

					return;
					
					break;	
					
				default:
					break;
			}

			if ((public_val.Err_Flag == P_FINISH_LEARN1)
				|| (public_val.Err_Flag == P_FINISH_LEARN2))
			{
				if (public_val.Err_Flag == P_FINISH_LEARN1)    // ѧϰ���
				{
					LEDShowFlashString("P-- 1",0);

					if(!war_bell_step) 
						war_bell_step = 1;						

					cPauseBell = 1;						
				}
				else if (public_val.Err_Flag == P_FINISH_LEARN2)    // ѧϰ���
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

			
		case ST_TEST:        /* ���Բ������ý�����Ӧ added by james for v19407 */
			switch(Key_Val)
		    {
		        case KEY_SEL:
		        	Main_disp_other = 0;
					DisplayBitNum = 4;
					CycShowCount  = 0;
					now_menu.menu_id = 888;
					now_menu.event_flag = 1;
		            DisplayItemNum = 0;	
		            DisplayState   = ST_MAIN;											//����������
		            break;	

				case KEY_INCREASE:
					if (test_mode < 1)
					{
						test_mode++;

						write_test_mode_flag = 1;  /* д����ģʽ������ */
					}
					test_set_display(test_mode);
					
					if (test_mode > 0)  /* ����ģʽ */
					{
						//ResetDoorForTest();
					}					
					break;

				case KEY_REDUCE:
					if (test_mode > 0)
					{
						test_mode--;

						write_test_mode_flag = 1;  /* д����ģʽ������ */						
					}
					test_set_display(test_mode);					
					break;

				case KEY_IO7:  // ����
					LEDShowFlashString("   11",0);
				//add 0325
					cTestKeyStringName = "����";
				//add 0325
					break;

				case KEY_IO8:  // ֹͣ
					LEDShowFlashString("   12",0);
				  //add 0325
					cTestKeyStringName = "ֹͣ";
				//add 0325
					break;

				case KEY_RED:  // ��ֽ
					LEDShowFlashString("   13",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
				//add 0325
					break;

				case KEY_IO9:  // ��ֽ
					LEDShowFlashString("   14",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
				//add 0325
					break;

				case KEY_IO2:  // ��ֽ
					LEDShowFlashString("   15",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
				//add 0325
					break;

				case KEY_IO1:  // ��ֽ
					LEDShowFlashString("   16",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
				//add 0325
					break;

				case KEY_IO4:  // ����
					LEDShowFlashString("   17",0);
					//add 0325
					cTestKeyStringName = "����";
				//add 0325
					break;

				case KEY_IO5:  // ����
					LEDShowFlashString("   18",0);
					//add 0325
					cTestKeyStringName = "����";
				//add 0325
					break;

				case KEY_IO6:  // ֹͣ��ֽ
					LEDShowFlashString("   19",0);
					//add 0325
					cTestKeyStringName = "ֽͣ";
				//add 0325
					break;

				case KEY_IO3:  // ��ֽ
					LEDShowFlashString("   20",0);
					//add 0325
					cTestKeyStringName = "��ֽ";
				//add 0325
					break;
					
				case KEY_GREEN:  // ֽ��
					LEDShowFlashString("   21",0);
					//add 0325
					cTestKeyStringName = "ֽ��";
				//add 0325
					break;

				case KEY_BLUE:  // ����
					LEDShowFlashString("   22",0);	
					//add 0325
					cTestKeyStringName = "����";		//����
				//add 0325
					break;
					
				default:
					break;
			}

		break;
			
	    case ST_MENU:			/* �˵����ݲ鿴 */
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
							DisplayState   = ST_MAIN;											//����������
					//DIS_OPTIONING=0;
						
						//ֱ�ӷ���������
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

										cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//�����������ʼֵ
										
										//�Գ����־����,��ʾ�������
										
										//add 0322 by yi
										cPauseBell = 0;		//����������ͣ����
										
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
										
										szDispHz1 = "ֽ��";
										szDispHz2 = "����";
										szDispHz3 = "����";
									
										unRow1Data = unPaterLength;
										unRow2Data = unBunchNum;
										unRow3Data = TotalNum;
										
										Main_disp_other = 0;
										
										cMainUiFlag = 1;
										
										//add 0327 06:32
										cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
										//add 0327 06:32
										
										cFirstEnterAlarmFlag = 0;
									
									//del 0325 11:58
									//	TM1637_Write();		//д��ʾ
										//del 0325 11:58
									}
						
		            break;	
						
		      case KEY_REDUCE:   //�ϼ�,�鿴��һ�˵�����
						
					if (now_menu.menu_id == 0)
					{
						if (DisplayItemNum-- == 0)			//DisplayItemNum: �˵������
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
					
		        case KEY_INCREASE:		//�ϼ�,�鿴��һ�˵�����

					if (now_menu.menu_id == 0)
					{
						if (++DisplayItemNum >= DisplayMenuTotal) 	//DisplayItemNum:�˵��������1
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
						
					zc_key_flag = 5;		//�˵��鿴ȷ�����������޸�					
					    
		       break;			
		    }        

			//������ֽ����, �����ST_MENUģʽ,�ڸ�ģʽ,���zc_key_flag�Զ�����, ��zc_key_flag>5ʱ����Ĭ�ϰ���ȷ���������Զ�����༭����(�൱�ڰ���enter��)
			if (now_menu.menu_id == 0 && zc_key_flag)
				{
					zc_key_flag ++;
				}

			if(zc_key_flag == 5)
				{
						zc_key_flag = 0;
					TempDataB = DisplayItemNum;				//�˵���ţ������︳ֵ��Ȼ��,ʹ��: pDataValue=&pMenu[TempDataB];, �ٻ�ȡ:pDataValue->m_data_vale��ֵ
					if (TY_D == TY_B  || 
						TY_D == TY_D  ||  
						TY_D == TY_H)									//��ֵ��
					{
						DisplayBitNum = 4;
						DataType=TY_D;
						TempInputData = *(int *)pDataValue->m_data_vale;
						
						//TempInputData��ֵ���������:pDataValue->m_data_vale�Ƿ�Ϊͬһֵ��
						
						DisplayState=ST_DATA_INPUT;										//��������������

						pDataValue=&pMenu[TempDataB];			//����ǰ�˵�ָ�븳ָ���ֵ
						DataType=TY_D;
						TempValue=DataToStr(pDataValue->m_data_vale,
					                        INT16_SIZE,
										    DEFAUT_POSE,
										    DEFAUT_POT,
										    DisplayBuff,0,DataType);				
						
						//InputDataLen: �ò�����Ĭ�ϳ��� (����3λ,2λ����1λ)
						InputDataLen = DEFAUT_POSE+DEFAUT_POT;    //3 + 0.  ����: InputDataLen == 3
						//DisplayBitNum = InputDataLen - 1; 
						PasswordBuff[0] = '0';
						PasswordBuff[1] = '0';
						//ʵ���ϣ��ڷ��صĴ���ʱ,��û���жϿո���,����,����Ϊ��:return 0;
						//add by 0321
						//���صĿո���Ϊ: TempValue
						//TempValue = 0;		//ʵ���Ϸ���ʼ��Ϊ0.
						//add by 0321
						switch(InputDataLen)
						{	/*���ڸ�����ʾ����������*/				//������ʾ���������ݵ� PasswordBuff(�ַ����洢). �ڲ����༭ʱȡ��    
						case 3:    //���ݳ���Ϊ3λ
							PasswordBuff[2]=DisplayBuff[TempValue];      //
						//�����3λ����,��3λ(���λ)Ϊ�ո�,��'0'����. ��ʾ��������ߴ洢: PasswordBuff[0]����
							if(PasswordBuff[2] == ' ') PasswordBuff[2] = '0';
							PasswordBuff[3]=DisplayBuff[TempValue+1];
						 //��4λΪ�ո�,��'0'����
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];		//���һλ��������Ϊ�ո�		
						    break;			
						case 2:    //���ݳ���Ϊ2λ
							PasswordBuff[2]=' ';    //����3λ��Ϊ�ո�
							PasswordBuff[3]=DisplayBuff[TempValue+1];		//���ֻ��һλ�ո�
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];				
						    break;		
						case 1:    //���ݳ���Ϊ1λ, ��2λ��Ϊ�ո�
							PasswordBuff[2]=' ';
							PasswordBuff[3]=' ';
							PasswordBuff[4]=DisplayBuff[TempValue+2];
							break;				
						}
						pr_long_input_flag = 0;
						DisplayState=ST_DATA_INPUT;										//��������������
					}	
				}
				
			if(TempCount > 0)  	    
			{	
				TempCount-- ;
				break;          
			}
			
			//��ȡ�˵�ˢ��Ƶ��
			TempCount = main_menu_cfg[0].m_rate;  
			
			  //��ȡ��ǰ��ʾ�˵���  �˵�ָ��
			pDataValue=&pMenu[DisplayItemNum];	//pMenu: MENU_SHOW * pMenu;								//�˵�ָ��
		    TempInputData = *(int *)pDataValue->m_data_vale;   //��ȡ�˵���ָ����ֵ       //ʹ��lcdʱ,Ҳ��ֱ����ʾ����ֵ

			//��ʾ������Ŀ���,����ʾ��Ӧ��ŵĲ���
			if (now_menu.menu_id == 0)
			{
				LEDShowCaption(DisplayItemNum);		//��ȡ��Ӧ�˵���ŵ�������
				
			}
			if (now_menu.menu_id == 1)
			{
				LEDShowCaption(DisplayItemNum - Menu0_Number);
			}
			
			//���˵�ָ����ֵ:TempInputData,ת��Ϊ�ַ���.  ��ʾ����������
			//����ʵ����Ϊ�˵��鿴ʱ����ʾֵ
			TempValue=DataToStr(&TempInputData,   //pDataValue->m_data_vale
			                    INT32_SIZE,      //pDataValue->m_size,
								DEFAUT_POSE,
								DEFAUT_POT,
								DisplayBuff,0,TY_D );		
			//�Ӳ������ո��DisplayBuff,������ʾ������2��ʼ��λ��(���3λ)
			LED_ShowNumber(&DisplayBuff[TempValue],2);    //TempValue:���ظ�λ�ո���, 2:��ʾ�ӵڶ�λ��ʼ��ʾ, 0,1Ϊ��ʾ�˵����
	    }
			
			if (Key_Val != KEY_SEL)			
			{
				unRow1Data = 0;
				unRow2Data = 0;
				unRow3Data = 0;	
			}
			
	    break;

	    case ST_DATA_INPUT: /* �������뻭�� */
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
							DisplayState   = ST_MAIN;											//����������
							
													
							//add 0325
							if (cZcKunShuFlag)
							{
								cZcKunShuFlag = 0;
								//cZcOptSelEnterRtnMainFlag = 1;
							}
							//add 0325							
					
							//ֱ�ӷ���������
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

								cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
								//del end
								
								if(!war_bell_step) 
									war_bell_step = 1;		//�����������ʼֵ
								
								//�Գ����־����,��ʾ�������
								
								//add 0322 by yi
								cPauseBell = 0;		//����������ͣ����
								
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

								szDispHz1 = "ֽ��";
								szDispHz2 = "����";
								szDispHz3 = "����";
							
								unRow1Data = unPaterLength;
								unRow2Data = unBunchNum;
								unRow3Data = TotalNum;
								
								Main_disp_other = 0;
								
								cMainUiFlag = 1;
								
								//add 0327 06:32
								cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
								//add 0327 06:32
							
								//	TM1637_Write();		//д���
							}							
						}
					else
		        		DisplayState = ST_MENU;			
								
					
		            break;	
		        case KEY_REDUCE: //����
		        	if(pMenu[DisplayItemNum].ext_property & 0x01)
		        		{
		        			;
		        		}
		        	else
		        		{
							TempInputData-=long_key_step;
							if(TempInputData<pDataValue->m_min) //���Ƿ񳬷�Χ
								TempInputData = pDataValue->m_min; //pDataValue->m_min;   //���Ƴ���Χ
						}
						
						//add
					//	GpuSend("CLS(0);\r\n");
					//	DELAY_US(UART2_DELAY);
					//add
						
		            break;	
		        case KEY_INCREASE: //����
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
								TempInputData=StrToData(PasswordBuff,5,DataType);		//��PasswordBuff�л�ȡ���ݵ� :TempInputData
								//���Ƿ񳬳�������С��Χ
						if(TempInputData <= pDataValue->m_max && TempInputData >= pDataValue->m_min)    //pDataValue�ڽ���ST_DATA_INPUT֮ǰ����ȷ����ʱ,���Ѹ���ǰ�˵���ָ���ֵ
						{
					        switch(INT16_SIZE)
						    {
						    case INT32_SIZE:
						        *(long *)pDataValue->m_data_vale = TempInputData;			//���༭����������˵����ݻ�����
						        break;
						    case INT16_SIZE:
						        *(int *)pDataValue->m_data_vale = (int)TempInputData;
						        break;			
						    case INT8_SIZE:
						        *(char *)pDataValue->m_data_vale = (char)TempInputData;
						        break;			
							}	
							if(PasswordBuff[2] == '0')    //ǰ����Ϊ�ո�,��Ϊ0; ����Ϊ�Ƿ��Ϊ�ո�
								{
									PasswordBuff[2] = ' ';
									if(PasswordBuff[3] == '0') PasswordBuff[3] = ' ';
								}
								//��led��,��ȡ�ո�(0x20)��λ����Ϊ0,���Բ���ʾ.
								//����lcd�оͲ�ͬ,0x20��asc��ֵΪ32.
							LED_ShowNumber(&PasswordBuff[2],2);

							//�˵��༭����,��"ȷ��"��,�洢���в���
							func_menu_update(0);		//��ɶԲ˵����ݵĴ洢���������õĲ��������浽eeprom
							
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
								//ʵ��������������Զ�����������(����Ҫ��ʱ)
											DisplayState   = ST_MAIN;											//����������
								
								//add
								//if (cZcKunShuFlag == 1)
								//{
								
										//��˵��༭��־
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
										//ֱ�ӷ���������
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

										cMainUiFlag = 0;		//���棺	DisplayState == ST_ERROR.			  
										//del end
										
										if(!war_bell_step) 
											war_bell_step = 1;		//�����������ʼֵ
										
										//�Գ����־����,��ʾ�������
										
										//add 0322 by yi
										cPauseBell = 0;		//����������ͣ����
										
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
										

										szDispHz1 = "ֽ��";
										szDispHz2 = "����";
										szDispHz3 = "����";
									
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
										cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
										//add 0327 06:32
										
										//add 0404 11:54		//ֱ�ӷ���
										
										
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
										
										
										//unRow3Data = TotalNum;				//����ѭ���У��鿴�����仯ֻ��������ֵ
									
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
								
										//TM1637_Write();		//д��ʾ                //����������䣬�������޷���̬��ʾ
									}
										
										//add 0324
					//					cMainUiDispFlag = 1;
										//add 0324
									
										//return;						//����ӷ���				
								//	}
							
							//}
							//add 0323
						}
					else
		        		DisplayState = ST_MENU;	
					
		            break;			
		    }
				
				
				//��˸����ʾ��һ����������ź�����
	        if( flash_flag && !long_input_flag )  //��˸����
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
						LED_ShowNumber(&PasswordBuff[2],2);				//����ˢ����ʾ������
						
						
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
		//			TM1637_Write();		//д��ʾ
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
			flash_flag = !flash_flag;		//��˸��־
			UpdateState = 0;
			
			//add 0326 20:21
			cFlashAlarmFlag = !cFlashAlarmFlag;
			//add 0326 20:21
			
			//0330 14:25
			cKeyOptionFlash = !cKeyOptionFlash;
			//0330 14:25
		}		
	}


	//����war_bell_stepΪ1,������������ͣ���
	switch(war_bell_step)  /* �������������� */   //��ʱ���������ʱ��Ͳ����ʱ��
	{
		case 1:
			war_bell_delay = public_val.ms_timer;
			Bell_flag |= 0x02;
			war_bell_step = 10;
		break;
		case 10:
			iBellOnTime = (cPauseBell == 0) ? 500 : 50;		//cPauseBell: 1������������ͣ��
			//iBellOnTime = (cPauseBell == 0) ? 2000 : 50;
			if(public_val.ms_timer - war_bell_delay > iBellOnTime) //���ʱ��
				{
					war_bell_delay = public_val.ms_timer;
					Bell_flag &= 0xfd;
					war_bell_step = 20;
				}
		break;
		case 20:
			iBellOffTime = (cPauseBell == 0) ? 2000 : 2000;			//����������ͣ����
			//iBellOffTime = (cPauseBell == 0) ? 200 : 2000;			
			if(public_val.ms_timer - war_bell_delay > iBellOffTime) //�����ʱ��
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
					war_bell_step = 0;   /* �ޱ��� */
				}
			else
				{
					war_bell_step = 1;		//���ݴ����־public_val.Err_Flag�����÷��������ֵ
				}
		break;
	}
	
//#define BELL P25	
	
	if(Bell_flag)		//���������־  1,2 ��  0�����졣(1:������, 2: ������)
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
	if (clookVerFlag == 1)		//�汾�鿴ʱ,io״̬�п���Ϊ0
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
	

	TM1637_Write();		//д��ʾ
	
}
		

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
���ܣ����α��ַ���

������nBit:����λ������nPoint:С��λ����  ,nFlag==1����λǰ��0�Կո񲹳�

����ֵ����λ�ո���
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static char DataToStr(void * const pData,char size,char nBit,char nPoint,char *string,char nFlag,char nType)
{	
	long DataS32Bit;
	char spaces;
	char Temp,module;
	char SignZero;//������־λ,  >0: 0,  <0: 1
    	
	if(size==0 || size>4)//�ж�size�ĺ�����
	    return 0;
	size--;    //1
//	if( ( *((char *)(pData)+size) ) & 0x80 )//�ж����λ�Ƿ�Ϊ1
//	    SignZero=1;
//	else
	    SignZero=0;	//����
	    
    DataS32Bit=0;  //����ֵΪ0
	if(size==3)//LONG������
	{			    
		if(SignZero)
		    *((long *)&DataS32Bit) = 0 - *((long *)pData);
		else
		    *((long *)&DataS32Bit) = *((long *)pData);
	}	
	else if(size==1)//INT������   (������ʵ��Ϊ����)
	{
	    if(SignZero)
		    *((int *)&DataS32Bit) = 0 - *((int *)pData);						
		else
		    *((int *)&DataS32Bit) = *((int *)pData);	//Ϊ���� ---------   ��ȡpData��ֵ��DataS32Bit
	}	
	else if(size==0)//char������
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
        module=10;		//������,nType == TY_D (10����), ����, module=10;
		break;
	case TY_H:
        module=16;
		break;
	}

    spaces = 3-(nPoint+nBit);     //��ȡ spaces��ֵ(Ϊ0). ������nBit==3, nPoint = 0
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
					  //�ڶ���ѭ�����Ժ�, ȡDataS32Bit��ֵ��string[Temp].
            string[Temp]='0'+(char)(DataS32Bit%(module));
            DataS32Bit/=module;
        }
        else 
				{
					if(nPoint != 0)   //nPoint==0
							string[Temp]='.';
					else
						string[Temp]=' ';  //��һ��ѭ��(Temp==3): string[3] = ' '.
				}
        
    }while( Temp-- >0 );
        
	if(nFlag)//nFlag==1����λǰ��0�Կո񲹳�
	{
	    if(SignZero && spaces<3)
		    string[spaces]='-';
		return 0;
  }
	
	
	Temp=0;
	spaces=0;
	do
	{
		 //�жϻ�ȡ��string[Temp],����Ϊ0����.
		 if(string[Temp]=='0' && (Temp + 1 < nBit)) 
		 {
			 string[Temp]=' ';     //string[0]�洢�������λ��ֵ��string[2]�洢���λ��ֵ,�ȴ����λ��ʼ�ж�
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
	    string[--spaces]='-';   //����Ǹ���,����'-'
	}
	
	Temp=nPoint+nBit-spaces;
	
	if(Temp<3)
	    spaces -= (3-Temp);
	return spaces;        
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++
���ܣ���Ҫһ���������ַ�������

������len Ϊ�ַ�����

����ֵ����
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
	if(string[0]=='-' || string[1]=='-' || string[2]=='-' )//����
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
	if (qz_wait == 1 || cQzFlag == 1 )		//�Ƿ������������: ��ֽ������ʱ��δ��ȷ�����������ֶ������ܲ���
	{
		qz_wait = 0;   // ��ȴ�״̬								
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
			
			
			szDispHz1 = "ֽ��";
			szDispHz2 = "����";
			szDispHz3 = "����";
		
			unRow1Data = unPaterLength;
			unRow2Data = unBunchNum;
			unRow3Data = TotalNum;
			
			//Main_disp_other = 0;
			
			cMainUiFlag = 1;
			
			//add 0327 06:32
			cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
			
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
			
			
			szDispHz1 = "ֽ��";
			szDispHz2 = "����";
			szDispHz3 = "����";
		
			unRow1Data = unPaterLength;
			unRow2Data = unBunchNum;
			unRow3Data = TotalNum;
			
			//Main_disp_other = 0;
			
			cMainUiFlag = 1;
			
			
			cMainNotFlashFlag = 0;		//��һ�ν���, ȫ����ʾ
			
			cFirstEnterAlarmFlag = 0;
		}
	}
	
	if (qz_wait == 1 || cQzFlag == 1 )		//�Ƿ������������: ��ֽ������ʱ��δ��ȷ�����������ֶ������ܲ���
	{
		qz_wait = 0;   // ��ȴ�״̬								
		cQzFlag = 0;
	}
}

#endif

