/**
 *********************************************************************
 * @file    main.h
 * @brief   Main header for FreeRTOS + GDemWin + 5-inch LCD project
 *********************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#include "gd32h7xx.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Project configuration
 *===========================================================================*/

/* LCD panel: 5-inch, 800x480, RGB565 */
#define LCD_WIDTH           800
#define LCD_HEIGHT          480

/*===========================================================================
 * Function prototypes
 *===========================================================================*/
void BSP_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
