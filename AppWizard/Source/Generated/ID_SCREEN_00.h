/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2026  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_00.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_00_H
#define ID_SCREEN_00_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_BOX_00         (GUI_ID_USER + 1)
#define ID_TEXT_00        (GUI_ID_USER + 2)
#define ID_TEXT_00_Copy   (GUI_ID_USER + 3)
#define ID_TEXT_00_Copy1  (GUI_ID_USER + 4)
#define ID_TEXT_00_Copy2  (GUI_ID_USER + 5)
#define ID_TEXT_00_Copy3  (GUI_ID_USER + 6)
#define ID_TEXT_00_Copy4  (GUI_ID_USER + 7)
#define ID_TEXT_00_Copy5  (GUI_ID_USER + 8)
#define ID_TEXT_00_Copy6  (GUI_ID_USER + 9)
#define ID_TEXT_00_Copy8  (GUI_ID_USER + 11)
#define ID_TEXT_00_Copy9  (GUI_ID_USER + 12)
#define ID_TEXT_00_Copy10 (GUI_ID_USER + 13)
#define ID_TEXT_00_Copy11 (GUI_ID_USER + 14)
#define ID_TEXT_00_Copy12 (GUI_ID_USER + 15)
#define ID_TEXT_00_Copy13 (GUI_ID_USER + 16)
#define ID_TEXT_00_Copy14 (GUI_ID_USER + 17)
#define ID_TEXT_00_Copy15 (GUI_ID_USER + 18)
#define ID_TEXT_00_Copy19 (GUI_ID_USER + 22)
#define ID_BUTTON_00      (GUI_ID_USER + 23)
#define ID_IMAGE_00       (GUI_ID_USER + 10)

/*********************************************************************
*
*       Slots
*/
void ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_TEXT_00_Copy__APPW_JOB_SETCOLOR(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_IMAGE_00__APPW_JOB_SETBITMAP   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_00(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_00_H

/*************************** End of file ****************************/
