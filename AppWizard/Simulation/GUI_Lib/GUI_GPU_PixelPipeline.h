/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.52 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_GPU_PixelPipeline.h
Purpose     : 
---------------------------END-OF-HEADER------------------------------
*/


#ifndef GUI_GPU_PIXELPIPELINE_H
#define GUI_GPU_PIXELPIPELINE_H

#include "GUI_GPU.h"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_PXP_CONFIG
* 
*  Description
*    Configuration structure used for Pixel Pipeline driver (GUI_GPU_PXP)
*    during creation with GUI_GPU_Create().
*/
typedef struct {
  void  * pPXP;                                  // PXP peripheral base address.
  void  * pPXPReadQOS;                           // PXP master read priority address.
  GUI_VOID_FUNCTION * pfPreExecHook;             // Hook to be called before the execution of one action.
  GUI_VOID_FUNCTION * pfPostExecHook;            // Hook to be called after the execution of one action.
  GUI_VOID_FUNCTION * pfTransferCompleteHook;    // Hook to be called from within the IRQ handler after
                                                 // the transfer has been completed.
  U8      EnableTransferCompleteInterrupt : 1;   // If true, the Transfer Complete Interrupt will be enabled.
  U8      WaitForComplete                 : 1;   // If true, a flag will be polled to wait for completion of an action.
  U8      PersistentBuffer                : 1;   // Some operations require a buffer the size of the display. If this flag
                                                 // is true, this buffer will stay persistent in memory, improving overall
                                                 // performance. If this flag is false, the buffer will be dynamically
                                                 // allocated and freed with each operation.
} GUI_GPU_PXP_CONFIG;

#endif /* GUI_GPU_PIXELPIPELINE_H */

/*************************** End of file ****************************/
