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
File        : GUI_ARRAY.h
Purpose     : Array handling routines
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_ARRAY_H
#define GUI_ARRAY_H

#include "GUI_Private.h"

/*********************************************************************
*
*       Public types
*
**********************************************************************
*/
typedef GUI_HMEM GUI_ARRAY;

typedef struct {
  void   ** ppItem;
  unsigned  i;
  GUI_ARRAY hArray;
} GUI_ARRAY_ITERATOR;

typedef void (GUI_ARRAY_ITERATE_CALLBACK)   (GUI_ARRAY hArray, unsigned i, void * pItem);
typedef int  (GUI_ARRAY_ITERATE_EX_CALLBACK)(GUI_ARRAY hArray, unsigned i, void * pItem, void * pUser);

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
GUI_ARRAY GUI_ARRAY_Create          (void);
int       GUI_ARRAY_AddItem         (GUI_ARRAY hArray, const void * pNew, int Len);
void      GUI_ARRAY_Delete          (GUI_ARRAY hArray);
void      GUI_ARRAY_DeletePtr       (GUI_ARRAY * phArray);
GUI_HMEM  GUI_ARRAY_GethItem        (GUI_ARRAY hArray, unsigned int Index);
GUI_HMEM  GUI_ARRAY_GetLasthItem    (GUI_ARRAY hArray);
unsigned  GUI_ARRAY_GetNumItems     (GUI_ARRAY hArray);
void    * GUI_ARRAY_GetpItemLocked  (GUI_ARRAY hArray, unsigned int Index);
int       GUI_ARRAY_SethItem        (GUI_ARRAY hArray, unsigned int Index, GUI_HMEM hItem);
GUI_HMEM  GUI_ARRAY_SetItem         (GUI_ARRAY hArray, unsigned int Index, const void * pData, int Len);
void      GUI_ARRAY_DeleteItem      (GUI_ARRAY hArray, unsigned int Index);
char      GUI_ARRAY_InsertBlankItem (GUI_ARRAY hArray, unsigned int Index);
GUI_HMEM  GUI_ARRAY_InsertItem      (GUI_ARRAY hArray, unsigned int Index, int Len);
void    * GUI_ARRAY_ResizeItemLocked(GUI_ARRAY hArray, unsigned int Index, int Len);
void      GUI_ARRAY_BeginIterator   (GUI_ARRAY hArray, void ** ppItem, GUI_ARRAY_ITERATOR * pIterator);
void    * GUI_ARRAY_GetNextItem     (GUI_ARRAY_ITERATOR * pIterator);
void      GUI_ARRAY_EndIterator     (GUI_ARRAY_ITERATOR * pIterator);
void      GUI_ARRAY_Iterate         (GUI_ARRAY hArray, GUI_ARRAY_ITERATE_CALLBACK * cbIterate);
void      GUI_ARRAY_IterateEx       (GUI_ARRAY hArray, GUI_ARRAY_ITERATE_EX_CALLBACK * cbIterate, void * pUser);

#endif /* GUI_ARRAY_H */

/*************************** End of file ****************************/
