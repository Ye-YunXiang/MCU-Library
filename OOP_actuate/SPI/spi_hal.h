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

// 这里只是定义了协议接口，具体内容请自己实现

#ifndef __SPI_HAL_H_
#define __SPI_HAL_H_

// 目前有的驱动------------------------------
typedef enum
{
    ENUM_SPI_DEIVER_RC522,
}SPI_DRIVER_COUNT_T;

// 驱动返参定义------------------------------
typedef enum
{
    ENUM_SPI_OVER_TIME, // 发送超时
    ENUM_SPI_SUCCEED,   // 发送完成
}SPI_RETURN_STATE_T;

// 驱动状态定义------------------------------
typedef enum
{
    ENUM_SPI_NO_INIT,   // 没有初始化
    ENUM_SPI_OPEN,      // 驱动打开
    ENUM_SPI_CLOSE,     // 驱动关闭
}SPI_RUN_STATE_T;

// 驱动对象定义------------------------------
typedef struct spi_object
{
	struct spi_ops *ops;        // 接口
	void *uset_data;            // 用户数据
    SPI_RUN_STATE_T spi_state;  // 对象状态
}spi_object_t;

// 驱动传参结构体------------------------------
typedef struct spi_ops_param
{
    struct spi_object *self;   // 对象结构体
    unsigned char *data;      // R/W 数据指针
    unsigned char address;    // 地址
    unsigned char length;     // 数据长度
}spi_ops_param_t;

// 抽象驱动的调用接口------------------------------
typedef void (*init_fn_t)(spi_object_t *self);
typedef SPI_RETURN_STATE_T (*write_fn_t)(spi_ops_param_t *spi_param);
typedef SPI_RETURN_STATE_T (*read_fn_t)(spi_ops_param_t *spi_param);
typedef void (*close_fn_t)(spi_object_t *self);

typedef struct spi_ops
{
    init_fn_t init;
    write_fn_t write;
    read_fn_t read;
    close_fn_t close;
}spi_ops_t;

// 抽象驱动的外部调用内联接口------------------------------

static inline void spi_hal_init(spi_object_t *self)
{
    return (*(spi_ops_t **)self)->init(self);
}

static inline SPI_RETURN_STATE_T spi_hal_write(spi_ops_param_t *spi_param)
{
    return (*(spi_ops_t **)spi_param->self)->write(spi_param);
}

static inline SPI_RETURN_STATE_T spi_hal_read(spi_ops_param_t *spi_param)
{
    return (*(spi_ops_t **)spi_param->self)->read(spi_param);
}

static inline void spi_hal_close(spi_object_t *self)
{
    return (*(spi_ops_t **)self)->close(self);
}

// 本抽象驱动的注册和初始化------------------------------
void spi_driver_init(spi_object_t *self, SPI_DRIVER_COUNT_T driver);    

#endif  //__SPI_HAL_H_
