#include "System_ds1307z.h"

struct DS1307Z_Struct Date_Time;


void DS1307Z_Init(void)
{
		printf("���ڽ���ʱ���ʼ��\r\n");
		if(DS1307Z_Read(FLAG_ADDR) != FLAG_VAL)
		{	
			DS1307Z_Set_Time(YEAR,MON,DAY,HOUR,MINN,SEC,WEEK);        //2023��7��11�� 17:00:00 ���ڶ�
			
			printf("#�ڴ�ֵ��Ϊ�� %d \r\n",DS1307Z_Read(FLAG_ADDR));
			
			printf("#ʱ���ʼ�����\r\n");
		}
		else
		{
			printf("ʱ���Ѿ���ʼ�����ˣ�\r\n");
		}
		
		//printf("ʱ���ʼ�����\r\n");
}

//ʱ���ʼ������
void DS1307Z_Set_Time(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec,uint8_t week)
{
	year-=2000;//��Ĭ��2000�꿪ʼ
	if(year > 100) year = 0;
	
	//ʮ����תΪBCD��
	year = ((year /10) << 4) + year %10;
	mon  = ((mon  /10) << 4) + mon  %10;
	day  = ((day  /10) << 4) + day  %10;
	week = ((week /10) << 4) + week %10;
	hour = ((hour /10) << 4) + hour %10;
	min  = ((min  /10) << 4) + min  %10;
	sec  = ((sec  /10) << 4) + sec  %10;
	
	//д��Ĵ�����ͬʱ���һ����ַ
	DS1307Z_Wirte(0x07,1); 
	DS1307Z_Wirte(FLAG_ADDR,FLAG_VAL);	
	DS1307Z_Wirte(0x00,sec);    //������
  DS1307Z_Wirte(0x01,min);    //���÷�
  DS1307Z_Wirte(0x02,hour);   //����ʱ
  DS1307Z_Wirte(0x03,week);   //������
  DS1307Z_Wirte(0x04,day);    //������
  DS1307Z_Wirte(0x05,mon);    //������	
  DS1307Z_Wirte(0x06,year);   //������		
	DS1307Z_Wirte(0x07,0);    
}

uint8_t DS1307Z_Read(uint8_t address)
{
	uint8_t dat;	
	IIC_Start();
	IIC_Send_Byte(0xD0);//�����豸��ַ��дָ��
	IIC_Wait();
	IIC_Send_Byte(address);//���ͼĴ�����ַ
	IIC_Wait();
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte(0xD1);	//�����豸��ַ����ָ��
	IIC_Wait();

	dat = IIC_Read_Byte(); //��һ���ֽ�����
	
  IIC_Nack();	
	IIC_Stop();
	return dat;
}


void DS1307Z_Wirte(uint8_t address , uint8_t dat)
{ 
	IIC_Start();
	IIC_Send_Byte(0xD0);//�����豸��ַ��дָ�� 11010000
	IIC_Wait();
	IIC_Send_Byte(address);//���ͼĴ�����ַ
	IIC_Wait();
	IIC_Send_Byte(dat);//дһ���ֽ�����
	IIC_Wait();
	IIC_Stop();	
}

//��ȡ��ǰʱ��
void DS1307Z_Get_Time(void)
{
	uint8_t sec,min,hour,week,day,mon,year;
	
	sec  = DS1307Z_Read(0x00);
	min  = DS1307Z_Read(0x01);
	hour = DS1307Z_Read(0x02);
	week = DS1307Z_Read(0x03);	
	day  = DS1307Z_Read(0x04);
	mon  = DS1307Z_Read(0x05);
	year = DS1307Z_Read(0x06);
	
	//BCD��תʮ����
	Date_Time.sec  = (sec/16)*10 + sec%16;
	Date_Time.min  = (min/16)*10 + min%16;
	Date_Time.hour = (hour/16)*10 + hour%16;
	Date_Time.day  = (day/16)*10  + day%16;
	Date_Time.mon  = (mon/16)*10  + mon%16;
	Date_Time.year = (year/16)*10 + year%16 + 2000;
	Date_Time.week = (week/16)*10 + week%16;   	

// char LCD_Display_A[] = "  2023-07-17 2  "; 
// char LCD_Display_B[] = "   18:30  30s   "; 
//	sprintf ( Date_Time.LCD_Display_A, "  %d-%d-%d %d   ",Date_Time.year, Date_Time.mon,Date_Time.day,Date_Time.week);
//	sprintf ( Date_Time.LCD_Display_B, "   %d:%d  %ds   ",Date_Time.hour, Date_Time.min, Date_Time.sec);
}
