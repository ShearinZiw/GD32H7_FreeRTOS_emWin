/**
  ******************************************************************************
  * @file    bsp_gpio_key.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   key应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDF303-霸道 GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 
  
#include "key/bsp_gpio_key.h" 

static const uint32_t key_gpio_port[KEYX]                   = {KEY1_GPIO_PORT, KEY2_GPIO_PORT};
static const uint32_t key_gpio_pin[KEYX]                    = {KEY1_GPIO_PIN, KEY2_GPIO_PIN};
static const rcu_periph_enum key_gpio_clk_port[KEYX]        = {KEY1_GPIO_CLK_PORT, KEY2_GPIO_CLK_PORT};
static const exti_line_enum key_gpio_exti_line[KEYX]        = {KEY1_GPIO_EXTI_LINE,KEY2_GPIO_EXTI_LINE};
static const uint8_t key_gpio_exti_port_source[KEYX]        = {KEY1_GPIO_EXTI_PORT_SOURCE, KEY2_GPIO_EXTI_PORT_SOURCE};
static const uint8_t key_gpio_exti_exti_pin_source[KEYX]    = {KEY1_GPIO_EXTI_EXTI_PIN_SOURCE, KEY2_GPIO_EXTI_EXTI_PIN_SOURCE};
static const uint8_t key_gpio_exti_irq[KEYX]                = {KEY1_GPIO_EXTI_IRQ, KEY2_GPIO_EXTI_IRQ};

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void KEY_GPIO_Config(void)
{
	/*开启按键端口的时钟*/
	rcu_periph_clock_enable(KEY1_GPIO_CLK_PORT);
	rcu_periph_clock_enable(KEY2_GPIO_CLK_PORT);

	/*选择按键的引脚、设置按键的引脚为浮空输入、设置GPIO速率为50MHz*/
    gpio_mode_set(KEY1_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY1_GPIO_PIN);
    gpio_output_options_set(KEY1_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, KEY1_GPIO_PIN);
    
    gpio_mode_set(KEY2_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY2_GPIO_PIN);
    gpio_output_options_set(KEY2_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, KEY2_GPIO_PIN);

}

/*!
    \brief      初始化按键
    \param[in]  keynum: specify the key to be configured
    \param[in]  keymode: specify button mode
      \arg        KEY_MODE_GPIO: 用作简单的IO
      \arg        KEY_MODE_EXTI: 连接到EXTI线中断
    \param[out] none
    \retval     none
*/
void Key_Init(key_typedef_enum keynum, keymode_typedef_enum keymode)
{
    /*开启按键端口的时钟*/
    rcu_periph_clock_enable(key_gpio_clk_port[keynum]);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* configure button pin as input */
    gpio_mode_set(key_gpio_port[keynum], GPIO_MODE_INPUT, GPIO_PUPD_NONE, key_gpio_pin[keynum]);

    if (keymode == KEY_MODE_EXTI) {
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(key_gpio_exti_irq[keynum], 2U, 0U);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(key_gpio_exti_port_source[keynum], key_gpio_exti_exti_pin_source[keynum]);

        /* configure key EXTI line */
        exti_init(key_gpio_exti_line[keynum], EXTI_INTERRUPT, EXTI_TRIG_RISING);
        exti_interrupt_flag_clear(key_gpio_exti_line[keynum]);
    }
}

/**
  * @brief  检测是否有按键按下
  * @param  GPIOx：x 可以是 A，B，C，D或者 E
  *	@param	     GPIO_Pin：待读取的端口位
  * @retval KEY_OFF(没按下按键)、KEY_ON（按下按键）
*/
uint8_t Key_Scan(uint32_t GPIOx,uint16_t GPIO_Pin)
{			
	/*检测是否有按键按下 */
	if(gpio_input_bit_get(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*等待按键释放 */
		while(gpio_input_bit_get(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
    {
    	return KEY_OFF;
    }

}
/*********************************************END OF FILE**********************/
