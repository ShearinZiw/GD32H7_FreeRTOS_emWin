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
 *   MPU → ICache/DCache → NVIC → USART → SDRAM → SD+FatFS → TLI LCD → LED
 *   → FreeRTOS
 ******************************************************************************
 */

#include "gd32h7xx.h"
#include "usart/bsp_usart.h"
#include "lcd/bsp_lcd.h"
#include "sdram/bsp_exmc_sdram.h"
#include "led/bsp_gpio_led.h"
#include "led/bsp_buzzer.h"
#include "sdio/bsp_sdio_sdcard.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "GUI.h"
#include "touch/touch_task.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"

extern int FS_Init(void);   /* GUI_X_FS.c — mounts SD + OSPI */

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
    /* Region 1: SDRAM 0xC0000000, 4 MB, non-cacheable (TLI DMA) */
    MPU->RBAR = ARM_MPU_RBAR(1, 0xC0000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0,
                             ARM_MPU_REGION_SIZE_4MB);
    /* Region 2: OSPI Flash MM 0x90000000, 256 MB, non-cacheable */
    MPU->RBAR = ARM_MPU_RBAR(2, 0x90000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0,
                             ARM_MPU_REGION_SIZE_256MB);
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

    /* Initialize SD card and mount FatFS */
    if (disk_initialize(0) == RES_OK) {
        printf("SD OK | ");
    } else {
        printf("SD FAIL | ");
    }
    FS_Init();

    BSP_LCD_Init();
    LCD_LayerInit();
    printf("LCD OK | ");
    LED_GPIO_Config();
    Buzzer_GPIO_Config();
    printf("LED OK | BEEP OK\r\n");
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
    xTaskCreate(GUITask, "GUITask", 8192, NULL, 2, &GUITask_Handle);
    vTaskDelete(AppTaskCreate_Handle);
}

static void GUITask(void *pvParameters)
{
    (void)pvParameters;

    GUI_Init();

    MainTask();  /* AppWizard entry point — never returns */
}

static void LEDTask(void *pvParameters)
{
    (void)pvParameters;
    while (1) {
        LED1_TOGGLE;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
