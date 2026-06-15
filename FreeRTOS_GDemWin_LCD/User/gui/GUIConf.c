/**
 *********************************************************************
 * @file    GUIConf.c
 * @brief   emWin runtime config — provides memory pool
 *
 * CRITICAL: GUI_ALLOC_AssignMemory() provides a dedicated memory pool
 * for emWin. Without it, the precompiled library's internal pool may
 * be too small, causing GUI_Init() to hang.
 *********************************************************************
 */

#include "GUI.h"

/* 64KB memory pool for emWin internal allocations */
static unsigned char _GUI_Memory[64 * 1024] __attribute__((aligned(32)));

void GUI_X_Config(void)
{
    GUI_ALLOC_AssignMemory(_GUI_Memory, sizeof(_GUI_Memory));
}
