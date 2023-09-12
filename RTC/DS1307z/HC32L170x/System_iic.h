#ifndef __SYSTEM_IIC_H
#define __SYSTEM_IIC_H

#include "main_file_define.h"


#define IIC_SDA_PORT			(GpioPortF)
#define IIC_SDA_PIN				(GpioPin7)

#define IIC_SCL_PORT			(GpioPortF)
#define IIC_SCL_PIN				(GpioPin6)


// 设置状态
#define IIC_SCL_HIGH()		Gpio_SetIO(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_SCL_LOW()			Gpio_ClrIO(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_SDA_HIGH()		Gpio_SetIO(IIC_SDA_PORT,IIC_SDA_PIN)

#define IIC_SDA_LOW()			Gpio_ClrIO(IIC_SDA_PORT,IIC_SDA_PIN)

// 获取状态
#define IIC_READ_SDA()		Gpio_GetInputIO(IIC_SDA_PORT,IIC_SDA_PIN)



#define IIC_Delay_4us() delay1us(4)


void IIC_SDA_OUT(void);
void IIC_SDA_IN(void);
void IIC_Start(void);         
void IIC_Stop(void);
uint8_t IIC_Wait(void);      //等待响应
void IIC_ACK(void);       //响应l 
void IIC_Nack(void);      //没有响应
void IIC_Send_Byte(uint8_t data);    //发送一个字节
uint8_t IIC_Read_Byte(void);         //接收一个字节

void IIC_Port_Init(void);


#endif
