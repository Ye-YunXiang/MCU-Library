#include "system_lcd1602.h"
#include "System_ds1307z.h"

struct payg_token
{
	uint16_t PayG_Token_Days;
	uint8_t PayG_Token_Days_l;
	uint8_t PayG_Token_Days_m;
	uint8_t PayG_Token_Days_h;
	
	uint8_t PayG_Token_Hours;
	uint8_t PayG_Token_Hours_l;
	uint8_t PayG_Token_Hours_h;
	
	uint32_t PayG_Token_Second;
}PayG;

void LCD_Display_Init(void)
{
	stc_gpio_cfg_t Stc_LCD_Port_Init;
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
		Stc_LCD_Port_Init.enDir = GpioDirOut;
//		Stc_LCD_Port_Init.enDrv = GpioDrvL;
		Stc_LCD_Port_Init.enPu = GpioPuDisable;
		Stc_LCD_Port_Init.enPd = GpioPdDisable;
//		Stc_LCD_Port_Init.enOD = GpioOdDisable;
//		Stc_LCD_Port_Init.enCtrlMode = GpioAHB;
	
	Gpio_SetIO(LCDD4_PORT,LCDD4_PIN);
	Gpio_SetIO(LCDD5_PORT,LCDD5_PIN);
	Gpio_SetIO(LCDD6_PORT,LCDD6_PIN);
	Gpio_SetIO(LCDD7_PORT,LCDD7_PIN);
	
  Gpio_Init(LCDRS_PORT, LCDRS_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDEN_PORT, LCDEN_PIN, &Stc_LCD_Port_Init);		
	Gpio_Init(LCDRW_PORT, LCDRW_PIN, &Stc_LCD_Port_Init);
	
	Gpio_Init(LCDD4_PORT, LCDD4_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDD5_PORT, LCDD5_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDD6_PORT, LCDD6_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDD7_PORT, LCDD7_PIN, &Stc_LCD_Port_Init);
		
	Gpio_Init(LCD_LIGHT_PORT,LCD_LIGHT_PIN ,&Stc_LCD_Port_Init);

}

static void LCD_Dispaly_Write_CMD_OR_Data(uint8_t Cmdordata,uint8_t Writetype)
{
	if(Writetype==0)
	{
		Gpio_ClrIO(LCDRS_PORT,LCDRS_PIN);		//write command
	}
	else
	{
		Gpio_SetIO(LCDRS_PORT,LCDRS_PIN);			//write data
	}
//	PORT_SetBits(PortA,cmdordata&0x00ff);								// 8 线驱动
//	PORT_ResetBits(PortA,(~cmdordata)&0x00ff);					// 8 线驱动
	Gpio_SetPort(GpioPortA,(Cmdordata&0xf0)&0x00f0);			// 4 线驱动
	Gpio_ClrPort(GpioPortA,((~Cmdordata)&0xf0)&0x00f0);		// 4 线驱动
	delay1ms(5);
	Gpio_SetIO(LCDEN_PORT,LCDEN_PIN);
	delay1ms(5);
	Gpio_ClrIO(LCDEN_PORT,LCDEN_PIN);
		
	Gpio_SetPort(GpioPortA,((Cmdordata&0x0f)<<4)&0x00f0);			// 4 线驱动
	Gpio_ClrPort(GpioPortA,(((~Cmdordata)&0x0f)<<4)&0x00f0);	// 4 线驱动
	delay1ms(5);
	Gpio_SetIO(LCDEN_PORT,LCDEN_PIN);
	delay1ms(5);
	Gpio_ClrIO(LCDEN_PORT,LCDEN_PIN);
}


void LCD1602_Dispaly_Init(void)
{
	delay1ms(200);
	LCD_Dispaly_Write_CMD_OR_Data(0x30,0);
	delay1ms(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x30,0);
	delay1ms(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x30,0);
	delay1ms(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x28,0);//配置16*2显示，5*7点阵，4位数据接口
	delay1ms(5);
	Gpio_SetIO(LCDEN_PORT,LCDEN_PIN);
	delay1ms(5);
	Gpio_ClrIO(LCDEN_PORT,LCDEN_PIN);
	delay1ms(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x28,0);//配置16*2显示，5*7点阵，4位数据接口
	delay1ms(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x0C,0);//设置开显示，不显示光标
	LCD_Dispaly_Write_CMD_OR_Data(0x06,0);//写字符后地址自动加1
	LCD_Dispaly_Write_CMD_OR_Data(0x01,0);//显示清0，数据指针清0
	delay1ms(5);
}

void LCD1602_Token_String_Dispaly(char *Lcd_string,uint8_t String_len, uint8_t Row_num)
{
	LCD_Dispaly_Write_CMD_OR_Data(Row_num,0);
	for(uint8_t i = 0; i < String_len; i++)
	{
		LCD_Dispaly_Write_CMD_OR_Data(*Lcd_string,1);
		Lcd_string++;
		delay1ms(20);
	}
}

void LCD1602_Token_Data_Display(char *Openpaygo_Token,uint8_t Token_Num, uint8_t Dispalay_Type)
{
	if(Dispalay_Type == 0x00)
	{
		LCD_Dispaly_Write_CMD_OR_Data(0x81+Token_Num,0);				
		LCD_Dispaly_Write_CMD_OR_Data(Openpaygo_Token[Token_Num]+0x30,1);
	}
	else if(Dispalay_Type == 0x23)
	{
		LCD_Dispaly_Write_CMD_OR_Data(0x81+Token_Num,0);				
		LCD_Dispaly_Write_CMD_OR_Data(0x23,1);
	}
	else if(Dispalay_Type == 0x2A)
	{
		LCD_Dispaly_Write_CMD_OR_Data(0x80+Token_Num,0);				
		LCD_Dispaly_Write_CMD_OR_Data(0x2A,1);
	}
	else 
	{;}
}

void LCD1602_Token_Clear_Dispaly(void)
{
	LCD_Dispaly_Write_CMD_OR_Data(0x01,0);//显示清0，数据指针清0
}

//void LCD_Dispaly_Payg_Token(uint32_t PayG_Token)
//{
//	PayG.PayG_Token_Second = PayG_Token;
//	PayG.PayG_Token_Days   = (uint16_t)(PayG.PayG_Token_Second / 3600) /24;
//	PayG.PayG_Token_Days_h = (uint8_t)(PayG.PayG_Token_Days / 100);
//	PayG.PayG_Token_Days_m = (uint8_t)((PayG.PayG_Token_Days % 100) / 10);
//	PayG.PayG_Token_Days_l = (uint8_t)(((PayG.PayG_Token_Days %100) % 10) %10);
//	
//	LCD_Dispaly_Write_CMD_OR_Data(0xC0,0);
//	LCD_Dispaly_Write_CMD_OR_Data(0x30+PayG.PayG_Token_Days_h ,1);
//	LCD_Dispaly_Write_CMD_OR_Data(0xC1,0);
//	LCD_Dispaly_Write_CMD_OR_Data(0x30+PayG.PayG_Token_Days_m ,1);
//	LCD_Dispaly_Write_CMD_OR_Data(0xC2,0);
//	LCD_Dispaly_Write_CMD_OR_Data(0x30+PayG.PayG_Token_Days_l ,1);
//		
//	PayG.PayG_Token_Hours = (PayG.PayG_Token_Second / 3600) % 24;
//	PayG.PayG_Token_Hours_h = (uint8_t)(PayG.PayG_Token_Hours / 10);
//	PayG.PayG_Token_Hours_l = (uint8_t)(PayG.PayG_Token_Hours % 10);
//	
//	LCD_Dispaly_Write_CMD_OR_Data(0xC9,0);
//	LCD_Dispaly_Write_CMD_OR_Data(0x30+PayG.PayG_Token_Hours_h  ,1);
//	LCD_Dispaly_Write_CMD_OR_Data(0xCA,0);
//	LCD_Dispaly_Write_CMD_OR_Data(0x30+PayG.PayG_Token_Hours_l  ,1);
//}

//void User_Openpaygo_id_display(uint32_t Openpaygo_id_diplay)
//{
//	uint8_t Openpaygo_id_dis[9];
//	uint32_t nexus_id_display_count;
//	Openpaygo_id_dis[0] = (uint8_t)(Openpaygo_id_diplay / 100000000);
//	Openpaygo_id_dis[1] = (uint8_t)((Openpaygo_id_diplay % 100000000) / 10000000);
//	Openpaygo_id_dis[2] = (uint8_t)(((Openpaygo_id_diplay % 100000000) % 10000000) / 1000000);
//	Openpaygo_id_dis[3] = (uint8_t)((((Openpaygo_id_diplay % 100000000) % 10000000) % 1000000) / 100000);
//	Openpaygo_id_dis[4] = (uint8_t)(((((Openpaygo_id_diplay % 100000000) % 10000000) % 1000000) % 100000) / 10000);
//	Openpaygo_id_dis[5] = (uint8_t)((((((Openpaygo_id_diplay % 100000000) % 10000000) % 1000000) % 100000) % 10000) / 1000);
//	Openpaygo_id_dis[6] = (uint8_t)(((((((Openpaygo_id_diplay % 100000000) % 10000000) % 1000000) % 100000) % 10000) % 1000) / 100);
//	Openpaygo_id_dis[7] = (uint8_t)((((((((Openpaygo_id_diplay % 100000000) % 10000000) % 1000000) % 100000) % 10000) % 1000) % 100) / 10);
//	Openpaygo_id_dis[8] = (uint8_t)((((((((Openpaygo_id_diplay % 100000000) % 10000000) % 1000000) % 100000) % 10000) % 1000) % 100) % 10);
//	
//	nexus_id_display_count = 10;
//	LCD1602_Token_Clear_Dispaly();
//	while(nexus_id_display_count > 0)
//	{
//		LCD_Dispaly_Write_CMD_OR_Data(0x80,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[0] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x81,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[1] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x82,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[2] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x83,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[3] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x84,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[4] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x85,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[5] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x86,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[6] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x87,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[7] ,1);
//		
//		LCD_Dispaly_Write_CMD_OR_Data(0x88,0);
//		LCD_Dispaly_Write_CMD_OR_Data(0x30+Openpaygo_id_dis[8] ,1);
//		nexus_id_display_count--;
//	}
//}


void LCD_Display_Time(void)
{
		if(  Date_Time.hour == 0 && Date_Time.min == 0 && Date_Time.sec == 0)
		{
				LCD_Display_Time_Init();
		}
		else if((Date_Time.min == 0 && Date_Time.sec == 0) || (Date_Time.min == 0 && Date_Time.sec == 1) || (Date_Time.min == 0 && Date_Time.sec == 2))
		{
				if(Date_Time.hour < 10)
				{
					sprintf ( Date_Time.LCD_Display_B, "   0%d:          ",Date_Time.hour);
						LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC0);
				}
				else
				{
					sprintf ( Date_Time.LCD_Display_B, "   %d:          ",Date_Time.hour);
						LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC0);
				}
				
				if(Date_Time.min < 10)
				{
						sprintf ( Date_Time.LCD_Display_B, "0%d",Date_Time.min);
						LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC6);
				}
				else
				{
						sprintf ( Date_Time.LCD_Display_B, "%d",Date_Time.min);
						LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC6);
				}
		}
		else if(Date_Time.sec == 0 || Date_Time.sec == 1 || Date_Time.sec == 2 || Date_Time.sec == 3)
		{
				if(Date_Time.min < 10)
				{
							sprintf ( Date_Time.LCD_Display_B, "0%d  ",Date_Time.min);
							LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC6);
				}
				else
				{
							sprintf ( Date_Time.LCD_Display_B, "%d    ",Date_Time.min);
							LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC6);
				}
				sprintf ( Date_Time.LCD_Display_B, "%ds    ",Date_Time.sec);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC9);
		}
		else
		{
				sprintf ( Date_Time.LCD_Display_B, "%ds    ",Date_Time.sec);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC9);
		}			
}


void LCD_Display_Time_Init(void)
{
		sprintf ( Date_Time.LCD_Display_A, "  %d-  -   %d   ",Date_Time.year, Date_Time.week);
		LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_A, strlen(Date_Time.LCD_Display_A),0x80);
		
		if(Date_Time.mon < 10)
		{
				sprintf ( Date_Time.LCD_Display_A, "0%d",Date_Time.mon);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_A, strlen(Date_Time.LCD_Display_A),0x87);
		}
		else
		{
				sprintf ( Date_Time.LCD_Display_A, "%d",Date_Time.mon);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_A, strlen(Date_Time.LCD_Display_A),0x87);
		}
	
		if(Date_Time.day < 10)
		{
				sprintf ( Date_Time.LCD_Display_A, "0%d",Date_Time.day);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_A, strlen(Date_Time.LCD_Display_A),0x8A);
		}
		else
		{
				sprintf ( Date_Time.LCD_Display_A, "%d",Date_Time.day);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_A, strlen(Date_Time.LCD_Display_A),0x8A);
		}
		
		if(Date_Time.hour < 10)
		{
			sprintf ( Date_Time.LCD_Display_B, "   0%d:          ",Date_Time.hour);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC0);
		}
		else
		{
			sprintf ( Date_Time.LCD_Display_B, "   %d:          ",Date_Time.hour);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC0);
		}
		
		if(Date_Time.min < 10)
		{
				sprintf ( Date_Time.LCD_Display_B, "0%d",Date_Time.min);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC6);
		}
		else
		{
				sprintf ( Date_Time.LCD_Display_B, "%d",Date_Time.min);
				LCD1602_Token_String_Dispaly(Date_Time.LCD_Display_B, strlen(Date_Time.LCD_Display_B),0xC6);
		}
	
}
