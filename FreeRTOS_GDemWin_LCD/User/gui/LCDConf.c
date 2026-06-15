/**
 *********************************************************************
 * @file    LCDConf.c
 * @brief   Display adaptation for GD32H7 TLI + GUIDRV_LIN_16
 *********************************************************************
 */

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "gd32h7xx.h"

#define XSIZE_PHYS  800
#define YSIZE_PHYS  480
#define COLOR_CONVERSION  GUICC_M565
#define DISPLAY_DRIVER    GUIDRV_LIN_16
#define VRAM_ADDR         ((void *)0xC0000000)

void LCD_X_Config(void)
{
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
    LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVRAMAddrEx(0, VRAM_ADDR);
}

int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *pData)
{
    (void)LayerIndex;
    (void)pData;

    switch (Cmd) {
    case LCD_X_INITCONTROLLER:
        return 0;

    case LCD_X_SETVRAMADDR:
    {
        LCD_X_SETVRAMADDR_INFO *pInfo = (LCD_X_SETVRAMADDR_INFO *)pData;
        TLI_LXFBADDR(0) = (uint32_t)(pInfo->pVRAM);
        tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
        return 0;
    }

    case LCD_X_ON:
        tli_enable();
        return 0;

    case LCD_X_OFF:
        tli_disable();
        return 0;

    default:
        return -1;
    }
}
