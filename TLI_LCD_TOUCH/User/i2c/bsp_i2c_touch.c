/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   电容触摸屏的专用iic驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "i2c/bsp_i2c_touch.h"
#include "lcd/bsp_lcd_gt1151qm.h"
#include "usart/bsp_usart.h"
#include "dwt/bsp_dwt_delay.h"   

/**
 * @brief  CPU发起I2C总线开始信号
 * @param  无
 * @retval 无
 */
void I2C_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	TP_I2C_SDA_1();
	TP_I2C_SCL_1();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SDA_0();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SCL_0();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
}

/**
 * @brief  CPU发起I2C总线停止信号
 * @param  无
 * @retval 无
 */
void I2C_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	TP_I2C_SDA_0();
	TP_I2C_SCL_1();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SDA_1();
}

/**
  * @brief  配置触摸屏检测I2C总线的GPIO，采用模拟IO的方式实现
  * @param  无
  * @retval 无
  */
static void GTP_GPIO_Config(void)
{
    /*开启TP相关的GPIO外设时钟*/
    rcu_periph_clock_enable(TP_RCU_I2C_SCL_PORT);	/* 打开GPIO时钟 */
	rcu_periph_clock_enable(TP_RCU_I2C_SDA_PORT);	/* 打开GPIO时钟 */
    rcu_periph_clock_enable(TP_RCU_RST_PORT);	/* 打开GPIO时钟 */
    rcu_periph_clock_enable(TP_RCU_INT_PORT);	/* 打开GPIO时钟 */
    
    /*选择要控制的GPIO引脚、设置GPIO模式为通用开漏输出、设置GPIO速率为200MHz*/
    gpio_mode_set(TP_I2C_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, TP_I2C_SCL_GPIO_PIN);
    gpio_output_options_set(TP_I2C_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_100_220MHZ, TP_I2C_SCL_GPIO_PIN);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为通用推挽输出、设置GPIO速率为200MHz*/
    gpio_mode_set(TP_I2C_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, TP_I2C_SDA_GPIO_PIN);
    gpio_output_options_set(TP_I2C_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_100_220MHZ, TP_I2C_SDA_GPIO_PIN);
    
    //设置为下拉，方便初始化
    /*选择要控制的GPIO引脚、设置GPIO模式为通用开漏输出、设置GPIO速率为200MHz*/
    gpio_mode_set(TP_RST_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, TP_RST_GPIO_PIN);
    gpio_output_options_set(TP_RST_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_RST_GPIO_PIN);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为通用推挽输出、设置GPIO速率为200MHz*/
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, TP_INT_GPIO_PIN);
    gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_INT_GPIO_PIN);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	I2C_Stop();
}


/**
  * @brief  对GT芯片进行复位
  * @param  无
  * @retval 无
  */
void GPT_I2C_ResetChip(void)
{
    /*选择要控制的GPIO引脚、设置GPIO模式为通用推挽输出、设置GPIO速率为200MHz*/
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, TP_INT_GPIO_PIN);
    gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_INT_GPIO_PIN);

    /*初始化GT,rst为高电平，int为低电平，则gt的设备地址被配置为GTP_ADDRESS*/

    /*复位为低电平，为初始化做准备*/
    gpio_bit_reset(TP_RST_GPIO_PORT,TP_RST_GPIO_PIN);
    CPU_TS_Tmr_Delay_US(1000);

    /*拉高一段时间，进行初始化*/
    gpio_bit_set(TP_RST_GPIO_PORT,TP_RST_GPIO_PIN);
    CPU_TS_Tmr_Delay_US(1000);

    /*把INT引脚设置为浮空输入模式*/
    /*!< Configure INT */
    /*选择要控制的GPIO引脚、设置GPIO模式为通用推挽输出、设置GPIO速率为200MHz*/
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TP_INT_GPIO_PIN);
    gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_INT_GPIO_PIN);

}

/**
  * @brief  I2C 外设(GT)初始化
  * @param  无
  * @retval 无
  */
void GPT_I2C_Touch_Init(void)
{
  GTP_GPIO_Config(); 
  GPT_I2C_ResetChip();

}

/**
 * @brief  CPU向I2C总线设备发送8bit数据
 * @param  无
 * @retval 无
 */
void I2C_SendByte(uint8_t byte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (byte & 0x80)
		{
			TP_I2C_SDA_1();
		}
		else
		{
			TP_I2C_SDA_0();
		}
		CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
		TP_I2C_SCL_1();
		CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);	
		TP_I2C_SCL_0();
		if (i == 7)
		{
			 TP_I2C_SDA_1(); // 释放总线
		}
		byte <<= 1;	/* 左移一个bit */
		CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	}
}

/**
 * @brief  CPU向I2C总线设备读取8bit数据
 * @param  无
 * @retval 无
 */
uint8_t I2C_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		TP_I2C_SCL_1();
		CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
		if(TP_I2C_SDA_READ())
		{
			value++;
		}
		TP_I2C_SCL_0();
		CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	}
	return value;
}

/**
 * @brief  CPU产生一个时钟，并读取器件的ACK应答信号
 * @param  无
 * @retval 无
 */
uint8_t I2C_WaitAck(void)
{
	uint8_t re;

	TP_I2C_SDA_1();	/* CPU释放SDA总线 */
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	if(TP_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	TP_I2C_SCL_0();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	return re;
}

/**
 * @brief CPU产生一个ACK信号
 * @param 无
 * @note  无
 * @retval 无
 */
void I2C_Ack(void)
{
	TP_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SCL_1();	/* CPU产生1个时钟 */
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SCL_0();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SDA_1();	/* CPU释放SDA总线 */
}

/**
 * @brief CPU产生1个NACK信号
 * @param 无
 * @note  无
 * @retval 无
 */
void I2C_NAck(void)
{
	TP_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SCL_1();	/* CPU产生1个时钟 */
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
	TP_I2C_SCL_0();
	CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);	
}



#define I2C_DIR_WR	0		/* 写控制bit */
#define I2C_DIR_RD	1		/* 读控制bit */

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint32_t GPT_I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	
	/* 第1步：发起I2C总线启动信号 */
	I2C_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	I2C_SendByte(ClientAddr | I2C_DIR_RD);	/* 此处是读指令 */
	
	/* 第3步：等待ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}

	while(NumByteToRead) 
    {
        if(NumByteToRead == 1)
        {
            I2C_Ack();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
          
            /* 发送I2C总线停止信号 */
            I2C_Stop();
        }
    
        *pBuffer = I2C_ReadByte();
    
        /* 读指针自增 */
        pBuffer++; 
          
        /*计数器自减 */
        NumByteToRead--;
        
        I2C_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */  
    }

	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 0;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 1;
}

/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t GPT_I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	uint16_t m;	

    /*　第0步：发停止信号，启动内部写操作　*/
    I2C_Stop();
  
    /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
    CLK频率为200KHz时，查询次数为30次左右
    */
    for (m = 0; m < 1000; m++)
    {				
        /* 第1步：发起I2C总线启动信号 */
        I2C_Start();

        /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
        I2C_SendByte(ClientAddr | I2C_DIR_WR);	/* 此处是写指令 */

        /* 第3步：发送一个时钟，判断器件是否正确应答 */
        if (I2C_WaitAck() == 0)
        {
          break;
        }
    }
    if (m  == 1000)
    {
        goto cmd_fail;	/* EEPROM器件写超时 */
    }	
	
    while(NumByteToWrite--)
    {
        /* 第4步：开始写入数据 */
        I2C_SendByte(*pBuffer);

        /* 第5步：检查ACK */
        if (I2C_WaitAck() != 0)
        {
            goto cmd_fail;	/* 器件无应答 */
        }

        pBuffer++;	/* 地址增1 */		
    }
	
	/* 命令执行成功，发送I2C总线停止信号 */
	I2C_Stop();
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 1;
}



/*********************************************END OF FILE**********************/
