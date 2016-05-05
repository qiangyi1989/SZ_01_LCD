
#include "reg51.h"
#include "intrins.h"
#include "led_display.h"
#include "uart.h"
#include "global.h"
#include "save_cnt.h"
#include "sys.h"
#include <string.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 11059200L          //ϵͳƵ��
#define BAUD 9600             //���ڲ�����

#define NONE_PARITY     0       //��У��
#define ODD_PARITY      1       //��У��
#define EVEN_PARITY     2       //żУ��
#define MARK_PARITY     3       //���У��
#define SPACE_PARITY    4       //�հ�У��

#define PARITYBIT NONE_PARITY   //����У��λ

//#define UART_REC_FRAME_DELAY 50
#define UART_REC_FRAME_DELAY 10


extern xdata public_stru public_val;
extern xdata char uart_send_buf[100];
extern code MENU_ITEM MenuItem[];
extern xdata MENU_CHANG now_menu;


//sfr AUXR  = 0x8e;               //�����Ĵ���

//sbit P22 = P2^2;

bit busy;  /* 1���ڷ��ͣ�0������� */

//void SendString(char *s);

xdata unsigned char Uart_RecBuf[100];
void * com_data = Uart_RecBuf;


char temp_char;




void SendData(BYTE dat);

void uart_init()
{
#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8λ�ɱ䲨����
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9λ�ɱ䲨����,У��λ��ʼΪ1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9λ�ɱ䲨����,У��λ��ʼΪ0
#endif

    AUXR = 0x40;                //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x00;                //��ʱ��1Ϊģʽ0(16λ�Զ�����)
    TL1 = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
    EA = 1;

}

/*----------------------------
UART �жϷ������
-----------------------------*/

void UartRecIsr() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                 //���RIλ
        Uart_RecBuf[public_val.uart_rec_count++] = SBUF;
        public_val.uart_rec_statu = 1;
        public_val.uart_rec_start_timer = public_val.ms_timer;
    }
    if (TI)
    {
        TI = 0;                 //���TIλ
        busy = 0;               //��æ��־
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   void check_uart_rec(void)
���ܣ�  ��������֡�ж�
������  ��
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void check_uart_rec(void)
{
	if(public_val.uart_rec_statu == 1)
	{
		/* �������50msδ���յ����ݣ���Ϊ��ǰ֡�����꣬��ʼ���� */
		if(public_val.ms_timer - public_val.uart_rec_start_timer > UART_REC_FRAME_DELAY)
		{
			public_val.uart_rec_statu = 2;  /* �����ڴ���״̬ */
		}
	}
}

/*----------------------------
���ʹ�������
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //�ȴ�ǰ������ݷ������
    ACC = dat;                  //��ȡУ��λP (PSW.0)
    if (P)                      //����P������У��λ
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //����У��λΪ0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //����У��λΪ1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //����У��λΪ1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //����У��λΪ0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //д���ݵ�UART���ݼĴ���
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   Send_UartBuf(char * buf)
���ܣ�  ���ݷ��� ��У��
������  ��
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void Send_UartBuf(char * buf)
{
	int i;
	static char frame_id = 1;
	char sum_byte = 0;
	
	//buf[0] = (buf[0] & 0x03) | (frame_id << 2);
	for(i = 0; i < buf[1] + 2; i++)
	{
		sum_byte += buf[i];
		SendData(buf[i]);
	}
	SendData(sum_byte);
	frame_id++;
	if(!frame_id) frame_id = 1;
}

/*----------------------------
�����ַ���
----------------------------*/
/*void SendString(char *s)
{
    while (*s)                  //����ַ���������־
    {
        SendData(*s++);         //���͵�ǰ�ַ�
    }
}*/

#if 0
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   void frame_par_func(void)
���ܣ�  �����ֻ�֡����
������  ��
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void frame_par_func()
{
	if(now_menu.chang_par_flag == 1)
		{
			if( ((rt_updata_menu_stru *)com_data)->Statu == 0x00FF )
				{
					now_menu.chang_par_flag = 2;
				}
			else
				{
					now_menu.chang_par_flag = 3;
				}
		}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   void frame_info_func(void)
���ܣ�  ��֡��Ϣ�ִ���
������  ��
����ֵ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void frame_info_func(void)
{
	char menu_id,i;
	switch(((get_ver_stru *)com_data)->cmd_type)
		{
			case GET_VER_CMD:
				public_val.drive_type_com = ((get_ver_stru *)com_data)->ver;
				public_val.drive_index_com = ((get_ver_stru *)com_data)->drive_indx;
				if(public_val.drive_type_com != public_val.drive_type)
					{
						public_val.com_log_step = COMD_LOG_STEP_GET_CFG;
					}
				else
					{
						public_val.com_log_step = COMD_LOG_STEP_DOWN;
					}
				public_val.send_frame_idx = 0;
			break;
			case SET_VER_IDX_CMD:
				public_val.drive_indx = ((set_ver_idx_stru *)com_data)->ver_idx;
				public_val.drive_type = 0; //����汾��,�´��ϵ����»�ȡ����
				func_ver_update();
			break;
			case GET_MENU_CFG_INFO:
				menu_id = ((rec_menu_cfg_stru *)com_data)->menu_id;
				*((char*)(MenuItem[menu_id].com_max_item)) = ((rec_menu_cfg_stru *)com_data)->menu_count;
				for(i = 0; i < *(MenuItem[menu_id].com_max_item); i++)
					{
						MenuItem[menu_id].m_pcfg[i].m_max = 
							((rec_menu_cfg_stru *)com_data)->val[i].menu_max;
						MenuItem[menu_id].m_pcfg[i].m_min = 
							((rec_menu_cfg_stru *)com_data)->val[i].menu_min;
						MenuItem[menu_id].m_pcfg[i].ext_property = 
							((rec_menu_cfg_stru *)com_data)->val[i].ext_property;
					}
				func_cfg_update(menu_id);
				if(menu_id == MENU_NUM - 1) //��ɲ˵��������ݴ���,����汾���汾���
					{
						//debug
						public_val.drive_type = public_val.drive_type_com;
						public_val.drive_indx = public_val.drive_index_com;
						func_ver_update();
					}
			break;
			case GET_DEFAUT_VAL:
				menu_id = ((rec_menu_cfg_stru *)com_data)->menu_id;
				for(i = 0; i < *(MenuItem[menu_id].com_max_item); i++)
					{
						*((int *)MenuItem[menu_id].m_pcfg[i].m_data_vale) = 
							((rec_defaut_stru *)com_data)->val[i];
					}
				//����˵�����ֵ��EEPROM
				func_menu_update(menu_id);
			break;
			case GET_PAR_INFO:
				menu_id = ((rec_menu_cfg_stru *)com_data)->menu_id;
				for(i = 0; i < *(MenuItem[menu_id].com_max_item); i++)
					{
						*((int *)MenuItem[menu_id].m_pcfg[i].m_data_vale) = 
							((rec_defaut_stru *)com_data)->val[i];
					}
				//����˵�����ֵ��EEPROM
				func_menu_update(menu_id);
			break;
			case GET_STATU_INFO:
				public_val.Err_Flag &= 0xf0;
				public_val.Err_Flag |= (((rec_statu_stru *)com_data)->err_flag) & 0x0f;
				public_val.Work_Statu = ((rec_statu_stru *)com_data)->statu_num;
				public_val.Work_Pause_Flag = ((rec_statu_stru *)com_data)->pass_flag;
				public_val.Other_Bit_Flag = ((rec_statu_stru *)com_data)->other_bit_flag;
				public_val.main_disp_val = ((rec_statu_stru *)com_data)->disp_count;
				public_val.io_read = ((rec_statu_stru *)com_data)->io_read;
				if(public_val.Work_Statu  == WORK_STATU_WAIT)
					{
						public_val.Init_Over = 0;
						public_val.com_log_step = COMD_LOG_STEP_GET_VER;
					}
				else if(public_val.Work_Statu  >= WORK_STATU_START)
					{
						public_val.Init_Over = 1;
					}
			break;
		}
}

#endif


void rec_data_func(void)
{
	char i;
	char sum_byte;
    char *pcTemp = 0;
	unsigned long uiCnt;

	static char ucSaveLearn1 = 0, ucSaveLearn2 = 0;

	/* �����巴��������֡��У�� */
	sum_byte = Uart_RecBuf[0];
	for(i = 1; i < public_val.uart_rec_count - 1; i++)
	{
		sum_byte += Uart_RecBuf[i];
	}

	/* У��ʧ�ܱ����� */
	if(sum_byte != Uart_RecBuf[public_val.uart_rec_count - 1])
	{
		//public_val.Err_Flag = ERR_NUM_ZK_COM_ERR;		
		public_val.Err_Flag =  LED_CHECK_ERROR;       /* ���յ�У�����֡�󲻷��ͺ���֡ */

		send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */
		
		return; 		
	}


	/* У��ɹ����Խ���֡���д��� */	
	switch(Uart_RecBuf[0])
	{
		/* ����д����֡ */
		case UP_WRITE_PARAM:

			/* ��ȡ�������� */
			//Menu_Number[0] = Uart_RecBuf[1] >> 1;    /* �غɳ��ȳ���2Ϊ�������� */
//			Menu_Number[0] = (Uart_RecBuf[1] + Uart_RecBuf[2]) >> 1;  //���в�������
//			Menu0_Number = Uart_RecBuf[1] >> 1;  					  //�˵�0��������

			/* ��ȡ�·��Ĳ��� */
			pcTemp = (char *)&public_val.menu_parm[0];
			
			for (i = 0; i < (COM_MENU_MAX_ITEM << 1); i++)
			{
				//pcTemp[i] = Uart_RecBuf[2 + i];
				pcTemp[i] = Uart_RecBuf[3 + i];
			}

			already_read_param = 1;  /* ��Ϊ�Ѷ������ò��� */

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */
			
		break;

		/* ����д״̬֡ */
		case UP_WRITE_STATE:

			for(i = 0; i < PASS_LOCK_COUNT; i++)
				{
					if( (public_val.Disp_Dz_10104[i] > 0)
						&& (public_val.work_count > ((unsigned long)public_val.Disp_Dz_10104[i] * LOCK_UNIT))
						)
						{
							public_val.Lock_Flag = i + 1;
						}
				}
			if(public_val.Disp_Dz_10104[PASS_LOCK_COUNT - 1] == 0) //��ȫ����
				{
					public_val.Lock_Flag = 0;
				}
			
			public_val.version 
				= ((unsigned int)Uart_RecBuf[3] << 8) | (unsigned int)Uart_RecBuf[2];			
			
			public_val.io_read
				= ((unsigned int)Uart_RecBuf[5] << 8) | (unsigned int)Uart_RecBuf[4];
					
			//if ((Uart_RecBuf[7] != 0) || (Uart_RecBuf[6] != 0))
			//{
			public_val.Err_Flag 
				= ((unsigned int)Uart_RecBuf[7] << 8) | (unsigned int)Uart_RecBuf[6];
			//}

			//public_val.disp_cnt_upload
			uiCnt
				= ((unsigned long)Uart_RecBuf[9] << 8) | (unsigned long)Uart_RecBuf[8]
				| ((unsigned long)Uart_RecBuf[11] << 24) | ((unsigned long)Uart_RecBuf[10] << 16);

			if (uiCnt != public_val.cnt_after_powered)  /* ���Ͳ���и��£��򱣴���� */
			{
				public_val.cnt_after_powered = uiCnt;
		   		public_val.work_count++;
				if(public_val.work_count - public_val.work_count_user > 99999)
					{
						public_val.work_count_user = public_val.work_count;
						save_user_count();
					}
								
				save_sys_lock();
			}
#if 0
			if ((public_val.version == 11111) && (ucSaveLearn1 == 0))
			{
				public_val.menu_parm[PARAM_FZ_START_PWM_SCALE] = (int)(uiCnt & 0xFFFF);
				//public_val.menu_parm[PARAM_QZ_START_PWM_SCALE] = (int)((uiCnt >> 16) & 0xFFFF);
				
				func_menu_update(0);

				ucSaveLearn1 = 1;
			}

			if ((public_val.version == 22222) && (ucSaveLearn2 == 0))
			{
				public_val.menu_parm[PARAM_FZ_START_PWM_SCALE] = (int)(uiCnt & 0xFFFF);
				//public_val.menu_parm[PARAM_QZ_START_PWM_SCALE] = (int)((uiCnt >> 16) & 0xFFFF);
				
				func_menu_update(0);

				ucSaveLearn2 = 1;
			}
#endif			
			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */
			
		break;

		/* ����д������Χ֡ */
		case UP_WRITE_PARAM_RANGE:

			for (i = 0; i < (Uart_RecBuf[1] >> 2); i++)  /* ��������Ϊ�غɳ��ȳ���4 */
			{
				MenuItem[MENU_ID_00000].m_pcfg[i].m_min 
					= ((short)Uart_RecBuf[2 + (i << 2)] << 8)    /* ���ո��ֽ� */
						| (short)Uart_RecBuf[2 + (i << 2) + 1];	
				
				MenuItem[MENU_ID_00000].m_pcfg[i].m_max 
					= ((short)Uart_RecBuf[2 + (i << 2) + 2] << 8) 
						| (short)Uart_RecBuf[2 + (i << 2) + 3];				
			}

			//Menu_Number[0] = (Uart_RecBuf[1] >> 2);  

			already_read_param_range = 1;  /* ��Ϊ�Ѷ������ò�����Χ */	

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */	

		break;

		/* ����д���ղ������ */
		case UP_WRITE_REC_PARAM_DONE:

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;

		/* ����д�������ģʽ�ɹ� */
		case UP_WRITE_TEST_MODE_DONE:

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;

		/* ����д����ѧϰģʽ�ɹ� */
		case UP_WRITE_LEARN_MODE_DONE:

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;

		/* ����д�л��ٶ�ģʽ�ɹ� */
		case UP_WRITE_SPEED_MODE_DONE:

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;

		/* ����д������Ϣ�ɹ� */
		case UP_WRITE_WARNING_MSG_DONE:

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;



		/* ����д����ѳ�Ͳ���ɹ� */
		case UP_WRITE_CLEAR_CNT_DONE:

			main_borad_finish_clear = 1;      /* ���������������� */

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;		

		/* ����д�����ɹ� */
		case UP_WRITE_KEY_DONE:

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */

		break;	


		/* У��ʧ��֡������ */
		case UP_WRITE_CHECK_FAIL:  
			
			//public_val.Err_Flag = ERR_NUM_ZK_COM_ERR;	
			public_val.Err_Flag = MAIN_CHECK_ERROR;		  /* ���յ�У�����֡�󲻷��ͺ���֡ */	

			send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */
			
		break;

		default:
			break;
	}
	
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
����:   send_data_func(void)
���ܣ�  ͨѶ������� �����������
������  ��
����ֵ����
˵��: �汾��ƥ��: ��ȡ�˵�������Ϣ->��ȡĬ�ϲ���->�·��˵�����
      �汾ƥ��: �·��˵�����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void send_data_func(void)
{
	int i;

    char *pcTemp = 0;
	unsigned char sum_byte;
	

	/* ÿ1s��ȡһ������״̬ */
#if 1
	if(public_val.ms_timer - cnt_read_state > READ_STATE_PERIOD)
	{	
		if (write_param_flag == 0)
		{
			if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ������Ͷ�״̬֡ */
			{
				send_frame_type = DOWN_READ_STATE;
			}
		}

		cnt_read_state = public_val.ms_timer;
	}
#endif

	/* �Ѷ�������״̬����δ�����ò��������ȡһ�����ò��� */
#if 1	
	if ((public_val.version != 0) && (already_read_param == 0))
	{
		//if (public_val.ms_timer - cnt_read_param > 100)
		{		
#if 1
			if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ������Ͷ�����֡ */
			{
				//send_frame_type = DOWN_READ_PARAM;				

				// дĬ�ϲ���
				// ���һ����������0x55aa��дһ��Ĭ�ϲ���
				if (public_val.Disp_Dz_10101[MENU_ID_10101_MAX_ITEM] != 0x55aa)
				{
					public_val.Disp_Dz_00000[0] = 310;   //ֽ��					
					public_val.Disp_Dz_00000[1] = 300;   /* �趨Ͳ�� */
					
					public_val.Disp_Dz_10101[0] = 2;     /* 1����������ʱ��:s */
					public_val.Disp_Dz_10101[1] = 15;    /* 2����Ͳ���ʱ��1, ʱ����Ӧ����Ͳ���1��Ӧ�������ӳ�ʱ��:0.1s */
					public_val.Disp_Dz_10101[2] = 15;    /* 3����Ͳ���ʱ��2, ʱ����Ӧ����Ͳ���2��Ӧ�������ӳ�ʱ��:0.1s */
					public_val.Disp_Dz_10101[3] = 0;     /* 4������������� */
					public_val.Disp_Dz_10101[4] = 0;     // 5����ֽ�ٶ�����
					public_val.Disp_Dz_10101[5] = 320;   /* 6������������嵱��:0.1��/mm (4000*2 /(2*3.14*40)) ���ٱ�2 */
					public_val.Disp_Dz_10101[6] = 89;    /* 7����ֽ������ּ��ٱ� */
					public_val.Disp_Dz_10101[7] = 400;   /* 8����ֽѹ�ְ뾶:0.1mm */
					public_val.Disp_Dz_10101[8] = 20;   /* 9����ֽ���PID���ٵı���ϵ��*100 */
				#ifdef SZ_01    // SZ_01
					public_val.Disp_Dz_10101[9] = 85;   /* 10����ֽ�������PWMռ�ձ� */
					public_val.Disp_Dz_10101[10] = 85;   /* 11����ֽ�������PWMռ�ձ� */
					public_val.Disp_Dz_10101[12] = 70;   /* 12����ֽ���Ŀ��PWMռ�ձ� */
					public_val.Disp_Dz_10101[13] = 10;   /* 13����ֽ����ٶ� */
					public_val.Disp_Dz_10101[14] = 1;    /* 14����ֽ����ٶ� */
					public_val.Disp_Dz_10101[15] = 40;   /* 15����ֽ�Ƕ� */
					public_val.Disp_Dz_10101[16] = 3;    /* 16���е�����ģʽ 3:�߶ε��� */
					public_val.Disp_Dz_10101[17] = 84;   /* 17����ֹֽͣ�ٶ� */
					public_val.Disp_Dz_10101[18] = 1;    /* 18������ */
					public_val.Disp_Dz_10101[19] = 1;    /* 19������ */
					public_val.Disp_Dz_10101[20] = 60;   /* 20����������ٶ� */
				#endif
					
				#ifdef SZ_02            // SZ_02
					public_val.Disp_Dz_10101[9] = 84;   /* 10����ֽ�������PWMռ�ձ� */				
					public_val.Disp_Dz_10101[10] = 2;    /* 11����ֽ������ּ��ٱ� */
					public_val.Disp_Dz_10101[11] = 120;  /* 12����ֽ���Ŀ���ٶ�:0.1khz */
					public_val.Disp_Dz_10101[12] = 10;   /* 13����ֽ����ٶ� */
					public_val.Disp_Dz_10101[13] = 1;    /* 14����ֽ����ٶ� */
					public_val.Disp_Dz_10101[14] = 73;   /* 15����ֽ�Ƕ�:0.1k���� */
					public_val.Disp_Dz_10101[15] = 80;    /* 16������ */
					public_val.Disp_Dz_10101[16] = 1;    /* 16������ */
					public_val.Disp_Dz_10101[17] = 1;   /* 17������ */
					public_val.Disp_Dz_10101[18] = 1;    /* 18������ */
					public_val.Disp_Dz_10101[19] = 1;    /* 19������ */
					public_val.Disp_Dz_10101[20] = 1;   /* 20������ */
				#endif

				#ifdef SZ_03
					public_val.Disp_Dz_10101[1] = 30;    /* 2����Ͳ���ʱ��1, ʱ����Ӧ����Ͳ���1��Ӧ�������ӳ�ʱ��:0.1s */
					public_val.Disp_Dz_10101[2] = 30;    /* 3����Ͳ���ʱ��2, ʱ����Ӧ����Ͳ���2��Ӧ�������ӳ�ʱ��:0.1s */
					public_val.Disp_Dz_10101[3] = 1;     /* 4������������� */
					public_val.Disp_Dz_10101[4] = 20;    // 5����ֽ�ٶ�����					
					public_val.Disp_Dz_10101[5] = 490;   /* 6�� ��ֽ����������嵱��:0.1��/mm (4000*(36/12) /(2*3.14*39)) ���ٱ�36/12 */
					public_val.Disp_Dz_10101[6] = 1;     /* 7����ֽ������ּ��ٱ� */
					public_val.Disp_Dz_10101[7] = 150;   /* 8����ֽ���Ŀ���ٶ�:0.1khz */
					public_val.Disp_Dz_10101[8] = 37;   /* 9����ֽ�Ƕ�:0.1k���� */
					public_val.Disp_Dz_10101[11] = 1;    /* 10������ */
					public_val.Disp_Dz_10101[12] = 1;    /* 11������ */
					public_val.Disp_Dz_10101[13] = 1;    /* 12������ */
					public_val.Disp_Dz_10101[14] = 1;    /* 13������ */	
					public_val.Disp_Dz_10101[15] = 1;    /* 14������ */
					public_val.Disp_Dz_10101[16] = 1;    /* 16������ */
					public_val.Disp_Dz_10101[17] = 1;   /* 17������ */
					public_val.Disp_Dz_10101[18] = 1;    /* 18������ */
					public_val.Disp_Dz_10101[19] = 1;    /* 19������ */
					public_val.Disp_Dz_10101[20] = 1;   /* 20������ */
				#endif

					public_val.Disp_Dz_10101[MENU_ID_10101_MAX_ITEM] = 0x55aa;					

					func_menu_update(MENU_ID_00000);
					func_menu_update(MENU_ID_10101);
					
				}

				write_param_flag = 1;    // ������λ������

				already_read_param = 1;  /* ��Ϊ�Ѷ������ò��� */

				send_frame_type = UP_REPLY_DONE;  /* ���Է�����һ֡ */				
				
			}
#endif
			//cnt_read_param = public_val.ms_timer;
		}
	}
#endif	

	/* �Ѷ������ò�������δ�����ò�����Χ�����ȡһ�����ò�����Χ */
#if 1	
	if ((already_read_param == 1) && (already_read_param_range == 0))
	{
		//if (public_val.ms_timer - cnt_read_param > 100)
		{		
			if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ������Ͷ�����֡ */
			{
				send_frame_type = DOWN_READ_PARAM_RANGE;
			}

			//cnt_read_param = public_val.ms_timer;
		}
	}
#endif	


	/* ��ǰ��Ҫ�������ò��������� */
#if 1
	if (write_param_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����֡ */
		{
			send_frame_type = DOWN_WRITE_PARAM;
		}
	}
#endif


	/* ��ǰ��Ҫдѧϰģʽ������ */
#if 1
	if (write_learn_mode_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����ģʽ֡ */
		{
			send_frame_type = DOWN_WRITE_LEARN_MODE;
		}
	}
#endif

#if 1
	if (write_warning_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����ģʽ֡ */
		{
			send_frame_type = DOWN_WRITE_WARNING_MSG;
		}
	}
#endif



	/* ��ǰ��Ҫд����ģʽ������ */
#if 1
	if (write_test_mode_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����ģʽ֡ */
		{
			send_frame_type = DOWN_WRITE_TEST_MODE;
		}
	}
#endif

	/* ��ǰ��Ҫд�ٶ�ģʽ������ */
#if 1
	if (write_speed_mode_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����ģʽ֡ */
		{
			send_frame_type = DOWN_WRITE_SPEED_MODE;
		}
	}
#endif


	/* ��ǰ��Ҫд����ѳ�Ͳ������ */
#if 1
	if (write_clear_cnt_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����ģʽ֡ */
		{
			send_frame_type = DOWN_WRITE_CLEAR_CNT;
		}
	}
#endif

	/* ��ǰ��Ҫд���������� */
#if 1
	if (write_key_flag != 0)
	{
		if (send_frame_type == UP_REPLY_DONE)  /* ��һ֡���յ��ظ�������д����֡ */
		{
			send_frame_type = DOWN_WRITE_KEY;
		}
	}	
#endif

	/* ������֡ */
	switch (send_frame_type)
	{
		case DOWN_READ_PARAM:  /* ���ж�����֡ */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));
			
			uart_send_buf[0] = DOWN_READ_PARAM;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 1;

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_READ_PARAM_SENT;   /* ��Ϊ������֡�ѷ��� */
			
			break;

		case DOWN_READ_STATE:  /* ���ж�״̬ */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));
			
			uart_send_buf[0] = DOWN_READ_STATE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 2;

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_READ_STATE_SENT;   /* ��Ϊ��״̬֡�ѷ��� */
			
			break;

		case DOWN_READ_PARAM_RANGE:  /* ���ж�������Χ */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));

			uart_send_buf[0] = DOWN_READ_PARAM_RANGE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 4;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_READ_PARAM_RANGE_SENT;   /* ��Ϊ��������Χ֡�ѷ��� */

			
			break;


		case DOWN_WRITE_PARAM:  /* ����д���ò��� */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_PARAM;
			uart_send_buf[1] = COM_MENU_MAX_ITEM << 1;	

			pcTemp = (char *)&public_val.menu_parm[0];

			sum_byte = uart_send_buf[0] + uart_send_buf[1];			
			
			for (i = 0; i < (COM_MENU_MAX_ITEM << 1); i++)
			{
				uart_send_buf[2 + i] = pcTemp[i];

				/* ����У��� */
				sum_byte += pcTemp[i];				
			}
			
			//sum_byte &= 0xff;
			
			uart_send_buf[2 + (COM_MENU_MAX_ITEM << 1)] = sum_byte;
	
			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_PARAM_SENT;   /* ��Ϊд����֡�ѷ��� */


			write_param_flag = 0;  /* �����һ��д���ò�������Ϊ��д״̬ */	
			
			break;

		case DOWN_WRITE_TEST_MODE:  /* ����д�������ģʽ */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_TEST_MODE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = test_mode;
			uart_send_buf[3] = 5 + test_mode;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_TEST_MODE_SENT;   /* ��Ϊд����ģʽ֡�ѷ��� */			

			write_test_mode_flag = 0;  /* �����һ��д����ģʽ����Ϊ��д״̬ */	

			break;

		case DOWN_WRITE_LEARN_MODE:  /* ����д����ѧϰģʽ */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_LEARN_MODE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;  // no use
			uart_send_buf[3] = 8;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_LEARN_MODE_SENT;   /* ��Ϊдѧϰģʽ֡�ѷ��� */			

			write_learn_mode_flag = 0;  /* �����һ��дѧϰģʽ����Ϊ��д״̬ */	

			break;

		case DOWN_WRITE_SPEED_MODE:  /* ����д�л��ٶ�ģʽ */

			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_SPEED_MODE;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = speed_mode;  
			uart_send_buf[3] = 9 + speed_mode;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_SPEED_MODE_SENT;   /* ��Ϊд�ٶ�ģʽ֡�ѷ��� */			

			write_speed_mode_flag = 0;  /* �����һ��д�ٶ�ģʽ����Ϊ��д״̬ */	


			break;

		case DOWN_WRITE_WARNING_MSG:

			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_WARNING_MSG;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;   // no use
			uart_send_buf[3] = 10;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_WARNING_MSG_SENT;   

			write_warning_flag = 0;  

			break;

		case DOWN_WRITE_CLEAR_CNT:  /* ����д����ѳ�Ͳ�� */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_CLEAR_CNT;
			uart_send_buf[1] = 1;			
			uart_send_buf[2] = 0;
			uart_send_buf[3] = 6;		

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_CLEAR_CNT_SENT;   			

			write_clear_cnt_flag = 0;  

			break;

		case DOWN_WRITE_KEY:  /* ����д�������� */
			memset(uart_send_buf, 0, sizeof(uart_send_buf));			

			uart_send_buf[0] = DOWN_WRITE_KEY;
			uart_send_buf[1] = 3;			
			uart_send_buf[2] = (char)gsKeyVal;
			uart_send_buf[3] = (char)(gsKeyVal >> 8);
			uart_send_buf[4] = gcKeyType;			
			uart_send_buf[5] = 9 + uart_send_buf[2] + uart_send_buf[3] 
				+ uart_send_buf[4];

			Send_UartBuf(uart_send_buf);

			send_frame_type = DOWN_WRITE_KEY_SENT;   			

			write_key_flag = 0;  

			break;

		default:
			break;
	}


	//if (send_frame_type != 0x7f)     /* ��֡Ҫ���� */
	//{		
	//	Send_UartBuf(uart_send_buf);  //deleted for test

		//send_frame_type = 0x7f;      /* ������ɣ���Ϊ��Ч֡����֡���� */
	//}
	
}


