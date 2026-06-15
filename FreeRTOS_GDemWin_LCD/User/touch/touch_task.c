/**
 *********************************************************************
 * @file    touch_task.c
 * @brief   FreeRTOS task: polls GT1151QM at 20 ms and feeds emWin
 *
 * Calls GTP_Init_Panel() once, then polls GTP_Execu() every 20 ms.
 * Touch events are translated to GUI_PID_StoreState() for emWin.
 *********************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include "GUI.h"
#include "touch/bsp_gt1151qm.h"

/*===========================================================================
 * Task stack and priority
 *===========================================================================*/
#define TOUCH_TASK_STACK_SIZE    512
#define TOUCH_TASK_PRIORITY      2       /* Same as GUITask */
#define TOUCH_POLL_PERIOD_MS     20      /* 50 Hz touch poll rate */

/*===========================================================================
 * TouchTask
 *===========================================================================*/

static void TouchTask(void *pvParameters)
{
    (void)pvParameters;
    int x, y;
    TickType_t xLastWake = xTaskGetTickCount();

    GTP_Init_Panel();

    while (1) {
        if (GTP_Execu(&x, &y) == 1) {
            GUI_PID_STATE State;
            State.x       = x;
            State.y       = y;
            State.Pressed = 1;
            State.Layer   = 0;
            GUI_PID_StoreState(&State);
        } else {
            GUI_PID_STATE State;
            State.x       = 0;
            State.y       = 0;
            State.Pressed = 0;
            State.Layer   = 0;
            GUI_PID_StoreState(&State);
        }
        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(TOUCH_POLL_PERIOD_MS));
    }
}

/*===========================================================================
 * Public API
 *===========================================================================*/

void TouchTask_Create(void)
{
    xTaskCreate(TouchTask, "TouchTask", TOUCH_TASK_STACK_SIZE,
                NULL, TOUCH_TASK_PRIORITY, NULL);
}
