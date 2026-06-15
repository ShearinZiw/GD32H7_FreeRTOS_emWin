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
File        : WM_GUI.h
Purpose     : Windows manager include for low level GUI routines
----------------------------------------------------------------------
*/

#ifndef WM_GUI_H            /* Make sure we only include it once */
#define WM_GUI_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

int       WM__InitIVRSearch(const GUI_RECT* pMaxRect);
int       WM__GetNextIVR   (void);
int       WM__GetOrgX_AA(void);
int       WM__GetOrgY_AA(void);

#define WM_ITERATE_START(pRect)                   \
  {                                               \
    if (WM__InitIVRSearch(pRect))                 \
      do {

#define WM_ITERATE_END()                          \
    } while (WM__GetNextIVR());                   \
  }

#define WM_ADDORGX(x)       (x += GUI_pContext->xOff)
#define WM_ADDORGY(y)       (y += GUI_pContext->yOff)
#define WM_ADDORG(x0,y0)    WM_ADDORGX(x0); WM_ADDORGY(y0)
#define WM_ADDORGX_AA(x)    (x += WM__GetOrgX_AA())
#define WM_ADDORGY_AA(y)    (y += WM__GetOrgY_AA())
#define WM_ADDORG_AA(x0,y0) WM_ADDORGX_AA(x0); WM_ADDORGY_AA(y0)
#define WM_SUBORGX(x)       (x -= GUI_pContext->xOff)
#define WM_SUBORGY(y)       (y -= GUI_pContext->yOff)
#define WM_SUBORG(x0,y0)    WM_SUBORGX(x0); WM_SUBORGY(y0)

#if defined(__cplusplus)
  }
#endif


#endif   /* Avoid multiple inclusion */

/*************************** End of file ****************************/
