#ifndef __SYSTEM_IIC_H
#define __SYSTEM_IIC_H

#include "main_file_define.h"


#define IIC_SDA_PORT			(GpioPortF)
#define IIC_SDA_PIN				(GpioPin7)

#define IIC_SCL_PORT			(GpioPortF)
#define IIC_SCL_PIN				(GpioPin6)


// ����״̬
#define IIC_SCL_HIGH()		Gpio_SetIO(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_SCL_LOW()			Gpio_ClrIO(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_SDA_HIGH()		Gpio_SetIO(IIC_SDA_PORT,IIC_SDA_PIN)

#define IIC_SDA_LOW()			Gpio_ClrIO(IIC_SDA_PORT,IIC_SDA_PIN)

// ��ȡ״̬
#define IIC_READ_SDA()		Gpio_GetInputIO(IIC_SDA_PORT,IIC_SDA_PIN)



#define IIC_Delay_4us() delay1us(4)


void IIC_SDA_OUT(void);
void IIC_SDA_IN(void);
void IIC_Start(void);         
void IIC_Stop(void);
uint8_t IIC_Wait(void);      //�ȴ���Ӧ
void IIC_ACK(void);       //��Ӧl 
void IIC_Nack(void);      //û����Ӧ
void IIC_Send_Byte(uint8_t data);    //����һ���ֽ�
uint8_t IIC_Read_Byte(void);         //����һ���ֽ�

void IIC_Port_Init(void);


#endif
