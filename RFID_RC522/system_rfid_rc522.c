// Encoding UTF-8

#include "system_rfid_rc522.h"


// XXX: 延时接口--------------------------------------------------------
/*
延时函数，微秒级
*/
static void _rc522_delay_us(uint32_t us)
{
    delay1us(us);
}

static void _rc522_delay_ms(uint32_t ms)
{
    delay1ms(ms);
}


// XXX: 与RC522底层SPI通讯接口---------------------------------------

/*
函数功能：移植接口--SPI时序读写一个字节
函数参数：data:要写入的数据
返 回 值：读到的数据
*/
static uint8_t _rc522_spi_read_write_one_byte(uint8_t tx_data)
{			  	 
    uint8_t rx_data = 0;				 
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        RC522_SCK_0(); 

        _rc522_delay_us(1); 

        if(tx_data & 0x80)
        {
            RC522_MOSI_1();
        }
        else 
        {
            RC522_MOSI_0();
        }

        tx_data <<= 1;

        _rc522_delay_us(1); 

        RC522_SCK_1();

        _rc522_delay_us(1); 

        rx_data <<= 1;

        if(RC522_MISO())
        {
            rx_data |= 0x01;
        }

        _rc522_delay_us(1); 
    }
    return rx_data; 
}


/*
功    能：读RC632寄存器
参数说明：Address[IN]:寄存器地址
返    回：读出的值
*/
static uint8_t _rc522_read_raw_rc(uint8_t address)
{
    uint8_t addr;
    uint8_t result = 0;		
	RC522_CS_0();						//片选选中RC522
    addr = ((address << 1) & 0x7E) | 0x80;
    _rc522_spi_read_write_one_byte(addr);		  //发送命令
    result = _rc522_spi_read_write_one_byte(0); //读取RC522返回的数据
    RC522_CS_1();						   //释放片选线(PF0)
    return result;         //返回读到的数据
}
 
 
/*
功    能：写RC632寄存器
参数说明：Address[IN]:寄存器地址
          value[IN] :写入的值
*/
static void _rc522_write_raw_rc(uint8_t address,uint8_t value)
{  
    uint8_t addr;
    RC522_CS_0(); //PF0写 0 (SDA)(SPI1片选线，低电平有效)
    addr = ((address << 1) & 0x7E); 
    _rc522_spi_read_write_one_byte(addr); //SPI1发送一个字节
    _rc522_spi_read_write_one_byte(value);  //SPI1发送一个字节
    RC522_CS_1(); //PF1写1(SDA)(SPI1片选线)
}
 
 
/*
功    能：置RC522寄存器位
参数说明：reg[IN]:寄存器地址
          mask[IN]:置位值
*/
static void _rc522_set_bit_mask(uint8_t reg,uint8_t mask)  
{
    uint8_t tmp = 0x0;
    tmp = _rc522_read_raw_rc(reg);		    //读RC632寄存器
    _rc522_write_raw_rc(reg, tmp | mask);   //写RC632寄存器
}
 
 
/*
功    能：清RC522寄存器位
参数说明：reg[IN]:寄存器地址
         mask[IN]:清位值
*/
static void _rc522_clear_bit_mask(uint8_t reg,uint8_t mask)  
{
    char tmp=0x0;
    tmp=_rc522_read_raw_rc(reg);        //读RC632寄存器
    _rc522_write_raw_rc(reg,tmp&~mask); // clear bit mask
}



// XXX: RC522芯片功能通讯接口---------------------------------------

/*
功    能：用MF522计算CRC16函数
参    数：
				*pIn ：要读数CRC的数据
				len：-数据长度
				*pOut：计算的CRC结果
*/
void _rc522_calulate_crc(uint8_t *input , uint8_t len, uint8_t *out_data )
{
    uint8_t i, n;
    _rc522_clear_bit_mask(DivIrqReg, 0x04);  //CRCIrq = 0  
    _rc522_write_raw_rc(commandReg, PCD_IDLE);
    _rc522_set_bit_mask(FIFOLevelReg, 0x80); //清FIFO指针
    
	//向FIFO中写入数据  
	for(i = 0; i < len; i++)
    {  
		_rc522_write_raw_rc(FIFODataReg, *(input + i));  //开始RCR计算
	}
		
		_rc522_write_raw_rc(commandReg, PCD_CALCCRC);   //等待CRC计算完成 
		i = 0xFF;
    do 
    {
        n = _rc522_read_raw_rc(DivIrqReg);
        i--;
    }
    while((i != 0) && !(n & 0x04));//CRCIrq = 1
	  
	//读取CRC计算结果 
	out_data[0] = _rc522_read_raw_rc(CRCResultRegL);
    out_data[1] = _rc522_read_raw_rc(CRCResultRegM);
}


 
/*
函数功能：设置RC522的工作方式 
*/
static uint8_t _rc522_config_iso_type(uint8_t type)
{
    if(type == 'A')                                //ISO14443_A
    { 
        _rc522_clear_bit_mask(Status2Reg, 0x08);     //清RC522寄存器位
        _rc522_write_raw_rc(ModeReg, 0x3D);          //3F//CRC初始值0x6363
        _rc522_write_raw_rc(RxSelReg, 0x86);         //84
        _rc522_write_raw_rc(RFCfgReg, 0x7F);         //4F  //调整卡的感应距离//RxGain = 48dB调节卡感应距离  
        _rc522_write_raw_rc(TReloadRegL, 30);        //tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
        _rc522_write_raw_rc(TReloadRegH, 0);
        _rc522_write_raw_rc(TModeReg, 0x8D);
        _rc522_write_raw_rc(TPrescalerReg, 0x3E);
        _rc522_delay_us(2);
        system_rc522_pcd_antenna_on();		            //开启天线 
    }
    else 
    {
        // 失败
        return 1;
    }

    //成功返回0
    return MI_OK;
}


/*
功    能：软件复位RC522
返    回：成功返回MI_OK
*/
uint8_t _rc522_pcd_reset(void)
{
    RC522_RST_1();
    _rc522_delay_us(1);
    RC522_RST_0();
    _rc522_delay_us(1);
    RC522_RST_1();
    _rc522_delay_us(1);

    _rc522_write_raw_rc(commandReg, PCD_RESETPHASE);    //写RC632寄存器，复位
    _rc522_write_raw_rc(commandReg, PCD_RESETPHASE);	//写RC632寄存器，复位
    _rc522_delay_us(1);

    _rc522_write_raw_rc(ModeReg, 0x3D);                 //和Mifare卡通讯，CRC初始值0x6363
    _rc522_write_raw_rc(TReloadRegL, 30);               //写RC632寄存器   
    _rc522_write_raw_rc(TReloadRegH, 0);
    _rc522_write_raw_rc(TModeReg, 0x8D);
    _rc522_write_raw_rc(TPrescalerReg, 0x3E);

    _rc522_write_raw_rc(TxAutoReg, 0x40);             //必须要

    return MI_OK;
}


/*
功    能：通过RC522和ISO14443卡通讯
参数说明：command[IN]:RC522命令字
          input [IN]:通过RC522发送到卡片的数据
          input_len_byte[IN]:发送数据的字节长度
          out_data [OUT]:接收到的卡片返回数据
          *out_data_len_bit[OUT]:返回数据的位长度
*/
static uint8_t _rc522_pcd_com_rc522(
    uint8_t command, 
    uint8_t *input,
    uint8_t input_len_byte,
    uint8_t *out_data,
    uint8_t *out_data_len_bit)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitFor = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;
	
    switch(command)
    {
        case PCD_AUTHENT:    //验证密钥
            irqEn = 0x12;
            waitFor = 0x10;
            break;

        case PCD_TRANSCEIVE: //发送并接收数据
            irqEn = 0x77;
            waitFor = 0x30;
            break;

        default:
            break;
    }

    _rc522_write_raw_rc(ComIEnReg, irqEn | 0x80);	
    _rc522_clear_bit_mask(ComIrqReg, 0x80);    //清所有中断位
    _rc522_write_raw_rc(commandReg, PCD_IDLE);	
    _rc522_set_bit_mask(FIFOLevelReg, 0x80);	//清FIFO缓存
    
    for(i = 0; i < input_len_byte; i++)
    {   
        _rc522_write_raw_rc(FIFODataReg, input[i]);
    }
		
    _rc522_write_raw_rc(commandReg, command);	 

    if(command == PCD_TRANSCEIVE)
    {  
        _rc522_set_bit_mask(BitFramingReg, 0x80);	 //开始传送
    }
    
	//有问题，下面的循环
    //操作M1卡最大等待时间25ms
	i = 200;
    do 
    {
        n=_rc522_read_raw_rc(ComIrqReg);
        i--;
        _rc522_delay_us(100);
    }
    while((i != 0) && !(n & 0x01) && !(n & waitFor));
				
    _rc522_clear_bit_mask(BitFramingReg, 0x80);
    if(i != 0)
    {    
        if(!(_rc522_read_raw_rc(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if ( n & irqEn & 0x01 )
            {
                status = MI_NOTAGERR;
            }
            if(command == PCD_TRANSCEIVE)
            {
               	n = _rc522_read_raw_rc(FIFOLevelReg);

              	lastBits = _rc522_read_raw_rc(ControlReg) & 0x07;
                
                if(lastBits)
                {
                    *out_data_len_bit = (n - 1) * 8 + lastBits;
                }
                else
                {   
                    *out_data_len_bit = n * 8;   
                }
								
                if(n == 0)
                {
                    n = 1;
                }

                if(n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for(i = 0; i < n; i++)
                {   
                    out_data[i] = _rc522_read_raw_rc(FIFODataReg);    
                }
            }
        }
        else
        {   
            status = MI_ERR;   
        }
    }

    _rc522_set_bit_mask(ControlReg, 0x80);// stop timer now
    _rc522_write_raw_rc(commandReg, PCD_IDLE); 

    return status;
}


// XXX: 实际使用外部接口----------------------------------------------------
 
 
/*
函数功能：初始化RC522的IO口	 
*/
void system_rc522_io_init(void)
{
    stc_gpio_cfg_t 	stc_gpio_init;
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	
	stc_gpio_init.enDir		= GpioDirOut;
	stc_gpio_init.enDrv		= GpioDrvL;
	stc_gpio_init.enPu 		= GpioPuDisable;
	stc_gpio_init.enPd 		= GpioPdDisable;
	stc_gpio_init.enCtrlMode    = GpioAHB;
	
	Gpio_Init(RC522_SDA_PORT, RC522_SDA_PIN, &stc_gpio_init);
    Gpio_Init(RC522_SCK_PORT, RC522_SCK_PIN, &stc_gpio_init);
    Gpio_Init(RC522_MOSI_PORT, RC522_MOSI_PIN, &stc_gpio_init);
    Gpio_Init(RC522_RST_PORT, RC522_RST_PIN, &stc_gpio_init);

    stc_gpio_init.enDir		= GpioDirIn;
    stc_gpio_init.enPd 		= GpioPdEnable;

    Gpio_Init(RC522_IRQ_PORT, RC522_IRQ_PIN, &stc_gpio_init);
    Gpio_Init(RC522_MISO_PORT, RC522_MISO_PIN, &stc_gpio_init);

    RC522_MOSI_1();
    RC522_CS_1();
    RC522_SCK_1();
    RC522_RST_1();
}	


/*
函数功能：RC522芯片初始化
*/
void system_rc522_init(void)
{
  system_rc522_io_init();	    //RC522初始化

  _rc522_pcd_reset();  			//复位RC522 

  system_rc522_pcd_antenna_off();	    //关闭天线

  _rc522_delay_ms(2);  		    //延时2毫秒

  system_rc522_pcd_antenna_on();		    //开启天线

  _rc522_config_iso_type('A');    //设置RC632的工作方式
}
 
 
/*
函数功能：复位RC522
*/
void system_rc522_reset(void)
{
  _rc522_pcd_reset();	            //复位RC522

  system_rc522_pcd_antenna_off();	    //关闭天线

  _rc522_delay_ms(2);  		    //延时2毫秒

  system_rc522_pcd_antenna_on();		    //开启天线  	
}    


 /*
功能描述:选卡读取卡存储器容量
输入参数:ser_num 传入卡序列号
返 回 值:成功返回卡容量
*/
uint8_t system_rc522_rc522_select_tag(uint8_t *ser_num) //读取卡存储器容量
{     
	uint8_t i;     
	uint8_t status;     
	uint8_t size;     
	uint8_t recvBits;     
	uint8_t buffer[9];
	     
	buffer[0] = PICC_ANTICOLL1;	  //防撞码1     
	buffer[1] = 0x70;
	buffer[6] = 0x00;						     
	for(i = 0; i < 4; i++)					
	{
		buffer[i + 2] =* (ser_num + i);	    //buffer[2]-buffer[5]为卡序列号
		buffer[6] ^= *(ser_num + i);    //卡校验码
	}
	
	_rc522_calulate_crc(buffer, 7, &buffer[7]);	//buffer[7]-buffer[8]为RCR校验码
	_rc522_clear_bit_mask(Status2Reg, 0x08);
	status = _rc522_pcd_com_rc522(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	
	if((status == MI_OK) && (recvBits == 0x18))    
	{
    	size = buffer[0];     
    }
	else    
	{
        size = 0;
    }
	
	return size; 
} 


/*
功    能: 寻卡
参数说明: req_code[IN]:寻卡方式
                0x52   = 寻感应区内所有符合14443A标准的卡
                0x26   = 寻未进入休眠状态的卡
          			tag_type[OUT]:卡片类型代码
                0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
返 回 值: 成功返回MI_OK
*/
uint8_t system_rc522_pcd_request(uint8_t req_code, uint8_t *tag_type)
{
	char status;  
	uint8_t len;
	uint8_t com_rf522_buf[MAXRLEN];  	   // MAXRLEN  18
 
	_rc522_clear_bit_mask(Status2Reg, 0x08);	//清RC522寄存器位,/接收数据命令
	_rc522_write_raw_rc(BitFramingReg, 0x07); //写RC632寄存器
	_rc522_set_bit_mask(TxControlReg, 0x03);  //置RC522寄存器位
 
	com_rf522_buf[0] = req_code; 	    //寻卡方式
	//通过RC522和ISO14443卡通讯
	status = _rc522_pcd_com_rc522(
        PCD_TRANSCEIVE, com_rf522_buf, 1, com_rf522_buf, &len);
	
	if((status == MI_OK) && (len == 0x10))
	{    
		*tag_type = com_rf522_buf[0];
		*(tag_type + 1) = com_rf522_buf[1];
	}
	else
	{
	  status = MI_ERR;
	}  
	return status;
}

 
/*
功    能: 防冲撞
参数说明: snr[OUT]:卡片序列号，4字节
返    回: 成功返回MI_OK
*/
uint8_t system_rc522_pcd_anticoll(uint8_t *snr)
{
    char status;
    uint8_t i = 0;
    uint8_t snr_check = 0;
    uint8_t len;
    uint8_t com_rf522_buf[MAXRLEN]; 
    
    _rc522_clear_bit_mask(Status2Reg, 0x08);  //清RC522寄存器位 
    _rc522_write_raw_rc(BitFramingReg, 0x00); //写
    _rc522_clear_bit_mask(CollReg, 0x80);     //清
 
    com_rf522_buf[0] = PICC_ANTICOLL1;   //PICC_ANTICOLL1 = 0x93
    com_rf522_buf[1] = 0x20;
	
    //0x0c,通过RC522和ISO14443卡通讯
    status = _rc522_pcd_com_rc522(
        PCD_TRANSCEIVE,     //PCD_TRANSCEIVE =发送并接收数据
        com_rf522_buf,      
        2,                  //2：写入卡里的数据字节长度
        com_rf522_buf,      //com_rf522_buf:存放数据的地址
        &len);            //len：从卡里读出的数据长度							 
											 
    if(status == MI_OK)
    {
    	for(i = 0; i < 4; i++)
        {   
            *(snr + i) = com_rf522_buf[i];  //把读到的卡号赋值给snr
            snr_check ^= com_rf522_buf[i];
        }
        if(snr_check != com_rf522_buf[i])
        {
            status = MI_ERR;
        }
    }   
    _rc522_set_bit_mask(CollReg, 0x80);
    return status;
}
 
 
/*
功    能：选定卡片
参数说明：snr[IN]:卡片序列号，4字节
返    回：成功返回MI_OK
*/
uint8_t system_rc522_pcd_select(uint8_t *snr)
{
    uint8_t status;
    uint8_t i;
    uint8_t len;
    uint8_t com_rf522_buf[MAXRLEN]; 
    
    com_rf522_buf[0] = PICC_ANTICOLL1;
    com_rf522_buf[1] = 0x70;
    com_rf522_buf[6] = 0;
	
    for(i = 0; i < 4; i++)
    {
    	com_rf522_buf[i + 2] = *(snr + i);
    	com_rf522_buf[6] ^= *(snr + i);
    }
    //用MF522计算CRC16函数，校验数据
    _rc522_calulate_crc(com_rf522_buf, 7, &com_rf522_buf[7]); 
    //清RC522寄存器位
    _rc522_clear_bit_mask(Status2Reg, 0x08);
    status = _rc522_pcd_com_rc522(
        PCD_TRANSCEIVE, com_rf522_buf, 9, com_rf522_buf, &len);
    if((status == MI_OK) && (len == 0x18))
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }
		
    return status;
}
 
 
/*
功    能：验证卡片密码
参数说明：auth_mode[IN]: 密码验证模式
                 0x60 = 验证A密钥
                 0x61 = 验证B密钥 
          addr[IN]：块地址
          pKey[IN]：扇区密码
          snr[IN]：卡片序列号，4字节
返    回：成功返回MI_OK
*/               
uint8_t system_rc522_pcd_auth_state(
    uint8_t auth_mode,uint8_t addr,uint8_t *key,uint8_t *snr)
{
    uint8_t status;
    uint8_t len;
    uint8_t com_rf522_buf[MAXRLEN];  //MAXRLEN  18(数组的大小)
	  
	//验证模式+块地址+扇区密码+卡序列号   
    com_rf522_buf[0] = auth_mode;		
    com_rf522_buf[1] = addr;				
    memcpy(&com_rf522_buf[2], key, 6); //拷贝，复制
    memcpy(&com_rf522_buf[8], snr, 4); 
	 
    status = _rc522_pcd_com_rc522(
        PCD_AUTHENT, com_rf522_buf, 12, com_rf522_buf, &len);
    if((status != MI_OK) || (!(_rc522_read_raw_rc(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}
 
 
/*
功    能：读取M1卡一块数据
参数说明： 
					addr：块地址
          p   ：读出的块数据，16字节
返    回：成功返回MI_OK
*/ 
uint8_t system_rc522_pcd_read(uint8_t addr, uint8_t *p)
{
    char status;
    uint8_t len;
    uint8_t i, com_rf522_buf[MAXRLEN]; //18
 
    com_rf522_buf[0] = PICC_READ;
    com_rf522_buf[1] = addr;
    _rc522_calulate_crc(com_rf522_buf, 2, &com_rf522_buf[2]);
    //通过RC522和ISO14443卡通讯
    status = _rc522_pcd_com_rc522(
        PCD_TRANSCEIVE, com_rf522_buf, 4, com_rf522_buf,&len);

    if((status == MI_OK && (len == 0x90)))
    {
        for(i = 0; i < 16; i++)
        {
            *(p + i) = com_rf522_buf[i];
        }
    }
    else
    {   
        status = MI_ERR;
    }

    return status;
}
 
 
/*
功    能：写数据到M1卡指定块
参数说明：addr：块地址
          p   ：向块写入的数据，16字节
返    回：成功返回MI_OK
*/                  
uint8_t system_rc522_pcd_write(uint8_t addr,uint8_t *p)
{
    char status;
    uint8_t len;
    uint8_t i,com_rf522_buf[MAXRLEN]; 
    
    com_rf522_buf[0] = PICC_WRITE;// 0xA0 //写块
    com_rf522_buf[1] = addr;      //块地址
    _rc522_calulate_crc(com_rf522_buf, 2, &com_rf522_buf[2]);
 
    status = _rc522_pcd_com_rc522(
        PCD_TRANSCEIVE, com_rf522_buf, 4, com_rf522_buf, &len);
 
    if((status != MI_OK) || (len != 4) || ((com_rf522_buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }
		
    if(status == MI_OK)
    {
        for(i = 0; i < 16; i++)//向FIFO写16Byte数据 
        {    
        	com_rf522_buf[i] = *(p + i);   
        }
        _rc522_calulate_crc(com_rf522_buf, 16, &com_rf522_buf[16]);
        status = _rc522_pcd_com_rc522(
            PCD_TRANSCEIVE, com_rf522_buf, 18, com_rf522_buf, &len);
        if((status != MI_OK) || (len != 4) || ((com_rf522_buf[0] & 0x0F) != 0x0A))
        {   
            status = MI_ERR;   
        }
    }
    return status;
}
 
 
/*
功    能：命令卡片进入休眠状态
返    回：成功返回MI_OK
*/
uint8_t system_rc522_pcd_halt(void)
{
    uint8_t status;
    uint8_t len;
    uint8_t com_rf522_buf[MAXRLEN]; //MAXRLEN==18

    com_rf522_buf[0] = PICC_HALT;
    com_rf522_buf[1] = 0;
    _rc522_calulate_crc(com_rf522_buf, 2, &com_rf522_buf[2]);
    status = _rc522_pcd_com_rc522(
        PCD_TRANSCEIVE, com_rf522_buf, 4, com_rf522_buf, &len);

    // return MI_OK;
    return status;
}

 
/*
函数功能：开启天线  
参    数：每次启动或关闭天险发射之间应至少有1ms的间隔
*/
void system_rc522_pcd_antenna_on(void)
{
    uint8_t i;
    i = _rc522_read_raw_rc(TxControlReg);
    if(!(i & 0x03))
    {
        _rc522_set_bit_mask(TxControlReg, 0x03);
    }
}
 
 
/*
函数功能：关闭天线  
参    数：每次启动或关闭天险发射之间应至少有1ms的间隔
*/
void system_rc522_pcd_antenna_off(void)
{
	_rc522_clear_bit_mask(TxControlReg, 0x03); //清RC522寄存器位
}
 

