#ifndef _SYS_H
#define _SYS_H
#include "reg51.h"

sfr IE2       = 0xaf;               //中断使能寄存器2

sfr AUXR = 0x8E;   //0000,0000 辅助寄存器
sfr P5 = 0xc8;
sfr P1M1 = 0x91;
sfr P1M0 = 0x92;
sfr P2M1 = 0x95;
sfr P2M0 = 0x96;
sfr P3M1 = 0xB1;
sfr P3M0 = 0xB2;

sbit P12 = P1^2;   // IO7
sbit P13 = P1^3;   // IO8
sbit P16 = P1^6;   // IO9
sbit P17 = P1^7;   // RED

sbit P20 = P2^0;   // IO1
sbit P21 = P2^1;   // K4
sbit P22 = P2^2;   // K3
sbit P23 = P2^3;   // K2
sbit P24 = P2^4;   // K1
sbit P26 = P2^6;   // IO5
sbit P27 = P2^7;   // IO6

sbit P35 = P3^5;   // IO2
sbit P33 = P3^3;   // IO3
sbit P32 = P3^2;   // IO4

sbit P37 = P3^7;
sbit P36 = P3^6;
sbit P31 = P3^1;
sbit P30 = P3^0;

sbit P54 = P5^4;   // GREEN
sbit P55 = P5^5;   // BLUE


sfr T2H = 0xD6;               //定时器2高8位
sfr T2L = 0xD7;               //定时器2低8位


#endif
