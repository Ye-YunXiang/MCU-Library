#ifndef __SYSTEM_DS1307Z_H
#define __SYSTEM_DS1307Z_H

#include "main_file_define.h"

#include "System_iic.h"


//时间设置
#define YEAR 2023       // 年
#define MON 8           // 月
#define DAY 14          // 日
#define HOUR 16         // 时
#define MINN 40          // 分
#define SEC 30          // 秒
#define WEEK 1          // 星期几

#define FLAG_ADDR		0x09       // 判断是否初始化地址
#define FLAG_VAL_1	1          //	标识1
#define FLAG_VAL_2	2          // 标识2
#define FLAG_VAL FLAG_VAL_1

struct DS1307Z_Struct
{
    uint8_t     week;
    uint8_t     sec;
    uint8_t     min;
    uint8_t     hour;
    uint8_t     day;
    uint8_t     mon;
    uint16_t    year;
		char        LCD_Display_A[16];
		char        LCD_Display_B[16];
};

extern struct DS1307Z_Struct Date_Time;



//初始化
void DS1307Z_Init(void);
// 时间初始化设置
void DS1307Z_Set_Time(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec,uint8_t week);
// 读取数据
uint8_t DS1307Z_Read(uint8_t address);
// 写入数据
void DS1307Z_Wirte(uint8_t address , uint8_t dat);
//获取时间
void DS1307Z_Get_Time(void);





#endif
