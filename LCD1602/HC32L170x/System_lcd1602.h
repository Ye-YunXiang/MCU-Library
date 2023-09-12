#ifndef __SYSTEM_LCD1602_H
#define __SYSTEM_LCD1602_H

#include "main_file_define.h"

#define LCDRS_PORT    (GpioPortA)			// RS
#define LCDRS_PIN     (GpioPin1)

#define LCDRW_PORT		(GpioPortA)			// R/W
#define LCDRW_PIN			(GpioPin2)

#define LCDEN_PORT		(GpioPortA)			// EN
#define LCDEN_PIN			(GpioPin3)

#define LCDD4_PORT		(GpioPortA)			// DB4
#define LCDD4_PIN			(GpioPin4)

#define LCDD5_PORT		(GpioPortA)			// DB5
#define LCDD5_PIN			(GpioPin5)

#define LCDD6_PORT		(GpioPortA)			// DB6
#define LCDD6_PIN			(GpioPin6)

#define LCDD7_PORT		(GpioPortA)			// DB7
#define LCDD7_PIN			(GpioPin7)

#define	LCD_LIGHT_PORT	(GpioPortB)		// DB8		控制背光正极
#define LCD_LIGHT_PIN		(GpioPin1)

#define LCD_Light_Enable()	Gpio_SetIO(LCD_LIGHT_PORT,LCD_LIGHT_PIN)
#define LCD_Light_Disable() Gpio_ClrIO(LCD_LIGHT_PORT,LCD_LIGHT_PIN)


void LCD_Display_Init(void);
void LCD1602_Dispaly_Init(void);
void LCD1602_Token_String_Dispaly(char *Lcd_string,uint8_t String_len, uint8_t Row_num);
void LCD1602_Token_Data_Display(char *Openpaygo_Token,uint8_t Token_Num, uint8_t Dispalay_Type);
void LCD1602_Token_Clear_Dispaly(void);
void LCD_Dispaly_Payg_Token(uint32_t PayG_Token);
void User_Openpaygo_id_display(uint32_t Openpaygo_id_diplay);

void LCD_Display_Time(void);
void LCD_Display_Time_Init(void);


#endif
