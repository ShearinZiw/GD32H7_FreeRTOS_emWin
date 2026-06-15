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
File        : DIALOG_Type.h
Purpose     : Dialog box include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef DIALOG_TYPE_H
#define DIALOG_TYPE_H

#include "WM.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {        // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct  GUI_WIDGET_CREATE_INFO_struct GUI_WIDGET_CREATE_INFO;
typedef WM_HWIN GUI_WIDGET_CREATE_FUNC        (const GUI_WIDGET_CREATE_INFO * pCreate, WM_HWIN hWin, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       Structures
*
**********************************************************************
*/
struct GUI_WIDGET_CREATE_INFO_struct {
  GUI_WIDGET_CREATE_FUNC * pfCreateIndirect;
  const char             * pName;            // Text ... Not used on all widgets
  I16                      Id;               // ID ... should be unique in a dialog
  I16                      x0;               // x position
  I16                      y0;               // y position
  I16                      xSize;            // x size
  I16                      ySize;            // y size
  U32                      Flags;            // Widget specific create flags (opt.)
  I32                      Para;             // Widget specific parameter (opt.)
  U32                      NumExtraBytes;    // Number of extra bytes usable with <WIDGET>_SetUserData & <WIDGET>_GetUserData
};

/*********************************************************************
*
*       Public API functions
*
**********************************************************************
*/
WM_HWIN            GUI_CreateDialogBox   (const GUI_WIDGET_CREATE_INFO * paWidget, int NumWidgets, WM_CALLBACK * cb, WM_HWIN hParent, int x0, int y0);
void               GUI_EndDialog         (WM_HWIN hWin, int r);
int                GUI_ExecDialogBox     (const GUI_WIDGET_CREATE_INFO * paWidget, int NumWidgets, WM_CALLBACK * cb, WM_HWIN hParent, int x0, int y0);
int                GUI_ExecCreatedDialog (WM_HWIN hDialog);
WM_DIALOG_STATUS * GUI_GetDialogStatusPtr(WM_HWIN hDialog);                                    // Not to be documented
void               GUI_SetDialogStatusPtr(WM_HWIN hDialog, WM_DIALOG_STATUS * pDialogStatus);  // Not to be documented

/*********************************************************************
*
*       Obsolete
*/
LCD_COLOR          DIALOG_GetBkColor(void);
LCD_COLOR          DIALOG_SetBkColor(LCD_COLOR BkColor);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // DIALOG_TYPE_H

/*************************** End of file ****************************/
