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
File        : HEADER_Private.h
Purpose     : Private HEADER include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef HEADER_PRIVATE_H
#define HEADER_PRIVATE_H

#include "WM_Intern.h"
#include "HEADER.h"
#include "WIDGET.h"
#include "GUI_ARRAY.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  int     Width;
  I16     Align;
  WM_HMEM hDrawObj;
  char    acText[1];
} HEADER_COLUMN;

typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} HEADER_SKIN_PRIVATE;

typedef struct {
  const GUI_FONT    * pFont;
  GUI_COLOR           BkColor;
  GUI_COLOR           TextColor;
  GUI_COLOR           ArrowColor;
  HEADER_SKIN_PRIVATE SkinPrivate;
  U8                  BorderH;
  U8                  BorderV;
} HEADER_PROPS;

typedef struct {
  WIDGET              Widget;
  HEADER_PROPS        Props;
  WIDGET_SKIN const * pWidgetSkin;
  GUI_ARRAY           Columns;
  int                 CapturePosX;
  int                 CaptureItem;
  int                 ScrollPos;
  int                 Sel;
  int                 DirIndicatorColumn;
  int                 DirIndicatorReverse;
  unsigned            Fixed;
  U8                  DragLimit;
  U8                  ResizeableColumns;
  U8                  Height;
  U8                  xOff;
} HEADER_Obj;

/*********************************************************************
*
*       Private (module internal) data
*
**********************************************************************
*/

extern HEADER_PROPS        HEADER__DefaultProps;
extern const GUI_CURSOR  * HEADER__pDefaultCursor;

extern const WIDGET_SKIN   HEADER__SkinClassic;
extern       WIDGET_SKIN   HEADER__Skin;

extern WIDGET_SKIN const * HEADER__pSkinDefault;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define HEADER_INIT_ID(p)  (p->Widget.DebugId = WIDGET_TYPE_HEADER)
#else
  #define HEADER_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  HEADER_Obj * HEADER_LockH(HEADER_Handle h);
  #define HEADER_LOCK_H(h)   HEADER_LockH(h)
#else
  #define HEADER_LOCK_H(h)   (HEADER_Obj *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private (module internal) functions
*
**********************************************************************
*/
void         HEADER__SetDrawObj       (HEADER_Handle hObj, unsigned Index, GUI_DRAW_HANDLE hDrawObj);
void         HEADER__SetOffset        (HEADER_Handle hObj, U8 xOff);
const char * HEADER__GetItemTextLocked(HEADER_Handle hObj, unsigned Index, void ** ppUnlock);

#endif // GUI_WINSUPPORT
#endif // Avoid multiple inclusion

/*************************** End of file ****************************/
