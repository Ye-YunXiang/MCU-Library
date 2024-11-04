/*
 * This file is part of the MCU-Library.
 *
 * MIT License
 *
 * Copyright (c) 2024, YeYunXiang, <poetrycloud@foxmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// Encoding:UTF-8

#include <stdio.h>

#include "spi_hal.h"
#include "main_file_define.h"

// 定义 ---------------------------------------
#define SET_IO(a,b)   Gpio_SetIO(a,b)
#define CLR_IO(a,b)   Gpio_ClrIO(a,b)
#define GET_IO(a,b)   Gpio_GetInputIO(a,b)

#define SPI_DELAY_1US(x)   user_delay_1us(x)
#define SPI_DELAY_1MS(x)   user_delay_1ms(x)

typedef struct
{
    spi_object_t *device;

    en_gpio_port_t sck_port;
    en_gpio_pin_t sck_pin;
    en_gpio_port_t mosi_port;
    en_gpio_pin_t mosi_pin;
    en_gpio_port_t miso_port;
    en_gpio_pin_t miso_pin;
    en_gpio_port_t cs_port;
    en_gpio_pin_t cs_pin;
}user_data_t;

// 初始化SPI对象--------------------------------
static spi_object_t rc522_spi;

// 初始化用户数据
static user_data_t rc522_data = {
    &rc522_spi,
    GpioPortB, GpioPin3,
    GpioPortB, GpioPin4,
    GpioPortB, GpioPin5,
    GpioPortA, GpioPin15,
};

// 初始化接口----------------------------------
static void _init(spi_object_t *self);
static SPI_RETURN_STATE_T _write(spi_ops_param_t *spi_param);
static SPI_RETURN_STATE_T _read(spi_ops_param_t *spi_param);
static void _close(spi_object_t *self);

static spi_ops_t rc522_spi_ops = {
    .init = _init,
    .write = _write,
    .read = _read,
    .close = _close,
};

// 创建使用函数----------------------------------
void spi_driver_init(spi_object_t *self, SPI_DRIVER_COUNT_T driver)
{
    if(self->spi_state != ENUM_SPI_NO_INIT)
    {
        return;
    }

    switch (driver)
    {
        case ENUM_SPI_DEIVER_RC522:
            self->ops = &rc522_spi_ops;
            self->uset_data = &rc522_data;
            self->spi_state = ENUM_SPI_NO_INIT;
            break;
        
        default:
            break;
    }
}

// 本类内部的工具-------------------------------------
static uint8_t _spi_read_write_one_byte(user_data_t *input_user, uint8_t tx_data)
{			
    uint8_t rx_data = 0;

    for(uint8_t i = 0; i < 8; i++)
    {
        CLR_IO(input_user->sck_port , input_user->sck_pin);

        SPI_DELAY_1US(1); 

        if(tx_data & 0x80)
        {
            SET_IO(input_user->mosi_port, input_user->mosi_pin);
        }
        else 
        {
            CLR_IO(input_user->mosi_port, input_user->mosi_pin);
        }

        tx_data <<= 1;

        SPI_DELAY_1US(1); 

        SET_IO(input_user->sck_port, input_user->sck_pin);

        SPI_DELAY_1US(1); 

        rx_data <<= 1;

        if(GET_IO(input_user->miso_port, input_user->miso_pin))
        {
            rx_data |= 0x01;
        }

        SPI_DELAY_1US(1); 
    }
    return rx_data; 
}

// 外部 OPS 的接口----------------------------------------
static void _init(spi_object_t *self)
{
    user_data_t *input_user = self->uset_data;
    
    // 这里是软件IIC ----------------------------
    stc_gpio_cfg_t 	stc_gpio_init;
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
	stc_gpio_init.enDir		= GpioDirOut;
	stc_gpio_init.enDrv		= GpioDrvL;
	stc_gpio_init.enPu 		= GpioPuDisable;
	stc_gpio_init.enPd 		= GpioPdDisable;
	stc_gpio_init.enCtrlMode    = GpioAHB;
	
	Gpio_Init(input_user->cs_port, input_user->cs_pin, &stc_gpio_init);
    Gpio_Init(input_user->sck_port, input_user->sck_pin, &stc_gpio_init);
    Gpio_Init(input_user->mosi_port, input_user->mosi_pin, &stc_gpio_init);

    stc_gpio_init.enDir		= GpioDirIn;
    stc_gpio_init.enPd 		= GpioPdEnable;

    Gpio_Init(input_user->miso_port, input_user->miso_pin, &stc_gpio_init);

    SET_IO(input_user->mosi_port, input_user->mosi_pin);
    SET_IO(input_user->cs_port, input_user->cs_pin);
    SET_IO(input_user->sck_port, input_user->sck_pin);

    self->spi_state = ENUM_SPI_OPEN;
}

static void _close(spi_object_t *self)
{
    user_data_t *input_user = self->uset_data;
    
    // 这里是软件IIC ----------------------------
    stc_gpio_cfg_t 	stc_gpio_init;
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
	stc_gpio_init.enDir		= GpioDirOut;
	stc_gpio_init.enDrv		= GpioDrvL;
	stc_gpio_init.enPu 		= GpioPuDisable;
	stc_gpio_init.enPd 		= GpioPdDisable;
	stc_gpio_init.enCtrlMode    = GpioAHB;
	
	Gpio_Init(input_user->cs_port, input_user->cs_pin, &stc_gpio_init);
    Gpio_Init(input_user->sck_port, input_user->sck_pin, &stc_gpio_init);
    Gpio_Init(input_user->mosi_port, input_user->mosi_pin, &stc_gpio_init);
    Gpio_Init(input_user->miso_port, input_user->miso_pin, &stc_gpio_init);

    CLR_IO(input_user->cs_port, input_user->cs_pin);
    CLR_IO(input_user->sck_port, input_user->sck_pin);
    CLR_IO(input_user->mosi_port, input_user->mosi_pin);
    CLR_IO(input_user->miso_port, input_user->miso_pin);

    self->spi_state = ENUM_SPI_CLOSE;
}

static SPI_RETURN_STATE_T _write(spi_ops_param_t *spi_param)
{
    if(spi_param->self->spi_state != ENUM_SPI_OPEN)
    {
        return ENUM_SPI_SEND_FILL;
    }
    
    uint8_t i = 0;
    uint8_t addr = 0;
    user_data_t *input_user = spi_param->self->uset_data;

    CLR_IO(input_user->cs_port, input_user->cs_pin);
    addr = ((spi_param->address << 1) & 0x7E); 
    _spi_read_write_one_byte(input_user, addr); // 地址

    for (i = 0; i < spi_param->length; i++)  // 发送数据
    {
        _spi_read_write_one_byte(input_user, spi_param->data[i]);
    }

    SET_IO(input_user->cs_port, input_user->cs_pin);

    return ENUM_SPI_SEND_SUCCEED;
}

static SPI_RETURN_STATE_T _read(spi_ops_param_t *spi_param)
{
    if(spi_param->self->spi_state != ENUM_SPI_OPEN)
    {
        return ENUM_SPI_SEND_FILL;
    }
    
    uint8_t i = 0;
    uint8_t addr = 0;
    user_data_t *input_user = spi_param->self->uset_data;
	
	CLR_IO(input_user->cs_port, input_user->cs_pin);
    addr = ((spi_param->address << 1) & 0x7E) | 0x80;
    _spi_read_write_one_byte(input_user, addr);

    for (i = 0; i < spi_param->length; i++)  // 接收数据
    {
        spi_param->data[i] = _spi_read_write_one_byte(input_user, 0);
    }

    SET_IO(input_user->cs_port, input_user->cs_pin);
    
    return ENUM_SPI_SEND_SUCCEED;
}
