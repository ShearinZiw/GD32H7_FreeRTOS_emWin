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
*       Disabled for initial build. Enable when GT1151QM driver is
*       added and GUI_PID_StoreState() is called.
*       When set to 1, also add GUI_X_Touch.c to the project.
**********************************************************************/
#define GUI_SUPPORT_TOUCH         0

/*********************************************************************
*       Window manager
*       Must be enabled to use widgets (BUTTON, EDIT, etc.) and
*       the WM_ API.
**********************************************************************/
#define GUI_WINSUPPORT            1


/*********************************************************************
*       Default font
*       Used when no font is explicitly selected.
**********************************************************************/
#define GUI_DEFAULT_FONT          &GUI_Font6x8

/*********************************************************************
*       Dynamic memory pool size for emWin (bytes)
*       emWin uses this pool internally for windows, widgets,
*       clip rectangles, font cache, etc.
*       64KB is sufficient for moderate widget counts.
**********************************************************************/
#define GUI_ALLOC_SIZE            65536

/*********************************************************************
*       Color format
*       0 = ABGR format (standard emWin default)
*       1 = ARGB format
*       Keep at 0 for standard compatibility.
**********************************************************************/
#define GUI_USE_ARGB              0

/*********************************************************************
*       Memory device / custom draw support
*       0 = No hardware acceleration (DMA2D).
*       Set to 1 if you implement custom draw callbacks for DMA2D.
**********************************************************************/
#define GUI_MEMDEV_SUPPORT_CUSTOMDRAW  0

#endif /* GUICONF_H */
