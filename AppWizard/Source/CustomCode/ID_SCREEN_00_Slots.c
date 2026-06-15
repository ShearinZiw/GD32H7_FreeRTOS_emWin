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
File        : ID_SCREEN_00_Slots.c
Purpose     : AppWizard managed file, function content could be changed
---------------------------END-OF-HEADER------------------------------
*/

#include "Application.h"
#include "../Generated/Resource.h"
#include "../Generated/ID_SCREEN_00.h"
#include "GUI_Demo.h"

/*** Begin of user code area ***/

/* Triggered when BUTTON_00 is clicked — demo multimedia from SD card */
static void _OnButtonClick(void) {
  GUI_Demo_JPEG("0:/test.jpg", 400, 200);
  GUI_Demo_GIF("0:/test.gif", 400, 200);
}

/*** End of user code area ***/

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void cbID_SCREEN_00(WM_MESSAGE * pMsg) {
  GUI_USE_PARA(pMsg);
}

void ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_TEXT_00_Copy__APPW_JOB_SETCOLOR(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
  _OnButtonClick();
}

/*************************** End of file ****************************/
