#include "system_internal_rtc.h"

stc_rtc_time_t readtime;		// ������ RTC ��ȡ������BCD��

/**
	** \brief  ��ʼ��RTC������ʱ�ӣ������������ж�
  ** @param  ��
  ** \retval ��
	*/
void system_internal_rtc_init(void)
{
    stc_rtc_initstruct_t RtcInitStruct;
    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);		//RTCģ��ʱ�Ӵ�
	
			RtcInitStruct.rtcAmpm = RtcPm;                     		//Сʱ��
			RtcInitStruct.rtcClksrc = RtcClkRcl;               		//����ʱ��
			RtcInitStruct.rtcPrdsel.rtcPrdsel = RtcPrdx;      		//�����ж�����PRDX
			RtcInitStruct.rtcPrdsel.rtcPrdx = 1u;              		//�����ж�ʱ���� 1��
		
			RtcInitStruct.rtcTime.u8Second 		= SEC_T;  	        //����RTCʱ��
			RtcInitStruct.rtcTime.u8Minute 		= MIN_T;
			RtcInitStruct.rtcTime.u8Hour   		= HOUR_T;
			RtcInitStruct.rtcTime.u8Day    		= DAY_T;
			RtcInitStruct.rtcTime.u8DayOfWeek = WEEK_T;
			RtcInitStruct.rtcTime.u8Month  		= MON_T;
			RtcInitStruct.rtcTime.u8Year   		= YEAR_T;
		
			RtcInitStruct.rtcCompen = RtcCompenEnable; 	  	      // ʹ��ʱ������
			RtcInitStruct.rtcCompValue = 0;             	  	    //����ֵ  ����ʵ��������в���
	
    Rtc_Init(&RtcInitStruct);
  
		rtc_alarm_irq_open();                              			//ʹ�������ж�    
    rtc_Nvic_open();              											 		//ʹ��RTC�ж�����
    rtc_cmd_open();                                       	//ʹ��RTC��ʼ����
}

/**
	** \brief  ����RTCʱ��
  ** @param  ʱ��ṹ��ָ�룬���ú�Ҫ���õ�ʱ��󴫽���
  ** \retval ��
	*/
uint8_t system_internal_rtc_set_time(rtc_time_t *rtc_time)
{
		rtc_time->year -= 2000;//��Ĭ��2000�꿪ʼ
		if(rtc_time->year > 100) rtc_time->year = 0; 
		//ʮ����תΪBCD��
		rtc_time->year = ((rtc_time->year /10) << 4) + rtc_time->year %10;
		rtc_time->mon  = ((rtc_time->mon  /10) << 4) + rtc_time->mon  %10;
		rtc_time->day  = ((rtc_time->day  /10) << 4) + rtc_time->day  %10;
		rtc_time->week = ((rtc_time->week /10) << 4) + rtc_time->week %10;
		rtc_time->hour = ((rtc_time->hour /10) << 4) + rtc_time->hour %10;
		rtc_time->min  = ((rtc_time->min  /10) << 4) + rtc_time->min  %10;
		rtc_time->sec  = ((rtc_time->sec  /10) << 4) + rtc_time->sec  %10;
	
		rtc_Nvic_close();      											         		//ʹ��RTC�ж�����
		rtc_alarm_irq_close();
		rtc_cmd_close();
	
		stc_rtc_time_t artcTime;
				artcTime.u8Second 		= rtc_time->sec;  	            					 	//����RTCʱ��
				artcTime.u8Minute 		= rtc_time->min;
				artcTime.u8Hour   		= rtc_time->hour;
				artcTime.u8Day    		= rtc_time->day;
				artcTime.u8DayOfWeek 	= rtc_time->week;
				artcTime.u8Month  		= rtc_time->mon;
				artcTime.u8Year   		= rtc_time->year;
		uint8_t enRet = Rtc_SetTime(&artcTime);
	
		rtc_alarm_irq_open();
		rtc_Nvic_open();               		//ʹ��RTC�ж�����
		rtc_cmd_open(); 
		
		return enRet;
}

/**
	** \brief  ��ȡRTCʱʱ�ӣ���䵽�βε�ָ����
  ** @param  ʱ��ṹ��ָ��
  ** \retval ��
	*/
void system_internal_rtc_get_time(rtc_time_t *get_rtc)
{
	Rtc_ReadDateTime(&readtime);  // ��ȡRTCʱ��ֵ
	
	get_rtc->sec	 = (readtime.u8Second/16)*10 + readtime.u8Second%16;   // ��ȡʱ��ֵ
	get_rtc->min	 = (readtime.u8Minute/16)*10 + readtime.u8Minute%16;
	get_rtc->hour = (readtime.u8Hour/16)*10 + readtime.u8Hour%16;
	get_rtc->day  = (readtime.u8Day/16)*10 + readtime.u8Day%16;
	get_rtc->week = (readtime.u8DayOfWeek/16)*10 + readtime.u8DayOfWeek%16;
	get_rtc->mon  = (readtime.u8Month/16)*10 + readtime.u8Month%16;
	get_rtc->year = (readtime.u8Year/16)*10 + readtime.u8Year%16;
}
