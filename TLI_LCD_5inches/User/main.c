/**
  ******************************************************************************
  * @file        main.c
  * @author      embedfire
  * @version     V1.0
  * @date        2024
  * @brief       TLI_LCD液晶显示英文
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6-GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 

#include "gd32h7xx.h"
#include "usart/bsp_usart.h"
#include <stdio.h>
#include "dwt/bsp_dwt_delay.h"  
#include "lcd/bsp_lcd.h"
#include "systick/systick.h"
#include "sdram/bsp_exmc_sdram.h"

void BOARD_ConfigMPU(void);
    
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{   
    /* 初始化内存保护单元 */
    BOARD_ConfigMPU();
    
    /* configure systick */
    Systick_Int();    
    
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    
    /* 初始化调试串口 */
    USART_Config(); 
    
    /* 初始化SDRAM */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    
    printf("欢迎使用野火教程——TLI_LCD!\r\n");
    printf("\r\n屏幕测试例程\r\n");
    
    /*初始化液晶屏*/
    LCD_Init();
    /*初始化画板*/
    LCD_LayerInit();
	/*选择字体*/
	LCD_SetFont(&LCD_DEFAULT_FONT);
    
    while(1)
    {
 
        /*设置字体颜色及字体的背景颜色(此处的背景不是指LCD的背景层！注意区分)*/
        LCD_SetColors(LCD_COLOR565_BLACK,LCD_COLOR565_WHITE);
        LCD_DisplayStringLine(1,(uint8_t* )"Welcome to the YH Routine - TLI_LCD!");
        LCD_DisplayStringLine(2,(uint8_t* )"The driver currently only supports RGB565");
        LCD_DisplayStringLine(3,(uint8_t* )"Only enable Layer 0 ");
        LCD_DisplayStringLine(4,(uint8_t* )"Layer 1 need to add drivers by yourself");
        
        /* 画直线 */    
        LCD_SetColors(LCD_COLOR565_BLACK,LCD_COLOR565_WHITE);
        LCD_DisplayStringLine(5,(uint8_t* )"Draw line:");    
        LCD_SetTextColor(LCD_COLOR565_BLUE);    
        LCD_DrawLine(50,150,750,150);  
        LCD_SetTextColor(LCD_COLOR565_RED);
        LCD_DrawLine(50,200,750,200); 
        LCD_SetTextColor(LCD_COLOR565_GREEN); 
        LCD_DrawLine(300,150,400,300);  
        LCD_DrawLine(600,150,600,300);      

        /* 画圆 */
        LCD_SetColors(LCD_COLOR565_BLACK,LCD_COLOR565_WHITE);
        LCD_DisplayStringLine(9,(uint8_t* )"Draw Circle:");   
        LCD_SetTextColor(LCD_COLOR565_MAGENTA);     
        LCD_DrawCircle(500,350,50);
        LCD_SetTextColor(LCD_COLOR565_YELLOW);     
        LCD_DrawCircle(200,350,100);
        
        Delay_S(1);

    }
}
/* MPU configuration. */
void BOARD_ConfigMPU(void)
{
    /* Region 1 setting, set last 4MB of SDRAM can't be accessed by cache, glocal variables which are not expected to be
     * accessed by cache can be put here */
    /* Memory with Normal type, not shareable, non-cacheable */
    MPU->RBAR = ARM_MPU_RBAR(1, 0xC0000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_4MB);

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
    
    /* 使能cpu高速缓存*/ 
    SCB_EnableICache();     // 使能 I-Cache
    SCB_EnableDCache();     // 使能 D-Cache
    
}
/*********************************************END OF FILE**********************/
