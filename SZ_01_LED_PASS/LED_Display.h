#ifndef _LED_DISP_H
#define _LED_DISP_H

#include "global.h"

#define DISP_C

#define EEPROM_MENU_1 0x1000
#define EEPROM_MENU_2 0x1200
#define EEPROM_MENU_3 0x1400
#define EEPROM_MENU_4 0x1600
#define EEPROM_MENU_5 0x1C00   //�ж���������,��һ����ַ
#define EEPROM_MENU_6 0x1A00
#define EEPROM_MENU_7 0x1F00

#define EEPROM_PASSWORD 0x0000

//#define EEPROM_DRIVE_TYPE 0x2400 //װ�ð汾�Ŵ洢��ַ
//#define EEPROM_DRIVE_INDX 0x2410 //װ�ð汾������

#define MENU_NUM 6 //�˵��ܸ���

#define MENU_ID_00000 0
#define MENU_ID_10101 1
#define MENU_ID_10102 2
#define MENU_ID_10103 3
#define MENU_ID_10104 4
#define MENU_ID_20101 5

#define CFG_MAX_ITEM 40

#define MENU_ID_00000_MAX_ITEM 2    /* ��������� */
#define MENU_ID_10101_MAX_ITEM 30   //10101����
#define MENU_ID_10102_MAX_ITEM 14	//�û��������� 6��
#define MENU_ID_10103_MAX_ITEM 14	//�û�����洢 6��
#define MENU_ID_10104_MAX_ITEM 7	//�趨���� ��λ:��� ��7����ʾ���������� ǰ6��Ϊ�趨����
#define MENU_ID_20101_MAX_ITEM 1	//����������

#define PASS_LOCK_COUNT 7           //��������

#define COM_MENU_MAX_ITEM (MENU_ID_00000_MAX_ITEM + MENU_ID_10101_MAX_ITEM)

#define MENU_PASS_START 16   //������ز�����������
#define MENU_PASS_COUNT 6    //������ز�����������
#define MENU_PASS_ON    1
#define LOCK_UNIT       10000L //10000L //�������õ�λ
//#define MENU_ID_00000_MAX_ITEM  usParamNum

#define DEFAUT_POSE 3
#define DEFAUT_POT 0

#define INT8_SIZE 1
#define INT16_SIZE 2
#define INT32_SIZE 4

#define ERR_NUM_COM_ERR 0x10 //ͨѶ���� ��λ4��ʼ��ֵ
#define ERR_NUM_EEP_ERR 0x20 //дEEPROM����
#define ERR_NUM_ZK_COM_ERR 0x30 //���ذ�У�������
#define ERR_LOCK_SYS_ERR 20   //��������,�����־����ͨѶ����

#define LED_CHECK_ERROR   30   /* ������У����� */
#define MAIN_CHECK_ERROR  40   /* ���巴����У����� */




#define WELCOM_DELAY_US 2000000

//#define KEY_SEL 0xF7										///<��ѡ�����
//#define KEY_ENT 0xFE										///<��ȷ�ϼ���
//#define KEY_INCREASE 0xFB										///<���Ӻż���
//#define KEY_REDUCE 0xFD										///<�����ż���

#define KEY_SEL 0xFFFE										///<��ѡ����� K1
#define KEY_ENT 0xFFF7										///<��ȷ�ϼ��� K4
#define KEY_INCREASE 0xFFFD										///<���Ӻż��� K2
#define KEY_REDUCE 0xFFFB										///<�����ż��� K3

#define KEY_K1  0xFFFE         /* ѡ�� */
#define KEY_K2  0xFFFD  	   /* �� */
#define KEY_K3  0xFFFB         /* �� */
#define KEY_K4  0xFFF7         /* ȷ�� */
#define KEY_IO1 0xFFEF         /* ��ֽ */
#define KEY_IO2 0xFFDF		   /* ��ֽ */
#define KEY_IO3 0xFFBF         /* ��ֽ */
#define KEY_IO4 0xFF7F		   /* ���� */
#define KEY_IO5 0xFEFF         /* ���� */
#define KEY_IO6 0xFDFF         /* ֹͣ��ֽ */
#define KEY_IO7 0xFBFF         /* ���� */
#define KEY_IO8 0xF7FF	       /* ֹͣ */
#define KEY_IO9 0xEFFF         /* ��ֽ */
#define KEY_RED 0xDFFF         /* ��ֽ */
#define KEY_GREEN 0xBFFF	   /* ֽ�� */
#define KEY_BLUE  0x7FFF       /* ���� */
#define KEY_DJ 0xFFEE       /* �� */



/*------------------------------ ���涨�� ------------------------------------*/
#define ST_MAIN          0x01								//--������
#define ST_PASSWORD      0x02								//--����
#define ST_MENU          0x03								//--�����鿴����
#define ST_DATA_INPUT    0x04								//--�������뻭��
#define ST_CODE_INPUT    0x05								//--�������뻭��
#define ST_Show_Err      0x06								//--������ʾ����
#define ST_ADJ           0x07								//--ֱ�ӵ���������
#define ST_WAIT_VER      0x08								//--�ȴ���У���
#define ST_DISP_ERR      0x09                               //��ʱ��ʾ�޸ĳ���
#define ST_TEST          0x10                               //--���Բ�������  added by james for v19407
#define ST_LEARN         0x11                               //--ѧϰģʽ

/*----------------------------- ������ʾ���� ---------------------------------*/
#define TY_B         0x01									// 2����
#define TY_D         0x02									//10����
#define TY_H         0x03									//16����
#define TY_T         0x04									//����
#define TY_D_L       0x08									//10����,��λ����
#define TY_D_A       0x40									//10����,��λ�Զ�����

#define TY_RD        0x10									//ֻ����������
#define TY_RT        0x20									//ֻ����������


#define P_FINISH_LEARN1     11   /* һ�����ѧϰģʽ��� */
#define P_FINISH_LEARN2     12   /* �������ѧϰģʽ��� */


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

extern int  DisplayState;									///<����ʾ����״̬��

extern xdata char test_mode; 

extern xdata int gsKeyVal;    /* ����ֵ */
extern xdata char gcKeyType;  /* ��������: 0�̰�, 1�����£�2����̧�� */


//��������
// λ0: 0:�ɶ�д 1:ֻ����ѡ��ʱ���߼����
// λ1: 0:������ 1:����
// λ2: 0:����   1:������
// λ3:          1:�ָ���������
typedef struct _menu_show{ /*�������û������ݶ���*/
    void  * m_data_vale;								//���ݵ�ַ
    int   m_max;										//���ֵ,����Ϊ�˵��������־(Ϊ0Ϊ�˵�����)
    int   m_min;										//��Сֵ 
    char  ext_property;									//��������
}MENU_SHOW,* pMENU_SHOW;

typedef struct _main_show{ /*���������ݶ���*/
    const char  * const m_str;								//��ʾ���ַ���    
	const unsigned char    m_type;							//������ʾ���ͣ�TY_B,TY_D,TY_H  
    const void  *  m_ext_code;								//�������ṹ��
    void  * const m_data_vale;								//���ݵ�ַ
    const unsigned char   m_size;							//���ݳ���(�ֽ�)
    const unsigned char   m_pos;							//����λ��
    unsigned char   m_pot;									//С����λ��
    const long   m_max;										//���ֵ
    const long   m_min;										//��Сֵ        
}MAIN_SHOW,* pMAIN_SHOW;

typedef struct _data_value{ /*��ֵ�������ݶ���*/
    void  * const m_data;									//���ݵ�ַ
    const unsigned char   m_size;							//���ݳ���(�ֽ�)
    const unsigned char   m_pos;							//����λ��
    const unsigned char   m_pot;							//С����λ��
    const long   m_max;										//���ֵ
    const long   m_min;										//��Сֵ    
}DATA_VALUE,* pDATA_VALUE;   
 
typedef struct _type_code{ /*�����������ݶ���*/
    const char  * const m_str;								//��ʾ���ַ���
}TYPE_CODE,* pTYPE_CODE;

typedef struct _type_alarm{ /*�����������ݶ���*/
    const char  * const m_str;								//��ʾ���ַ���
}TYPE_ALARM,* pTYPE_ALARM;

typedef struct _data_code{ /*�����������ݶ���*/
    const void * const m_data;								//���ݵ�ַ
	const unsigned char        m_size;						//���ݵ�ַ
    const TYPE_CODE *  const m_code;						//��Ӧ����ĵ�ַ
    const unsigned char        m_max;						//���ֵ
    const unsigned char        m_min;						//��Сֵ	
}DATA_CODE,* pDATA_CODE;
    
typedef struct _menu_item{ /*�˵���Ŀ���ݶ���*/
    const unsigned char   MenuID;							//�˵����
    const MENU_SHOW * const m_pcfg;							//�˵�����ָ��	
    const char * const  m_pass;								//����
	const unsigned int eeprom_add;        //���ݴ洢EEPROM�׵�ַ
	const char MenuItem_Max;     //�˵�����б���(�̶�)
	const unsigned int eeprom_cfg_add;    //�˵���������EEPROM�׵�ַ
	const char * com_max_item;     //�˵��б���(ͨѶ����)
}MENU_ITEM,* pMENU_ITEM;

typedef struct _main_pic{
    const unsigned char  m_time;							//������ÿ����ʾ����
    const unsigned char  m_rate;							//���ݸ���Ƶ��
    const MAIN_SHOW * const m_main;
	const MENU_ITEM * const m_menu;
}MAIN_PIC,*pMAIN_PIC;

typedef struct _menu_chang{ //�����˵���
	char menu_id;   //�˵����
	char par_id;    //�������
	char event_flag; //�����¼�
	char chang_par_flag; //�����޸��¼� 1:��ʼ�·� 2:�·��޸ĳɹ� 3:�޸�ʧ��
	char chang_next_step; //�����޸ĳɹ���תҳ��
	char chang_err_next_step; //�����޸�ʧ����תҳ��
	unsigned int err_dsip_delay;   //�����޸ı�����ʱ
}MENU_CHANG,* pMENU_CHANG;

//���� ���ز���
typedef struct _updata_menu_stru{ 
	char frame_id;
	char payload_num;
	char menu_id;   //�˵����
	char par_id;    //�������
	int par_val[CFG_MAX_ITEM];
}updata_menu_stru,* pupdata_menu_stru;

//����: �����ֻ�֡���ݽṹ��
typedef struct _rt_updata_menu_stru{
	char frame_id;
	char payload_num;
	char menu_id;   //�˵����
	char par_id;    //�������
	int  Statu;    //�޸�״̬ 0x00FF:�ɹ��޸� other:���ɹ�
}rt_updata_menu_stru;

//����: ���ذ��֡�汾����汾���
typedef struct _get_ver_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	int ver;       //�汾��
	char drive_indx; //�汾���
	char bak;
}get_ver_stru,* pget_ver_stru;

//����: ���ð汾���,��ȡ�汾�Žṹ��
typedef struct _set_ver_idx_stru{
	char frame_id;
	char payload_num;
	char cmd_type;     //��������
	char ver_idx;       //�������
	char bak;
}set_ver_idx_stru;

//���ز˵��������ݽṹ��
typedef struct _menu_item_stru{
	int menu_max;   //���ֵ
	int menu_min;   //��Сֵ
	char ext_property; //��������
}menu_item_stru;

typedef struct _rec_menu_cfg_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;
	char menu_count;       //�˵��б���
	menu_item_stru val[CFG_MAX_ITEM];   //�б�ֵ����
}rec_menu_cfg_stru,* prec_menu_cfg_stru;

//����Ĭ��ֵ���ݽṹ��
typedef struct _rec_defaut_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;
	int val[CFG_MAX_ITEM];       //�˵�Ĭ��ֵ
}rec_defaut_stru;

//�������ذ�״̬��
typedef struct _rec_statu_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char err_flag;       //�����
	char statu_num;      //״̬��
	char pass_flag;      //��ͣ��� λ7Ϊ1����ʾ��������
	char other_bit_flag; //����λ��־
	long disp_count;    //����������
	char  io_read;    //����״̬ 
}rec_statu_stru;


//��ȡ�˵���������֡�ṹ��
typedef struct _get_cfg_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;       //�˵���
	char bak[2];
}get_cfg_stru;

//��ȡ�˵���������֡�ṹ��
typedef struct _get_data_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char menu_id;       //�˵���
	char bak[2];
}get_data_stru;


//�����ֽṹ��
typedef struct _cmd_stru{
	char frame_id;
	char payload_num;
	char cmd_type;
	char bak[2];
}cmd_stru;

//�����ṹ��
typedef struct
{
	///����FLASH_SAVE_WORD_NUM��������Ϊ��������
	int Disp_Dz_00000[MENU_ID_00000_MAX_ITEM + 1];
	int Disp_Dz_10101[MENU_ID_10101_MAX_ITEM + 1];
	int Disp_Dz_10102[MENU_ID_10102_MAX_ITEM + 1];
	int Disp_Dz_10103[MENU_ID_10103_MAX_ITEM + 1];
	int Disp_Dz_10104[MENU_ID_10104_MAX_ITEM + 1];
	int Disp_Dz_20101[MENU_ID_20101_MAX_ITEM + 1];
	int menu_parm[COM_MENU_MAX_ITEM];				//�˵�0�Ͳ˵�1����ƴ��

	unsigned int ms_timer;				///<�������ʱ����
	unsigned int Err_Flag;              //�����־ ��4λ:���ذ����� ��4λ:���������
	char Work_Pause_Flag; 				//������ͣ��־ λ7:ֵΪ1��������
	char Bell_Flag;                     //���������־:1�죬0����
	char Other_Bit_Flag;                //����λ��־
	unsigned int io_read;               //IO��״ֵ̬
	unsigned int version;               //��������汾��
	unsigned long cnt_after_powered;    //���巴���Ŀ������Ͳ��
#if 0	
	unsigned long cnt_before_powered;   //�ϵ��ȡ�Ŀ���ǰ�ѱ���Ͳ�� 
#endif
	unsigned long main_disp_val;        //��������ʾֵ = ����ǰ+������

	unsigned int uart_rec_start_timer;  //֡�����ʱ
	//unsigned int uart_rec_out_timer;  //ͨѶ��ʱ�ж���ʱ
	unsigned char hand_number;        //����֡����,����ֻ֡�̶���10��
	unsigned char uart_rec_statu;    //����״̬ 1:���ڽ��� 2:������ϣ����ڴ��� 3:�������
	unsigned char uart_rec_count;    //�����ֽڸ���
	unsigned char first_flag;        //����λ������ͨѶ 1:�������� 0:�������
	unsigned char dsp_debug;         //ͨѶ��ʱ����Ҫ��������,����DSP��������
	unsigned int drive_type;						//�������汾��
	unsigned char drive_indx;						//�汾������(���һ�����ذ��ж���汾)
	unsigned int drive_type_com;					//ͨѶ�汾��,������������汾��У��
	unsigned char drive_index_com;                  //�����ذ巵�ص�ͨѶ�汾���
	char com_log_step;
	char start_next_step;           //���ֳɹ�����һ��Ҫ����֡,�ɿ�����������
	char send_frame_idx;
	char uart_send_flag;
	char Work_Statu;
	char Init_Over;
	char Disp_debug;                    //��ʾ������ģʽ
	char Delay_Hand_En;                  //��ʱ�Ƿ���������,�����ֿغ��Ƿ��ܰ���

	char Lock_Flag;						//������־
	
	char input_pass_mask;               //����У�Ա�־
	unsigned int password_index_disp;               //�������
	unsigned int password_index;               //�������
	unsigned int password_val[2];               //����ֵ
	
	unsigned long work_count;        //����������¼ֵ �ϵ��EEPROM��ȡ
	unsigned long work_count_user;        //����������¼ֵ �û�����ֵ �ϵ��EEPROM��ȡ

	unsigned int last_speed;            //�������һ���ٶ�ֵ  added for v19416

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


///����λ����������
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
