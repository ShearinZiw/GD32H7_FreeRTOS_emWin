/**
 *********************************************************************
 * @file    bsp_i2c.c
 * @brief   Bit-bang I2C driver for GT1151QM touch controller
 *
 * Uses register-level GPIO manipulation (GPIO_BOP/BC) for speed.
 * Bus speed: ~250 kHz with 4 us half-cycle delays via DWT.
 *********************************************************************
 */

#include "touch/bsp_i2c.h"
#include "dwt/bsp_dwt_delay.h"

/*===========================================================================
 * Low-level I2C primitives
 *===========================================================================*/

/** I2C Start condition: SDA high→low while SCL is high */
void I2C_Start(void)
{
    TP_I2C_SDA_1();
    TP_I2C_SCL_1();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SDA_0();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SCL_0();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
}

/** I2C Stop condition: SDA low→high while SCL is high */
void I2C_Stop(void)
{
    TP_I2C_SDA_0();
    TP_I2C_SCL_1();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SDA_1();
}

/** Send one byte (MSB first), release SDA on last bit for slave ACK */
void I2C_SendByte(uint8_t byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        if (byte & 0x80) {
            TP_I2C_SDA_1();
        } else {
            TP_I2C_SDA_0();
        }
        CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
        TP_I2C_SCL_1();
        CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
        TP_I2C_SCL_0();
        if (i == 7) {
            TP_I2C_SDA_1();  /* Release SDA for slave ACK */
        }
        byte <<= 1;
        CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    }
}

/** Read one byte (MSB first) */
uint8_t I2C_ReadByte(void)
{
    uint8_t i;
    uint8_t value = 0;

    for (i = 0; i < 8; i++) {
        value <<= 1;
        TP_I2C_SCL_1();
        CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
        if (TP_I2C_SDA_READ()) {
            value++;
        }
        TP_I2C_SCL_0();
        CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    }
    return value;
}

/** Wait for slave ACK. Returns 0 on ACK, 1 on NACK. */
uint8_t I2C_WaitAck(void)
{
    uint8_t re;

    TP_I2C_SDA_1();  /* Release SDA */
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SCL_1();  /* Clock pulse — slave drives SDA low for ACK */
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    if (TP_I2C_SDA_READ()) {
        re = 1;  /* NACK */
    } else {
        re = 0;  /* ACK */
    }
    TP_I2C_SCL_0();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    return re;
}

/** Send ACK (SDA = 0) */
void I2C_Ack(void)
{
    TP_I2C_SDA_0();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SCL_1();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SCL_0();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SDA_1();  /* Release SDA */
}

/** Send NACK (SDA = 1) */
void I2C_NAck(void)
{
    TP_I2C_SDA_1();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SCL_1();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
    TP_I2C_SCL_0();
    CPU_TS_Tmr_Delay_US(I2C_SPEED_TIME);
}

/*===========================================================================
 * GPIO initialization
 *===========================================================================*/

static void GTP_GPIO_Config(void)
{
    /* Enable GPIO clocks */
    rcu_periph_clock_enable(TP_RCU_I2C_SCL_PORT);
    rcu_periph_clock_enable(TP_RCU_I2C_SDA_PORT);
    rcu_periph_clock_enable(TP_RCU_RST_PORT);
    rcu_periph_clock_enable(TP_RCU_INT_PORT);

    /* I2C SCL — open-drain output with pull-up */
    gpio_mode_set(TP_I2C_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, TP_I2C_SCL_GPIO_PIN);
    gpio_output_options_set(TP_I2C_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_100_220MHZ, TP_I2C_SCL_GPIO_PIN);

    /* I2C SDA — open-drain output with pull-up (also used as input for reads) */
    gpio_mode_set(TP_I2C_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, TP_I2C_SDA_GPIO_PIN);
    gpio_output_options_set(TP_I2C_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_100_220MHZ, TP_I2C_SDA_GPIO_PIN);

    /* RST — push-pull output, initially low */
    gpio_mode_set(TP_RST_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, TP_RST_GPIO_PIN);
    gpio_output_options_set(TP_RST_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_RST_GPIO_PIN);

    /* INT — push-pull output, initially low */
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, TP_INT_GPIO_PIN);
    gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_INT_GPIO_PIN);

    /* Send stop to reset I2C bus state */
    I2C_Stop();
}

/*===========================================================================
 * Chip reset
 *===========================================================================*/

/**
 * Reset GT1151QM via RST pin pulse.
 * Sequence: RST low (1 ms) → RST high (1 ms) → INT configured for I2C address selection.
 */
void GPT_I2C_ResetChip(void)
{
    /* INT as output, initially low */
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, TP_INT_GPIO_PIN);
    gpio_output_options_set(TP_INT_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, TP_INT_GPIO_PIN);

    /* RST low 1 ms (hardware reset) */
    gpio_bit_reset(TP_RST_GPIO_PORT, TP_RST_GPIO_PIN);
    CPU_TS_Tmr_Delay_US(1000);

    /* RST high 1 ms (release reset) */
    gpio_bit_set(TP_RST_GPIO_PORT, TP_RST_GPIO_PIN);
    CPU_TS_Tmr_Delay_US(1000);

    /* INT as input floating (for touch interrupt) */
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, TP_INT_GPIO_PIN);
}

/*===========================================================================
 * Public init
 *===========================================================================*/

void GPT_I2C_Touch_Init(void)
{
    GTP_GPIO_Config();
    GPT_I2C_ResetChip();
}

/*===========================================================================
 * High-level I2C read/write
 *===========================================================================*/

#define I2C_DIR_WR  0  /* Write direction bit */
#define I2C_DIR_RD  1  /* Read direction bit */

/**
 * Read multiple bytes from I2C slave.
 * Returns 0 on success, 1 on failure.
 */
uint32_t GPT_I2C_ReadBytes(uint8_t ClientAddr, uint8_t *pBuffer, uint16_t NumByteToRead)
{
    I2C_Start();
    I2C_SendByte(ClientAddr | I2C_DIR_RD);

    if (I2C_WaitAck() != 0) {
        goto cmd_fail;
    }

    while (NumByteToRead) {
        if (NumByteToRead == 1) {
            I2C_NAck();  /* Last byte: NACK before reading */
            *pBuffer = I2C_ReadByte();
        } else {
            *pBuffer = I2C_ReadByte();
            I2C_Ack();   /* Intermediate byte: ACK after reading */
        }
        pBuffer++;
        NumByteToRead--;
    }

    I2C_Stop();
    return 0;

cmd_fail:
    I2C_Stop();
    return 1;
}

/**
 * Write multiple bytes to I2C slave.
 * Retries up to 1000 times waiting for slave to become ready.
 * Returns 0 on success, 1 on failure.
 */
uint32_t GPT_I2C_WriteBytes(uint8_t ClientAddr, uint8_t *pBuffer, uint8_t NumByteToWrite)
{
    uint16_t m;

    I2C_Stop();  /* Start with a clean stop condition */

    /* Poll until slave ACKs its address (internal write may be in progress) */
    for (m = 0; m < 1000; m++) {
        I2C_Start();
        I2C_SendByte(ClientAddr | I2C_DIR_WR);
        if (I2C_WaitAck() == 0) {
            break;  /* Slave is ready */
        }
    }
    if (m == 1000) {
        goto cmd_fail;
    }

    while (NumByteToWrite--) {
        I2C_SendByte(*pBuffer);
        if (I2C_WaitAck() != 0) {
            goto cmd_fail;
        }
        pBuffer++;
    }

    I2C_Stop();
    return 0;

cmd_fail:
    I2C_Stop();
    return 1;
}
