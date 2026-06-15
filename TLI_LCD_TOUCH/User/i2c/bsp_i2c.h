#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H
#include <inttypes.h>

#define LCD_I2C_WR	0		/* 写控制bit */
#define LCD_I2C_RD	1		/* 读控制bit */

// I2C GPIO 引脚宏定义
#define  LCD_I2C_SCL_GPIO_APBXCLKCMD    rcu_periph_clock_enable
#define  LCD_I2C_SDA_GPIO_APBXCLKCMD    rcu_periph_clock_enable
#define  LCD_I2C_GPIO_CLK_SCL           RCU_GPIOH
#define  LCD_I2C_GPIO_CLK_SDA           RCU_GPIOH
#define  LCD_I2C_SCL_GPIO_PORT         GPIOH   
#define  LCD_I2C_SCL_GPIO_PIN          GPIO_PIN_7
#define  LCD_I2C_SDA_GPIO_PORT         GPIOH
#define  LCD_I2C_SDA_GPIO_PIN          GPIO_PIN_8


#define I2C_OWN_ADDRESS7     0x72
#define LCD_ADDRESS_ADDRESS7   0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78

/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#if 0	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define LCD_I2C_SCL_1()  gpio_bit_set(OLED_I2C_SCL_GPIO_PORT, OLED_I2C_SCL_GPIO_PIN)		/* SCL = 1 */
	#define LCD_I2C_SCL_0()  gpio_bit_reset(OLED_I2C_SCL_GPIO_PORT, OLED_I2C_SCL_GPIO_PIN)		/* SCL = 0 */
            
	#define LCD_I2C_SDA_1()  gpio_bit_set(OLED_I2C_SDA_GPIO_PORT, OLED_I2C_SDA_GPIO_PIN)		/* SDA = 1 */
	#define LCD_I2C_SDA_0()  gpio_bit_reset(OLED_I2C_SDA_GPIO_PORT, OLED_I2C_SDA_GPIO_PIN)		/* SDA = 0 */
            
	#define LCD_I2C_SDA_READ()  gpio_input_bit_get(OLED_I2C_SDA_GPIO_PORT, OLED_I2C_SDA_GPIO_PIN)	/* 读SDA口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    /*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define OLED_I2C_SCL_1()  GPIO_BOP(OLED_I2C_SCL_GPIO_PORT) = OLED_I2C_SCL_GPIO_PIN				/* SCL = 1 */
	#define OLED_I2C_SCL_0()  GPIO_BC(OLED_I2C_SCL_GPIO_PORT) = OLED_I2C_SCL_GPIO_PIN				/* SCL = 0 */
	
	#define OLED_I2C_SDA_1()  GPIO_BOP(OLED_I2C_SDA_GPIO_PORT) = OLED_I2C_SDA_GPIO_PIN				/* SDA = 1 */
	#define OLED_I2C_SDA_0()  GPIO_BC(OLED_I2C_SDA_GPIO_PORT) = OLED_I2C_SDA_GPIO_PIN				/* SDA = 0 */
	
	#define OLED_I2C_SDA_READ()  ((GPIO_ISTAT(OLED_I2C_SDA_GPIO_PORT) & OLED_I2C_SDA_GPIO_PIN )!= 0)	/* 读SDA口线状态 */
#endif


void I2C_CONFIG(void);;
static void I2C_GPIO_CONFIG(void);
void I2C_Init(void);

#endif /* _BSP_I2C_GPIO_H */

