/**
 *********************************************************************
 * @file    bsp_gt1151qm.c
 * @brief   GT1151QM touch driver — uses proven Goodix_TS_Work_Func
 *
 * Touch data flow: Goodix_TS_Work_Func() → g_tp_* globals → GTP_Execu()
 *********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "touch/bsp_gt1151qm.h"
#include "touch/bsp_i2c.h"
#include "dwt/bsp_dwt_delay.h"

/*===========================================================================
 * I2C helper structures
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
    int im, ret = 0;
    for (im = 0; ret == 0 && im != num; im++) {
        if (msgs[im].flags & I2C_M_RD)
            ret = GPT_I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);
        else
            ret = GPT_I2C_WriteBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);
    }
    return ret ? ret : im;
}

static int32_t GTP_I2C_Read(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msgs[2];
    int32_t ret = -1, retries = 0;
    msgs[0].flags = 0; msgs[0].addr = client_addr;
    msgs[0].len = GTP_ADDR_LENGTH; msgs[0].buf = &buf[0];
    msgs[1].flags = I2C_M_RD; msgs[1].addr = client_addr;
    msgs[1].len = len - GTP_ADDR_LENGTH; msgs[1].buf = &buf[GTP_ADDR_LENGTH];
    while (retries < 5) { ret = I2C_Transfer(msgs, 2); if (ret == 2) break; retries++; }
    return ret;
}

static int32_t GTP_I2C_Write(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msg;
    msg.flags = 0; msg.addr = client_addr; msg.len = len; msg.buf = buf;
    return I2C_Transfer(&msg, 1);
}

/*===========================================================================
 * I2C comm test
 *===========================================================================*/

static int8_t GTP_I2C_Test(void)
{
    uint8_t retry = 0, ret;
    uint8_t test_buf[3] = { GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xFF, 0 };
    while (retry < 5) {
        ret = GTP_I2C_Read(GTP_ADDRESS, test_buf, 2 + 1);
        if (ret > 0) { GTP_INFO("I2C test OK"); return 0; }
        GTP_ERROR("I2C test retry %d", retry);
        retry++; CPU_TS_Tmr_Delay_US(10000);
    }
    return -1;
}

/*===========================================================================
 * Read version
 *===========================================================================*/

int32_t GTP_Read_Version(void)
{
    int32_t ret;
    uint8_t buf[8] = { GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff };
    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
    if (ret < 0) { GTP_ERROR("Read version failed"); return ret; }
    GTP_INFO("Chip ID: %02X%02X%02X%02X, FW: %02X%02X",
             buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    return 2;
}

/*===========================================================================
 * IRQ control
 *===========================================================================*/

static void GTP_IRQ_Enable(void)
{
    gpio_mode_set(TP_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, TP_INT_GPIO_PIN);
}

/*===========================================================================
 * Init
 *===========================================================================*/

int32_t GTP_Init_Panel(void)
{
    int32_t ret;
    GPT_I2C_Touch_Init();
    ret = GTP_I2C_Test();
    if (ret < 0) { GTP_ERROR("I2C communication ERROR!"); return ret; }
    GTP_Read_Version();

    /* Soft reset + normal mode */
    {
        uint8_t cmd[3] = { 0x80, 0x40, 0x02 };
        GTP_I2C_Write(GTP_ADDRESS, cmd, 3);
        CPU_TS_Tmr_Delay_US(50000);
        cmd[2] = 0x00;
        GTP_I2C_Write(GTP_ADDRESS, cmd, 3);
        CPU_TS_Tmr_Delay_US(10000);
    }

    GTP_IRQ_Enable();
    GTP_INFO("GT1151QM init OK (800x480)");
    return 0;
}

/*===========================================================================
 * Touch state globals (updated by Goodix_TS_Work_Func)
 *===========================================================================*/

static int g_tp_x = 0, g_tp_y = 0, g_tp_down = 0;

static void GTP_Touch_Down(int32_t id, int32_t x, int32_t y, int32_t w)
{
    (void)id; (void)w;
    g_tp_x = x; g_tp_y = y; g_tp_down = 1;
}

static void GTP_Touch_Up(int32_t id)
{
    (void)id;
    g_tp_down = 0;
}

/*===========================================================================
 * Goodix_TS_Work_Func — verified working in TLI_LCD_TOUCH reference
 *===========================================================================*/

static void Goodix_TS_Work_Func(void)
{
    uint8_t  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    uint8_t  touch_num = 0, finger = 0;
    static uint16_t pre_touch = 0;
    static uint8_t  pre_id[GTP_MAX_TOUCH] = {0};
    uint8_t *coor_data = NULL;
    int32_t  input_x = 0, input_y = 0, input_w = 0;
    uint8_t  id = 0;
    int32_t  i = 0, ret = -1;

    ret = GTP_I2C_Read(GTP_ADDRESS, point_data, 12);
    if (ret < 0) return;

    finger = point_data[GTP_ADDR_LENGTH];
    if (finger == 0x00) return;
    if ((finger & 0x80) == 0) goto exit_work_func;

    touch_num = finger & 0x0f;
    if (touch_num > GTP_MAX_TOUCH) goto exit_work_func;

    if (touch_num > 1) {
        uint8_t buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};
        ret = GTP_I2C_Read(GTP_ADDRESS, buf, 2 + 8 * (touch_num - 1));
        memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));
    }

    if (pre_touch > touch_num) {
        for (i = 0; i < pre_touch; i++) {
            uint8_t j;
            for (j = 0; j < touch_num; j++) {
                coor_data = &point_data[j * 8 + 3];
                id = coor_data[0] & 0x0F;
                if (pre_id[i] == id) break;
                if (j >= touch_num - 1) GTP_Touch_Up(pre_id[i]);
            }
        }
    }

    if (touch_num) {
        for (i = 0; i < touch_num; i++) {
            coor_data = &point_data[i * 8 + 3];
            id = coor_data[0] & 0x0F;
            pre_id[i] = id;
            input_x = coor_data[1] | (coor_data[2] << 8);
            input_y = coor_data[3] | (coor_data[4] << 8);
            input_w = coor_data[5] | (coor_data[6] << 8);
            GTP_Touch_Down(id, input_x, input_y, input_w);
        }
    } else if (pre_touch) {
        for (i = 0; i < pre_touch; i++) GTP_Touch_Up(pre_id[i]);
    }
    pre_touch = touch_num;

exit_work_func:
    GTP_I2C_Write(GTP_ADDRESS, end_cmd, 3);
}

/*===========================================================================
 * GTP_Execu — single-touch API for emWin
 *===========================================================================*/

int GTP_Execu(int *x, int *y)
{
    Goodix_TS_Work_Func();
    if (g_tp_down) {
        *x = g_tp_x;
        *y = g_tp_y;
        return 1;
    }
    return 0;
}
