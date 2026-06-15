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
File        : LCD_X_SERIAL_3Wire.c
Purpose     : Port routines
----------------------------------------------------------------------
*/

#ifndef LCD_DELAY
  #define LCD_DELAY(x)
#endif

#ifndef LCD_SET_CS
  #define LCD_SET_CS()
  #define LCD_CLR_CS()
#endif

/*********************************************************************
*
*           Hardware configuration
*
**********************************************************************

  Needs to be adapted to your target hardware.
*/

/* Configuration example:

#define LCD_CLR_RESET()   P7 &= ~(1<<2)
#define LCD_SET_RESET()   P7 |=  (1<<2) 
#define LCD_CLR_CLK()     P7 &= ~(1<<3)
#define LCD_SET_CLK()     P7 |=  (1<<3) 
#define LCD_CLR_DATA()    P7 &= ~(1<<4)
#define LCD_SET_DATA()    P7 |=  (1<<4) 
#define LCD_SET_DIR_OUT() P7D = 0xff
#define LCD_CLR_CS()      P7 &= ~(1<<0)      // Optional
#define LCD_SET_CS()      P7 |=  (1<<0)      // Optional
#define LCD_DELAY(ms)     GUI_Delay(ms)      // Optional

*/

/*********************************************************************
*
*           Initialisation
*
**********************************************************************
  This routine should be called from your application program
  to set port pins to their initial values
*/

void LCD_X_Init(void) {
  LCD_CLR_RESET();
  LCD_SET_DIR_OUT();
  LCD_SET_CS();
  LCD_SET_RESET();
  LCD_SET_CLK();
  LCD_SET_DATA();
  LCD_DELAY(20);
  LCD_SET_RESET();
  LCD_DELAY(20);
}

/*********************************************************************
*
*           Send1 routine
*
**********************************************************************

  Usually, there is no need to modify this routine.
  It should be sufficient ot modify the low-level macros above or
  in a configuration file.
*/

/* Write 1 byte, MSB first */
static void Send1(unsigned char Data, unsigned char IsData) {
  if (IsData)          LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 7) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 6) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 5) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 4) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 3) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 2) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 1) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
  if ((Data >> 0) & 1) LCD_SET_DATA(); else LCD_CLR_DATA(); LCD_CLR_CLK(); LCD_SET_CLK();
}

/*********************************************************************
*
*           Controller 0
*
**********************************************************************

*/

/* Write to controller, with A0 = 1 */
void LCD_X_Write01(unsigned char Data) {
  LCD_CLR_CS();
  Send1(Data, 1);
  LCD_SET_CS();
}

/* Write multiple bytes to controller, with A0 = 1 */
void LCD_X_WriteM01(unsigned char * pData, int NumBytes) {
  LCD_CLR_CS();
  for (; NumBytes; NumBytes--) {
    Send1(*pData++, 1);
  }
  LCD_SET_CS();
}

/* Write to controller, with A0 = 0 */
void LCD_X_Write00(unsigned char Cmd) {
  LCD_CLR_CS();
  Send1(Cmd, 0);
  LCD_SET_CS();
}
