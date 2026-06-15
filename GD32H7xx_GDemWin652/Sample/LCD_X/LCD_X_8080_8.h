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
File        : LCD_X_8080_8.h
Purpose     : Interface definition:
              Port routines 8080 interface, 8 bit data bus
----------------------------------------------------------------------
*/

#ifndef LCD_X_8080_8_H
#define LCD_X_8080_8_H

/*********************************************************************
*
*       Public routines
*/
void          LCD_X_8080_8_Init(void);
void          LCD_X_8080_8_Sync(void);
void          LCD_X_8080_8_Write00(unsigned char c);
void          LCD_X_8080_8_Write01(unsigned char c);
void          LCD_X_8080_8_WriteM01(unsigned char * pData, int NumBytes);
void          LCD_X_8080_8_WriteM00(unsigned char * pData, int NumBytes);
unsigned char LCD_X_8080_8_Read00(void);
unsigned char LCD_X_8080_8_Read01(void);
void          LCD_X_8080_8_ReadM00(unsigned char * pData, int NumBytes);
void          LCD_X_8080_8_ReadM01(unsigned char * pData, int NumBytes);

#endif /* LCD_X_8080_8_H */

/*************************** End of file ****************************/
