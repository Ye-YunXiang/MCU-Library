#ifndef __SYSTEM_INTERNAL_RTC_H
#define __SYSTEM_INTERNAL_RTC_H

#include "main_file_define.h"

// ���������ж�
#define rtc_alarm_irq_open()		Rtc_AlmIeCmd(TRUE)
#define rtc_alarm_irq_close()		Rtc_AlmIeCmd(FALSE)
// �����ж��������ص�������Rtc_IRQHandler()
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
    uint8_t  sec;				//ʱ�䣺��    
    uint8_t  min;				//ʱ�䣺��
    uint8_t  hour;    	//ʱ�䣺ʱ
    uint8_t  week;			//ʱ�䣺��
    uint8_t  day;				//ʱ�䣺��    
    uint8_t  mon;       //ʱ�䣺��
    uint16_t year;			//ʱ�䣺��
} rtc_time_t;


// ��ʼ��RTC
void system_internal_rtc_init(void);
// ����RTCʱ��
uint8_t system_internal_rtc_set_time(rtc_time_t *rtc_time);
// ��ȡRTCʱ��
void system_internal_rtc_get_time(rtc_time_t *get_rtc);

#endif
