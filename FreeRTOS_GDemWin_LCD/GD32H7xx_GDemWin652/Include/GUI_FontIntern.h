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
File        : GUI_FontIntern.h
Purpose     : Internal declarations used in font files
---------------------------END-OF-HEADER------------------------------
*/

#ifndef  GUI_FONTINTERN_H    /* Guard against multiple inclusion */
#define  GUI_FONTINTERN_H

#include "GUI.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

//
// Proportional fonts
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font10S_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font10_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13H_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13HB_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16_1_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_CHARINFO_EXT  GUI_Font16_HK_CharInfo[169];
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font20_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font20B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_FontComic18B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_FontComic24B_ASCII_Prop1;

//
// Monospaced fonts
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font6x8_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x8_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x13_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x15B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x16_ASCII_Prop1;

//
// Proportional fonts, antialiased
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font12_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font48_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font64_ASCII_AA4_Prop1;

//
// Monospaced fonts, antialiased
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font5x12_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x16_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font12x24_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16x32_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24x48_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32x64_ASCII_AA4_Prop1;

#if defined(__cplusplus)
  }
#endif


#endif   /* Guard against multiple inclusion */

/*************************** End of file ****************************/
