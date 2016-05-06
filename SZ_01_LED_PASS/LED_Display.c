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

#ifdef DISP_C

#define BELL P25

#define MAIN_ADJ_DELAY 2000
#define KEY_DOU_DELY 50
#define KEY_LONG_DELY 100
#define BELL_ON_DELY 50
#define LONG_KEY_STEP_SWITCH 50
#define DISP_ERR_TIMER 1000 //��ʱ�����޸�ʧ��ʱ�� �� ������ʾʱ��

//#define NO_KEY_RETURN_MAIN 15000 //00000�˵��ް���������ҳ����ʱ
#define NO_KEY_RETURN_MAIN 30000 //00000�˵��ް���������ҳ����ʱ

#define TEST_MODE_CNT    40
//#define LEARN_MODE_CNT   20
#define LEARN_MODE_CNT   30


extern char eeprom_en;
extern xdata unsigned int disp_buf[5];

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

//extern void ResetDoorForTest();


//sbit P17 = P1^7;
//sbit P37 = P3^7;
sbit P25 = P2^5;
//sbit P21 = P2^1;
//sbit P22 = P2^2;
//sbit P14 = P1^4;

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

static xdata char  DisplayMainID;									//���������
static xdata char  DisplayMenuID;									//�˵��������

static xdata char  DisplayMainTotal;								//����������
static xdata char  MenuTotals;									//���˵���������


static xdata char  DisplayMenuTotal;								//�˵���������

static xdata char  DisplayItemNum;								//��ʾ��Ŀ���
static xdata char  DisplayBitNum;									//��ʾλ��

//static xdata char  MenuUpdateState;								//�˵���ʾ����״̬
static xdata char  PasswordBuff[5];								//�������ݻ�����
static xdata char  DisplayBuff[10];

int  DisplayState;											//����״̬
int  LCDDisplayState;

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


xdata char uart_send_buf[100];


//�˵����ý���
//�����鶨��
//#define DEBUG_MENU_GS_VAL 30
//#define USE_MENU_NUM 16

static xdata MENU_SHOW Menu00000[MENU_ID_00000_MAX_ITEM];
static xdata MENU_SHOW Menu10101[MENU_ID_10101_MAX_ITEM];
static xdata MENU_SHOW Menu10102[MENU_ID_10102_MAX_ITEM];
static xdata MENU_SHOW Menu10103[MENU_ID_10103_MAX_ITEM];
static xdata MENU_SHOW Menu10104[MENU_ID_10104_MAX_ITEM];
static xdata MENU_SHOW Menu20101[MENU_ID_20101_MAX_ITEM];

xdata char Menu_Number[MENU_NUM] = {MENU_ID_00000_MAX_ITEM,
									MENU_ID_10101_MAX_ITEM,
									MENU_ID_10102_MAX_ITEM,
									MENU_ID_10104_MAX_ITEM,
									MENU_ID_20101_MAX_ITEM
	};
//xdata char Menu0_Number = 10, Menu1_Number = 20;

/*--------------�˵����û��� ��������-------------*/
code MENU_ITEM MenuItem[]={
  //�˵����,�˵���ַ,���� ���ݴ洢�׵�ַ ������ݸ���
	{MENU_ID_00000,Menu00000,"00000",EEPROM_MENU_1,MENU_ID_00000_MAX_ITEM,0,&Menu_Number[0]}, 
	{MENU_ID_10101,Menu10101,"10101",EEPROM_MENU_2,MENU_ID_10101_MAX_ITEM,0,&Menu_Number[2]}, 
	{MENU_ID_10102,Menu10102,"10102",EEPROM_MENU_3,MENU_ID_10102_MAX_ITEM,0,&Menu_Number[3]}, 
	{MENU_ID_10103,Menu10103,"10103",EEPROM_MENU_4,MENU_ID_10103_MAX_ITEM,0,&Menu_Number[4]}, 
	{MENU_ID_10104,Menu10104,"10104",EEPROM_MENU_5,MENU_ID_10104_MAX_ITEM,0,&Menu_Number[5]}, 
	{MENU_ID_20101,Menu20101,"20101",EEPROM_MENU_6,MENU_ID_20101_MAX_ITEM,0,&Menu_Number[6]}, 
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
 

xdata char speed_mode = 1;   //�ٶ�ģʽ: 1���٣� 0����

xdata char qz_wait = 0;      //�ȴ���ֽȷ��: 0δ�ȴ���1�ȴ�

//add by yq
xdata char Menu0_Number;						   
						   
/*++++++++++++++++++++++++++++++++++++++++++++++++
���ܣ���ʾ�ַ�����

������buff:�����͵����ݻ�������x:����

����ֵ���ޡ�

˵����
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LEDShowFlashString(const char  * buff,char x) 
{
	do
	{
		disp_buf[x++] = LED_ASCII[*buff++];
	}while(x<6 && *buff!=0);
}

void  LEDShowCaption(char num) 
{
char *buff,x = 0;
switch(num)
	{
		case 0:
			buff = "1-";
		break;
		case 1:
			buff = "2-";
		break;		
		case 2:
			buff = "3-";
		break;	
		case 3:
			buff = "4-";
		break;		
		case 4:
			buff = "5-";
		break;		
		case 5:
			buff = "6-";
		break;		
		case 6:
			buff = "7-";
		break;		
		case 7:
			buff = "8-";
		break;		
		case 8:
			buff = "9-";
		break;		
		case 9:
			buff = "10";
		break;		
		case 10:
			buff = "11";
		break;		
		case 11:
			buff = "12";
		break;		
		case 12:
			buff = "13";
		break;	
		case 13:
			buff = "14";
		break;	
		case 14:
			buff = "15";
		break;	
		case 15:
			buff = "16";
		break;	
		case 16:
			buff = "17";
		break;
		case 17:
			buff = "18";
		break;	
		case 18:
			buff = "19";
		break;	
		case 19:
			buff = "20";
		break;			
		case 20:
			buff = "21";
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
	do
	{
		disp_buf[x++] = LED_ASCII[*buff++];
	}while(x<6 && *buff!=0);
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

����ֵ���ޡ�

˵���� �ޡ�
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LEDClearChar(char x,char n)
{
	char i;
	for(i = x; i< (x+n); i++ )
		{
			disp_buf[i] = 0;		
		}
}


/*++++++++++++++++++++++++++++++++++++++++++++++++
���ܣ���ʾ���֡�
������buff:�����͵����ݻ�������x:����
����ֵ���ޡ�
˵����
++++++++++++++++++++++++++++++++++++++++++++++++*/
void  LED_ShowNumber(char * buff,char x) 
{
	char TempFlag=0;
	do
	{
		if(*(buff+1)!='.' || TempFlag==1)
		{
			disp_buf[x++] = LED_ASCII[*buff++];
			//SendLed(x++,LED_ASCII[*buff++]);
		}
		else
		{
			disp_buf[x++] = LED_ASCII[*buff++] | 0x80;
			//SendLed(x++,(LED_ASCII[*buff++] | 0x80));
			TempFlag=1;
			buff++;
		}
	}while(x<6);
	
}

//�˵�����ָ��
void menu_cfg(void)
{
	char i;
	for(i = 0; i < MENU_ID_00000_MAX_ITEM ; i++)
		{
			Menu00000[i].m_data_vale = &public_val.Disp_Dz_00000[i];
		}

	for(i = 0; i < MENU_ID_10101_MAX_ITEM ; i++)
		{
			Menu10101[i].m_data_vale = &public_val.Disp_Dz_10101[i];
		}

	for(i = 0; i < MENU_ID_10102_MAX_ITEM ; i++)
		{
			Menu10102[i].m_data_vale = &public_val.Disp_Dz_10102[i];
		}
	
	for(i = 0; i < MENU_ID_10103_MAX_ITEM ; i++)
		{
			Menu10103[i].m_data_vale = &public_val.Disp_Dz_10103[i];
		}

	for(i = 0; i < MENU_ID_10104_MAX_ITEM ; i++)
		{
			Menu10104[i].m_data_vale = &public_val.Disp_Dz_10104[i];
		}

	for(i = 0; i < MENU_ID_20101_MAX_ITEM ; i++)
		{
			Menu20101[i].m_data_vale = &public_val.Disp_Dz_20101[i];
		}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   read_menu_data(char menu_id)
���ܣ�  ��ȡ�˵��������� �� �˵�����
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
	
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max; i++)
		{
			if(menu_id == MENU_ID_10104)
				MenuItem[menu_id].m_pcfg[i].m_max = 450;
			else
				MenuItem[menu_id].m_pcfg[i].m_max = 999;
			MenuItem[menu_id].m_pcfg[i].m_min = 0;
			MenuItem[menu_id].m_pcfg[i].ext_property = 0; //read_byte;
		}
	MenuItem[MENU_ID_10104].m_pcfg[PASS_LOCK_COUNT - 1].m_max = 500;
	
	switch(menu_id)
		{
			case MENU_ID_00000:
				progbyte = (unsigned char *)&public_val.Disp_Dz_00000[0];
			break;			
			case MENU_ID_10101:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10101[0];
			break;
			case MENU_ID_10102:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10102[0];
			break;
			case MENU_ID_10103:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10103[0];
			break;
			case MENU_ID_10104:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10104[0];
			break;
			case MENU_ID_20101:
				progbyte = (unsigned char *)&public_val.Disp_Dz_20101[0];
			break;
		}
	progadd = MenuItem[menu_id].eeprom_add;
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max + 1; i++)
		{
			*progbyte = IapReadByte(progadd++);
			progbyte++;
			*progbyte = IapReadByte(progadd++);
			progbyte++;
		}
		
	
}

//���˵�ֵ���µ�ͨѶ�˵�ֵ���ϲ������˵���
void update_menu_to_com(void)
{
	char TempB;
	for(TempB = 0; TempB < MENU_ID_00000_MAX_ITEM; TempB++)
		{
			public_val.menu_parm[TempB] = public_val.Disp_Dz_00000[TempB];
		}

	for(TempB = 0; TempB < MENU_ID_10101_MAX_ITEM; TempB++)
		{
			public_val.menu_parm[TempB + MENU_ID_00000_MAX_ITEM] = public_val.Disp_Dz_10101[TempB];
		}	
}

//��ȡMCU��ID�� ֵ����:0D 00 00 2C 02 4B E5
//					   0D 00 00 30 03 64 E4(��������MCU)
void read_id()
{
    char code *cptr;
	char i,read_byte[7];
    cptr = ID_ADDR_ROM;         //�ӳ�������ȡID��
    for(i=0; i<7; i++)         //��7���ֽ�
	    {
	    	read_byte[i] = *cptr++; // 0xff; // 
	    }
	public_val.password_index_disp = (int)read_byte[5];
	public_val.password_index_disp <<= 8;
	public_val.password_index_disp |= (int)read_byte[6] & 0xff;
	public_val.password_index = public_val.password_index_disp % 2000;

	cptr = EEPROM_PASSWORD;
	read_byte[0] = IapReadByte(EEPROM_PASSWORD + (public_val.password_index<<1));
	read_byte[1] = IapReadByte(EEPROM_PASSWORD + (public_val.password_index<<1)+1);
	public_val.password_val[0] = (int)read_byte[0] & 0xff;
	public_val.password_val[1] = (int)read_byte[1] & 0xff;
}	

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

	//LEDShowFlashString("Lt-2 ",0);
	LEDShowFlashString("60409",0);    /* ��ʾ������汾�� */
	
	TM1637_Write();

	///��ͳ����������Ҫ��ʾ��������
	pMain = main_menu_cfg[0].m_main;
 	for( TempB = 0; pMain[TempB].m_str; TempB++ ) ///<���մ���Ϊ0������Ϊ�١�
		{
			;
		}			
	DisplayMainTotal = TempB;

	///��ͳ���¼��Ӳ˵�������
	pItem = main_menu_cfg[0].m_menu;
	pMenu = pItem[0].m_pcfg;
	for(TempB = 0; TempB == pItem[TempB].MenuID; TempB++) 
		{
			read_menu_data(TempB); //��ȡ�˵�����ֵ;
		}
	MenuTotals = TempB;
	update_menu_to_com();

	menu_cfg();
	
	
	CycShowCount = 0;
	DisplayItemNum = 0;
	DisplayState = ST_MAIN;									//����������
	l_stop_poll = 0;										//Ĭ�϶�����ʾ
	
	//��־λ��ʼ��
	public_val.Work_Pause_Flag = 0;	
	public_val.Err_Flag = 0;
	public_val.Bell_Flag = 0;

	public_val.cnt_after_powered = 0;
	public_val.Lock_Flag = 0;

	//������ر�����ʼ��
	read_id();

	public_val.Disp_Dz_10103[12] = public_val.password_val[0];
	public_val.Disp_Dz_10103[13] = public_val.password_val[1];
	
	public_val.input_pass_mask = 0;
	for(TempB = 0; TempB < PASS_LOCK_COUNT; TempB++)
		{
			if( (public_val.Disp_Dz_10103[TempB * 2] == 0)
				&& (public_val.Disp_Dz_10103[TempB * 2 + 1] == 0)
				)
				{
					public_val.input_pass_mask |= 1 << TempB;
				}
			public_val.Disp_Dz_10102[TempB * 2] = 0;
			public_val.Disp_Dz_10102[TempB * 2 + 1] = 0;
		}
	
	read_loack_count();
	read_user_count();
	public_val.Disp_Dz_20101[0] = public_val.work_count / LOCK_UNIT; //������������ֵ
	/* added by james */
    public_val.ms_timer = 0;
	public_val.io_read = 0;

	unKey.all = 0xFFFF;

}


//��ɶԲ˵����ݵĴ洢
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
				progbyte = (unsigned char *)&public_val.Disp_Dz_00000[0];
			break;			
			case MENU_ID_10101:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10101[0];
			break;
			case MENU_ID_10102:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10102[0];
			break;
			case MENU_ID_10103:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10103[0];
			break;
			case MENU_ID_10104:
				progbyte = (unsigned char *)&public_val.Disp_Dz_10104[0];
			break;
			case MENU_ID_20101:
				progbyte = (unsigned char *)&public_val.Disp_Dz_20101[0];
			break;
			break;			
		}
	progadd = MenuItem[menu_id].eeprom_add;
	for(i = 0; i < MenuItem[menu_id].MenuItem_Max + 1; i++)
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
	update_menu_to_com();
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
	IapProgramByte(EEPROM_DRIVE_TYPE,progbyte);
	progbyte = (unsigned char)(public_val.drive_type & 0xff);
	IapProgramByte(EEPROM_DRIVE_TYPE + 1,progbyte);
	progbyte = public_val.drive_indx;
	
	IapProgramByte(EEPROM_DRIVE_INDX,progbyte);
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
    char TempCountB,TempDataB,TempValue,i,TempB;
    static xdata char Bell_flag = 0,zc_key_flag = 0;
    static xdata unsigned int UpdateCountB,TempCount,flash_flag = 0;	
	static xdata long DisplayValueTemp;
	
	//static xdata char Key_Val = 0,Pr_Key_Val = 0;
	//static xdata char old_key = 0;
	static xdata int Key_Val = 0,Pr_Key_Val = 0, old_key = 0;

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

	static xdata unsigned int dj_key_dely = 0,dj_key_step = 0;
	//ADD by yq
	xdata unsigned char  cMenuSn;
	
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
	unKey.u_bit.IO9 = P16;

	unKey.u_bit.RED = P17;
	unKey.u_bit.GREEN = P54;
	unKey.u_bit.BLUE = P55;	

	Key_Val = (int)unKey.all; 
	
	//Key_Val = (P2 >> 1) & 0x0f;
	//Key_Val |= 0xf0;

		
	if(Key_Val != old_key) //�а���
	{
		No_key_delay = public_val.ms_timer;
		
		if(!long_input_flag)
		{
			bell_delay = public_val.ms_timer;
			Bell_flag |= 0x01;
		}
		else
		{
			Bell_flag &= 0xfe;
		}
		
		if( (public_val.ms_timer - Read_key_dely_2) > KEY_DOU_DELY)
		{
			old_key = Key_Val;
			Read_key_dely = public_val.ms_timer;
			long_count = 0;
			long_input_flag = 0;
			long_key_step = 1;
		}
		else
		{
			Key_Val = 0;
		}
		
	}
	else
	{
		if( (public_val.ms_timer - bell_delay) > BELL_ON_DELY)
		{
			Bell_flag &= 0xfe;
		}

		if( (public_val.ms_timer - No_key_delay) > NO_KEY_RETURN_MAIN)
		{
			No_key_delay = public_val.ms_timer;
			
			//if(edit_menu_id == MENU_ID_00000)
			if ((edit_menu_id == MENU_ID_00000) && (DisplayState != ST_LEARN))
			{
				DisplayState = ST_MAIN;
				Main_disp_other = 0;    /* added by james for v19407 */
			}
		}
		
		if( (public_val.ms_timer - Read_key_dely) > KEY_DOU_DELY)
		{
			Read_key_dely_2 = public_val.ms_timer;
			
			//if( ( (Key_Val&0xff) != 0xff) && (!long_input_flag) )
			if( ( (Key_Val&0xffff) != 0xffff) && (!long_input_flag) )
			{
				long_count++;
				
				//if(long_count > 10)
				if(long_count > 100)   
				{							
					long_input_flag = 1;
					long_key_step = 1;
					long_key_count = 0;
					Read_key_dely_3 = public_val.ms_timer; 
					long_count = 10;
				}
			}					
		}
		else
		{
			Key_Val = 0;

		}
	}
	
	if(long_input_flag)  /* ���� */
	{
		if(public_val.ms_timer - Read_key_dely_3 > KEY_LONG_DELY)
		{
			long_key_count ++;
			if(long_key_count > LONG_KEY_STEP_SWITCH)
			{
				long_key_step = 10;
			}
			if(long_key_count)
				Read_key_dely_3 = public_val.ms_timer;
			Pr_Key_Val = 0xFF;
		}
	}
	
	if(Key_Val)
	{
		if(Key_Val != Pr_Key_Val)
		{
			Pr_Key_Val = Key_Val;
		}
		else
		{
			Key_Val = 0;
		}
	}


	/* �а������·�����ֵ�Ͱ������� */
		
	if ((Key_Val == KEY_IO6) || (Key_Val == KEY_IO7) 
		|| (Key_Val == KEY_IO8))
	{
		if(public_val.Lock_Flag && (Key_Val == KEY_IO7))
			{
				public_val.Err_Flag = ERR_LOCK_SYS_ERR + public_val.Lock_Flag;  //��������20
			}
		else
			{
				write_key_flag = 1;   /* �а�������Ҫд���������� */
				
				gsKeyVal = Key_Val;
				gcKeyType = KEY_SHORT_UP_FLAG;   			
			}

	}

	//�򽺰�����������
	switch(dj_key_step)
		{
			case 0:
				if(!P14)
				{
					dj_key_step = 10;
					dj_key_dely = public_val.ms_timer;
				}
			break;
			case 10:
				if(!P14)
					{
						if(public_val.ms_timer - dj_key_dely > 100)
							{
								dj_key_step = 20;
							}
					}
				else
					{
						dj_key_step = 0;
					}
			break;
			case 20:
				if(P14)
					{
						write_key_flag = 1;   /* �а�������Ҫд���������� */
						gsKeyVal = KEY_DJ;
						gcKeyType = KEY_SHORT_UP_FLAG;
						dj_key_step = 0;
					}
			break;
		}
		
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

						return;
					}
					else
					{
						if (qz_wait == 1)
						{
							qz_wait = 0;   // ��ȴ�״̬

							war_bell_step = 0;  // �������������
							cPauseBell = 0;							
						}
						
						Main_disp_other = 0;
					}
										
					
		            break;	
					
		        case KEY_REDUCE:	 //����
		        
					test_key_cnt = 0;
					//learn_key_cnt = 0;
					
					if (long_input_flag && (Main_disp_other == 0))
					{
						if(public_val.work_count_user != public_val.work_count)
							{
								public_val.work_count_user = public_val.work_count;
								write_clear_cnt_flag = 1;    /* �������������������������� */
								save_user_count();
							}
					}					

					if (speed_mode == 1)  //����
					{
						write_speed_mode_flag = 1;  
						speed_mode = 0;   //�е�����
					}
				
					break;	
					
		        case KEY_INCREASE:  	 //�Ӻ�

					test_key_cnt = 0;
					
					Main_adj_key_delay = public_val.ms_timer;					
					Main_disp_other = 5;
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
					
					if (Main_disp_other == 0)
					{
						/* ��������˵� */
					    pMenu = pItem[0].m_pcfg;	// ָ������	
						now_menu.menu_id = 0;
						now_menu.par_id = 0;
						now_menu.event_flag = 1;
		                DisplayItemNum  = 0;
						zc_key_flag = 1;
						//DisplayMenuTotal = Menu_Number[0];
					    edit_menu_id = 0;
						DisplayMenuTotal = pItem[edit_menu_id].MenuItem_Max;;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵�  						
	                    return;
					}
										
					break;	

		        case KEY_BLUE:  	 //����

					test_key_cnt = 0;
					
					if (Main_disp_other == 0)
					{
						/* ��������˵� */
					    pMenu = pItem[0].m_pcfg;	// ָ������				
						now_menu.menu_id = 0;
						now_menu.par_id = 1;
						now_menu.event_flag = 1;
		                DisplayItemNum  = 1;			
						zc_key_flag = 1;
						//DisplayMenuTotal = Menu_Number[0];
					    edit_menu_id = 0;
						DisplayMenuTotal = pItem[edit_menu_id].MenuItem_Max;;
						if(DisplayMenuTotal > 0)
		                	DisplayState=ST_MENU;								//������Ӧ�˵�  
						
	                    return;
					}
										
					break;	

					
		        case KEY_ENT:   //�鿴����汾

					if (qz_wait == 0)  //������ֽ�ȴ�״̬
					{
						//learn_key_cnt = 0;

						if (long_input_flag)
						{						
							if (test_key_cnt < TEST_MODE_CNT)
							{
								test_key_cnt++;
							}
						}
						else
						{
							test_key_cnt = 0;
						}
						
						Main_adj_key_delay = public_val.ms_timer;
						Main_disp_other = 2;
					} 
					else   // ��ֽ�ȴ�״̬
					{
						write_key_flag = 1;   /* �а�������Ҫд���������� */

						// �·���ֽ����
						gsKeyVal = KEY_IO3;
						gcKeyType = KEY_SHORT_UP_FLAG; 
						
						Main_disp_other = 0;  // ������ʾͲ��
						qz_wait = 0;   // ��ȴ�״̬

						war_bell_step = 0;  // �������������
						cPauseBell = 0;
						
					}
					
					break;		

				case KEY_IO3:   // ��ֽ

					if (qz_wait == 0)  //δ�ȴ���ֽ
					{
						write_warning_flag = 1;   // �·�������Ϣ
						
						//������ֽ�ȴ�
						Main_disp_other = 4;
						qz_wait = 1;						
					}
					
					break;
					
		    }
        
			if(Main_disp_other == 0)  /* ���������� */
			{
				TempCount = main_menu_cfg[0].m_rate;  
				TempDataB = DisplayItemNum;
			
				if(!public_val.Err_Flag)				//�ޱ���
				{
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
						public_val.main_disp_val = public_val.work_count - public_val.work_count_user;
						DisplayValueTemp = (long)public_val.main_disp_val; 
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[0],0);
						
						//Bell_flag &= 0xfd;   

						
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
				else
				{
			
					if ((public_val.Err_Flag == P_FINISH_LEARN1)
						|| (public_val.Err_Flag == P_FINISH_LEARN2)) 
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

						
					}
					else		 // ��������			
					{
						LEDShowFlashString("E--",0);		/* modified by james at 2014-4-12 */
						
						if(!war_bell_step) 
							war_bell_step = 1;
						
						DisplayValueTemp = public_val.Err_Flag;
						TempValue=DataToStr(&DisplayValueTemp,
						                    INT32_SIZE,
											5,
											0,
											DisplayBuff,0,TY_D );		
						LED_ShowNumber(&DisplayBuff[3],3);		

						cPauseBell = 0;
					}
				}

				if(l_stop_poll)												//ѭ����ʾ
				if(++CycShowCount > main_menu_cfg[0].m_time)				//---------------���ڼ������ж�     
				{
				    CycShowCount=0;
					DisplayItemNum++;
					TempCount = 0; 
				}
			}
			else if(Main_disp_other == 2)    /* ��ʾ�汾�� */
			{
				DisplayValueTemp = public_val.version;
				//DisplayValueTemp = public_val.input_pass_mask;
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);	
				if( (public_val.ms_timer - Main_adj_key_delay) > MAIN_ADJ_DELAY)
				{
					Main_disp_other = 3;
				}
			}
			else if(Main_disp_other == 3)    /* ��ʾ������������IOֵ */
			{
				DisplayValueTemp = (long)public_val.io_read;
				//DisplayValueTemp &= 0xff;
				DisplayValueTemp &= 0xffff;
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);	
			}
			else if (Main_disp_other == 4)    /* ��ֽ�ȴ� */
			{
				LEDShowFlashString("Q2---",0);

				if(!war_bell_step) 
					war_bell_step = 1;						

				cPauseBell = 1;		
			}
			else if(Main_disp_other == 5)    /* ��ʾ������������IOֵ */
			{
				DisplayValueTemp = (long)public_val.password_index_disp;
				//DisplayValueTemp = (long)(public_val.password_val[0] << 8) + public_val.password_val[1];
				//DisplayValueTemp &= 0xff;
				DisplayValueTemp &= 0xffff;
				TempValue=DataToStr(&DisplayValueTemp,
				                    INT32_SIZE,
									5,
									0,
									DisplayBuff,0,TY_D );		
				LED_ShowNumber(&DisplayBuff[0],0);	
			}
	    }

		if (test_key_cnt == TEST_MODE_CNT)  /* ���������������Բ������ý��� added by james for v19407 */
		{
			test_key_cnt = 0;
			DisplayState = ST_TEST;
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
		            return;  
		            break;	
		        case KEY_REDUCE:
		        	if(--DisplayBitNum < 0)
		        	    DisplayBitNum=4;         	
		            break;	
		        case KEY_INCREASE:
		        	if( ++PasswordBuff[DisplayBitNum] > '9')
		        	    PasswordBuff[DisplayBitNum]='0';            	
		            break;	
		        case KEY_ENT:
		            for(TempCountB=1;TempCountB<MenuTotals;TempCountB++)
		            {
		                if(PasswordBuff[0]==pItem[TempCountB].m_pass[0])//
		                if(PasswordBuff[1]==pItem[TempCountB].m_pass[1])//
		                if(PasswordBuff[2]==pItem[TempCountB].m_pass[2])//
		                if(PasswordBuff[3]==pItem[TempCountB].m_pass[3])//
		                if(PasswordBuff[4]==pItem[TempCountB].m_pass[4])//�������Ҫ��
		                {
		                	if(TempCountB == MENU_ID_10104)
		                		{
		                			if(  ((public_val.Disp_Dz_10103[10] == 0)     //����6Ϊ�ջ���У��ɹ����������10104
										&&(public_val.Disp_Dz_10103[11] == 0))
										||(public_val.input_pass_mask & 0x20)
										||(public_val.Disp_Dz_10104[PASS_LOCK_COUNT - 1] == 0)
									   )
		                				{
		                					;
		                				}
									else
										return;
		                		}
							edit_menu_id = TempCountB; 
						    pMenu = pItem[TempCountB].m_pcfg;					
							now_menu.menu_id = edit_menu_id;
							now_menu.par_id = 0;
							now_menu.event_flag = 1;
		                    DisplayItemNum  = 0;		                    	
							DisplayMenuTotal = pItem[edit_menu_id].MenuItem_Max;
							if(DisplayMenuTotal > 0)
		                    	DisplayState=ST_MENU;								//������Ӧ�˵�                    
		                    return;
		                }                
		            }								            					
					return;
		            break;			
		    }
	        if( flash_flag  && !long_input_flag  )												//��˸����
	        {	
	            LEDClearChar(DisplayBitNum, 1);
	        }
	        else
	        {
	            LEDShowFlashString(&PasswordBuff[0], 0);            
	        }	      	
	    
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
					break;

				case KEY_IO8:  // ֹͣ
					LEDShowFlashString("   12",0);
					break;

				case KEY_RED:  // ��ֽ
					LEDShowFlashString("   13",0);
					break;

				case KEY_IO9:  // ��ֽ
					LEDShowFlashString("   14",0);
					break;

				case KEY_IO2:  // ��ֽ
					LEDShowFlashString("   15",0);
					break;

				case KEY_IO1:  // ��ֽ
					LEDShowFlashString("   16",0);
					break;

				case KEY_IO4:  // ����
					LEDShowFlashString("   17",0);
					break;

				case KEY_IO5:  // ����
					LEDShowFlashString("   18",0);
					break;

				case KEY_IO6:  // ֹͣ��ֽ
					LEDShowFlashString("   19",0);
					break;

				case KEY_IO3:  // ��ֽ
					LEDShowFlashString("   20",0);
					break;
					
				case KEY_GREEN:  // ֽ��
					LEDShowFlashString("   21",0);
					break;

				case KEY_BLUE:  // ����
					LEDShowFlashString("   22",0);
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
		            break;	
		        case KEY_REDUCE:
		    	    if (DisplayItemNum-- == 0)
		    	        DisplayItemNum = DisplayMenuTotal-1; 	
					now_menu.par_id = DisplayItemNum;
					
					now_menu.par_id = DisplayItemNum;
					now_menu.event_flag = 1;
					//add by yq
					if (cMenuSn-- <= 10)
						cMenuSn = 29;
		            break;	
		        case KEY_INCREASE:

		            if (++DisplayItemNum >= DisplayMenuTotal) 
		                DisplayItemNum = 0;	

					now_menu.par_id = DisplayItemNum;
					now_menu.event_flag = 1;
					//add by yq
					if (cMenuSn++ >= 29)
						cMenuSn = 10;
		            break;	
		        case KEY_ENT:	
					if(
						(now_menu.menu_id == MENU_ID_10104)  //�趨��У���Ƿ���Ȩ���޸�
						)
						{
							if(public_val.input_pass_mask & (1<<DisplayItemNum))
								{
									zc_key_flag = 5;
								}
						}
					else if(now_menu.menu_id == MENU_ID_10103)
						{
							if(public_val.input_pass_mask & (1<<(DisplayItemNum>>1)))
								{
									zc_key_flag = 5;
								}
						}
					else
						{
							zc_key_flag = 5;
						}
		            break;			
		    }        

			//ֽ���������⴦��
			//////////////////////////////////////////////////////////////////////////////////////
			if (now_menu.menu_id == 0 && zc_key_flag)
				{
					zc_key_flag ++;
				}

			if(zc_key_flag == 5)
				{
					zc_key_flag = 0;
				    TempDataB = DisplayItemNum;
					if (TY_D == TY_B  || 
						TY_D == TY_D  ||  
						TY_D == TY_H)									//��ֵ��
					{
						DisplayBitNum = 4;
						DataType=TY_D;

						//������ز������⴦��
						if( (now_menu.menu_id == MENU_ID_10103)			//δ��������ʾ����
						    && (!(public_val.input_pass_mask & (1<<(DisplayItemNum>>1)))) //����״̬����������ȫ����ʾΪ0
							)
							TempInputData = 0;
					    else
					    	TempInputData = *(int *)pDataValue->m_data_vale;		    	

						DisplayState=ST_DATA_INPUT;										//��������������

						pDataValue=&pMenu[TempDataB];
						DataType=TY_D;
						TempValue=DataToStr(pDataValue->m_data_vale,
					                        INT16_SIZE,
										    DEFAUT_POSE,
										    DEFAUT_POT,
										    DisplayBuff,0,DataType);				
						
						InputDataLen = DEFAUT_POSE+DEFAUT_POT;
						//DisplayBitNum = InputDataLen - 1; 
						PasswordBuff[0] = '0';
						PasswordBuff[1] = '0';
						switch(InputDataLen)
						{	/*���ڸ�����ʾ����������*/				    
						case 3:
							PasswordBuff[2]=DisplayBuff[TempValue];
							if(PasswordBuff[2] == ' ') PasswordBuff[2] = '0';
							PasswordBuff[3]=DisplayBuff[TempValue+1];
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];				
						    break;			
						case 2:
							PasswordBuff[2]=' ';
							PasswordBuff[3]=DisplayBuff[TempValue+1];
							if(PasswordBuff[3] == ' ') PasswordBuff[3] = '0';
							PasswordBuff[4]=DisplayBuff[TempValue+2];				
						    break;		
						case 1:
							PasswordBuff[2]=' ';
							PasswordBuff[3]=' ';
							PasswordBuff[4]=DisplayBuff[TempValue+2];
							break;				
						}
						pr_long_input_flag = 0;
						//pDataValue.m_max = 999;
						//pDataValue.m_min = 0;
						DisplayState=ST_DATA_INPUT;										//��������������
					}	
				}
			//////////////////////////////////////////////////////////////////////////////////////
				
			if(TempCount > 0)  	    
			{	
				TempCount-- ;
				break;          
			}
			TempCount = main_menu_cfg[0].m_rate;  

		    pDataValue=&pMenu[DisplayItemNum];	
//����У��ɹ�������˵���ʾ���ݣ��Լ�����������ʾ����
#if MENU_PASS_ON
			//������ز������⴦��
			if( (now_menu.menu_id == MENU_ID_10103)			//δ��������ʾ����
			    && (!(public_val.input_pass_mask & (1<<(DisplayItemNum>>1)))) //����״̬����������ȫ����ʾΪ0
				)
				TempInputData = 0;
		    else
#endif				
		    	TempInputData = *(int *)pDataValue->m_data_vale;		    	

			LEDShowCaption(DisplayItemNum);

			TempValue=DataToStr(&TempInputData,   //pDataValue->m_data_vale
			                    INT32_SIZE,      //pDataValue->m_size,
								DEFAUT_POSE,
								DEFAUT_POT,
								DisplayBuff,0,TY_D );		
			LED_ShowNumber(&DisplayBuff[TempValue],2);
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
							if(TempInputData<pDataValue->m_min) //
								TempInputData = pDataValue->m_min; //pDataValue->m_min;
						}
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
						}
		            break;	
		        case KEY_ENT:
		        	if(!long_input_flag)
	        		{			
						TempInputData=StrToData(PasswordBuff,5,DataType);
						if(TempInputData <= pDataValue->m_max && TempInputData >= pDataValue->m_min)
						{
					        switch(INT16_SIZE)
						    {
						    case INT32_SIZE:
						        *(long *)pDataValue->m_data_vale = TempInputData;
						        break;
						    case INT16_SIZE:
						        *(int *)pDataValue->m_data_vale = (int)TempInputData;
						        break;			
						    case INT8_SIZE:
						        *(char *)pDataValue->m_data_vale = (char)TempInputData;
						        break;			
							}	
							if(PasswordBuff[2] == '0')
								{
									PasswordBuff[2] = ' ';
									if(PasswordBuff[3] == '0') PasswordBuff[3] = ' ';
								}
							LED_ShowNumber(&PasswordBuff[2],2);

#if MENU_PASS_ON
							//�޸Ĳ�����У�������Ƿ��뱣���һ��
							if( now_menu.menu_id == MENU_ID_10102)
								{
									TempB = DisplayItemNum / 2;
									//for(TempB = 0; TempB < PASS_LOCK_COUNT; TempB++)
										{
											if( (public_val.Disp_Dz_10103[TempB * 2] == public_val.Disp_Dz_10102[TempB * 2])
												&& (public_val.Disp_Dz_10103[TempB * 2 + 1] == public_val.Disp_Dz_10102[TempB * 2 + 1]) 
												)
												{
													public_val.input_pass_mask |= 1 << TempB;
													public_val.Disp_Dz_10104[TempB] = 0;
													for(i = 0; i < TempB; i ++)				//�˶����µĶ����
														public_val.Disp_Dz_10104[i] = 0;
													func_menu_update(MENU_ID_10104);
												}
										}
									
										DisplayState = ST_MENU;;									
								}
							else
#endif
								func_menu_update(now_menu.menu_id);
									

							/* �����������2��������Ϊ1��ȫ���ָ�Ĭ�ϲ��� added by james for v19407 */
							/*if ((DisplayItemNum == PARM_DEFAULT)
								&& (TempInputData == 1))
							{
								for (i = 0; i < MENU_ID_00000_MAX_ITEM; i++)
								{
									public_val.menu_parm[i] = iDefaultParm[i];
								}
							}*/
							
							
						}
	        		}

					if (now_menu.menu_id == 0)
						{
				        	Main_disp_other = 0;
							DisplayBitNum = 4;
							CycShowCount  = 0;
							now_menu.menu_id = 888;
							now_menu.event_flag = 1;
				            DisplayItemNum = 0;	
				            DisplayState   = ST_MAIN;											//����������
						}
					else
		        		DisplayState = ST_MENU;
					
		            break;			
		    }

	        if( flash_flag && !long_input_flag )  //��˸����
	        {	
	            LEDClearChar(DisplayBitNum,1);
	        }
	        else			
	        {
					LEDShowCaption(DisplayItemNum);
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
				LED_ShowNumber(&PasswordBuff[2],2);	
				
	        }
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
    if(LCDDisplayState != DisplayState)
	{
		LCDDisplayState = DisplayState;
		
		switch(LCDDisplayState)
		{
			case ST_MAIN:
				GpuSend("CLS(0);\r\n");
				DELAY_US(UART2_DELAY);
				//add 0407
				DELAY_US(UART1s_DELAY);
				//cAlarmFlag = 0;						
				//add 0407
				GpuSend("SPG(52);\r\n");
				
				break;
			case ST_PASSWORD:
				//add 0323
				GpuSend("CLS(0);\r\n");
				DELAY_US(UART2_DELAY);
				//add 0407
				DELAY_US(UART1s_DELAY);

				GpuSend("SPG(27);\r\n");
				DELAY_US(UART2_DELAY);		
				GpuSend("DS48(20,174,'00000',0);\r\n");
				DELAY_US(UART2_DELAY);
				
				break;
			case ST_MENU ://--�����鿴����

				GpuSend("CLS(0);\r\n");
				DELAY_US(UART2_DELAY);
				MenuEditLook(cMenuSn);
				break;
			case ST_DATA_INPUT://--�������뻭��
			break;
			case ST_CODE_INPUT://--�������뻭��
			case ST_Show_Err://--������ʾ����
			break;
			case ST_ADJ://--ֱ�ӵ���������
			break;
			case ST_WAIT_VER://--�ȴ���У���
			break;
			case ST_DISP_ERR://��ʱ��ʾ�޸ĳ���
			break;
			case ST_TEST://--���Բ�������  added by james for v19407
			break;
			case ST_LEARN://--ѧϰģʽ
			break;
			case 0:
			
			break;
		}
	}	

	if(!UpdateState)
	{
		UpdateState = 1;
		UpdateCountB = public_val.ms_timer;
	}
	else
	{
		if(public_val.ms_timer - UpdateCountB > 100)
		{
			flash_flag = !flash_flag;
			UpdateState = 0;
		}		
	}


	switch(war_bell_step)  /* �������������� */
	{
		case 1:
			war_bell_delay = public_val.ms_timer;
			Bell_flag |= 0x02;
			war_bell_step = 10;
		break;
		case 10:
			iBellOnTime = (cPauseBell == 0) ? 500 : 50;
			//iBellOnTime = (cPauseBell == 0) ? 2000 : 50;
			if(public_val.ms_timer - war_bell_delay > iBellOnTime) //���ʱ��
				{
					war_bell_delay = public_val.ms_timer;
					Bell_flag &= 0xfd;
					war_bell_step = 20;
				}
		break;
		case 20:
			iBellOffTime = (cPauseBell == 0) ? 2000 : 2000;			
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
					war_bell_step = 1;
				}
		break;
	}
	
	if(Bell_flag)
	{
		BELL = 1;
	}
	else
	{
		BELL = 0;
	}
	

	TM1637_Write();

	
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
	size--;
//	if( ( *((char *)(pData)+size) ) & 0x80 )//�ж����λ�Ƿ�Ϊ1
//	    SignZero=1;
//	else
	    SignZero=0;	
	    
    DataS32Bit=0;
	if(size==3)//LONG������
	{			    
		if(SignZero)
		    *((long *)&DataS32Bit) = 0 - *((long *)pData);
		else
		    *((long *)&DataS32Bit) = *((long *)pData);
	}	
	else if(size==1)//INT������
	{
	    if(SignZero)
		    *((int *)&DataS32Bit) = 0 - *((int *)pData);						
		else
		    *((int *)&DataS32Bit) = *((int *)pData);	
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
        module=10;
		break;
	case TY_H:
        module=16;
		break;
	}
	
    spaces = 3-(nPoint+nBit);
	Temp = nPoint+nBit;
	if(Temp<3)
	{
	    nBit += (3-Temp);
		Temp=3;
	}
		
	do
    {
        if(Temp!=nBit || nFlag)
        {
            string[Temp]='0'+(char)(DataS32Bit%(module));
            DataS32Bit/=module;
        }
        else 
		{
			if(nPoint!=0)
			    string[Temp]='.';
			else
			    string[Temp]=' ';
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
       if(string[Temp]=='0' && (Temp+1<nBit)) 
       {
		   string[Temp]=' ';
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
	    string[--spaces]='-';
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

#endif
