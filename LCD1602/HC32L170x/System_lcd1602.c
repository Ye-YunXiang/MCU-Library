// Encoding UTF-8
#include "system_lcd1602.h"

// 大量延时是为了适配使用4mHz的FreeRTOS系统，降低出错的概率。

/********************************************************************
 ** \brief  lcd显示字符
 ** @param  需要显示的字符
 ** @param  显示的屏幕位置
 ** \retval 无
********************************************************************/
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
	USER_DELAY_1US(DELAY_20_US);
	//@def 8线
//	PORT_SetBits(PortA,cmdordata&0x00ff);		
	//@def 4线
	Gpio_SetPort(GpioPortA,(Cmdordata&0xf0)&0x00f0);	
	Gpio_ClrPort(GpioPortA,((~Cmdordata)&0xf0)&0x00f0);		
	USER_DELAY_1US(DELAY_20_US);
	Gpio_SetIO(LCDEN_PORT,LCDEN_PIN);
	USER_DELAY_1US(DELAY_20_US);
	Gpio_ClrIO(LCDEN_PORT,LCDEN_PIN);
		
	USER_DELAY_1US(DELAY_20_US);
	//@def 4线
	Gpio_SetPort(GpioPortA,((Cmdordata&0x0f)<<4)&0x00f0);			
	Gpio_ClrPort(GpioPortA,(((~Cmdordata)&0x0f)<<4)&0x00f0);	
	USER_DELAY_1US(DELAY_20_US);
	Gpio_SetIO(LCDEN_PORT,LCDEN_PIN);
	USER_DELAY_1US(DELAY_20_US);
	Gpio_ClrIO(LCDEN_PORT,LCDEN_PIN);
	
}


/********************************************************************
 ** \brief  lcd引脚初始化
 ** \retval 无
********************************************************************/
void system_lcd1602_display_port_init(void)
{
	stc_gpio_cfg_t Stc_LCD_Port_Init;
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
		Stc_LCD_Port_Init.enDir = GpioDirOut;
//		Stc_LCD_Port_Init.enDrv = GpioDrvL;
		Stc_LCD_Port_Init.enPu = GpioPuDisable;
		Stc_LCD_Port_Init.enPd = GpioPdDisable;
//		Stc_LCD_Port_Init.enOD = GpioOdDisable;
//		Stc_LCD_Port_Init.enCtrlMode = GpioAHB;
	

  	Gpio_Init(LCDRS_PORT, LCDRS_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDEN_PORT, LCDEN_PIN, &Stc_LCD_Port_Init);		
	Gpio_Init(LCDRW_PORT, LCDRW_PIN, &Stc_LCD_Port_Init);
	
	Gpio_Init(LCDD4_PORT, LCDD4_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDD5_PORT, LCDD5_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDD6_PORT, LCDD6_PIN, &Stc_LCD_Port_Init);
	Gpio_Init(LCDD7_PORT, LCDD7_PIN, &Stc_LCD_Port_Init);

	Gpio_ClrIO(LCDD4_PORT,LCDD4_PIN);
	Gpio_ClrIO(LCDD5_PORT,LCDD5_PIN);
	Gpio_ClrIO(LCDD6_PORT,LCDD6_PIN);
	Gpio_ClrIO(LCDD7_PORT,LCDD7_PIN);
		
	Gpio_Init(LCD_LIGHT_PORT,LCD_LIGHT_PIN ,&Stc_LCD_Port_Init);
	System_LCD_Light_Disable();

}


/********************************************************************
 ** \brief  LCD1602初始化
 ** \retval 无
********************************************************************/
void system_lcd1602_dispaly_config_init(void)
{
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x30,0);
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x30,0);
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x30,0);
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x28,0);//配置16*2显示，5*7点阵，4位数据接口
	USER_DELAY_1MS(5);
	Gpio_SetIO(LCDEN_PORT,LCDEN_PIN);
	USER_DELAY_1MS(5);
	Gpio_ClrIO(LCDEN_PORT,LCDEN_PIN);
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x28,0);//配置16*2显示，5*7点阵，4位数据接口
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x0C,0);//设置开显示，不显示光标
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x06,0);//写字符后地址自动加1
	USER_DELAY_1MS(5);
	LCD_Dispaly_Write_CMD_OR_Data(0x01,0);//显示清0，数据指针清0
	USER_DELAY_1MS(5);
	
	system_lcd1602_display_clear_dispaly();
}


/********************************************************************
 ** \brief	lcd显示字符串
 ** @param  字符串指针
 ** @param	字符串长度
 ** @param	显示屏显示起始位置
 ** \retval 无
********************************************************************/
void system_lcd1602_string_dispaly(char *Lcd_string,uint8_t String_len, uint8_t Row_num)
{
	LCD_Dispaly_Write_CMD_OR_Data(Row_num,0);
	USER_DELAY_1MS(20);
	for(uint8_t i = 0; i < String_len; i++)
	{
		
		LCD_Dispaly_Write_CMD_OR_Data(*Lcd_string,1);
		Lcd_string++;
		USER_DELAY_1MS(30);
	}
}


/********************************************************************
 ** \brief  清除屏幕显示
 ** \retval 无
********************************************************************/
void system_lcd1602_display_clear_dispaly(void)
{
	//@def 显示清0，数据指针清0
	LCD_Dispaly_Write_CMD_OR_Data(0x01,0);
	USER_DELAY_1MS(20);
}
