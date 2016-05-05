#ifndef _MAIN_H_
#define _MAIN_H_

#define KEY_JIT_DELAY   20           /* 按键去抖动延时:ms */

#define KEY_LONG_DELAY  400          /* 按键长按延时:ms */

extern void key_detect();

extern void judge_key();

#endif