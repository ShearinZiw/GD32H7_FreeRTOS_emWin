/**
 *********************************************************************
 * @file    bsp_ospi_flash.h
 * @brief   OSPI NOR Flash driver for GD32H7 (Quad-SPI capable)
 *
 * Pin mapping (BTB dev board, OSPIM Port 0):
 *   PB10 = CSN,  PA3 = SCK
 *   PF8  = IO0,  PF9 = IO1,  PE2 = IO2,  PA6 = IO3
 *
 * Supports: Single/Dual/Quad SPI read, page program, sector/block
 * erase, memory-mapped mode for direct read access.
 *********************************************************************
 */

#ifndef __BSP_OSPI_FLASH_H
#define __BSP_OSPI_FLASH_H

#include "gd32h7xx.h"

/* OSPI memory-mapped base address */
#define OSPI_MM_BASE            0x90000000U

/* Flash chip commands (W25Q / GD25Q compatible) */
#define FLASH_CMD_WREN          0x06   /* Write Enable */
#define FLASH_CMD_WRDI          0x04   /* Write Disable */
#define FLASH_CMD_RDSR          0x05   /* Read Status Register */
#define FLASH_CMD_RDID          0x9F   /* Read JEDEC ID */
#define FLASH_CMD_READ          0x03   /* Read Data (1-1-1) */
#define FLASH_CMD_FAST_READ     0x0B   /* Fast Read (1-1-1, 8 dummy) */
#define FLASH_CMD_PP            0x02   /* Page Program (1-1-1) */
#define FLASH_CMD_SE            0x20   /* Sector Erase (4KB) */
#define FLASH_CMD_BE32          0x52   /* Block Erase 32KB */
#define FLASH_CMD_BE64          0xD8   /* Block Erase 64KB */
#define FLASH_CMD_CE            0xC7   /* Chip Erase */

#define FLASH_SECTOR_SIZE       4096    /* 4 KB */
#define FLASH_BLOCK_SIZE        65536   /* 64 KB */
#define FLASH_PAGE_SIZE         256     /* Typical page size */

void       OSPI_Flash_Init(void);
uint32_t   OSPI_Flash_ReadID(void);
void       OSPI_Flash_Read(uint32_t addr, uint8_t *buf, uint32_t len);
void       OSPI_Flash_Write(uint32_t addr, const uint8_t *buf, uint32_t len);
void       OSPI_Flash_Erase_Sector(uint32_t addr);
void       OSPI_Flash_Erase_Block(uint32_t addr);
void       OSPI_Flash_Erase_Chip(void);
void       OSPI_Flash_MemoryMapped(void);

#endif /* __BSP_OSPI_FLASH_H */
