#ifndef _eeprom_h_
#define _eeprom_h_
#include <reg51.h>

#include <intrins.h>

#define uint unsigned int
#define uchar unsigned char

sfr IAP_DATA   = 0xC2;         //IAP数据寄存器
sfr IAP_ADDRH  = 0xC3;		   //IAP地址寄存器高位
sfr IAP_ADDRL  = 0xC4;		   //IAP地址寄存器低位
sfr IAP_CMD    = 0xC5;		   //IAP命令寄存器
sfr IAP_TRIG   = 0xC6;		   //IAP命令触发寄存器
sfr IAP_CONTR  = 0xC7;		   //IAP控制寄存器				  

#define CMD_IDLE  0     // 空闲模式
#define CMD_READ  1     // IAP字节读命令
#define CMD_PROGRAM  2     //IAP字节编程命令
#define CMD_ERASE  3     //	IAP扇区擦除命令
#define ENABLE_IAP   0X83

//#define ID_ADDR_RAM 0xf1        //ID号的存放在RAM区的地址为0F1H
                                //ID号的存放在程序区的地址为程序空间的最后7字节
//#define ID_ADDR_ROM 0x03f9      //1K程序空间的MCU(如                                             STC15F201EA, STC15F101EA)
//#define ID_ADDR_ROM 0x07f9      //2K程序空间的MCU(如                              STC15F402EACS, STC15F202EA, STC15F102EA)
//#define ID_ADDR_ROM 0x0bf9      //3K程序空间的MCU(如                                             STC15F203EA, STC15F103EA)
//#define ID_ADDR_ROM 0x0ff9      //4K程序空间的MCU(如               STC15F804EACS, STC15F404EACS, STC15F204EA, STC15F104EA)
//#define ID_ADDR_ROM 0x13f9      //5K程序空间的MCU(如                                             STC15F205EA, STC15F105EA)
//#define ID_ADDR_ROM 0x1ff9      //8K程序空间的MCU(如STC15F2K08S2,  STC15F808EACS, STC15F408EACS)
//#define ID_ADDR_ROM 0x27f9      //10K程序空间的MCU(如                             STC15F410EACS)
//#define ID_ADDR_ROM 0x2ff9      //12K程序空间的MCU(如              STC15F812EACS, STC15F412EACS)
//#define ID_ADDR_ROM 0x3ff9      //16K程序空间的MCU(如STC15F2K16S2, STC15F816EACS)
//#define ID_ADDR_ROM 0x4ff9      //20K程序空间的MCU(如STC15F2K20S2, STC15F820EACS)
//#define ID_ADDR_ROM 0x5ff9      //24K程序空间的MCU(如              STC15F824EACS)
//#define ID_ADDR_ROM 0x6ff9      //28K程序空间的MCU(如              STC15F828EACS)
#define ID_ADDR_ROM 0x7ff9      //32K程序空间的MCU(如STC15F2K32S2)
//#define ID_ADDR_ROM 0x9ff9      //40K程序空间的MCU(如STC15F2K40S2)
//#define ID_ADDR_ROM 0xbff9      //48K程序空间的MCU(如STC15F2K48S2)
//#define ID_ADDR_ROM 0xcff9      //52K程序空间的MCU(如STC15F2K52S2)
//#define ID_ADDR_ROM 0xdff9      //56K程序空间的MCU(如STC15F2K56S2)
//#define ID_ADDR_ROM 0xeff9      //60K程序空间的MCU(如STC15F2K60S2)

void iapidle();
uchar  IapReadByte (uint addr);
void  IapProgramByte (uint addr,uchar dat);
void IapEraseSector(uint addr);

#endif
