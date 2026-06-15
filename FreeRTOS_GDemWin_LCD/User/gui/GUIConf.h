/**
 *********************************************************************
 * @file    GUIConf.h
 * @brief   emWin core configuration for GD32H7 + FreeRTOS
 *
 * This file is read by emWin's internal code at compile time via
 * GUI.h -> GUI_ConfDefaults.h. It configures what features emWin
 * compiles in and how it interacts with the OS and hardware.
 *********************************************************************
 */

#ifndef GUICONF_H
#define GUICONF_H

/*********************************************************************
*       Multi-tasking support
*       GUI_OS = 1: emWin calls GUI_X_Lock/Unlock (semaphore) before
*       every internal operation. Required when multiple tasks may
*       call emWin API. Safe even with single GUI task.
**********************************************************************/
#define GUI_OS                    1

/*********************************************************************
*       Number of display layers
*       Single layer (Layer 0 only) for our setup.
**********************************************************************/
#define GUI_NUM_LAYERS            1

/*********************************************************************
*       Touch support
*       GT1151QM touch driver integrated (I2C bit-bang on PH7/PH8).
*       TouchTask polls at 50 Hz and calls GUI_PID_StoreState().
**********************************************************************/
#define GUI_SUPPORT_TOUCH         1

/*********************************************************************
*       Window manager
*       Must be enabled to use widgets (BUTTON, EDIT, etc.) and
*       the WM_ API.
**********************************************************************/
#define GUI_WINSUPPORT            1

/*********************************************************************
*       Memory device support
*       Required for AppWizard. Without it, AppWizard.h:74 preprocessor
*       guard excludes all AppWizard types, causing build errors.
**********************************************************************/
#define GUI_SUPPORT_MEMDEV        1

/*********************************************************************
*       Default font
*       Used when no font is explicitly selected.
**********************************************************************/
#define GUI_DEFAULT_FONT          &GUI_Font6x8

/*********************************************************************
*       Dynamic memory pool size for emWin (bytes)
*       emWin uses this pool internally for windows, widgets,
*       clip rectangles, font cache, etc.
*       256KB for AppWizard + XBF font caching + multi-widget screens.
**********************************************************************/
#define GUI_ALLOC_SIZE            262144

/*********************************************************************
*       Color format
*       0 = ABGR format (standard emWin default)
*       1 = ARGB format
*       MUST be 1 for AppWizard — the precompiled GDemWin_CM7_OS_DP.lib
*       was built with ARGB byte order.
**********************************************************************/
#define GUI_USE_ARGB              1

/*********************************************************************
*       Memory device / custom draw support
*       0 = No hardware acceleration (DMA2D).
*       Set to 1 if you implement custom draw callbacks for DMA2D.
**********************************************************************/
#define GUI_MEMDEV_SUPPORT_CUSTOMDRAW  0

#endif /* GUICONF_H */
