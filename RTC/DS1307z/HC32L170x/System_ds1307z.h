#ifndef __SYSTEM_DS1307Z_H
#define __SYSTEM_DS1307Z_H

#include "main_file_define.h"

#include "System_iic.h"


//ʱ������
#define YEAR 2023       // ��
#define MON 8           // ��
#define DAY 14          // ��
#define HOUR 16         // ʱ
#define MINN 40          // ��
#define SEC 30          // ��
#define WEEK 1          // ���ڼ�

#define FLAG_ADDR		0x09       // �ж��Ƿ��ʼ����ַ
#define FLAG_VAL_1	1          //	��ʶ1
#define FLAG_VAL_2	2          // ��ʶ2
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



//��ʼ��
void DS1307Z_Init(void);
// ʱ���ʼ������
void DS1307Z_Set_Time(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec,uint8_t week);
// ��ȡ����
uint8_t DS1307Z_Read(uint8_t address);
// д������
void DS1307Z_Wirte(uint8_t address , uint8_t dat);
//��ȡʱ��
void DS1307Z_Get_Time(void);





#endif
