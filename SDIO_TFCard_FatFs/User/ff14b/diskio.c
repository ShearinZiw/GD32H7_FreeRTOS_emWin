/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"        /* FatFs lower layer API */
#include "ff.h"
#include "sdio/bsp_sdio_sdcard.h"
#include <string.h>
#include <stdio.h>

#define ATA                       0     /* Map MMC/SD card to physical drive */
#define SPI_FLASH                 1     /* Map SPI FLASH to physical drive */
#define INTERNAL_FLASH            2     /* Map MCU INTERNAL FLASH to physical drive */

#define BLOCKSIZE                 512   /* block size in bytes */

#define BUSMODE_1BIT    0
#define BUSMODE_4BIT    1

/* SDIO bus switch */
/* config SDIO bus mode, select: BUSMODE_1BIT/BUSMODE_4BIT */
#define SDIO_BUSMODE        BUSMODE_4BIT
/* config SDIO speed mode, select: SD_SPEED_DEFAULT/SD_SPEED_HIGH */
#define SDIO_SPEEDMODE      SD_SPEED_HIGH
/* config data transfer mode, select: SD_POLLING_MODE/SD_DMA_MODE */
#define SDIO_DTMODE         SD_POLLING_MODE


#define SPI_FLASH_ID              0xC84015U

#define SPI_FLASH_SECTOR_SIZE     4096
#define SPI_FLASH_SECTOR_NUM      512
#define SPI_FLASH_BLOCK_SIZE      1

#define INTERFLASH_SECTOR_SIZE    2048
#define INTERFLASH_SECTOR_NUM     128   //at least 128
#define INTERFLASH_BLOCK_SIZE     1
#define INTERFLASH_START_ADDRESS  0x08020000

extern sd_card_info_struct sd_cardinfo;

#define CACHE_LINE_BITS           0x1FU

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
    BYTE pdrv        /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS status = STA_NOINIT;

    switch(pdrv) {
    case ATA:    /* SD CARD */
        status = RES_OK;
        break;

    case SPI_FLASH:
//            if(SPI_FLASH_ID == spi_flash_read_id())
    {
        status = RES_OK;
    }
    break;
    case INTERNAL_FLASH:
        status = RES_OK;
        break;
    default:
        status = STA_NOINIT;
    }
    return status;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(
    BYTE pdrv          /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS status = STA_NOINIT;
    uint32_t cardstate = 0;
    uint32_t i = 0;
    switch(pdrv) {
    case ATA:
        /* SD CARD */
        /* initialize the card */
        status = sd_init();

        if(SD_OK == status) {
            status = sd_card_information_get(&sd_cardinfo);
        } else {
            return STA_NOINIT;
        }
        if(SD_OK == status) {
            status = sd_card_select_deselect(sd_cardinfo.card_rca);
        } else {
            return STA_NOINIT;
        }
        status = sd_cardstatus_get(&cardstate);
        if(cardstate & 0x02000000) {
            /* the card is locked */
            printf("\r\n the card is locked!");
            status = sd_lock_unlock(SD_UNLOCK);
            if(status != SD_OK) {
                return STA_NOINIT;
            } else {
                printf("\r\n the card is unlocked successfully!");
            }
        }

        /* configure the bus mode and data transfer mode */
        if(SD_OK == status) {
            /* set bus mode */
#if (SDIO_BUSMODE == BUSMODE_4BIT)
			status = sd_bus_mode_config(SDIO_BUSMODE_4BIT, SDIO_SPEEDMODE);
#else
			status = sd_bus_mode_config(SDIO_BUSMODE_1BIT, SDIO_SPEEDMODE);
#endif
        } else {
            return STA_NOINIT;
        }

        if(SD_OK == status) {
            /* set data transfer mode */
#if (SDIO_DTMODE == SD_DMA_MODE)
            status = sd_transfer_mode_config(SD_DMA_MODE);
            /* configure the SDIO NVIC */
            nvic_irq_enable(SDIO0_IRQn, 0, 0);
#else
            status = sd_transfer_mode_config(SD_POLLING_MODE);
#endif 
        } else {
            return STA_NOINIT;
        }

        if(SD_OK == status) {
            /* initialize success */
            return 0;
        } else {
            return STA_NOINIT;
        }

    case SPI_FLASH:         /* SPI Flash */
//            spi_flash_init();
        status = RES_OK;
        break;

    case INTERNAL_FLASH:
        fmc_unlock();
        status = RES_OK;
        break;

    default:
        status = STA_NOINIT;
    }
    return status;
}

//static __attribute__((section(".first_data"))) uint8_t pucDMABuffer[ 512 ] __attribute__((aligned(32)));

uint8_t __attribute__((aligned(32))) pucDMABuffer[512];  


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    BYTE *buff,       /* Data buffer to store read data */
    DWORD sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to read */
)
{
    sd_error_enum status = SD_ERROR;

//    printf("disk_read:%lx %d \r\n", sector, count);



    /* check the correctness of the parameters */
    if(NULL == buff) {
        return RES_PARERR;
    }
    if(!count) {
        return RES_PARERR;
    }

    switch(pdrv) {
    case ATA:    /* SD CARD */

#if (SDIO_DTMODE == SD_DMA_MODE)
        if((((uintptr_t) buff) & CACHE_LINE_BITS) == 0U) {
            /* The buffer is word-aligned. */
            if(1 == count) {
                /* single sector write */
                status = sd_block_read((uint32_t *)(&buff[0]), sector , BLOCKSIZE);
            } else {
                /* multiple sectors read */
                status = sd_multiblocks_read((uint32_t *)(&buff[0]), sector, BLOCKSIZE, count);
            }
            if(status == SD_OK) {
                /* clean and invalidate buffer in D-Cache */
                SCB_CleanInvalidateDCache_by_Addr((uint32_t *)buff, 512 * count);
            }
        } else {
            /* The buffer is NOT word-aligned. */
            uint32_t ulSector;
            for(ulSector = 0; ulSector < count; ulSector++) {

                status = sd_block_read((uint32_t *)(pucDMABuffer), (sector + ulSector), BLOCKSIZE);
                if(status == SD_OK) {
                    /* clean and invalidate buffer in D-Cache */
                    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)pucDMABuffer, 512);

                    memcpy(buff + 512U * ulSector, pucDMABuffer, 512U);

                }
            }

        }
#else
        if(1 == count) {
            /* single sector read */
            status = sd_block_read((uint32_t *)(&buff[0]), (uint32_t)sector, BLOCKSIZE);
        } else {
            /* multiple sectors read */
            status = sd_multiblocks_read((uint32_t *)(&buff[0]), (uint32_t)sector, BLOCKSIZE, (uint32_t)count);
        }
#endif /* DMA_MODE */

        if(SD_OK == status) {
            return RES_OK;
        }
        return RES_ERROR;

    case SPI_FLASH:
//            spi_flash_buffer_read(buff, sector*SPI_FLASH_SECTOR_SIZE, count*SPI_FLASH_SECTOR_SIZE);
        return RES_OK;

    case INTERNAL_FLASH: {
        uint8_t *pSource = (uint8_t *)(sector * INTERFLASH_SECTOR_SIZE + INTERFLASH_START_ADDRESS);
        uint16_t i;

        while(count--) {
            for(i = 0; i < INTERFLASH_SECTOR_SIZE; i++) {
                *buff++ = *pSource++;
            }
        }
    }
    return RES_OK;

    default:
        return RES_PARERR;
    }
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write(
    BYTE pdrv,              /* Physical drive nmuber to identify the drive */
    const BYTE *buff,       /* Data to be written */
    DWORD sector,           /* Start sector in LBA */
    UINT count              /* Number of sectors to write */
)
{
    uint32_t write_addr;
    sd_error_enum status = SD_ERROR;



//    printf("disk_write:%lx %d\r\n", sector, count);


    if(!count) {
        return RES_PARERR;        /* Check parameter */
    }

    switch(pdrv) {
    case ATA:    /* SD CARD */

#if (SDIO_DTMODE == SD_DMA_MODE)			
				 if((((uintptr_t) buff) & CACHE_LINE_BITS) == 0U) {
					 /* clean and invalidate buffer in D-Cache */
					 SCB_CleanInvalidateDCache_by_Addr((uint32_t *)buff, 512 * count);
			
					 /*NO DMA*/
					 if(1 == count) {
						 /* single sector write */
						 status = sd_block_write((uint32_t *)buff, sector , BLOCKSIZE);
					 } else {
						 /* multiple sectors write */
						 status = sd_multiblocks_write((uint32_t *) buff, sector, BLOCKSIZE, count);
					 }
				 } else {
					 /* The buffer is NOT word-aligned. */
					 uint32_t ulSector;
			
					 for(ulSector = 0; ulSector < count; ulSector++) {
						 memcpy(pucDMABuffer, buff + 512U * ulSector, 512U);
			
						 /* clean and invalidate buffer in D-Cache */
						 SCB_CleanInvalidateDCache_by_Addr((uint32_t *)pucDMABuffer, 512);
			
						 status = sd_block_write((uint32_t *)(pucDMABuffer), (sector + ulSector), BLOCKSIZE);
			
						 if(status != SD_OK) {
							 break;
						 }
					 }
				 }
			
#else
        /*NO DMA*/
        if(1 == count) {
            /* single sector write */
            status = sd_block_write((uint32_t *)buff, sector, BLOCKSIZE);
        } else {
            /* multiple sectors write */
            status = sd_multiblocks_write((uint32_t *)buff, sector, BLOCKSIZE, (uint32_t)count);
        }

#endif /* DMA_MODE */

        if(SD_OK == status) {
            return RES_OK;
        }
        return RES_ERROR;

    case SPI_FLASH:
        write_addr = sector * SPI_FLASH_SECTOR_SIZE;
//            spi_flash_sector_erase(write_addr);
//            spi_flash_buffer_write((uint8_t *)buff,write_addr,count*SPI_FLASH_SECTOR_SIZE);
        return RES_OK;

    case INTERNAL_FLASH: {
        uint32_t i, page;
        uint32_t start_page = sector * INTERFLASH_SECTOR_SIZE + INTERFLASH_START_ADDRESS;
        uint32_t *ptrs = (uint32_t *)buff;

        page = count;

        for(; page > 0; page--) {
            fmc_sector_erase(start_page);
            i = 0;
            do {
                fmc_word_program(start_page, *ptrs++);
                start_page += 4;
            } while(++i < INTERFLASH_SECTOR_SIZE / 4);
        }
    }
    return RES_OK;

    default:
        return RES_PARERR;
    }
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT disk_ioctl(
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE cmd,         /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    uint32_t capacity;
    DRESULT status = RES_PARERR;
    switch(pdrv) {
    case ATA:    /* SD CARD */
        switch(cmd) {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE :
            *(WORD *)buff = BLOCKSIZE;
            break;
        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE :
            *(DWORD *)buff = 1; //sd_cardinfo.card_blocksize;
            break;

        case GET_SECTOR_COUNT:
            capacity = sd_card_capacity_get();
            *(DWORD *)buff = capacity * 1024 / sd_cardinfo.card_blocksize;
            break;
        case CTRL_SYNC :
            break;
        }
        return RES_OK;

    case SPI_FLASH:
        switch(cmd) {
        /* 512*4096/1024/1024=2(MB), GD25Q16B size: 2MB */
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SPI_FLASH_SECTOR_NUM;
            break;
        /* sector size */
        case GET_SECTOR_SIZE :
            *(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
            break;
        /* block size */
        case GET_BLOCK_SIZE :
            *(DWORD *)buff = SPI_FLASH_BLOCK_SIZE;
            break;
        }
        return RES_OK;

    case INTERNAL_FLASH:
        switch(cmd) {
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = INTERFLASH_SECTOR_NUM;
            break;

        case GET_SECTOR_SIZE :
            *(WORD *)buff = INTERFLASH_SECTOR_SIZE;
            break;

        case GET_BLOCK_SIZE :
            *(DWORD *)buff = INTERFLASH_BLOCK_SIZE;
            break;
        }
        return RES_OK;

    default:
        status = RES_PARERR;
    }
    return status;
}
#endif

DWORD get_fattime(void)
{
    return ((DWORD)(2023 - 1980) << 25)       /* Year 2023 */
           | ((DWORD)8 << 21)                /* Month 1 */
           | ((DWORD)13 << 16)                /* Mday 1 */
           | ((DWORD)14 << 11)                /* Hour 0 */
           | ((DWORD)15 << 5)                 /* Min 0 */
           | ((DWORD)96 >> 1);                /* Sec 0 */
}
