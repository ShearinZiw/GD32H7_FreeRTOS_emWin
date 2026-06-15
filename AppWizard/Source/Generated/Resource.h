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
File        : Resource.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "AppWizard.h"

/*********************************************************************
*
*       Text
*/
#define ID_RTEXT_0 0

extern GUI_CONST_STORAGE unsigned char acAPPW_Language_0[];

/*********************************************************************
*
*       Font data
*/
extern APPW_FONT APPW__aFont[1];

/*********************************************************************
*
*       Fonts
*/
extern GUI_CONST_STORAGE unsigned char acNettoOT_40_Normal_EXT_AA4[];

/*********************************************************************
*
*       Images
*/
extern GUI_CONST_STORAGE unsigned char acDARK_Button_Up_100x30[];
extern GUI_CONST_STORAGE unsigned char acDARK_Button_Down_100x30[];

/*********************************************************************
*
*       Screens
*/
#define ID_SCREEN_00 (GUI_ID_USER + 4096)

extern APPW_ROOT_INFO ID_SCREEN_00_RootInfo;

#define APPW_INITIAL_SCREEN &ID_SCREEN_00_RootInfo

/*********************************************************************
*
*       Project path
*/
#define APPW_PROJECT_PATH "/"  /* non-empty path required; resources are compiled-in on embedded */

#endif  // RESOURCE_H

/*************************** End of file ****************************/
