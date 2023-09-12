#ifndef __SYSTEM_INTERNAL_RTC_H
#define __SYSTEM_INTERNAL_RTC_H

#include "main_file_define.h"

// 开关闹钟中断
#define rtc_alarm_irq_open()		Rtc_AlmIeCmd(TRUE)
#define rtc_alarm_irq_close()		Rtc_AlmIeCmd(FALSE)
// 开关中断向量，回调函数：Rtc_IRQHandler()
#define rtc_Nvic_open()			EnableNvic(RTC_IRQn, IrqLevel3, TRUE)
#define rtc_Nvic_close()		EnableNvic(RTC_IRQn, IrqLevel3, FALSE)

#define	rtc_cmd_open()			Rtc_Cmd(TRUE)
#define	rtc_cmd_close()			Rtc_Cmd(FALSE)

#define YEAR_T 	0x23
#define MON_T		0x01
#define DAY_T		0x01
#define HOUR_T	0x00
#define MIN_T		0x00
#define SEC_T		0x00
#define	WEEK_T	0x01

typedef struct rtc_time
{
    uint8_t  sec;				//时间：秒    
    uint8_t  min;				//时间：分
    uint8_t  hour;    	//时间：时
    uint8_t  week;			//时间：周
    uint8_t  day;				//时间：日    
    uint8_t  mon;       //时间：月
    uint16_t year;			//时间：年
} rtc_time_t;


// 初始化RTC
void system_internal_rtc_init(void);
// 设置RTC时间
uint8_t system_internal_rtc_set_time(rtc_time_t *rtc_time);
// 获取RTC时间
void system_internal_rtc_get_time(rtc_time_t *get_rtc);

#endif
