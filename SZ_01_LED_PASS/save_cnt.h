
#ifndef SAVE_CNT_H
#define SAVE_CNT_H


#include "eeprom.h"
#include "led_display.h"


#define EEPROM_LOCK_VAL 0x4400  //����ϴͲ�����ĵ�ַ��Χ 0x4400 - 0x4E00
#define EEPROM_USER_COUNT 0x2600 //�û��������

extern xdata public_stru public_val;
extern char eeprom_en;


extern void save_sys_lock(void);

extern void read_loack_count(void);

extern void read_user_count(void);

extern void save_user_count(void);
#endif

