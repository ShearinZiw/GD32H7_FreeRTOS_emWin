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

/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  //
  // Setup configuration dependent pointers
  //
  APPW_X_Setup();
  //
  // Initialize AppWizard
  //
  APPW_Init(APPW_PROJECT_PATH);
  //
  // Note: If XBF font creation fails at runtime (AppWizard library
  // internal font handling), manual font creation via GUI_XBF_CreateFont()
  // with a memory callback and GUI_XBF_TYPE_PROP_AA4_EXT may be needed
  // before APPW_CreateRoot(). See CLAUDE.md for details.
  //
  // Create all persistent screens except initial screen
  //
  APPW_CreatePersistentScreens();
  //
  // Create initial screen...
  //
  APPW_CreateRoot(APPW_INITIAL_SCREEN, WM_HBKWIN);
  //
  // ...and keep it alive
  //
  for (APPW_Run = 1; APPW_Run; ) {
    APPW_Exec();
    GUI_X_Delay(5);
  }
}

/*************************** End of file ****************************/
