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
File        : GUI_X_Linux.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include <time.h>
#include <pthread.h>

/*********************************************************************
*
*       Local data
*/
static pthread_mutex_t _hLock;

/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/

GUI_TIMER_TIME GUI_X_GetTime(void) {
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);
  return (spec.tv_nsec / 1000000) + (spec.tv_sec * 1000);
}

void GUI_X_Delay(int ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, &ts);
}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

void GUI_X_Init(void) {
}

/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) {}

void GUI_X_Unlock(void) {
  pthread_mutex_unlock(&_hLock);
}

void GUI_X_Lock(void) {
  pthread_mutex_lock(&_hLock);
}

U32  GUI_X_GetTaskId(void) {
  pthread_t   tid;
  tid = pthread_self();
  return (U32)tid;
}

void GUI_X_InitOS(void) {
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

void GUI_TOUCH_X_ActivateX(void) {}
void GUI_TOUCH_X_ActivateY(void) {}
void GUI_TOUCH_X_Disable  (void) {}
int  GUI_TOUCH_X_MeasureX (void) {return 0;}
int  GUI_TOUCH_X_MeasureY (void) {return 0;}

/*************************** End of file ****************************/
