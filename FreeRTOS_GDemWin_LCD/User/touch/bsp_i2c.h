/**
 *********************************************************************
 * @file    bsp_i2c.h
 * @brief   Bit-bang I2C driver for GT1151QM touch controller
 *
 * Pin mapping (confirmed with BTB dev board BGA176):
 *   SCL = PH7, SDA = PH8 (open-drain with pull-up)
 *   RST = PB1, INT = PC13
 *   I2C address: 0x28 (GT1151QM 5-inch)
 *********************************************************************
 */

#ifndef __BSP_I2C_TOUCH_H
#define __BSP_I2C_TOUCH_H

#include "gd32h7xx.h"

/* I2C speed: 250 kHz (4 us half-cycle) */
#define I2C_SPEED                250000
#define I2C_SPEED_TIME           (1000000 / I2C_SPEED)

/* GT1151QM 7-bit I2C address (5-inch LCD variant) */
#define GTP_ADDRESS              0x28

/*===========================================================================
 * GPIO pin definitions
 *===========================================================================*/
/* I2C bus — PH7=SCL, PH8=SDA */
#define TP_I2C_SCL_GPIO_PORT    GPIOH
#define TP_I2C_SDA_GPIO_PORT    GPIOH
#define TP_RCU_I2C_SCL_PORT     RCU_GPIOH
#define TP_RCU_I2C_SDA_PORT     RCU_GPIOH
#define TP_I2C_SCL_GPIO_PIN     GPIO_PIN_7
#define TP_I2C_SDA_GPIO_PIN     GPIO_PIN_8

/* Reset and interrupt — PB1=RST, PC13=INT */
#define TP_RST_GPIO_PORT        GPIOB
#define TP_INT_GPIO_PORT        GPIOC
#define TP_RCU_RST_PORT         RCU_GPIOB
#define TP_RCU_INT_PORT         RCU_GPIOC
#define TP_RST_GPIO_PIN         GPIO_PIN_1
#define TP_INT_GPIO_PIN         GPIO_PIN_13
#define TP_INT_READ()           gpio_input_bit_get(TP_INT_GPIO_PORT, TP_INT_GPIO_PIN)

/*===========================================================================
 * Register-level GPIO macros (faster than library calls)
 *===========================================================================*/
#define TP_I2C_SCL_1()          GPIO_BOP(TP_I2C_SCL_GPIO_PORT) = TP_I2C_SCL_GPIO_PIN
#define TP_I2C_SCL_0()          GPIO_BC(TP_I2C_SCL_GPIO_PORT)  = TP_I2C_SCL_GPIO_PIN

#define TP_I2C_SDA_1()          GPIO_BOP(TP_I2C_SDA_GPIO_PORT) = TP_I2C_SDA_GPIO_PIN
#define TP_I2C_SDA_0()          GPIO_BC(TP_I2C_SDA_GPIO_PORT)  = TP_I2C_SDA_GPIO_PIN

#define TP_I2C_SDA_READ()       ((GPIO_ISTAT(TP_I2C_SDA_GPIO_PORT) & TP_I2C_SDA_GPIO_PIN) != 0)

/*===========================================================================
 * Public functions
 *===========================================================================*/
void    GPT_I2C_Touch_Init(void);
void    GPT_I2C_ResetChip(void);
void    I2C_Start(void);
void    I2C_Stop(void);
void    I2C_SendByte(uint8_t byte);
uint8_t I2C_ReadByte(void);
uint8_t I2C_WaitAck(void);
void    I2C_Ack(void);
void    I2C_NAck(void);
uint32_t GPT_I2C_WriteBytes(uint8_t ClientAddr, uint8_t *pBuffer, uint8_t NumByteToWrite);
uint32_t GPT_I2C_ReadBytes(uint8_t ClientAddr, uint8_t *pBuffer, uint16_t NumByteToRead);

#endif /* __BSP_I2C_TOUCH_H */
