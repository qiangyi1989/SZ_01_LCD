#ifndef USER_H
#define USER_H

//#define FRAME_TYPE_PAR 0 //������
//#define FRAME_TYPE_CMD 1 //������
//#define FRAME_TYPE_INFO 2//��Ϣ��

/* ���� */
#define DOWN_READ_PARAM        0  	  /* ���ж����� */
#define DOWN_READ_STATE   	   1  	  /* ���ж�״̬ */
#define DOWN_READ_PARAM_RANGE  2  	  /* ���ж�������Χ */
#define DOWN_WRITE_PARAM  	   3  	  /* ����д���ò��� */
#define DOWN_WRITE_TEST_MODE   4      /* ����д�������ģʽ */
#define DOWN_WRITE_CLEAR_CNT   5      /* ����д����ѳ�Ͳ�� */
#define DOWN_WRITE_KEY   	   6      /* ����д�������� */
#define DOWN_WRITE_LEARN_MODE  7      /* ����д����ѧϰģʽ */
#define DOWN_WRITE_SPEED_MODE  8      /* ����д�л��ٶ�ģʽ */
#define DOWN_WRITE_WARNING_MSG  9      /* ����д������Ϣ */

/* ���� */
#define UP_WRITE_PARAM  	    10   	    /* ����д���� */
#define UP_WRITE_STATE  	    11   		/* ����д״̬ */
#define UP_WRITE_PARAM_RANGE    12 		    /* ����д������Χ */
#define UP_WRITE_REC_PARAM_DONE 13  		/* ����д���ղ������ */
#define UP_WRITE_TEST_MODE_DONE 14          /* ����д�������ģʽ�ɹ� */
#define UP_WRITE_CLEAR_CNT_DONE 15          /* ����д����ѳ�Ͳ���ɹ� */
#define UP_WRITE_KEY_DONE 		16          /* ����д���������ɹ� */
#define UP_WRITE_LEARN_MODE_DONE 17          /* ����д����ѧϰģʽ�ɹ� */
#define UP_WRITE_SPEED_MODE_DONE 18          /* ����д�л��ٶ�ģʽ�ɹ� */
#define UP_WRITE_WARNING_MSG_DONE 19         /* ����д������Ϣ�ɹ� */


#define UP_WRITE_CHECK_FAIL     20    	    /* ����дУ��ʧ�� */


#define DOWN_READ_PARAM_SENT  		50  
#define DOWN_READ_STATE_SENT  		51
#define DOWN_READ_PARAM_RANGE_SENT  52
#define DOWN_WRITE_PARAM_SENT       53
#define DOWN_WRITE_TEST_MODE_SENT   54
#define DOWN_WRITE_CLEAR_CNT_SENT   55
#define DOWN_WRITE_KEY_SENT         56
#define DOWN_WRITE_LEARN_MODE_SENT  57
#define DOWN_WRITE_SPEED_MODE_SENT  58
#define DOWN_WRITE_WARNING_MSG_SENT 59

#define UP_REPLY_DONE   0x7f


//��Ϣ������
#define GET_VER_CMD 0x01 //��ȡ�汾��
#define GET_DEFAUT_VAL 0x02 //��ȡĬ�ϲ���ֵ
#define GET_MENU_CFG_INFO 0x03 //��ȡ�˵�������Ϣ
#define GET_STATU_INFO 0x04 //��ȡ���ذ�״̬
#define GET_PAR_INFO 0x05 //��ȡ���ذ�˵����� ���ز���
#define SET_VER_IDX_CMD 0x07 //�������ذ�汾���

//����ֵ�߼����
#define CMD_INDX_0X88 0x88 //�����������,���빤��״̬
#define CMD_INDX_0XAA 0xAA //BOOT

//����״̬���
#define WORK_STATU_WAIT 0x01 //�ȴ��������·�����
#define WORK_STATU_START 0x02 //���빤��״̬,��ɵ���ȶ���

//�����ϴ���־ λ����
#define BIT_FLAG_DISP_ON 0x01  //1:��������˵� 0:����������˵�

//ͨѶ�����߼����
#define COMD_LOG_STEP_0X550XAA 0  //����
#define COMD_LOG_STEP_0X550XAA_DLY 1  //������ʱ
#define COMD_LOG_STEP_GET_VER 2 //��ȡ�汾��
#define COMD_LOG_STEP_GET_DEF 10 //�汾��ƥ���ȡĬ��ֵ
#define COMD_LOG_STEP_GET_CFG 20 //�汾��ƥ���ȡ�˵�������Ϣ
#define COMD_LOG_STEP_DOWN 30 //�·��˵����ݵ����ذ�
#define COMD_LOG_STEP_SART 40 //��ʼ����ɽ��빤��
#define COMD_LOG_STEP_BOOT_RUN 50 //������д����

#define COMD_LOG_STEP_OTHER_CMD 0x77 //������������ָ��
#define COMD_LOG_STEP_WAIT 0x88    //Ѳ��
#define COMD_LOG_STEP_DELAY 0x99   //��ʱ�÷��ͱ�־

#define COMD_LOG_DELAY_DZ 200 //����֡���ʱ�� ��λms
#define COMD_LOG_DELAY_PAR 100 //����֡��ʱʱ�� ��λms 
#define COMD_LOG_DELAY_WORK 500 //Ѳ��֡��ʱʱ�� ��λms 

//�����
#define ERR_NUM_COM 18 //ͨѶ����



void uart_init();
void SendData(unsigned char dat);
void Send_UartBuf(char * buf);
void check_uart_rec(void);
void rec_data_func(void);
void send_data_func(void);

#endif

