/**
 *********************************************************************
 * @file    bsp_gt1151qm.h
 * @brief   GT1151QM capacitive touch controller driver
 *
 * Features: single-touch coordinate reading, chip init, version check.
 * I2C address: 0x28 (7-bit), 5-inch 800x480 panel.
 *********************************************************************
 */

#ifndef __BSP_GT1151QM_H
#define __BSP_GT1151QM_H

#include "gd32h7xx.h"

/* Set to 1 to upload config to touch IC during init */
#define GTP_UPDATE_CONFIG       0

/* Touch panel resolution */
#define GTP_MAX_WIDTH           800
#define GTP_MAX_HEIGHT          480
#define GTP_MAX_TOUCH           5

/* GT1151QM register addresses */
#define GTP_READ_COOR_ADDR      0x814E
#define GTP_REG_VERSION         0x8140
#define GTP_REG_CONFIG_DATA     0x8050

/* Debug macros */
#define GTP_DEBUG_ON            1
#define GTP_INFO(fmt, ...)      printf("<<-GTP->> " fmt "\n", ##__VA_ARGS__)
#define GTP_ERROR(fmt, ...)     printf("<<-GTP-ERR->> " fmt "\n", ##__VA_ARGS__)

/*===========================================================================
 * Public functions
 *===========================================================================*/
int32_t GTP_Init_Panel(void);
int32_t GTP_Read_Version(void);
int     GTP_Execu(int *x, int *y);

#endif /* __BSP_GT1151QM_H */
