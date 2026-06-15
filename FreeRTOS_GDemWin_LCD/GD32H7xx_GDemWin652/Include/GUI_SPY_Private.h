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
File        : GUI_SPY_Private.h
Purpose     : Private header file for Spy
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_SPY_PRIVATE_H
#define GUI_SPY_PRIVATE_H

#include "GUI_Private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef   GUI_SPY_COM_BUFFER_SIZE
  #define GUI_SPY_COM_BUFFER_SIZE 1000
#endif

#define JOB_REQUEST_STATUS  1
#define JOB_REQUEST_VERSION 2
#define JOB_REQUEST_WINDOWS 3
#define JOB_REQUEST_INPUT   4
#define JOB_REQUEST_BMP     5
#define JOB_QUIT            6

#define INPUT_PID    1
#define INPUT_KEY    2
#define INPUT_MTOUCH 3

#define TRY(x) x; if (r) return 1

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct SPY_CONTEXT SPY_CONTEXT;

struct SPY_CONTEXT{
  GUI_tSend pfSend;
  GUI_tRecv pfRecv;
  void * pConnectInfo;
  U8 acBuffer[GUI_SPY_COM_BUFFER_SIZE];
  int NumBytesInBuffer;
  int Result;
  int (* pfFlush)   (SPY_CONTEXT * pContext);
  int (* pfStoreU8) (SPY_CONTEXT * pContext,  U8 Byte);
  int (* pfStoreU16)(SPY_CONTEXT * pContext, U16 Data);
  int (* pfStoreU32)(SPY_CONTEXT * pContext, U32 Data);
  int (* pfRead)    (SPY_CONTEXT * pContext, U8 * buf, int len);
};

typedef struct {
  U16 Type;
  U32 Time;
  union {
    GUI_KEY_STATE    StateKEY;
    GUI_PID_STATE    StatePID;
    GUI_MTOUCH_STATE StateMTOUCH;
  } u;
} SPY_INPUT;



#endif // GUI_SPY_PRIVATE_H

/*************************** End of file ****************************/
