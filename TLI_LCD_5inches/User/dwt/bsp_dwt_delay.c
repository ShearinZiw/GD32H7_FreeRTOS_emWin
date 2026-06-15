/**
  ******************************************************************
  * @file    bsp_dwt_delay.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   使用内核寄存器精确延时
  ******************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6-GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************
  */
  
#include "dwt/bsp_dwt_delay.h"   


/*
**********************************************************************
*         时间戳相关寄存器定义
**********************************************************************
*/
/*
 在Cortex-M里面有一个外设叫DWT(Data Watchpoint and Trace)，
 该外设有一个32位的寄存器叫CYCCNT，它是一个向上的计数器，
 记录的是内核时钟运行的个数，最长能记录的时间为：
 10.74s=2的32次方/400000000
 (假设内核频率为400M，内核跳一次的时间大概为1/400M=2.5ns)
 当CYCCNT溢出之后，会清0重新开始向上计数。
 使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 */


#define  DWT_CR      *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/**
  * @brief  初始化时间戳
  * @param  无
  * @retval 无
  * @note   使用延时函数前，必须调用本函数
  */
void HAL_InitTick(uint32_t TickPriority)
{
    /* 使能DWT外设 */
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

    /* DWT CYCCNT寄存器计数清0 */
    DWT_CYCCNT = (uint32_t)0u;

    /* 使能Cortex-M DWT CYCCNT寄存器 */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

/**
  * @brief  读取当前时间戳
  * @param  无
  * @retval 当前时间戳，即DWT_CYCCNT寄存器的值
  */
uint32_t CPU_TS_TmrRd(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}

/**
  * @brief  读取当前时间戳
  * @param  无
  * @retval 当前时间戳，即DWT_CYCCNT寄存器的值
  */
uint32_t HAL_GetTick(void)
{        
  return ((uint32_t)DWT_CYCCNT/SYSCLOCKFREQ*1000);
}


/**
  * @brief  采用CPU的内部计数实现精确延时，32位计数器
  * @param  us : 延迟长度，单位1 us
  * @retval 无
  * @note   使用本函数前必须先调用CPU_TS_TmrInit函数使能计数器，
            或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
            最大延时值为8秒，即8*1000*1000
  */
void CPU_TS_Tmr_Delay_US(uint32_t us)
{
  uint32_t ticks;
  uint32_t told,tnow,tcnt=0;

  /* 在函数内部初始化时间戳寄存器， */  
#if (CPU_TS_INIT_IN_DELAY_FUNCTION)  
  /* 初始化时间戳并清零 */
  HAL_InitTick(5);
#endif
  
  ticks = us * (GET_CPU_CLKFREQ() / 1000000);  /* 需要的节拍数 */      
  tcnt = 0;
  told = (uint32_t)CPU_TS_TmrRd();         /* 刚进入时的计数器值 */

  while(1)
  {
    tnow = (uint32_t)CPU_TS_TmrRd();  
    if(tnow != told)
    { 
        /* 32位计数器是递增计数器 */    
      if(tnow > told)
      {
        tcnt += tnow - told;  
      }
      /* 重新装载 */
      else 
      {
        tcnt += UINT32_MAX - told + tnow; 
      } 
      
      told = tnow;

      /*时间超过/等于要延迟的时间,则退出 */
      if(tcnt >= ticks)break;
    }  
  }
}

/*********************************************END OF FILE**********************/
