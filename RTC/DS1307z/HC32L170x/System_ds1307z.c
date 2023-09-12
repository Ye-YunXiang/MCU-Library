#include "System_ds1307z.h"

struct DS1307Z_Struct Date_Time;


void DS1307Z_Init(void)
{
		printf("正在进行时间初始化\r\n");
		if(DS1307Z_Read(FLAG_ADDR) != FLAG_VAL)
		{	
			DS1307Z_Set_Time(YEAR,MON,DAY,HOUR,MINN,SEC,WEEK);        //2023年7月11日 17:00:00 星期二
			
			printf("#内存值改为： %d \r\n",DS1307Z_Read(FLAG_ADDR));
			
			printf("#时间初始化完成\r\n");
		}
		else
		{
			printf("时间已经初始化过了！\r\n");
		}
		
		//printf("时间初始化完成\r\n");
}

//时间初始化设置
void DS1307Z_Set_Time(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec,uint8_t week)
{
	year-=2000;//年默认2000年开始
	if(year > 100) year = 0;
	
	//十进制转为BCD码
	year = ((year /10) << 4) + year %10;
	mon  = ((mon  /10) << 4) + mon  %10;
	day  = ((day  /10) << 4) + day  %10;
	week = ((week /10) << 4) + week %10;
	hour = ((hour /10) << 4) + hour %10;
	min  = ((min  /10) << 4) + min  %10;
	sec  = ((sec  /10) << 4) + sec  %10;
	
	//写入寄存器，同时标记一个地址
	DS1307Z_Wirte(0x07,1); 
	DS1307Z_Wirte(FLAG_ADDR,FLAG_VAL);	
	DS1307Z_Wirte(0x00,sec);    //设置秒
  DS1307Z_Wirte(0x01,min);    //设置分
  DS1307Z_Wirte(0x02,hour);   //设置时
  DS1307Z_Wirte(0x03,week);   //设置周
  DS1307Z_Wirte(0x04,day);    //设置日
  DS1307Z_Wirte(0x05,mon);    //设置月	
  DS1307Z_Wirte(0x06,year);   //设置年		
	DS1307Z_Wirte(0x07,0);    
}

uint8_t DS1307Z_Read(uint8_t address)
{
	uint8_t dat;	
	IIC_Start();
	IIC_Send_Byte(0xD0);//发送设备地址，写指令
	IIC_Wait();
	IIC_Send_Byte(address);//发送寄存器地址
	IIC_Wait();
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte(0xD1);	//发送设备地址，读指令
	IIC_Wait();

	dat = IIC_Read_Byte(); //读一个字节数据
	
  IIC_Nack();	
	IIC_Stop();
	return dat;
}


void DS1307Z_Wirte(uint8_t address , uint8_t dat)
{ 
	IIC_Start();
	IIC_Send_Byte(0xD0);//发送设备地址，写指令 11010000
	IIC_Wait();
	IIC_Send_Byte(address);//发送寄存器地址
	IIC_Wait();
	IIC_Send_Byte(dat);//写一个字节数据
	IIC_Wait();
	IIC_Stop();	
}

//获取当前时间
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
	
	//BCD码转十进制
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
