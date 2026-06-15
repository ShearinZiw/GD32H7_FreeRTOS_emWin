/**
  ******************************************************************************
  * @file        main.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief      sdio sd卡测试实验（不含文件系统）。
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
#include "led/bsp_gpio_led.h"
#include "key/bsp_gpio_key.h"
#include "sdio/bsp_sdio.h"
#include "sdio/bsp_sdio_sdcard.h"
#include "systick/systick.h"
#include "dwt/bsp_dwt_delay.h"  
#include <stdio.h>
#include "ff.h"
#include "diskio.h"        /* FatFs lower layer API */

FATFS fs;						  /* FatFs文件系统对象 */
FIL fnew;						  /* 文件对象 */
FRESULT res_sd;                   /* 文件操作结果 */
UINT fnum;            			  /* 文件成功读写数量 */
BYTE read_buffer[1024]={0};       /* 读缓冲区 */
BYTE write_buffer[] = "关于野火\r\n\
  开源共享，共同进步\r\n\
  野火在发布第一块STM32开发板之初，就喊出开源共享，共同进步 的口号， 把代码和文档教程都免费提供给用户下载，而我们也一直把这个理念贯穿至今。\r\n\
  目前我们的产品已经包括 瑞萨RA系列、STM32、NXP i.MX RT系列、兆易创新、上海先楫、华芯微特、捷联微芯、瑞芯微、雅特力、FPGA、Linux、鲁班猫卡片电脑\r\n\
嵌入式操作系统教程、仿真器、脱机烧录器、开发工具 等分支，覆盖电子工程应用领域的各种常用技术， 其中教学类产品的代码和文档一直保持着开源的姿态发布\r\n\
到网络上， 为电子工程师排忧解难，让嵌入式没有难用的技术是我们最大的愿望。\r\n\
\r\n\
联系方式\r\n\
地址：东莞市大岭山镇石大路2号艺华综合办公大楼301 1 2 3 4楼\r\n\
\r\n\
官网： http://www.embedfire.com\r\n\
\r\n\
论坛： http://www.firebbs.cn\r\n\
\r\n\
资料： https://doc.embedfire.com/\r\n\
\r\n\
天猫： https://yehuosm.tmall.com\r\n\
\r\n\
京东： https://yehuo.jd.com/\r\n\
\r\n\
邮箱： embedfire@embedfire.com\r\n\
\r\n\
电话： 0769-89957037"; /* 写缓冲区*/
uint32_t __attribute__((aligned(32))) buf_write[128];       /* store the data written to the card */
uint32_t __attribute__((aligned(32))) buf_read[128];        /* store the data read from the card */                                         /* uncomment the macro to print out the data */

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* 使能cpu高速缓存*/ 
	SCB_EnableICache();     // 使能 I-Cache
	SCB_EnableDCache();     // 使能 D-Cache
    uint16_t i = 5;
    
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
      
    /* 初始化调试串口 */
    USART_Config(); 
    
    /* 初始化LED */
    LED_GPIO_Config();	    
    
    /* 初始化按键 */
    KEY_GPIO_Config();

    printf("\r\n欢迎使用野火教程——sdio sd卡测试实验（文件系统）!\r\n");
    
    res_sd = f_mount(&fs,ATA_BOOT_PATH,1);
  
    card_info_get();
/*----------------------- 格式化测试 -----------------*/  
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if(res_sd == FR_NO_FILESYSTEM)
    {
        printf("\r\n\r\n》SD卡还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        res_sd=f_mkfs(ATA_BOOT_PATH,0,0);							

        if(res_sd == FR_OK)
        {
            res_sd = FR_TIMEOUT;
            printf("\r\n》SD卡已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
            while(res_sd != FR_OK)
            {
                res_sd = f_mount(NULL ,ATA_BOOT_PATH,1);
                printf("\r\n！！SD卡挂载文件系统失败。(%d)",res_sd);
            }
            res_sd = FR_TIMEOUT;	
            /* 重新挂载	*/	
            while(res_sd == FR_NO_FILESYSTEM)
            {
                
                res_sd = f_mount(&fs,ATA_BOOT_PATH,1);
            }            
        }
        else
        {
            printf("\r\n《《格式化失败。》》\r\n");
            while(1);
        }
    }
    else if(res_sd!=FR_OK)
    {
        printf("\r\n\r\n！！SD卡挂载文件系统失败。(%d)",res_sd);
        printf("\r\n！！可能原因： SD卡初始化不成功。\r\n");
        printf("请下载 SDIO-SD读写测试 例程测试，如果正常，在该例程f_mount语句下if语句前临时多添加一句 res_sd = FR_NO_FILESYSTEM; 让重新直接执行格式化流程\r\n");
        while(1);
    }
    else
    {
        printf("\r\n\r\n》文件系统挂载成功，可以进行读写测试\r\n");
    }
     
    /*----------------------- 文件系统测试：写测试 -------------------*/
	/* 打开文件，每次都以新建的形式打开，属性为可写 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");
    
    sd_lock_unlock(SD_UNLOCK);	
	res_sd = f_open(&fnew, "0:/FATFS.TXT",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		printf("》打开/创建文件成功，向FATFS.TXT文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
		res_sd=f_write(&fnew,write_buffer,sizeof(write_buffer),&fnum);
        if(res_sd==FR_OK)
        {
          printf("》文件写入成功，写入字节数据：%d\n",fnum);
          printf("》向文件写入的数据为：%s\r\n",write_buffer);
        }
        else
        {
          printf("！！文件写入失败：(%d)\n",res_sd);
        }    
            /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {	
        printf("！！打开/创建文件失败：(%d)\n。\r\n",res_sd);
    }
/*------------------- 文件系统测试：读测试 --------------------------*/
	printf("\r\n****** 即将进行文件读取测试... ******\r\n");
	res_sd = f_open(&fnew,"0:/FATFS.TXT",FA_OPEN_EXISTING | FA_READ); 	    
	if(res_sd == FR_OK)
	{
		printf("》打开文件成功。\r\n");
		res_sd = f_read(&fnew, read_buffer, sizeof(read_buffer), &fnum); 
        if(res_sd==FR_OK)
        {
          printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
          printf("》读取得的文件数据为：\r\n%s \r\n", read_buffer);	
        }
        else
        {
          printf("！！文件读取失败：(%d)\n",res_sd);
        }		
	}
	else
	{
		printf("！！打开文件失败(%d)\n。\r\n",res_sd);
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
  
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,ATA_BOOT_PATH,1);
  
    /* 操作完成，停机 */
	while(1)
	{
	}

}



/*********************************************END OF FILE**********************/
