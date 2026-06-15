/**
 *********************************************************************
 * @file    bsp_ospi_flash.c
 * @brief   OSPI NOR Flash driver (Quad-SPI, GD25Q/W25Q compatible)
 *
 * Pin mapping (BTB dev board, OSPIM Port 0) — verified from Fire demo:
 *   PA3  = SCK (AF12),  PB10 = CSN (AF9)
 *   PF8  = IO0 (AF10),  PF9  = IO1 (AF10)
 *   PE2  = IO2 (AF9),   PA6  = IO3 (AF6)
 *
 * Reference: OSPI_Flash_FatFs project (EmbedFire GD32H7 demo)
 *********************************************************************
 */

#include "ospi/bsp_ospi_flash.h"
#include <stdio.h>

static void _OSPI_WaitBusy(void);
static void _OSPI_WriteEnable(void);

/*===========================================================================
 * GPIO + OSPIM init (verified against Fire bsp_ospi.c)
 *===========================================================================*/

static void _OSPI_GPIO_Config(void)
{
    rcu_periph_clock_enable(RCU_OSPIM);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* OSPIM_P0_CLK (PA3) — AF12 */
    gpio_af_set(GPIOA, GPIO_AF_12, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_3);

    /* OSPIM_P0_NCS (PB10) — AF9 */
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_10);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_10);

    /* OSPIM_P0_IO0 (PF8), OSPIM_P0_IO1 (PF9) — AF10 */
    gpio_af_set(GPIOF, GPIO_AF_10, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8 | GPIO_PIN_9);

    /* OSPIM_P0_IO2 (PE2) — AF9 */
    gpio_af_set(GPIOE, GPIO_AF_9, GPIO_PIN_2);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_2);

    /* OSPIM_P0_IO3 (PA6) — AF6 */
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_6);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_6);
}

static void _OSPIM_Config(void)
{
    ospim_deinit();

    ospim_port_sck_config(OSPIM_PORT0, OSPIM_PORT_SCK_ENABLE);
    ospim_port_csn_config(OSPIM_PORT0, OSPIM_PORT_CSN_ENABLE);
    ospim_port_io3_0_config(OSPIM_PORT0, OSPIM_IO_LOW_ENABLE);
    ospim_port_io7_4_config(OSPIM_PORT0, OSPIM_IO_HIGH_DISABLE);

    rcu_periph_clock_enable(RCU_OSPI0);

    ospim_port_sck_source_select(OSPIM_PORT0, OSPIM_SCK_SOURCE_OSPI0_SCK);
    ospim_port_csn_source_select(OSPIM_PORT0, OSPIM_CSN_SOURCE_OSPI0_CSN);
    ospim_port_io3_0_source_select(OSPIM_PORT0, OSPIM_SRCPLIO_OSPI0_IO_LOW);
}

/*===========================================================================
 * OSPI init (verified against Fire bsp_ospi.c)
 *===========================================================================*/

void OSPI_Flash_Init(void)
{
    ospi_parameter_struct ospi_cfg;

    ospi_deinit(OSPI0);
    _OSPI_GPIO_Config();
    _OSPIM_Config();

    ospi_struct_init(&ospi_cfg);
    ospi_cfg.prescaler        = 9;                         /* ~60 MHz from AHB3 */
    ospi_cfg.fifo_threshold   = OSPI_FIFO_THRESHOLD_5;
    ospi_cfg.sample_shift     = OSPI_SAMPLE_SHIFTING_NONE;
    ospi_cfg.device_size      = OSPI_MESZ_8_MBS;           /* 8 MB flash */
    ospi_cfg.cs_hightime      = OSPI_CS_HIGH_TIME_3_CYCLE;
    ospi_cfg.memory_type      = OSPI_MICRON_MODE;
    ospi_cfg.wrap_size        = OSPI_DIRECT;
    ospi_cfg.delay_hold_cycle = OSPI_DELAY_HOLD_NONE;

    ospi_init(OSPI0, &ospi_cfg);
    ospi_enable(OSPI0);

    printf("OSPI Flash init OK. JEDEC ID: %08X\r\n", OSPI_Flash_ReadID());
}

/*===========================================================================
 * Read JEDEC ID (single-SPI, no address)
 *===========================================================================*/

uint32_t OSPI_Flash_ReadID(void)
{
    uint8_t rx[3] = {0};

    ospi_functional_mode_config(OSPI0, OSPI_INDIRECT_READ);

    ospi_regular_cmd_struct cmd = {0};
    cmd.operation_type   = OSPI_OPTYPE_COMMON_CFG;
    cmd.instruction      = 0x9F;        /* RDID */
    cmd.ins_mode         = OSPI_INSTRUCTION_1_LINE;
    cmd.ins_size         = OSPI_INSTRUCTION_8_BITS;
    cmd.addr_mode        = OSPI_ADDRESS_NONE;
    cmd.data_mode        = OSPI_DATA_1_LINE;
    cmd.nbdata           = 3;
    cmd.dummy_cycles     = OSPI_DUMYC_CYCLES_0;

    ospi_command_config(OSPI0, NULL, &cmd);
    ospi_receive(OSPI0, rx);

    return ((uint32_t)rx[0] << 16) | ((uint32_t)rx[1] << 8) | rx[2];
}

/*===========================================================================
 * Read Data (single-SPI 0x03, 24-bit address)
 *===========================================================================*/

void OSPI_Flash_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    ospi_functional_mode_config(OSPI0, OSPI_INDIRECT_READ);

    ospi_regular_cmd_struct cmd = {0};
    cmd.operation_type   = OSPI_OPTYPE_COMMON_CFG;
    cmd.instruction      = 0x03;        /* READ */
    cmd.ins_mode         = OSPI_INSTRUCTION_1_LINE;
    cmd.ins_size         = OSPI_INSTRUCTION_8_BITS;
    cmd.address          = addr;
    cmd.addr_mode        = OSPI_ADDRESS_1_LINE;
    cmd.addr_size        = OSPI_ADDRESS_24_BITS;
    cmd.data_mode        = OSPI_DATA_1_LINE;
    cmd.nbdata           = len;
    cmd.dummy_cycles     = OSPI_DUMYC_CYCLES_0;

    ospi_command_config(OSPI0, NULL, &cmd);
    ospi_receive(OSPI0, buf);
}

/*===========================================================================
 * Page Program (single-SPI 0x02)
 *===========================================================================*/

static void _OSPI_WriteCmd(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    ospi_functional_mode_config(OSPI0, OSPI_INDIRECT_WRITE);

    ospi_regular_cmd_struct cmd = {0};
    cmd.operation_type   = OSPI_OPTYPE_COMMON_CFG;
    cmd.instruction      = 0x02;        /* PAGE PROGRAM */
    cmd.ins_mode         = OSPI_INSTRUCTION_1_LINE;
    cmd.ins_size         = OSPI_INSTRUCTION_8_BITS;
    cmd.address          = addr;
    cmd.addr_mode        = OSPI_ADDRESS_1_LINE;
    cmd.addr_size        = OSPI_ADDRESS_24_BITS;
    cmd.data_mode        = OSPI_DATA_1_LINE;
    cmd.nbdata           = len;
    cmd.dummy_cycles     = OSPI_DUMYC_CYCLES_0;

    ospi_command_config(OSPI0, NULL, &cmd);
    ospi_transmit(OSPI0, (uint8_t *)buf);
}

void OSPI_Flash_Write(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    while (len) {
        uint32_t chunk = len;
        uint32_t page_remain = FLASH_PAGE_SIZE - (addr & (FLASH_PAGE_SIZE - 1));
        if (chunk > page_remain) chunk = page_remain;

        _OSPI_WriteEnable();
        _OSPI_WriteCmd(addr, buf, chunk);
        _OSPI_WaitBusy();

        addr += chunk;
        buf  += chunk;
        len  -= chunk;
    }
}

/*===========================================================================
 * Erase Operations
 *===========================================================================*/

static void _OSPI_EraseCmd(uint8_t erase_cmd, uint32_t addr)
{
    ospi_functional_mode_config(OSPI0, OSPI_INDIRECT_WRITE);

    ospi_regular_cmd_struct cmd = {0};
    cmd.operation_type   = OSPI_OPTYPE_COMMON_CFG;
    cmd.instruction      = erase_cmd;
    cmd.ins_mode         = OSPI_INSTRUCTION_1_LINE;
    cmd.ins_size         = OSPI_INSTRUCTION_8_BITS;
    cmd.address          = addr;
    cmd.addr_mode        = OSPI_ADDRESS_1_LINE;
    cmd.addr_size        = OSPI_ADDRESS_24_BITS;
    cmd.data_mode        = OSPI_DATA_NONE;
    cmd.nbdata           = 0;
    cmd.dummy_cycles     = OSPI_DUMYC_CYCLES_0;

    ospi_command_config(OSPI0, NULL, &cmd);
}

void OSPI_Flash_Erase_Sector(uint32_t addr)
{
    _OSPI_WriteEnable();
    _OSPI_EraseCmd(0x20, addr);  /* SE: Sector Erase 4KB */
    _OSPI_WaitBusy();
}

void OSPI_Flash_Erase_Block(uint32_t addr)
{
    _OSPI_WriteEnable();
    _OSPI_EraseCmd(0xD8, addr);  /* BE64: Block Erase 64KB */
    _OSPI_WaitBusy();
}

void OSPI_Flash_Erase_Chip(void)
{
    _OSPI_WriteEnable();
    _OSPI_EraseCmd(0xC7, 0);     /* CE: Chip Erase */
    _OSPI_WaitBusy();
}

/*===========================================================================
 * Helper: Write Enable (0x06)
 *===========================================================================*/

static void _OSPI_WriteEnable(void)
{
    ospi_functional_mode_config(OSPI0, OSPI_INDIRECT_WRITE);

    ospi_regular_cmd_struct cmd = {0};
    cmd.operation_type   = OSPI_OPTYPE_COMMON_CFG;
    cmd.instruction      = 0x06;        /* WREN */
    cmd.ins_mode         = OSPI_INSTRUCTION_1_LINE;
    cmd.ins_size         = OSPI_INSTRUCTION_8_BITS;
    cmd.addr_mode        = OSPI_ADDRESS_NONE;
    cmd.data_mode        = OSPI_DATA_NONE;
    cmd.nbdata           = 0;
    cmd.dummy_cycles     = OSPI_DUMYC_CYCLES_0;

    ospi_command_config(OSPI0, NULL, &cmd);
}

/*===========================================================================
 * Helper: Wait Busy (poll RDSR bit 0)
 *===========================================================================*/

static void _OSPI_WaitBusy(void)
{
    uint8_t sr;
    do {
        ospi_functional_mode_config(OSPI0, OSPI_INDIRECT_READ);

        ospi_regular_cmd_struct c = {0};
        c.operation_type   = OSPI_OPTYPE_COMMON_CFG;
        c.instruction      = 0x05;      /* RDSR */
        c.ins_mode         = OSPI_INSTRUCTION_1_LINE;
        c.ins_size         = OSPI_INSTRUCTION_8_BITS;
        c.addr_mode        = OSPI_ADDRESS_NONE;
        c.data_mode        = OSPI_DATA_1_LINE;
        c.nbdata           = 1;
        c.dummy_cycles     = OSPI_DUMYC_CYCLES_0;

        ospi_command_config(OSPI0, NULL, &c);
        ospi_receive(OSPI0, &sr);
    } while (sr & 0x01);
}

/*===========================================================================
 * Memory-Mapped Mode
 *===========================================================================*/

void OSPI_Flash_MemoryMapped(void)
{
    ospi_regular_cmd_struct read_cfg = {0};
    read_cfg.operation_type = OSPI_OPTYPE_READ_CFG;
    read_cfg.instruction    = 0x03;     /* READ */
    read_cfg.ins_mode       = OSPI_INSTRUCTION_1_LINE;
    read_cfg.ins_size       = OSPI_INSTRUCTION_8_BITS;
    read_cfg.addr_mode      = OSPI_ADDRESS_1_LINE;
    read_cfg.addr_size      = OSPI_ADDRESS_24_BITS;
    read_cfg.data_mode      = OSPI_DATA_1_LINE;
    read_cfg.nbdata         = 0;
    read_cfg.dummy_cycles   = OSPI_DUMYC_CYCLES_0;

    ospi_functional_mode_config(OSPI0, OSPI_MEMORY_MAPPED);
    ospi_command_config(OSPI0, NULL, &read_cfg);
}
