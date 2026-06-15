/**
 *********************************************************************
 * @file    GUI_X_FreeRTOS.c
 * @brief   emWin OS abstraction layer for FreeRTOS
 *********************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "GUI.h"

static SemaphoreHandle_t   _GUISemaphore;

/*===========================================================================
 * Timing
 *===========================================================================*/
int GUI_X_GetTime(void)
{
    return (int)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

void GUI_X_Delay(int Period)
{
    vTaskDelay(pdMS_TO_TICKS(Period));
}

void GUI_X_ExecIdle(void)
{
    vTaskDelay(pdMS_TO_TICKS(1));
}

/*===========================================================================
 * Multitasking (GUI_OS = 1)
 *===========================================================================*/
void GUI_X_InitOS(void)
{
    _GUISemaphore = xSemaphoreCreateCounting(1, 1);
}

void GUI_X_Lock(void)
{
    if (_GUISemaphore != NULL) {
        xSemaphoreTake(_GUISemaphore, portMAX_DELAY);
    }
}

void GUI_X_Unlock(void)
{
    if (_GUISemaphore != NULL) {
        xSemaphoreGive(_GUISemaphore);
    }
}

U32 GUI_X_GetTaskId(void)
{
    return (U32)xTaskGetCurrentTaskHandle();
}

/*===========================================================================
 * Init / Logging
 *===========================================================================*/
void GUI_X_Init(void) {}
void GUI_X_Log(const char *s)     { GUI_USE_PARA(s); }
void GUI_X_Warn(const char *s)    { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }
