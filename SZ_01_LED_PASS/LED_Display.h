#ifndef _LED_DISP_H
#define _LED_DISP_H

#include "global.h"

#define DISP_C

#define EEPROM_MENU_1 0x1000
#define EEPROM_MENU_2 0x1200
#define EEPROM_MENU_3 0x1400
#define EEPROM_MENU_4 0x1600
#define EEPROM_MENU_5 0x1C00   //有丢数据现象,换一个地址
#define EEPROM_MENU_6 0x1A00
#define EEPROM_MENU_7 0x1F00

#define EEPROM_PASSWORD 0x0000

//#define EEPROM_DRIVE_TYPE 0x2400 //装置版本号存储地址
//#define EEPROM_DRIVE_INDX 0x2410 //装置版本索引号

#define MENU_NUM 6 //菜单总个数

#define MENU_ID_00000 0
#define MENU_ID_10101 1
#define MENU_ID_10102 2
#define MENU_ID_10103 3
#define MENU_ID_10104 4
#define MENU_ID_20101 5

#define CFG_MAX_ITEM 40

#define MENU_ID_00000_MAX_ITEM 2    /* 非密码参数 */
#define MENU_ID_10101_MAX_ITEM 30   //10101参数
#define MENU_ID_10102_MAX_ITEM 14	//用户密码输入 6组
#define MENU_ID_10103_MAX_ITEM 14	//用户密码存储 6组
#define MENU_ID_10104_MAX_ITEM 7	//设定根数 单位:万根 第7个显示已生产根数 前6个为设定密码
#define MENU_ID_20101_MAX_ITEM 1	//已生产根数

#define PASS_LOCK_COUNT 7           //密码组数

#define COM_MENU_MAX_ITEM (MENU_ID_00000_MAX_ITEM + MENU_ID_10101_MAX_ITEM)

#define MENU_PASS_START 16   //锁机相关操作参数索引
#define MENU_PASS_COUNT 6    //锁机相关操作参数个数
#define MENU_PASS_ON    1
#define LOCK_UNIT       10000L //10000L //锁机设置单位
//#define MENU_ID_00000_MAX_ITEM  usParamNum

#define DEFAUT_POSE 3
#define DEFAUT_POT 0

#define INT8_SIZE 1
#define INT16_SIZE 2
#define INT32_SIZE 4

#define ERR_NUM_COM_ERR 0x10 //通讯错误 从位4开始赋值
#define ERR_NUM_EEP_ERR 0x20 //写EEPROM错误
#define ERR_NUM_ZK_COM_ERR 0x30 //主控板校验码错误
#define ERR_LOCK_SYS_ERR 20   //锁机错误,错误标志不被通讯更新

#define LED_CHECK_ERROR   30   /* 操作板校验错误 */
#define MAIN_CHECK_ERROR  40   /* 主板反馈的校验错误 */




#define WELCOM_DELAY_US 2000000

//#define KEY_SEL 0xF7										///<【选择键】
//#define KEY_ENT 0xFE										///<【确认键】
//#define KEY_INCREASE 0xFB										///<【加号键】
//#define KEY_REDUCE 0xFD										///<【减号键】

#define KEY_SEL 0xFFFE										///<【选择键】 K1
#define KEY_ENT 0xFFF7										///<【确认键】 K4
#define KEY_INCREASE 0xFFFD										///<【加号键】 K2
#define KEY_REDUCE 0xFFFB										///<【减号键】 K3

#define KEY_K1  0xFFFE         /* 选择 */
#define KEY_K2  0xFFFD  	   /* 加 */
#define KEY_K3  0xFFFB         /* 减 */
#define KEY_K4  0xFFF7         /* 确认 */
#define KEY_IO1 0xFFEF         /* 进纸 */
#define KEY_IO2 0xFFDF		   /* 退纸 */
#define KEY_IO3 0xFFBF         /* 切纸 */
#define KEY_IO4 0xFF7F		   /* 下料 */
#define KEY_IO5 0xFEFF         /* 搓条 */
#define KEY_IO6 0xFDFF         /* 停止送纸 */
#define KEY_IO7 0xFBFF         /* 启动 */
#define KEY_IO8 0xF7FF	       /* 停止 */
#define KEY_IO9 0xEFFF         /* 发纸 */
#define KEY_RED 0xDFFF         /* 收纸 */
#define KEY_GREEN 0xBFFF	   /* 纸长 */
#define KEY_BLUE  0x7FFF       /* 捆数 */
#define KEY_DJ 0xFFEE       /* 打胶 */



/*------------------------------ 画面定义 ------------------------------------*/
#define ST_MAIN          0x01								//--主画面
#define ST_PASSWORD      0x02								//--密码
#define ST_MENU          0x03								//--参数查看画面
#define ST_DATA_INPUT    0x04								//--数据输入画面
#define ST_CODE_INPUT    0x05								//--代码输入画面
#define ST_Show_Err      0x06								//--错误提示画面
#define ST_ADJ           0x07								//--直接调参数画面
#define ST_WAIT_VER      0x08								//--等待返校结果
#define ST_DISP_ERR      0x09                               //延时显示修改出错
#define ST_TEST          0x10                               //--测试参数设置  added by james for v19407
#define ST_LEARN         0x11                               //--学习模式

/*----------------------------- 数据显示类型 ---------------------------------*/
#define TY_B         0x01									// 2进制
#define TY_D         0x02									//10进制
#define TY_H         0x03									//16进制
#define TY_T         0x04									//代码
#define TY_D_L       0x08									//10进制,高位补零
#define TY_D_A       0x40									//10进制,单位自动调整

#define TY_RD        0x10									//只读数据属性
#define TY_RT        0x20									//只读代码属性


#define P_FINISH_LEARN1     11   /* 一个电机学习模式完成 */
#define P_FINISH_LEARN2     12   /* 两个电机学习模式完成 */


#define PARAM_PAPER_LEN     0
#define PARAM_SET_NUM       1
#define PARAM_WARNING_TIME  2
#define PARAM_PROTECT_TIME1 3 
#define PARAM_PROTECT_TIME2 4
#define PARAM_BJ_DIR        5
#define PARAM_BJ_VEL_GAIN   6
#define PARAM_BJ_SCALE      7
#define PARAM_FZ_SCALE      8
#define PARAM_FZ_WHEEL_R    9
#define PARAM_KP            10
#define PARAM_FZ_START_PWM_SCALE   11
#define PARAM_QZ_START_PWM_SCALE   12
#define PARAM_QZ_TARGET_PWM_SCALE  13
#define PARAM_FZ_SPEED_MAX         14
#define PARAM_FZ_SPEED_MIN         15
#define PARAM_QZ_W_ZERO            16

#define USER_PARAM_NUM         2
#define MACHINE_PARAM_NUM      22

extern int  DisplayState;									///<【显示界面状态】

extern xdata char test_mode; 

extern xdata int gsKeyVal;    /* 按键值 */
extern xdata char gcKeyType;  /* 按键类型: 0短按, 1长按下，2长按抬起 */


//附属属性
// 位0: 0:可读写 1:只读且选择时发逻辑序号
// 位1: 0:不重启 1:重启
// 位2: 0:保存   1:不保存
// 位3:          1:恢复出厂参数
typedef struct _menu_show{ /*参数设置画面数据定义*/
    void  * m_data_vale;								//数据地址
    int   m_max;										//最大值,并做为菜单项结束标志(为0为菜单结束)
    int   m_min;										//最小值 
    char  ext_property;									//附属属性
}MENU_SHOW,* pMENU_SHOW;

typedef struct _main_show{ /*主画面数据定义*/
    const char  * const m_str;								//显示的字符串    
	const unsigned char    m_type;							//数据显示类型，TY_B,TY_D,TY_H  
    const void  *  m_ext_code;								//特殊代码结构体
    void  * const m_data_vale;								//数据地址
    const unsigned char   m_size;							//数据长度(字节)
    const unsigned char   m_pos;							//整数位数
    unsigned char   m_pot;									//小数点位数
    const long   m_max;										//最大值
    const long   m_min;										//最小值        
}MAIN_SHOW,* pMAIN_SHOW;

typedef struct _data_value{ /*数值类型数据定义*/
    void  * const m_data;									//数据地址
    const unsigned char   m_size;							//数据长度(字节)
    const unsigned char   m_pos;							//整数位数
    const unsigned char   m_pot;							//小数点位数
    const long   m_max;										//最大值
    const long   m_min;										//最小值    
}DATA_VALUE,* pDATA_VALUE;   
 
typedef struct _type_code{ /*代码类型数据定义*/
    const char  * const m_str;								//显示的字符串
}TYPE_CODE,* pTYPE_CODE;

typedef struct _type_alarm{ /*代码类型数据定义*/
    const char  * const m_str;								//显示的字符串
}TYPE_ALARM,* pTYPE_ALARM;

typedef struct _data_code{ /*代码类型数据定义*/
    const void * const m_data;								//数据地址
	const unsigned char        m_size;						//数据地址
    const TYPE_CODE *  const m_code;						//对应代码的地址
    const unsigned char        m_max;						//最大值
    const unsigned char        m_min;						//最小值	
}DATA_CODE,* pDATA_CODE;
    
typedef struct _menu_item{ /*菜单项目数据定义*/
    const unsigned char   MenuID;							//菜单序号
    const MENU_SHOW * const m_pcfg;							//菜单数据指针	
    const char * const  m_pass;								//密码
	const unsigned int eeprom_add;        //数据存储EEPROM首地址
	const char MenuItem_Max;     //菜单最大列表数(固定)
	const unsigned int eeprom_cfg_add;    //菜单配置数据EEPROM首地址
	const char * com_max_item;     //菜单列表数(通讯配置)
}MENU_ITEM,* pMENU_ITEM;

typedef struct _main_pic{
    const unsigned char  m_time;							//主画面每点显示次数
    const unsigned char  m_rate;							//数据更新频率
    const MAIN_SHOW * const m_main;
	const MENU_ITEM * const m_menu;
}MAIN_PIC,*pMAIN_PIC;

typedef struct _menu_chang{ //操作菜单项
	char menu_id;   //菜单序号
	char par_id;    //参数序号
	char event_flag; //按键事件
	char chang_par_flag; //参数修改事件 1:开始下发 2:下发修改成功 3:修改失败
	char chang_next_step; //参数修改成功跳转页面
	char chang_err_next_step; //参数修改失败跳转页面
	unsigned int err_dsip_delay;   //参数修改报错延时
}MENU_CHANG,* pMENU_CHANG;

//下行 下载参数
typedef struct _updata_menu_stru{ 
	char frame_id;
	char payload_num;
	char menu_id;   //菜单序号
	char par_id;    //参数序号
	int par_val[CFG_MAX_ITEM];
}updata_menu_stru,* pupdata_menu_stru;

//上行: 参数字回帧数据结构体
typedef struct _rt_updata_menu_stru{
	char frame_id;
	char payload_num;
	char menu_id;   //菜单序号
	char par_id;    //参数序号
	int  Statu;    //修改状态 0x00FF:成功修改 other:不成功
}rt_updata_menu_stru;

//上行: 主控板回帧版本号与版本序号
typedef struct _get_ver_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	int ver;       //版本号
	char drive_indx; //版本序号
	char bak;
}get_ver_stru,* pget_ver_stru;

//下行: 设置版本序号,获取版本号结构体
typedef struct _set_ver_idx_stru{
	char frame_id;
	char payload_num;
	char cmd_type;     //命令类型
	char ver_idx;       //命令参数
	char bak;
}set_ver_idx_stru;

//返回菜单配置数据结构体
typedef struct _menu_item_stru{
	int menu_max;   //最大值
	int menu_min;   //最小值
	char ext_property; //附属属性
}menu_item_stru;

typedef struct _rec_menu_cfg_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;
	char menu_count;       //菜单列表数
	menu_item_stru val[CFG_MAX_ITEM];   //列表值属性
}rec_menu_cfg_stru,* prec_menu_cfg_stru;

//返回默认值数据结构体
typedef struct _rec_defaut_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;
	int val[CFG_MAX_ITEM];       //菜单默认值
}rec_defaut_stru;

//返回主控板状态号
typedef struct _rec_statu_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char err_flag;       //错误号
	char statu_num;      //状态号
	char pass_flag;      //暂停序号 位7为1，表示蜂鸣器响
	char other_bit_flag; //其他位标志
	long disp_count;    //工作计数器
	char  io_read;    //开入状态 
}rec_statu_stru;


//获取菜单配置命令帧结构体
typedef struct _get_cfg_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;       //菜单号
	char bak[2];
}get_cfg_stru;

//获取菜单数据命令帧结构体
typedef struct _get_data_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;       //菜单号
	char bak[2];
}get_data_stru;


//命令字结构体
typedef struct _cmd_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char bak[2];
}cmd_stru;

//参数结构体
typedef struct
{
	///以下FLASH_SAVE_WORD_NUM个变量作为参数保存
	int Disp_Dz_00000[MENU_ID_00000_MAX_ITEM + 1];
	int Disp_Dz_10101[MENU_ID_10101_MAX_ITEM + 1];
	int Disp_Dz_10102[MENU_ID_10102_MAX_ITEM + 1];
	int Disp_Dz_10103[MENU_ID_10103_MAX_ITEM + 1];
	int Disp_Dz_10104[MENU_ID_10104_MAX_ITEM + 1];
	int Disp_Dz_20101[MENU_ID_20101_MAX_ITEM + 1];
	int menu_parm[COM_MENU_MAX_ITEM];				//菜单0和菜单1数据拼起

	unsigned int ms_timer;				///<【毫秒计时器】
	unsigned int Err_Flag;              //错误标志 低4位:主控板错误号 高4位:操作板错误
	char Work_Pause_Flag; 				//工作暂停标志 位7:值为1蜂鸣器叫
	char Bell_Flag;                     //蜂鸣器响标志:1响，0不响
	char Other_Bit_Flag;                //其他位标志
	unsigned int io_read;               //IO口状态值
	unsigned int version;               //主板软件版本号
	unsigned long cnt_after_powered;    //主板反馈的开机后的筒数
#if 0	
	unsigned long cnt_before_powered;   //上电读取的开机前已保存筒数 
#endif
	unsigned long main_disp_val;        //主画面显示值 = 开机前+开机后

	unsigned int uart_rec_start_timer;  //帧间隔延时
	//unsigned int uart_rec_out_timer;  //通讯超时判断延时
	unsigned char hand_number;        //握手帧计数,握手帧只固定发10次
	unsigned char uart_rec_statu;    //接收状态 1:正在接收 2:接收完毕，正在处理 3:处理完毕
	unsigned char uart_rec_count;    //接收字节个数
	unsigned char first_flag;        //与下位机重新通讯 1:重新握手 0:握手完毕
	unsigned char dsp_debug;         //通讯超时不需要重新握手,用于DSP调程序用
	unsigned int drive_type;						//操作屏版本号
	unsigned char drive_indx;						//版本索引号(针对一个主控板有多个版本)
	unsigned int drive_type_com;					//通讯版本号,用于与操作屏版本号校对
	unsigned char drive_index_com;                  //从主控板返回的通讯版本序号
	char com_log_step;
	char start_next_step;           //握手成功后下一步要发的帧,由开机按键决定
	char send_frame_idx;
	char uart_send_flag;
	char Work_Statu;
	char Init_Over;
	char Disp_debug;                    //显示器调试模式
	char Delay_Hand_En;                  //超时是否重新握手,接了手控盒是否还能按键

	char Lock_Flag;						//锁机标志
	
	char input_pass_mask;               //密码校对标志
	unsigned int password_index_disp;               //密码序号
	unsigned int password_index;               //密码序号
	unsigned int password_val[2];               //密码值
	
	unsigned long work_count;        //插引次数记录值 上电从EEPROM获取
	unsigned long work_count_user;        //插引次数记录值 用户清零值 上电从EEPROM获取

	unsigned int last_speed;            //保存的上一次速度值  added for v19416

}public_stru;



typedef struct 
{
	int min;
	int max;
}STRU_PARAM_RANGE;


typedef struct T_BIT_PARAM
{
	int 		K1:1;								
	int		    K2:1;							
	int		    K3:1;							
	int 		K4:1;								
	int		    IO1:1;							
	int		    IO2:1;
	int		    IO3:1;							
	int		    IO4:1;
	int		    IO5:1;							
	int		    IO6:1;
	int		    IO7:1;							
	int		    IO8:1;	
	int         IO9:1;
	int         RED:1;
	int         GREEN:1;
	int         BLUE:1;
	
}_T_BIT_PARAM;


///比特位参数联合体
typedef union U_BIT_PARAM
{
	int  			    all;
	_T_BIT_PARAM	u_bit;
}_U_BIT_PARAM;

extern xdata char Menu_Number[MENU_NUM];
//extern xdata char Menu0_Number, Menu1_Number;
extern xdata _U_BIT_PARAM unKey;
extern xdata char speed_mode;



void InitLEDDisplay(void);
void LEDDisplay(void);
void  LEDShowCaption(char num);

void func_menu_update(int menu_id);

void update_menu_to_com(void);


#endif
