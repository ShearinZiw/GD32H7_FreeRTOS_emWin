/*!
    \file    gd32h7xx_it.c
    \brief   interrupt service routines
    
    \version 2023-03-31, V1.0.0, demo for GD32H7xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32h7xx_it.h"
#include "main.h"
#include "systick/systick.h"
#include "usart/bsp_usart.h"
#include <stdio.h>

/* Quick blink on LED1 for visual fault indication */
static void FaultBlink(int n) {
    rcu_periph_clock_enable(RCU_GPIOD);
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_11);
    while (1) {
        for (int i = 0; i < n; i++) {
            gpio_bit_reset(GPIOD, GPIO_PIN_11);  /* LED ON */
            for (volatile int d = 0; d < 5000000; d++);
            gpio_bit_set(GPIOD, GPIO_PIN_11);    /* LED OFF */
            for (volatile int d = 0; d < 5000000; d++);
        }
        for (volatile int d = 0; d < 20000000; d++);  /* long pause between blinks */
    }
}

void NMI_Handler(void)
{
    printf("\r\n!!! NMI FAULT !!!\r\n");
    FaultBlink(1);
}

void HardFault_Handler(void)
{
    printf("\r\n!!! HARDFAULT !!!\r\n");
    FaultBlink(2);
}

void MemManage_Handler(void)
{
    printf("\r\n!!! MEMMANAGE FAULT !!!\r\n");
    FaultBlink(3);
}

void BusFault_Handler(void)
{
    printf("\r\n!!! BUSFAULT !!!\r\n");
    FaultBlink(4);
}

void UsageFault_Handler(void)
{
    printf("\r\n!!! USAGEFAULT !!!\r\n");
    FaultBlink(5);
}


/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

void USART0_IRQHandler(void)
{
    Debug_USART_IRQHandler();
}

/*
 * SVC_Handler, PendSV_Handler, and SysTick_Handler are provided by
 * FreeRTOS when FreeRTOS is used (via port.c + FreeRTOSConfig.h mapping).
 * They are NOT compiled here to avoid multiple-definition linker errors.
 *
 * When building WITHOUT FreeRTOS, uncomment the definitions below.
 */



/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHANDLER(void)
{

}*/
/*********************************************END OF FILE**********************/
