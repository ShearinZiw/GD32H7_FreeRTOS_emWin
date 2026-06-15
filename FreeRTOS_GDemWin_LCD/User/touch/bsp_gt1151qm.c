/**
 *********************************************************************
 * @file    bsp_gt1151qm.c
 * @brief   GT1151QM capacitive touch controller driver
 *
 * Communicates via bit-bang I2C (PH7=SCL, PH8=SDA).
 * Single-touch coordinate reading from register 0x814E.
 *
 * Adapted from TLI_LCD_TOUCH reference project.
 *********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "touch/bsp_gt1151qm.h"
#include "touch/bsp_i2c.h"
#include "dwt/bsp_dwt_delay.h"

/*===========================================================================
 * I2C helper structures and functions
 *===========================================================================*/

#define I2C_M_RD        0x0001
#define GTP_ADDR_LENGTH 2

struct i2c_msg {
    uint8_t  addr;
    uint16_t flags;
    uint16_t len;
    uint8_t *buf;
};

static int I2C_Transfer(struct i2c_msg *msgs, int num)
{
    int im;
    int ret = 0;

    for (im = 0; ret == 0 && im != num; im++) {
        if (msgs[im].flags & I2C_M_RD) {
            ret = GPT_I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);
        } else {
            ret = GPT_I2C_WriteBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);
        }
    }
    return (ret) ? ret : im;
}

/**
 * Read from touch IC: first write register address, then read data.
 * buf[0..1] = 16-bit register address, buf[2..len-1] = read buffer.
 */
static int32_t GTP_I2C_Read(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msgs[2];
    int32_t ret = -1;
    int32_t retries = 0;

    msgs[0].flags = 0;                               /* Write register address */
    msgs[0].addr  = client_addr;
    msgs[0].len   = GTP_ADDR_LENGTH;
    msgs[0].buf   = &buf[0];

    msgs[1].flags = I2C_M_RD;                         /* Read data */
    msgs[1].addr  = client_addr;
    msgs[1].len   = len - GTP_ADDR_LENGTH;
    msgs[1].buf   = &buf[GTP_ADDR_LENGTH];

    while (retries < 5) {
        ret = I2C_Transfer(msgs, 2);
        if (ret == 2) break;
        retries++;
    }
    return ret;
}

/**
 * Write to touch IC: register address + data.
 */
static int32_t GTP_I2C_Write(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msg;

    msg.flags = 0;
    msg.addr  = client_addr;
    msg.len   = len;
    msg.buf   = buf;

    return I2C_Transfer(&msg, 1);
}

/*===========================================================================
 * I2C communication test
 *===========================================================================*/

static int8_t GTP_I2C_Test(void)
{
    uint8_t retry = 0;
    uint8_t ret;
    uint8_t test_buf[3] = { GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xFF, 0 };

    while (retry < 5) {
        ret = GTP_I2C_Read(GTP_ADDRESS, test_buf, 2 + 1);
        if (ret > 0) {
            GTP_INFO("I2C test OK");
            return 0;
        }
        GTP_ERROR("I2C test retry %d", retry);
        retry++;
        CPU_TS_Tmr_Delay_US(10000);  /* 10 ms */
    }
    return -1;
}

/*===========================================================================
 * Chip version detection
 *===========================================================================*/

int32_t GTP_Read_Version(void)
{
    int32_t ret;
    uint8_t buf[8] = { GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff };

    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
    if (ret < 0) {
        GTP_ERROR("Read version failed");
        return ret;
    }

    GTP_INFO("Chip ID: %02X%02X%02X%02X, FW: %02X%02X",
             buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    return 2;
}

/*===========================================================================
 * IRQ control
 *===========================================================================*/

static void GTP_IRQ_Enable(void)
{
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, TP_INT_GPIO_PIN);
}

/*===========================================================================
 * Panel initialization
 *===========================================================================*/

int32_t GTP_Init_Panel(void)
{
    int32_t ret;

    GPT_I2C_Touch_Init();

    ret = GTP_I2C_Test();
    if (ret < 0) {
        GTP_ERROR("I2C communication ERROR!");
        return ret;
    }

    GTP_Read_Version();
    GTP_IRQ_Enable();
    GTP_INFO("GT1151QM init OK (800x480)");

    return 0;
}

/*===========================================================================
 * Touch coordinate reading
 *===========================================================================*/

/**
 * Read single-touch coordinates from GT1151QM.
 *
 * @param x  [out] Touch X coordinate (0..799)
 * @param y  [out] Touch Y coordinate (0..479)
 * @return   1 if touch detected, 0 if no touch
 *
 * Register layout (from 0x814E):
 *   [0..1]  register address (set by GTP_I2C_Read)
 *   [2]     status: bit7=buffer ready, bits3-0=touch count
 *   [3]     track ID
 *   [4..5]  X coord (little-endian)
 *   [6..7]  Y coord (little-endian)
 *   [8..9]  touch size
 */
int GTP_Execu(int *x, int *y)
{
    uint8_t  point_data[2 + 1 + 8 + 1] = { GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF };
    uint8_t  touch_num;
    uint8_t  finger;
    int32_t  input_x = 0;
    int32_t  input_y = 0;
    int32_t  ret;

    ret = GTP_I2C_Read(GTP_ADDRESS, point_data, 12);
    if (ret < 0) {
        return 0;
    }

    finger = point_data[GTP_ADDR_LENGTH];  /* Status register */

    if (finger == 0x00) {
        return 0;  /* No touch data */
    }

    if ((finger & 0x80) == 0) {
        return 0;  /* Buffer not ready */
    }

    touch_num = finger & 0x0f;
    if (touch_num > GTP_MAX_TOUCH || touch_num == 0) {
        return 0;
    }

    input_x = point_data[3 + 1] | (point_data[3 + 2] << 8);
    input_y = point_data[3 + 3] | (point_data[3 + 4] << 8);

    if (input_x < GTP_MAX_WIDTH && input_y < GTP_MAX_HEIGHT) {
        *x = input_x;
        *y = input_y;
        return 1;
    }

    return 0;
}
