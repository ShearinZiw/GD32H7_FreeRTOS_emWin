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
File        : APPW_MainTask.c
Purpose     : AppWizard application entry point
---------------------------END-OF-HEADER------------------------------
*/

#include "Generated/Resource.h"
#include "GUI.h"
#include <string.h>

extern void VehicleUI_Exec(void);

static int _cbGetFontData(U32 Off, U16 NumBytes, void *pVoid, void *pBuffer) {
  const unsigned char *pFontData = (const unsigned char *)pVoid;
  memcpy(pBuffer, pFontData + Off, NumBytes);
  return 0;
}

static void _InitFont(void) {
  extern GUI_CONST_STORAGE unsigned char acNettoOT_40_Normal_EXT_AA4[];
  APPW__aFont[0].FontData.pVoid     = (void *)acNettoOT_40_Normal_EXT_AA4;
  APPW__aFont[0].FontData.pfGetData = _cbGetFontData;
  GUI_XBF_CreateFont(&APPW__aFont[0].Font, &APPW__aFont[0].FontData,
                     GUI_XBF_TYPE_PROP_AA4_EXT, _cbGetFontData,
                     (void *)acNettoOT_40_Normal_EXT_AA4);
}

void MainTask(void) {
  APPW_X_Setup();
  APPW_Init(APPW_PROJECT_PATH);
  _InitFont();
  APPW_CreatePersistentScreens();
  APPW_CreateRoot(APPW_INITIAL_SCREEN, WM_HBKWIN);
  for (APPW_Run = 1; APPW_Run; ) {
    VehicleUI_Exec();
    APPW_Exec();
    GUI_X_Delay(5);
  }
}

/*************************** End of file ****************************/
