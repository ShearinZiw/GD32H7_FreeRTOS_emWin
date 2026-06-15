/**
  ******************************************************************************
  * @file    bsp_i2c_gpio.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   适用于GD32系列CPU。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6 GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 

#include "i2c/bsp_i2c.h"
#include "gd32h7xx.h"
#include "systick/systick.h"
#include "usart/bsp_usart.h" 

/**
  * @brief  I2C I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_CONFIG(void)
{
    // 打开GPIO外设的时钟
    LCD_I2C_SCL_GPIO_APBXCLKCMD(LCD_I2C_GPIO_CLK_SCL);
    LCD_I2C_SDA_GPIO_APBXCLKCMD(LCD_I2C_GPIO_CLK_SDA);
    
    // 将I2C_SCL的GPIO配置为开漏输出复用模式
    gpio_af_set(OLED_I2C_SCL_GPIO_PORT, OLED_I2C_GPIO_AF, OLED_I2C_SCL_GPIO_PIN);
    gpio_mode_set(OLED_I2C_SCL_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, OLED_I2C_SCL_GPIO_PIN);
    gpio_output_options_set(OLED_I2C_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_60MHZ, OLED_I2C_SCL_GPIO_PIN);
    
    // 将I2C_SDA的GPIO配置为开漏输出复用模式
    gpio_af_set(OLED_I2C_SDA_GPIO_PORT, OLED_I2C_GPIO_AF, OLED_I2C_SDA_GPIO_PIN);
    gpio_mode_set(OLED_I2C_SDA_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, OLED_I2C_SDA_GPIO_PIN);
    gpio_output_options_set(OLED_I2C_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_60MHZ, OLED_I2C_SDA_GPIO_PIN);

}

/*!
    \brief      配置I2C接口
    \param[in]  无
    \param[out] 无
    \retval     无
*/
void I2C_CONFIG(void)
{
    /* 配置 I2C 时序参数 */
    i2c_timing_config(OLED_I2CX, 0x3, 0x9, 0);
    i2c_master_clock_config(OLED_I2CX, 0x96, 0xDA);
    
    /* 配置I2C地址 */
    i2c_address_config(OLED_I2CX, I2C_OWN_ADDRESS7, I2C_ADDFORMAT_7BITS);
    /* 配置I2C从机地址 */
    i2c_master_addressing(OLED_I2CX, OLED_ADDRESS_ADDRESS7, I2C_MASTER_TRANSMIT);
    /* 配置传输的字节数 */
    i2c_transfer_byte_number_config(OLED_I2CX, 8);
    /* 使能 I2C */
    i2c_enable(OLED_I2CX);
}

 /**
  * @brief  I2C_Init，初始化I2C
  * @param  无
  * @retval 无
  */
void I2C_Init(void)
{
    I2C_GPIO_CONFIG();
    I2C_CONFIG();
    /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    i2c_stop_on_bus(OLED_I2CX);//关闭I2C1总线
    i2c_flag_clear(OLED_I2CX, I2C_FLAG_STPDET);/* 清除 STPDET */
}

 /**
  * @brief  I2C_WriteByte，向OLED寄存器地址写一个byte的数据
  * @param  addr：寄存器地址
  *		    data：要写入的数据
  * @retval 无
  */
void I2C_WriteByte(uint8_t addr,uint8_t data)
{
    /* 检测总线是否繁忙 */
    while(i2c_flag_get(OLED_I2CX, I2C_FLAG_I2CBSY));
    
	/* 产生I2C 起始信号*/
	i2c_start_on_bus(OLED_I2CX);//开启I2C0

    /*发送 OLED_I2CX 设备地址，发送模式 */
    i2c_master_addressing(OLED_I2CX, OLED_ADDRESS_ADDRESS7, I2C_MASTER_TRANSMIT);//器件地址 -- 默认0x78
    
     /*等待,直到传输数据缓冲区为空*/
    I2C_STAT(OLED_I2CX) |= I2C_STAT_TBE;
    while(!i2c_flag_get(OLED_I2CX, I2C_FLAG_TBE));
        
    i2c_data_transmit(OLED_I2CX, addr);//发送数据
    while(!i2c_flag_get(OLED_I2CX, I2C_FLAG_TI));
    
    i2c_data_transmit(OLED_I2CX, data);//发送数据
    while(!i2c_flag_get(OLED_I2CX, I2C_FLAG_TI));
     
    i2c_stop_on_bus(OLED_I2CX);//关闭I2C0总线
    while(!i2c_flag_get(OLED_I2CX, I2C_FLAG_STPDET));/* 等待，直到停止条件产生 */
    
    i2c_flag_clear(OLED_I2CX, I2C_FLAG_STPDET);/* 清除 STPDET */
    
}
    
/*********************************************END OF FILE**********************/
