#ifndef _eeprom_h_
#define _eeprom_h_
#include <reg51.h>

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
#define ENABLE_IAP   0X83

//#define ID_ADDR_RAM 0xf1        //ID�ŵĴ����RAM���ĵ�ַΪ0F1H
                                //ID�ŵĴ���ڳ������ĵ�ַΪ����ռ�����7�ֽ�
//#define ID_ADDR_ROM 0x03f9      //1K����ռ��MCU(��                                             STC15F201EA, STC15F101EA)
//#define ID_ADDR_ROM 0x07f9      //2K����ռ��MCU(��                              STC15F402EACS, STC15F202EA, STC15F102EA)
//#define ID_ADDR_ROM 0x0bf9      //3K����ռ��MCU(��                                             STC15F203EA, STC15F103EA)
//#define ID_ADDR_ROM 0x0ff9      //4K����ռ��MCU(��               STC15F804EACS, STC15F404EACS, STC15F204EA, STC15F104EA)
//#define ID_ADDR_ROM 0x13f9      //5K����ռ��MCU(��                                             STC15F205EA, STC15F105EA)
//#define ID_ADDR_ROM 0x1ff9      //8K����ռ��MCU(��STC15F2K08S2,  STC15F808EACS, STC15F408EACS)
//#define ID_ADDR_ROM 0x27f9      //10K����ռ��MCU(��                             STC15F410EACS)
//#define ID_ADDR_ROM 0x2ff9      //12K����ռ��MCU(��              STC15F812EACS, STC15F412EACS)
//#define ID_ADDR_ROM 0x3ff9      //16K����ռ��MCU(��STC15F2K16S2, STC15F816EACS)
//#define ID_ADDR_ROM 0x4ff9      //20K����ռ��MCU(��STC15F2K20S2, STC15F820EACS)
//#define ID_ADDR_ROM 0x5ff9      //24K����ռ��MCU(��              STC15F824EACS)
//#define ID_ADDR_ROM 0x6ff9      //28K����ռ��MCU(��              STC15F828EACS)
#define ID_ADDR_ROM 0x7ff9      //32K����ռ��MCU(��STC15F2K32S2)
//#define ID_ADDR_ROM 0x9ff9      //40K����ռ��MCU(��STC15F2K40S2)
//#define ID_ADDR_ROM 0xbff9      //48K����ռ��MCU(��STC15F2K48S2)
//#define ID_ADDR_ROM 0xcff9      //52K����ռ��MCU(��STC15F2K52S2)
//#define ID_ADDR_ROM 0xdff9      //56K����ռ��MCU(��STC15F2K56S2)
//#define ID_ADDR_ROM 0xeff9      //60K����ռ��MCU(��STC15F2K60S2)

void iapidle();
uchar  IapReadByte (uint addr);
void  IapProgramByte (uint addr,uchar dat);
void IapEraseSector(uint addr);

#endif
