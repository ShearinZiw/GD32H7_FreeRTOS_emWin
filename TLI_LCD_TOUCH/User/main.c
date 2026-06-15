/**
  ******************************************************************************
  * @file        main.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief      OLED测试
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
#include "systick/systick.h"
#include "lcd/bsp_lcd_gt1151qm.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    uint8_t i;
    /* 使能cpu高速缓存*/ 
    SCB_EnableDCache();     // 使能 D-Cache
    SCB_EnableICache();     // 使能 I-Cache
    
    /* 嵌套向量中断控制器组选择 */
    /* 提示 NVIC_PriorityGroupConfig() 在整个工程只需要调用一次来配置优先级分组*/
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    
    /* 初始化调试串口 */
    USART_Config();

    /* 初始化触摸屏 */
    GTP_Init_Panel();
    
    printf("\r\n提示：\r\n"
     "如果是4.3寸/5寸的RGB屏幕，使用的触摸芯片可能是GT1151QM。\r\n"
     "如果触摸或显示异常，请确认使用的屏幕触摸芯片型号，"
     "在“bsp_i2c_touch.h”文件修改宏定义 LCD_TOUCH_IC_GT1151QM。\r\n\r\n");

    printf("欢迎使用野火教程——LCD触摸屏检测!\r\n");
    
    Systick_Int();  //使用节拍的定时器中断函数触摸检测
    
    while(1)
	{	
        
        
	}
}
/*********************************************END OF FILE**********************/
