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
File        : KNOB.h
Purpose     : KNOB include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef KNOB_PRIVATE_H
#define KNOB_PRIVATE_H

#include "WM_Intern.h"
#include "KNOB.h"

#if (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  I32 Snap;          // Position where the knob snaps
  I32 Period;        // Time it takes to stop the knob in ms
  GUI_COLOR BkColor; // The Bk color
  I32 Offset;        // the offset
  I32 MinRange;
  I32 MaxRange;
  I32 MinVRange;
  I32 MaxVRange;
  I32 TickSize;      // Minimum movement range in 1/10 of degree
  I32 KeyValue;      // Range of movement for one key push
  U8  Invert;
} KNOB_PROPS;

typedef struct {
  WIDGET Widget;
  WIDGET_DRAW_ITEM_FUNC * pfOwnerDraw;
  void (* pfRotate)(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
  KNOB_PROPS Props;
  WM_HMEM hContext;
  I32 Angle;
  I32 Value;
  int xSize;
  int ySize;
  I32 AngleRotate;
  GUI_MEMDEV_Handle hMemSrc;
  GUI_MEMDEV_Handle hMemDst;
  GUI_MEMDEV_Handle hMemBk;
  I32               VRangeABS;
} KNOB_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define KNOB_INIT_ID(p) p->Widget.DebugId = WIDGET_TYPE_KNOB
#else
  #define KNOB_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  KNOB_OBJ * KNOB_LockH(KNOB_Handle h);
  #define KNOB_LOCK_H(h)   KNOB_LockH(h)
#else
  #define KNOB_LOCK_H(h)   (KNOB_OBJ *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
extern KNOB_PROPS KNOB__DefaultProps;

#endif   // (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)
#endif   // KNOB_PRIVATE_H
