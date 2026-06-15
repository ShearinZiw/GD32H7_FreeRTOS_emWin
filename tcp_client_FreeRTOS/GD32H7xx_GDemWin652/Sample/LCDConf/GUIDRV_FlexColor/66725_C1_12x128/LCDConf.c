/*********************************************************************
*                        SEGGER Software GmbH                        *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*               (c) 1996 - 2023  SEGGER Software GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.52 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  GigaDevice Semiconductor Inc. whose
registered office is situated at  Building No.8, IC Park, No.9 Fenghao
East Road, Haidian, Beijing, China solely for the purposes of creating
libraries  for  ARM Cortex-M processor-based  devices, sublicensed and
distributed  under  the  terms  and conditions of the End User License
Agreement supplied by GigaDevice Semiconductor Inc.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              GigaDevice Semiconductor Inc., Building No.8, IC Park, No.9 Fenghao East Road, Beijing Haidian, , CHINA
Licensed SEGGER software: emWin
License number:           GUI-00918
License model:            Buyout SRC [Buyout Source Code License], according to Buyout Agreement signed Sep. 12th 2023 by Ivo Geilenbruegge and Aug. 31th 2023 by Guangyi Jin
Licensed product:         Any
Licensed platform:        Cortex-M
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2023-08-11 - 2026-08-26
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration, internal use only

Display     : 

---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"

#include "GUIDRV_FlexColor.h"

#include "LCD_X_8080_8.h"

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE 128
#define YSIZE 128

//
// Color conversion
//
#define COLOR_CONVERSION &LCD_API_ColorConv_Cust

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

//#define DISPLAY_ORIENTATION (0)
#define DISPLAY_ORIENTATION (GUI_MIRROR_X)                               // Default
//#define DISPLAY_ORIENTATION (GUI_MIRROR_Y)                               // 180
//#define DISPLAY_ORIENTATION (GUI_SWAP_XY)                                // CW
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_MIRROR_Y | GUI_SWAP_XY)  // CCW
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_MIRROR_Y)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_SWAP_XY)
//#define DISPLAY_ORIENTATION (GUI_MIRROR_Y | GUI_SWAP_XY)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#ifndef WIN32
  GUI_PORT_API _HW_API;
#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_1
*/
static LCD_PIXELINDEX LCD_Color2Index_Cust(LCD_COLOR Color) {
  int r, g, b;
#if (GUI_USE_ARGB)
  b =  Color        & 255;
  g = (Color >>  8) & 255;
  r = (Color >> 16) & 255;
#else
  r =  Color        & 255;
  g = (Color >>  8) & 255;
  b = (Color >> 16) & 255;
#endif
  return ((r + g + b + 383) / (3 * 255)) ? 0x8000 : 0x0000;
}

/*********************************************************************
*
*       _Index2Color_1
*/
static LCD_COLOR _Index2Color_Cust(LCD_PIXELINDEX Index) {
#if (GUI_USE_ARGB)
  return (Index & 0x8000) ? 0xFFFFFFFFul : 0xFF000000ul;
#else
  return Index ? 0x00FFFFFFul : 0;
#endif
}

/*********************************************************************
*
*       _GetIndexMask_1
*/
static LCD_PIXELINDEX _GetIndexMask_Cust(void) {
  return 0x8000;
}

/*********************************************************************
*
*       LCD_API_ColorConv_1
*
* Purpose
*   API table for this color conversion mode. Only used by memory
*   devices in this mode.
*/
const LCD_API_COLOR_CONV LCD_API_ColorConv_Cust = {
  LCD_Color2Index_Cust,
  _Index2Color_Cust,
  _GetIndexMask_Cust,
  0,
  (tLCDDEV_Color2IndexBulk *)0,
  (tLCDDEV_Index2ColorBulk *)0,
};

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/
static void _InitController(void) {
  LCD_X_8080_8_Init();         // Initialize port lines and reset controller
  GUI_X_Delay(200);            // Delay 200ms
  _HW_API.pfWrite8_A0(0xE2);   // System reset
  GUI_X_Delay(300);            // Delay 200ms

  _HW_API.pfWrite8_A0(0x33);   // on-off mode to improve crosstalk 
  _HW_API.pfWrite8_A0(0x0C);   // or 0x08 
  _HW_API.pfWrite8_A0(0x2b);   // set internal power control  
  _HW_API.pfWrite8_A0(0x25);   // set TC=-0.01%  /****/ This setting will cause the display to stay OFF, change to 0x27 and it will work
  _HW_API.pfWrite8_A0(0xa1);   // set line rate 
  _HW_API.pfWrite8_A0(0xeb);   // set bias  

  _HW_API.pfWrite8_A0(0x81);   // set VLCD value 
  _HW_API.pfWrite8_A0(72);     // VLCD
  _HW_API.pfWrite8_A0(0xd1);   // set color pattern=RGB 
  _HW_API.pfWrite8_A0(0xd6);   // set color mode=64k-color 

  _HW_API.pfWrite8_A0(0xf1);   // set com end 
  _HW_API.pfWrite8_A0(0x7f);   // 128

  _HW_API.pfWrite8_A0(0xf4);   // Column start wpc0
  _HW_API.pfWrite8_A0(0);
  _HW_API.pfWrite8_A0(0xf6);   // Column end wpc1
  _HW_API.pfWrite8_A0(XSIZE - 1);
  _HW_API.pfWrite8_A0(0xf5);   // Row start wpp0
  _HW_API.pfWrite8_A0(0);
  _HW_API.pfWrite8_A0(0xf7);   // Row end wpp1
  _HW_API.pfWrite8_A0(YSIZE - 1);
  _HW_API.pfWrite8_A0(0xf8);

  _HW_API.pfWrite8_A0(0xc8);   // set N-line inversion 
  _HW_API.pfWrite8_A0(0x11); 
  _HW_API.pfWrite8_A0(0xad);   // set ON/OFF display enable:normal display
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(pData);
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // Nothing done here, see 'LCD_X_ON'.
      _InitController();
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  GUI_DEVICE     * pDevice;
  CONFIG_FLEXCOLOR Config = {0};
  //
  // Set display driver and color conversion
  //
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Common display driver configuration
  //
  if (DISPLAY_ORIENTATION & GUI_SWAP_XY) {
    LCD_SetSizeEx(0, YSIZE, XSIZE);
    LCD_SetVSizeEx(0, YSIZE, XSIZE);
  } else {
    LCD_SetSizeEx(0, XSIZE, YSIZE);
    LCD_SetVSizeEx(0, XSIZE, YSIZE);
  }
  //
  // Setup hardware access routines
  //
  _HW_API.pfWrite8_A0  = LCD_X_8080_8_Write00;
  _HW_API.pfWriteM8_A0 = LCD_X_8080_8_WriteM00;
  _HW_API.pfWrite8_A1  = LCD_X_8080_8_Write01;
  _HW_API.pfWriteM8_A1 = LCD_X_8080_8_WriteM01;
  _HW_API.pfRead8_A0   = LCD_X_8080_8_Read00;
  _HW_API.pfRead8_A1   = LCD_X_8080_8_Read01;
  _HW_API.pfReadM8_A1  = LCD_X_8080_8_ReadM01;
  GUIDRV_FlexColor_SetFunc(pDevice, &_HW_API, GUIDRV_FLEXCOLOR_F66725, GUIDRV_FLEXCOLOR_M16C1B8);
  Config.NumDummyReads = 1;
  Config.Orientation   = DISPLAY_ORIENTATION;
  GUIDRV_FlexColor_Config(pDevice, &Config);
}

/*************************** End of file ****************************/
