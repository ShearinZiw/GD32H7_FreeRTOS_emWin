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
File        : LCD_X_8080_16.h
Purpose     : Interface definition:
              Port routines 8080 interface, 16 bit data bus
----------------------------------------------------------------------
*/

#ifndef LCD_X_8080_16_H
#define LCD_X_8080_16_H

/*********************************************************************
*
*       Public routines
*/
void LCD_X_8080_16_Init       (void);
void LCD_X_8080_16_Write00_16 (U16 c);
void LCD_X_8080_16_Write01_16 (U16 c);
void LCD_X_8080_16_WriteM01_16(U16 * pData, int NumWords);
void LCD_X_8080_16_WriteM00_16(U16 * pData, int NumWords);
U16  LCD_X_8080_16_Read00_16  (void);
U16  LCD_X_8080_16_Read01_16  (void);
void LCD_X_8080_16_ReadM00_16 (U16 * pData, int NumWords);
void LCD_X_8080_16_ReadM01_16 (U16 * pData, int NumWords);

#endif /* LCD_X_8080_16_H */

/*************************** End of file ****************************/
