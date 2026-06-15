/**
 ******************************************************************************
 * @file        main.c
 * @brief       FreeRTOS + GDemWin + 5-inch LCD
 *
 * Key integration notes:
 *   - External memory pool via GUI_ALLOC_AssignMemory() is REQUIRED
 *   - BOARD_ConfigMPU() must configure SDRAM as non-cacheable for TLI DMA
 *   - LCD_Init renamed to BSP_LCD_Init to avoid conflict with emWin
 *   - FreeRTOS owns SysTick; do NOT call Systick_Int()
 *   - SVC/PendSV/SysTick handlers removed from gd32h7xx_it.c (FreeRTOS provides)
 *
 * Working hardware init sequence (from TLI_LCD_5inches):
 *   MPU → ICache/DCache → NVIC → USART → SDRAM → TLI LCD → LED → FreeRTOS
 ******************************************************************************
 */

#include "gd32h7xx.h"
#include "usart/bsp_usart.h"
#include "lcd/bsp_lcd.h"
#include "sdram/bsp_exmc_sdram.h"
#include "led/bsp_gpio_led.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "GUI.h"
#include "touch/touch_task.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t GUITask_Handle = NULL;
static TaskHandle_t LEDTask_Handle = NULL;

static void BOARD_ConfigMPU(void);
static void HW_Init(void);
static void AppTaskCreate(void *pvParameters);
static void GUITask(void *pvParameters);
static void LEDTask(void *pvParameters);
extern void MainTask(void);  /* Defined in AppWizard/Source/APPW_MainTask.c */

static void BOARD_ConfigMPU(void)
{
    MPU->RBAR = ARM_MPU_RBAR(1, 0xC0000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0,
                             ARM_MPU_REGION_SIZE_4MB);
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
    SCB_EnableICache();
    SCB_EnableDCache();
}

static void HW_Init(void)
{
    BOARD_ConfigMPU();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    USART_Config();
    printf("\r\n=== FreeRTOS + GDemWin + 5-inch LCD ===\r\n");
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    printf("SDRAM OK | ");
    BSP_LCD_Init();
    LCD_LayerInit();
    printf("LCD OK | ");
    LED_GPIO_Config();
    printf("LED OK\r\n");
    printf("Starting FreeRTOS...\r\n");
}

int main(void)
{
    HW_Init();
    xTaskCreate(AppTaskCreate, "AppTaskCreate", 512, NULL, 1, &AppTaskCreate_Handle);
    vTaskStartScheduler();
    while (1);
}

static void AppTaskCreate(void *pvParameters)
{
    (void)pvParameters;
    xTaskCreate(LEDTask, "LEDTask", 256, NULL, 1, NULL);
    TouchTask_Create();
    xTaskCreate(GUITask, "GUITask", 4096, NULL, 2, &GUITask_Handle);
    vTaskDelete(AppTaskCreate_Handle);
}

static void GUITask(void *pvParameters)
{
    (void)pvParameters;

    GUI_Init();

#if 0  /* Bare emWin demo — kept for reference, AppWizard is active path */
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_SetColor(GUI_BLUE);
    GUI_DispStringAt("FreeRTOS + GDemWin V6.52", 50, 30);
    unsigned int counter = 0;
    while (1) {
        char buf[64];
        GUI_SetColor(GUI_WHITE);
        GUI_FillRect(50, 140, 750, 200);
        GUI_SetFont(&GUI_Font20_ASCII);
        GUI_SetColor(GUI_BLACK);
        sprintf(buf, "Uptime: %u s", counter++);
        GUI_DispStringAt(buf, 50, 150);
        GUI_Delay(1000);
    }
#else
    MainTask();  /* AppWizard entry point — never returns */
#endif
}

static void LEDTask(void *pvParameters)
{
    (void)pvParameters;
    while (1) {
        LED1_TOGGLE;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
