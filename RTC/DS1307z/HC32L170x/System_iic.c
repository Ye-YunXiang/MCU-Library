#include "System_iic.h"


void IIC_SDA_OUT(void)
{ 
    stc_gpio_cfg_t stc_iic_port_dir_init;

//		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
			stc_iic_port_dir_init.enDir = GpioDirOut;
			stc_iic_port_dir_init.enDrv = GpioDrvL;
			stc_iic_port_dir_init.enPu = GpioPuDisable;
			stc_iic_port_dir_init.enPd = GpioPdDisable;
//			stc_iic_port_dir_init.enOD = GpioOdEnable;

		Gpio_Init(IIC_SDA_PORT, IIC_SDA_PIN, &stc_iic_port_dir_init);
}


void IIC_SDA_IN(void)  
{
    stc_gpio_cfg_t stc_iic_port_dir_init;

//		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
			stc_iic_port_dir_init.enDir = GpioDirIn;
			stc_iic_port_dir_init.enDrv = GpioDrvL;
			stc_iic_port_dir_init.enPu = GpioPuDisable;
			stc_iic_port_dir_init.enPd = GpioPdDisable;
//			stc_iic_port_dir_init.enOD = GpioOdEnable;

		Gpio_Init(IIC_SDA_PORT, IIC_SDA_PIN, &stc_iic_port_dir_init);
} 




// I2C 开始
void IIC_Start(void)
{
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    IIC_Delay_4us();
    IIC_SDA_LOW();
    IIC_Delay_4us();
    IIC_SCL_LOW();
    IIC_Delay_4us();
}

// I2C 停止
void IIC_Stop(void)
{
    IIC_SDA_LOW();
    IIC_SCL_HIGH();
    IIC_Delay_4us();
    IIC_SDA_HIGH();
    IIC_Delay_4us();
}

// I2C 等待响应
uint8_t IIC_Wait(void)
{
    uint32_t timeout = 0;

    IIC_SDA_HIGH();
    IIC_Delay_4us();
    IIC_SCL_HIGH();
    IIC_Delay_4us();

    IIC_SDA_IN();
    while(IIC_READ_SDA())
    {
        timeout++;
        if(timeout > 2000)
        {
            return 0;
        }
    }
    IIC_SDA_OUT();

    IIC_SCL_LOW();
    IIC_Delay_4us();
    return 1;
}

// I2C 响应
void IIC_Ack(void)
{
    IIC_SDA_LOW();
    IIC_Delay_4us();
    IIC_SCL_HIGH();
    IIC_Delay_4us();
    IIC_SCL_LOW();
    IIC_Delay_4us();
    IIC_SDA_HIGH();
}

// I2C 不响应
void IIC_Nack(void)
{
    IIC_SDA_HIGH();
    IIC_Delay_4us();
    IIC_SCL_HIGH();
    IIC_Delay_4us();
    IIC_SCL_LOW();
    IIC_Delay_4us();
}


// I2C 发送一个字节数据
void IIC_Send_Byte(uint8_t data)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        if(data & 0x80)
        {
            IIC_SDA_HIGH();
        }
        else
        {
            IIC_SDA_LOW();
        }

        IIC_Delay_4us();
        IIC_SCL_HIGH();
        IIC_Delay_4us();
        IIC_SCL_LOW();
        
        if(i == 7)
        {
            IIC_SDA_HIGH();
        }
        data <<= 1;
        IIC_Delay_4us();
    }
}

// I2C 读一个字节数据
uint8_t IIC_Read_Byte(void)
{
    uint8_t i, data = 0;

    for(i = 0; i < 8; i++ )
    {
        data <<= 1;
        IIC_SCL_HIGH();
        IIC_Delay_4us();

        IIC_SDA_IN();
        if(IIC_READ_SDA())
        {
            data++;
        }
        IIC_SDA_OUT();

        IIC_SCL_LOW();
        IIC_Delay_4us();
    }

    return data;
}



void IIC_Port_Init(void)
{
		stc_gpio_cfg_t stc_iic_port_init;
		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
			stc_iic_port_init.enDir = GpioDirOut;
			stc_iic_port_init.enDrv = GpioDrvL;
			stc_iic_port_init.enPu = GpioPuDisable;
			stc_iic_port_init.enPd = GpioPdDisable;
//			stc_iic_port_init.enOD = GpioOdEnable;

		Gpio_Init(IIC_SDA_PORT, IIC_SDA_PIN, &stc_iic_port_init);	// 初始化SDA引脚
		Gpio_Init(IIC_SCL_PORT, IIC_SCL_PIN, &stc_iic_port_init);	// 初始化SCL引脚
}
