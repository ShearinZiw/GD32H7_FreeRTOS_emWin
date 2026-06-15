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
File        : CHECKBOX_Private.h
Purpose     : CHECKBOX private header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef CHECKBOX_PRIVATE_H
#define CHECKBOX_PRIVATE_H

#include "WM_Intern.h"
#include "WM.h"
#include "WIDGET.h"
#include "CHECKBOX.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef   CHECKBOX_BKCOLOR0_DEFAULT
  #define CHECKBOX_BKCOLOR0_DEFAULT 0x808080           /* Inactive color */
#endif

#ifndef   CHECKBOX_BKCOLOR1_DEFAULT
  #define CHECKBOX_BKCOLOR1_DEFAULT GUI_WHITE          /* Active color */
#endif

#ifndef   CHECKBOX_FGCOLOR0_DEFAULT
  #define CHECKBOX_FGCOLOR0_DEFAULT 0x101010
#endif

#ifndef   CHECKBOX_FGCOLOR1_DEFAULT
  #define CHECKBOX_FGCOLOR1_DEFAULT GUI_BLACK
#endif

#ifndef   CHECKBOX_DEFAULT_SIZE
  #define CHECKBOX_DEFAULT_SIZE          15
#endif

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  unsigned              (* pfGetButtonSize)(void);
  WIDGET_DRAW_ITEM_FUNC  * pfDrawSkin;
} CHECKBOX_SKIN_PRIVATE;

typedef struct {
  const GUI_FONT      * pFont;
  GUI_COLOR             aBkColorBox[2]; /* Colors used to draw the box background */
  GUI_COLOR             BkColor;        /* Widget background color */
  GUI_COLOR             TextColor;
  GUI_COLOR             FocusColor;
  CHECKBOX_SKIN_PRIVATE SkinPrivate;
  I16                   Align;
  U8                    Spacing;
  const GUI_BITMAP    * apBm[6];
} CHECKBOX_PROPS;

typedef struct {
  WIDGET              Widget;
  CHECKBOX_PROPS      Props;
  WIDGET_SKIN const * pWidgetSkin;
  U8                  NumStates;
  U8                  CurrentState;
  WM_HMEM             hpText;
  U32                 ButtonSize;
} CHECKBOX_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define CHECKBOX_INIT_ID(p) (p->Widget.DebugId = WIDGET_TYPE_CHECKBOX)
#else
  #define CHECKBOX_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  CHECKBOX_Obj * CHECKBOX_LockH(CHECKBOX_Handle h);
  #define CHECKBOX_LOCK_H(h)   CHECKBOX_LockH(h)
#else
  #define CHECKBOX_LOCK_H(h)   (CHECKBOX_Obj *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
unsigned CHECKBOX__GetButtonSize(void);

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern CHECKBOX_PROPS      CHECKBOX__DefaultProps;

extern const WIDGET_SKIN   CHECKBOX__SkinClassic;
extern       WIDGET_SKIN   CHECKBOX__Skin;

extern WIDGET_SKIN const * CHECKBOX__pSkinDefault;

/*********************************************************************
*
*       Extern data
*
**********************************************************************
*/
extern const GUI_BITMAP    CHECKBOX__abmCheck[2];

#endif   /* GUI_WINSUPPORT */
#endif   /* CHECKBOX_PRIVATE_H */

/*************************** End of file ****************************/
