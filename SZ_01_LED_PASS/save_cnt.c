
#include "save_cnt.h"

//�����������
void save_sys_lock(void)
{
#if 1
unsigned char *progbyte,i;
unsigned int progadd = EEPROM_LOCK_VAL;
unsigned long add_index,temp_32;
unsigned int over_num = 0;

	add_index = public_val.work_count - 1;
	over_num = add_index>>7;
//	if(over_num >= 5) over_num = 0;  //����5����
	over_num %= 5;
	
	if(!(add_index % 0x80))      //128����һ����
		{
			eeprom_en = 0xaa;
			IapEraseSector(EEPROM_LOCK_VAL + (over_num << 9)); //�����´����
			eeprom_en = 0;	
		}
	temp_32 = add_index << 2; //һ����Ŵ��ĸ��ֽ�
	temp_32 %= 0xA00;

	progadd += (unsigned int)temp_32;         //��������ʱ�����ƫ�Ƶ�ַ
	progbyte = (unsigned char *)&public_val.work_count;

	//SendData(*(progbyte));
	eeprom_en = 0xaa;
	for(i = 0; i < 4; i++)       //��������洢
		{
			IapProgramByte(progadd,*(progbyte));
			//SendData(*(progbyte));
			progbyte++;
			progadd++;
		}
	eeprom_en = 0;	
#endif	
}



//��ȡ����ʱ��
void read_loack_count(void)
{
#if MENU_PASS_ON
unsigned char *progbyte,i_pr;
unsigned int progadd = EEPROM_LOCK_VAL;
unsigned long read_word[3],max_val = 0,max_idx = 10000;
unsigned long ver_data[3],max_ver_data[2];
unsigned int i;
	for(i = 0; i < 640; i++) //�����洢��,�ҳ����ֵ�������
		{
			progadd = EEPROM_LOCK_VAL + (i<<2);         //�������з���������ƫ�Ƶ�ַ
			progbyte = (unsigned char *)&read_word[0];
			*progbyte = IapReadByte(progadd);
//if(public_val.Disp_Dz_00123[0])
//			SendData(*(progbyte));
			progbyte++;
			*progbyte = IapReadByte(progadd+1);
//if(public_val.Disp_Dz_00123[0])
//			SendData(*(progbyte));
			progbyte++;
			*progbyte = IapReadByte(progadd+2);
//if(public_val.Disp_Dz_00123[0])
//			SendData(*(progbyte));
			progbyte++;
			*progbyte = IapReadByte(progadd+3);
//if(public_val.Disp_Dz_00123[0])
//			SendData(*(progbyte));
			ver_data[0] = ver_data[1];
			ver_data[1] = read_word[0];
			if(i == 639)
				ver_data[2] = read_word[0];   //��β��ӵ����
			if(read_word[0] <= 0x7FFFFFFF)
				{
					if(read_word[0] > max_val)
						{
							max_ver_data[0] = ver_data[0];
							max_ver_data[1] = ver_data[1];							
							max_val = read_word[0];
							max_idx = i;
							if(max_idx == 0)  //������Ϊ���ֵ�����
								{
									max_ver_data[0] = ver_data[2];
								}
						}
				}
			
			read_word[1] = read_word[0];
		}

	public_val.Lock_Flag = 0;
	if(max_idx != 10000)  //����������
		{
			if(max_ver_data[1] - max_ver_data[0] == 1)
				{
					public_val.work_count = max_val;
				}
			else
				{
					public_val.work_count = 0;
					//save_sys_lock();
				}
		}
	else
		{
			public_val.work_count = 0;
			save_sys_lock();
		}
#endif		
}

//��ȡ�������
void read_user_count(void)
{
#if 1
unsigned char *progbyte;
unsigned int progadd = EEPROM_USER_COUNT;
	progbyte = (unsigned char *)&public_val.work_count_user;
	*progbyte = IapReadByte(progadd);
//if(public_val.Disp_Dz_00123[0])
//	SendData(*(progbyte));
	progbyte++;
	*progbyte = IapReadByte(progadd+1);
//if(public_val.Disp_Dz_00123[0])
//	SendData(*(progbyte));
	progbyte++;
	*progbyte = IapReadByte(progadd+2);
//if(public_val.Disp_Dz_00123[0])
//	SendData(*(progbyte));
	progbyte++;
	*progbyte = IapReadByte(progadd+3);
//if(public_val.Disp_Dz_00123[0])
//	SendData(*(progbyte));

	if(public_val.work_count - public_val.work_count_user > 99999)
		{
			public_val.work_count_user = public_val.work_count;
			save_user_count();
		}
#endif		
}



//�����û��������ֵ
void save_user_count(void)
{
#if 1
unsigned char *progbyte,i;
unsigned int progadd = EEPROM_USER_COUNT;
unsigned long add_index,temp_32;
unsigned int over_num = 0;

	eeprom_en = 0xaa;
	IapEraseSector(EEPROM_USER_COUNT); //����������
	eeprom_en = 0;	

	public_val.work_count_user = public_val.work_count;
	progbyte = (unsigned char *)&public_val.work_count_user;

	//SendData(*(progbyte));
	eeprom_en = 0xaa;
	for(i = 0; i < 4; i++)       //��������洢
		{
			IapProgramByte(progadd,*(progbyte));
			//SendData(*(progbyte));
			progbyte++;
			progadd++;
		}
	eeprom_en = 0;	
#endif	
}


