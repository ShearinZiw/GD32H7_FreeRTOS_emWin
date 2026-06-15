/**
  ******************************************************************************
  * @file        bsp_sdio_test.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief      SDIO对SD卡的测试接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6 GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 

#include "sdio/bsp_sdio_sdcard.h"
#include "sdio/bsp_sdio_test.h"
#include "led/bsp_gpio_led.h"
#include "usart/bsp_usart.h"
#include "systick/systick.h"

//uint32_t buf_write[MULTI_BUFFER_SIZE];       /* store the data written to the card */
//uint32_t buf_read[MULTI_BUFFER_SIZE];        /* store the data read from the card */
uint32_t __attribute__((aligned(32))) buf_write_temp[BLOCK_32BIT_NUM];       /* store the data written to the card */
uint32_t __attribute__((aligned(32))) buf_write[BLOCK_32BIT_NUM];       /* store the data written to the card */
uint32_t __attribute__((aligned(32))) buf_read[BLOCK_32BIT_NUM];        /* store the data read from the card */
sd_error_enum sd_error;
ErrStatus state = ERROR;
extern sd_card_info_struct sd_cardinfo;                            /* information of SD card */

#ifdef DATA_PRINT
    uint8_t *pdata;
#endif /* DATA_PRINT */

/*!
    \brief      数据对比函数
    \param[in]  src : 参考数据
    \param[in]  dst : 待对比数据
    \param[in]  length : 对比长度
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint32_t *src, uint32_t *dst, uint16_t length)
{
    while(length--) 
    {
        if(*src++ != *dst++) 
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

 /**
  * @brief  初始化控制SDIO的IO测试
  * @param  无
  * @retval 无
  */
void SD_FormatInternal_Test(void)
{
    uint8_t i = 5;
    LED_1(ON);
    LED_2(ON);
    /* turn off all the LEDs */
    LED_1(OFF);
    LED_2(OFF);
    /* sd卡初始化 */                  
    do {
        sd_error = sd_io_init();
    } while((SD_OK != sd_error) && (--i));

    if(i) 
    {
        printf("\r\n SD卡初始化成功!\r\n");
    } 
    else 
    {
        printf("\r\n\r\n SD卡初始化失败!\r\n\r\n ");
        /* turn on LED1, LED2 */
        LED_1(ON);
        LED_2(ON);
        while(1);
    }
}

 /**
  * @brief  SD卡的上锁/解锁测试
  * @param  无
  * @retval 无
  */
void SD_Unlock_Lock_Test(void)
{
    uint8_t i;
    uint8_t lock_flag;
    printf("\r\n\r\n 开解锁测试开始：");
    /* 上锁与解锁测试 */
    if(SD_CCC_LOCK_CARD & sd_cardinfo.card_csd.ccc) 
    {
        /* 上锁 */
        sd_error = sd_lock_unlock(SD_LOCK);
        if(SD_OK != sd_error) 
        {
            printf("\r\n Lock failed!");
            /* 打开 LED1, LED2 */
            LED_1(ON);
            LED_2(ON);
            while(1);
        } 
        else 
        {
            printf("\r\n The card is locked!");
        }
        
        lock_flag =2;
        //测试是否上锁成功,上锁后不能擦除和读写
        for(i = 0;i<2;i++)
        {
            sd_error = sd_erase((BLOCK_SIZE * NUMBER_OF_BLOCKS_START), (BLOCK_SIZE * (NUMBER_OF_BLOCKS_START+1)));
            if(SD_OK != sd_error)
            {
                lock_flag--;
            }
            sd_error = sd_block_read(buf_read, (BLOCK_SIZE * NUMBER_OF_BLOCKS_START), BLOCK_32BIT_NUM);
            if(SD_OK != sd_error) 
            {
                lock_flag--;
            } 
        }
        if(lock_flag == 2)
        {
            printf("\r\n test of Lock is failed!");
        }
        else
        {
            printf("\r\n test of Lock is success!");
        }

        
//        /* 解锁 */
        sd_error = sd_lock_unlock(SD_UNLOCK);
        if(SD_OK != sd_error) 
        {
            printf("\r\n Unlock failed!");
            /* turn on LED1, LED2 */
            LED_1(ON);
            LED_2(ON);
            while(1);
        } 
        else 
        {
            printf("\r\n The card is unlocked!");
        }
        
        lock_flag =4;
        //测试是否解锁成功,解锁后可擦除和读写
        for(i = 0;i<2;i++)
        {
            sd_error = sd_erase((BLOCK_SIZE * NUMBER_OF_BLOCKS_START), (BLOCK_SIZE * (NUMBER_OF_BLOCKS_START+1)));
            if(SD_OK == sd_error)
            {
                lock_flag--;
            }
            sd_error = sd_block_read(buf_read, (BLOCK_SIZE * NUMBER_OF_BLOCKS_START), MULTI_BUFFER_SIZE);
            if(SD_OK == sd_error) 
            {
                lock_flag--;
            } 
        }
        if(lock_flag == 4)
        {
            printf("\r\n test of Lock is failed!");
        }
        else
        {
            printf("\r\n test of Lock is success!");
        }
    }
}

 /**
  * @brief  SD卡的单块擦除
  * @param  无
  * @retval 无
  */
void SD_SingleBlock_EraseTest(uint32_t number_of_block)
{
    uint32_t i;
    uint8_t *pdata1;
    uint32_t startaddr = number_of_block*BLOCK_SIZE;;
    uint32_t endaddr = (number_of_block+1)*BLOCK_SIZE;
    
    sd_error = sd_erase(startaddr, endaddr);
    if(SD_OK != sd_error) 
    {
        SD_SingleBlock_ReadTest(startaddr);
        SCB_CleanInvalidateDCache_by_Addr(buf_read, (endaddr-startaddr)*4);
        pdata1 = (uint8_t *)buf_read;
        for(i = 0; i < (endaddr-startaddr); i++) 
        {
            if(*pdata1 != 0x0 )
            {
                printf("\r\n Erase failed!");
            }
            pdata1++;
        }
        LED_1(ON);
        LED_2(ON);
    } 
    else 
    {
        printf("\r\n Erase success!");
    }
}

 /**
  * @brief  SD卡的单块读测试
  * @param  无
  * @retval 无
  */
void SD_SingleBlock_ReadTest(uint32_t number_of_block)
{
    uint32_t startaddr = number_of_block*BLOCK_SIZE;;
    uint32_t endaddr = (number_of_block+1)*BLOCK_SIZE;
    sd_error = sd_block_read(buf_read,  startaddr, BLOCK_32BIT_NUM);
    if(SD_OK != sd_error) 
    {
        printf("\r\n Block read fail!");
        /* 打开 LED1, LED2 */
        LED_1(ON);
        LED_2(ON);
        while(1);
    } 
    else 
    {
        printf("\r\n Block read success!");
       #ifdef DATA_PRINT
            uint8_t i;
            uint8_t *pdata2;
            SCB_CleanInvalidateDCache_by_Addr(buf_read, BLOCK_SIZE);
            pdata2 = (uint8_t *)buf_read;
            /* print data by USART */
            printf("\r\n");
            for(i = 0; i < BLOCK_SIZE/4; i++) 
            {
                printf(" 0x%02x%02x%02x%02x ", *(pdata2 + 3), *(pdata2 + 2), *(pdata2 + 1), *pdata2);
                pdata2 += 4;
                if(0 == (i + 1) % 4) 
                {
                    printf("\r\n");
                }
                if(0 == (i + 1) % BLOCK_SIZE) 
                {
                    printf("\r\n");
                }
            
            }
        #endif /* DATA_PRINT */  
    }
    
}

 /**
  * @brief  SD卡的单块写测试
  * @param  无
  * @retval 无
  */
void SD_SingleBlock_WriteTest(uint32_t number_of_block)
{
    uint32_t i_temp;
    uint32_t startaddr = number_of_block*BLOCK_SIZE;;
    uint32_t endaddr = (number_of_block+1)*BLOCK_SIZE;
    sd_error = sd_block_write(buf_write, startaddr, BLOCK_32BIT_NUM);
    if(SD_OK != sd_error) 
    {
        printf("\r\n Block write fail!");
        /* turn on LED1, LED2 */
        LED_1(ON);
        LED_2(ON);
        while(1);
    } 
    else 
    {
        printf("\r\n Block write success!");
    }    

}


 /**
  * @brief  SD卡的单块擦/读/写测试
  * @param  无
  * @retval 返回1:测试失败，0测试成功
  */
uint32_t SD_SingleBlock_Test(uint32_t number_of_block)
{
    uint32_t i = 0;
    printf("\r\n 第%d块擦/读/写测试开始：",number_of_block);
    /* 擦测试 */
    SD_SingleBlock_EraseTest(number_of_block);
    SD_Data_Preparationt();
    /* 写测试 */
    SD_SingleBlock_WriteTest(number_of_block);
    /* 读测试 */
    SD_SingleBlock_ReadTest(number_of_block);

    if(SUCCESS != memory_compare(buf_read, buf_write,BLOCK_32BIT_NUM)) 
    {
        printf("\r\n  compare of memory is error!\r\n ");
        i = 1;
    }
    else
    {
    
        printf("\r\n  compare of memory is success!\r\n ");
        i = 0;
    
    }
    
    /* 擦测试 */
    SD_SingleBlock_EraseTest(number_of_block); 
    
    return 1;

}

 /**
  * @brief  SD卡的多块擦/读/写测试
  * @param  无
  * @retval 无
  */
void SD_MultiBlock_Test(uint32_t number_of_startblock, uint32_t number_of_endblock)
{
    uint32_t i;
    /* 多次连续使用SD卡的单块擦/读/写测试来完成多块操作 */
    printf("\r\n\r\n第%d块到%d擦/读/写测试开始(含首尾块序)：",number_of_startblock,number_of_endblock);
    for(i = number_of_startblock;i<=number_of_endblock;i++)
    {
        if(SD_SingleBlock_Test(i))
        {
            printf("\r\n Block%d is test success!\r\n",i);
        }
    }
    
}
 /**
  * @brief  写数据缓冲区的准备
  * @param  无
  * @retval 无
  */
void SD_Data_Preparationt(void)
{
    uint32_t i_temp;
    /* 准备写数据 */
    for(i_temp = 0; i_temp < BLOCK_32BIT_NUM; i_temp++) 
    {
        buf_write[i_temp] = i_temp+1;
        buf_write_temp[i_temp] = buf_write[i_temp];
    }
    printf("\r\n buf_write_data is ready\r\n");
    /* clean and invalidate buffer in D-Cache */
    SCB_CleanInvalidateDCache_by_Addr(buf_write, BLOCK_SIZE);//(endaddr-startaddr)*4);
    #ifdef DATA_PRINT
        for(i_temp = 0; i_temp < BLOCK_32BIT_NUM; i_temp++) 
        {
            printf(" 0x%08x ", buf_write_temp[i_temp]);
            if(0 == (i_temp + 1) % 4) 
            {
                printf("\r\n");
            }
        }
    #endif /* DATA_PRINT */     
    
}


/*********************************************END OF FILE**********************/
