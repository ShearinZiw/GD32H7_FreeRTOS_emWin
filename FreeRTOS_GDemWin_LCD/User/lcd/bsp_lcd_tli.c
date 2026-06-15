/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  embedfire
  * @version V1.0
  * @date    2024
  * @brief   LCD应用函数接口，目前仅支持RGB565(不含中文显示)
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
  
#include "lcd/bsp_lcd.h"
#include "./fonts/font24.c"
#include "./fonts/font20.c"
#include "./fonts/font16.c"
#include "./fonts/font12.c"
#include "./fonts/font8.c"

#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y)) 

#define ABS(X)  ((X) > 0 ? (X) : -(X))

/* pointer to the starting address of the framebuffer in memory */
uint16_t  *framebuffer = (uint16_t *)LCD_FRAME_BUFFER;

/* Default LCD configuration with LCD Layer 0 */
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

 /**
  * @brief  初始化控制LCD的IO
  * @param  无
  * @retval 无
  */
void LCD_GPIO_Config(void)
{ 

    /* 使能LCD使用到的引脚时钟 */
    //红色数据线
    LTDC_R3_GPIO_CLK_ENABLE(LTDC_R3_GPIO_CLK);
    LTDC_R4_GPIO_CLK_ENABLE(LTDC_R4_GPIO_CLK);
    LTDC_R5_GPIO_CLK_ENABLE(LTDC_R5_GPIO_CLK);
    LTDC_R6_GPIO_CLK_ENABLE(LTDC_R6_GPIO_CLK);
    LTDC_R7_GPIO_CLK_ENABLE(LTDC_R7_GPIO_CLK);
    //绿色数据线
    LTDC_G2_GPIO_CLK_ENABLE(LTDC_G2_GPIO_CLK);
    LTDC_G3_GPIO_CLK_ENABLE(LTDC_G3_GPIO_CLK);
    LTDC_G4_GPIO_CLK_ENABLE(LTDC_G4_GPIO_CLK);
    LTDC_G5_GPIO_CLK_ENABLE(LTDC_G5_GPIO_CLK);
    LTDC_G6_GPIO_CLK_ENABLE(LTDC_G6_GPIO_CLK);
    LTDC_G7_GPIO_CLK_ENABLE(LTDC_G7_GPIO_CLK);
    //蓝色数据线
    LTDC_B3_GPIO_CLK_ENABLE(LTDC_B3_GPIO_CLK);
    LTDC_B4_GPIO_CLK_ENABLE(LTDC_B4_GPIO_CLK);
    LTDC_B5_GPIO_CLK_ENABLE(LTDC_B5_GPIO_CLK);
    LTDC_B6_GPIO_CLK_ENABLE(LTDC_B6_GPIO_CLK);
    LTDC_B7_GPIO_CLK_ENABLE(LTDC_B7_GPIO_CLK);
    
    LTDC_CLK_GPIO_CLK_ENABLE(LTDC_CLK_GPIO_CLK);
    LTDC_HSYNC_GPIO_CLK_ENABLE(LTDC_HSYNC_GPIO_CLK);
    LTDC_VSYNC_GPIO_CLK_ENABLE(LTDC_VSYNC_GPIO_CLK);
    LTDC_DE_GPIO_CLK_ENABLE(LTDC_DE_GPIO_CLK);
    LTDC_BL_GPIO_CLK_ENABLE(LTDC_BL_GPIO_CLK);

    /* GPIO配置 */
    /* 红色数据线 */  

    gpio_af_set(LTDC_R3_GPIO_PORT, LTDC_R3_GPIO_AF, LTDC_R3_GPIO_PIN);
    gpio_mode_set(LTDC_R3_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R3_GPIO_PIN);
    gpio_output_options_set(LTDC_R3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R3_GPIO_PIN);

    gpio_af_set(LTDC_R4_GPIO_PORT, LTDC_R4_GPIO_AF, LTDC_R4_GPIO_PIN);
    gpio_mode_set(LTDC_R4_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R4_GPIO_PIN);
    gpio_output_options_set(LTDC_R4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R4_GPIO_PIN);

    gpio_af_set(LTDC_R5_GPIO_PORT, LTDC_R5_GPIO_AF, LTDC_R5_GPIO_PIN);
    gpio_mode_set(LTDC_R5_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R5_GPIO_PIN);
    gpio_output_options_set(LTDC_R5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R5_GPIO_PIN);
    
    gpio_af_set(LTDC_R6_GPIO_PORT, LTDC_R6_GPIO_AF, LTDC_R6_GPIO_PIN);
    gpio_mode_set(LTDC_R6_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R6_GPIO_PIN);
    gpio_output_options_set(LTDC_R6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R6_GPIO_PIN);
    
    gpio_af_set(LTDC_R7_GPIO_PORT, LTDC_R7_GPIO_AF, LTDC_R7_GPIO_PIN);
    gpio_mode_set(LTDC_R7_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_R7_GPIO_PIN);
    gpio_output_options_set(LTDC_R7_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_R7_GPIO_PIN);
    
    //绿色数据线
    gpio_af_set(LTDC_G2_GPIO_PORT, LTDC_G2_GPIO_AF, LTDC_G2_GPIO_PIN);
    gpio_mode_set(LTDC_G2_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G2_GPIO_PIN);
    gpio_output_options_set(LTDC_G2_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G2_GPIO_PIN);
    
    gpio_af_set(LTDC_G3_GPIO_PORT, LTDC_G3_GPIO_AF, LTDC_G3_GPIO_PIN);
    gpio_mode_set(LTDC_G3_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G3_GPIO_PIN);
    gpio_output_options_set(LTDC_G3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G3_GPIO_PIN);

    gpio_af_set(LTDC_G4_GPIO_PORT, LTDC_G4_GPIO_AF, LTDC_G4_GPIO_PIN);
    gpio_mode_set(LTDC_G4_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G4_GPIO_PIN);
    gpio_output_options_set(LTDC_G4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G4_GPIO_PIN);

    gpio_af_set(LTDC_G5_GPIO_PORT, LTDC_G5_GPIO_AF, LTDC_G5_GPIO_PIN);
    gpio_mode_set(LTDC_G5_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G5_GPIO_PIN);
    gpio_output_options_set(LTDC_G5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G5_GPIO_PIN);
    
    gpio_af_set(LTDC_G6_GPIO_PORT, LTDC_G6_GPIO_AF, LTDC_G6_GPIO_PIN);
    gpio_mode_set(LTDC_G6_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G6_GPIO_PIN);
    gpio_output_options_set(LTDC_G6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G6_GPIO_PIN);
    
    gpio_af_set(LTDC_G7_GPIO_PORT, LTDC_G7_GPIO_AF, LTDC_G7_GPIO_PIN);
    gpio_mode_set(LTDC_G7_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_G7_GPIO_PIN);
    gpio_output_options_set(LTDC_G7_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_G7_GPIO_PIN);
  
    //蓝色数据线
    gpio_af_set(LTDC_B3_GPIO_PORT, LTDC_B3_GPIO_AF, LTDC_B3_GPIO_PIN);
    gpio_mode_set(LTDC_B3_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B3_GPIO_PIN);
    gpio_output_options_set(LTDC_B3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B3_GPIO_PIN);

    gpio_af_set(LTDC_B4_GPIO_PORT, LTDC_B4_GPIO_AF, LTDC_B4_GPIO_PIN);
    gpio_mode_set(LTDC_B4_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B4_GPIO_PIN);
    gpio_output_options_set(LTDC_B4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B4_GPIO_PIN);

    gpio_af_set(LTDC_B5_GPIO_PORT, LTDC_B5_GPIO_AF, LTDC_B5_GPIO_PIN);
    gpio_mode_set(LTDC_B5_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B5_GPIO_PIN);
    gpio_output_options_set(LTDC_B5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B5_GPIO_PIN);
    
    gpio_af_set(LTDC_B6_GPIO_PORT, LTDC_B6_GPIO_AF, LTDC_B6_GPIO_PIN);
    gpio_mode_set(LTDC_B6_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B6_GPIO_PIN);
    gpio_output_options_set(LTDC_B6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B6_GPIO_PIN);
    
    gpio_af_set(LTDC_B7_GPIO_PORT, LTDC_B7_GPIO_AF, LTDC_B7_GPIO_PIN);
    gpio_mode_set(LTDC_B7_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_B7_GPIO_PIN);
    gpio_output_options_set(LTDC_B7_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_B7_GPIO_PIN);
  
    //控制信号线
  
    gpio_af_set(LTDC_CLK_GPIO_PORT, LTDC_CLK_GPIO_AF, LTDC_CLK_GPIO_PIN);
    gpio_mode_set(LTDC_CLK_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_CLK_GPIO_PIN);
    gpio_output_options_set(LTDC_CLK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_CLK_GPIO_PIN);
  
  
    gpio_af_set(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_GPIO_AF, LTDC_HSYNC_GPIO_PIN);
    gpio_mode_set(LTDC_HSYNC_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_HSYNC_GPIO_PIN);
    gpio_output_options_set(LTDC_HSYNC_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_HSYNC_GPIO_PIN);
  
    gpio_af_set(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_GPIO_AF, LTDC_VSYNC_GPIO_PIN);
    gpio_mode_set(LTDC_VSYNC_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_VSYNC_GPIO_PIN);
    gpio_output_options_set(LTDC_VSYNC_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_VSYNC_GPIO_PIN);
  
    gpio_af_set(LTDC_DE_GPIO_PORT, LTDC_DE_GPIO_AF, LTDC_DE_GPIO_PIN);
    gpio_mode_set(LTDC_DE_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LTDC_DE_GPIO_PIN);
    gpio_output_options_set(LTDC_DE_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_DE_GPIO_PIN);
  
    //背光BL                            
    gpio_mode_set(LTDC_BL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LTDC_BL_GPIO_PIN);
    gpio_output_options_set(LTDC_BL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_BL_GPIO_PIN);
    gpio_bit_set(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN);
    
}

/*!
    \brief      LCD 初始化配置
    \param[in]  无
    \param[out] 无
    \retval     无
*/
void LCD_Init(void)
{
    tli_parameter_struct tli_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    LCD_GPIO_Config();

    /* configure the pll2 input and output clock range */
    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_150M_420M);
    /* configure the PLL2 clock: CK_PLL2P/CK_PLL2Q/CK_PLL2R = HXTAL_VALUE / 25 * 500 / 3 */
    if(ERROR == rcu_pll2_config(25, 500, 3, 3, 3)) 
    {
        while(1);
    }
    /* enable PLL2R clock output */
    rcu_pll_clock_output_enable(RCU_PLL2R);
    rcu_tli_clock_div_config(RCU_PLL2R_DIV8);

    rcu_osci_on(RCU_PLL2_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) 
    {
        while(1);
    }

    /* 配置TLI参数结构 */
    /*信号极性配置*/
    /* 行同步信号极性 */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;                                                   //水平同步极性，设置低电平或是高电平有效
    /* 垂直同步信号极性 */  
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;                                                   //垂直同步极性
    /* 数据使能信号极性 */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;                                                     //数据使能极性
    /* 像素同步时钟极性 */ 
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;                                               //像素时钟极性
    /* LCD显示定时配置 */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;                                             //水平同步宽度
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;                                               //垂直同步宽度
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;                   //水平同步后沿宽度
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;                       //垂直同步后沿高度
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH- 1;     //有效宽度
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT -1;        //有效高度
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;    //总宽度
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;         //总高度
    
    tli_init_struct.backcolor_red =     0xFF;			//屏幕背景层红色部分
    tli_init_struct.backcolor_green =   0xFF;			//屏幕背景层绿色部分
    tli_init_struct.backcolor_blue =    0xFF;			//屏幕背景色蓝色部分
    
    tli_init(&tli_init_struct);

}



/*!
    \brief      背景层颜色设置
    \param[in]  layerx: LAYERx(x=0,1)
    \param[in]  redkey: color key red
    \param[in]  greenkey: color key green 
    \param[in]  bluekey: color key blue
    \param[out] none
    \retval     none
*/
void Colour_Set(uint32_t layerx,uint8_t redkey,uint8_t greenkey,uint8_t bluekey)
{
    tli_parameter_struct tli_init_struct;

    /* 配置TLI参数结构 */
    /*信号极性配置*/
    /* 行同步信号极性 */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;                                                   //水平同步极性，设置低电平或是高电平有效
    /* 垂直同步信号极性 */  
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;                                                   //垂直同步极性
    /* 数据使能信号极性 */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;                                                     //数据使能极性
    /* 像素同步时钟极性 */ 
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;                                               //像素时钟极性
    /* LCD显示定时配置 */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;                                             //水平同步宽度
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;                                               //垂直同步宽度
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;                   //水平同步后沿宽度
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;                       //垂直同步后沿高度
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH- 1;     //有效宽度
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT -1;        //有效高度
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;    //总宽度
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;         //总高度
    
    tli_init_struct.backcolor_red =     redkey;			    //屏幕背景层红色部分
    tli_init_struct.backcolor_green =   greenkey;			//屏幕背景层绿色部分
    tli_init_struct.backcolor_blue =    bluekey;			//屏幕背景色蓝色部分
    
    tli_init(&tli_init_struct);

}


/**
  * @brief 初始化LTD的 层 参数
  *           - 设置显存空间
  *           - 设置分辨率
  * @param  None
  * @retval None
  */
void LCD_LayerInit(void)
{
    
    tli_layer_parameter_struct         tli_layer_init_struct;
    
    /* TLI layer0配置 */
    /* TLI窗口大小配置 */
    tli_layer_init_struct.layer_window_leftpos = 00+0+lcd_param[cur_lcd].hsw + lcd_param[cur_lcd].hbp;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + lcd_param[cur_lcd].hsw + lcd_param[cur_lcd].hbp - 1);
    tli_layer_init_struct.layer_window_toppos = 00+0+lcd_param[cur_lcd].vsw + lcd_param[cur_lcd].vbp;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + 0 + lcd_param[cur_lcd].vsw + lcd_param[cur_lcd].vbp - 1);
    /* TLI窗口像素格式配置 */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;     //TLI_LxPPF  PPF[2:0]  010：RGB565
    /* TLI窗口指定alpha配置 */
    tli_layer_init_struct.layer_sa = 0xff;                   //层透明度 255为完全不透明
    /* TLI层默认alpha R、G、B值配置 */
    tli_layer_init_struct.layer_default_blue = 0x00;        //该层显示范围外的颜色
    tli_layer_init_struct.layer_default_green = 0x00;       
    tli_layer_init_struct.layer_default_red = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0;
    /* TLI窗口混合配置 */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;       //层混合模式 归一化的像素 Alpha 乘以归一化的恒定 Alpha
    tli_layer_init_struct.layer_acf2 = LAYER_ACF1_SA;       //111：归一化的像素 Alpha 乘以归一化的恒定 Alpha       
    /* TLI层帧缓冲基址配置 */
    /* 该成员应写入(一行像素数据占用的字节数+3)
    Line Lenth = 行有效像素个数 x 每个像素的字节数 + 3 
    行有效像素个数 = LCD_PIXEL_WIDTH 
    每个像素的字节数 = 2（RGB565/RGB1555）/ 3 (RGB888)/ 4（ARGB8888） */
    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER;     //缓存地址
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);       //行长度 这个值为一行的字节数+3
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);       //步幅偏移 定义了从某行起始处到下一行起始处之间的字节数
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;            //总行数 定义了一帧行数
    tli_layer_init(LAYER0, &tli_layer_init_struct);
    //关闭抖动，有些颜色可能不在rgb565范围内，抖动功能打开以后就会去用其他颜色去逼近那个颜色，屏幕尺寸大同时像素密度又比较小的话，可能会有颗粒现象
    tli_dither_config(TLI_DITHER_DISABLE);
    /* 填充为白色 */
    framebuffer_init(LCD_COLOR565_WHITE);
    
    tli_layer_enable(LAYER0);
    /* 重新加载配置 */
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();    
    
//    /*配置第 2 层，若没有重写某个成员的值，则该成员使用跟第1层一样的配置 */
//    /* 配置本层的显存首地址，这里配置它紧挨在第1层的后面*/     
//    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER + BUFFER_OFFSET;
//    
//    /* 配置混合因子，使用像素Alpha参与混合 */  
//    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;       //层混合模式 归一化的像素 Alpha 乘以归一化的恒定 Alpha
//    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;       //111：归一化的像素 Alpha 乘以归一化的恒定 Alpha 
//    /* 初始化第2层 */
//    tli_layer_init(LAYER1, &tli_layer_init_struct);
//    /* 填充为黄色 */
//    lcd_clear(LCD_COLOR565_YELLOW);
//     /*使能前景层 */
//    tli_layer_enable(LAYER1);
//    tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);

//    tli_reload_config(TLI_REQUEST_RELOAD_EN);
//    tli_enable();    
    
}



/* 不同液晶屏的参数 */
const LCD_PARAM_TypeDef lcd_param[LCD_TYPE_NUM]={

  /* 5寸屏参数 */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,   //HSYNC宽度
    .vsw = 3,   //VSYNC宽度

    .hfp = 40,  //HSYNC前的无效像素
    .vfp = 13,  //VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = ((uint16_t)800),//液晶分辨率，宽
    .lcd_pixel_height = ((uint16_t)480),//液晶分辨率，高

  },
  
   /* 7寸屏参数（与5寸一样） */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,  	//HSYNC宽度
    .vsw = 1,   //VSYNC宽度

    .hfp = 22,  	//HSYNC前的无效像素
    .vfp = 22,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = ((uint16_t)800),//液晶分辨率，宽
    .lcd_pixel_height = ((uint16_t)480),//液晶分辨率，高
  
  },

  /* 4.3寸屏参数 */
  {
      /*根据液晶数据手册的参数配置*/
    .hbp = 8,  //HSYNC后的无效像素
    .vbp = 2,  //VSYNC后的无效行数

    .hsw = 41,  	//HSYNC宽度
    .vsw = 10,   //VSYNC宽度

    .hfp = 4,  	//HSYNC前的无效像素
    .vfp = 4,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 15, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 15, //Argb8888等四字节像素时推荐使用的液晶时钟频率
    
    .lcd_pixel_width = 480,//液晶分辨率，宽
    .lcd_pixel_height = 272,//液晶分辨率，高
  }
};
LCD_TypeDef cur_lcd = INCH_5;

/*!
    \brief      在屏幕上画一个点
    \param[in]  x: 点的x坐标
    \param[in]  y: 点的y坐标
    \param[in]  color: RGB565格式的点的颜色
    \param[out] none
    \retval     none
*/
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if(x >= ACTIVE_WIDTH || y >= ACTIVE_HEIGHT) {
        return;
    }

    /* draw a simple square dot */
    for(int i = -POINT_SIZE / 2; i <= POINT_SIZE / 2; i++) {
        for(int j = -POINT_SIZE / 2; j <= POINT_SIZE / 2; j++) {
            if(x + i < ACTIVE_WIDTH && y + j < ACTIVE_HEIGHT) {
                framebuffer[(y + j) * ACTIVE_WIDTH + (x + i)] = color;
            }
        }
    }
}
/*!
    \brief      使用默认颜色初始化帧缓冲区
    \param[in]  none
    \param[out] none
    \retval     none
*/
void framebuffer_init(uint16_t color)
{
    uint32_t volatile x, y;
    for(uint32_t  y = 0; y < ACTIVE_HEIGHT; ++y) {
        for(uint32_t  x = 0; x < ACTIVE_WIDTH; ++x) {
            framebuffer[y * ACTIVE_WIDTH + x] = color;
        }
    }
}

/**
  * @brief  获取LCD当前有效层X轴的大小
  * @retval X轴的大小
  */
uint32_t LCD_GetXSize(void)
{
  return ACTIVE_WIDTH;
}

/**
  * @brief  获取LCD当前有效层Y轴的大小
  * @retval Y轴的大小
  */
uint32_t LCD_GetYSize(void)
{
  return ACTIVE_HEIGHT;
}

/**
  * @brief  设置LCD当前层文字颜色
  * @param  Color: 文字颜色
  * @retval 无
  */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  获取LCD当前层文字颜色
  * @retval 文字颜色
  */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  设置LCD当前层的文字背景颜色
  * @param  Color: 文字背景颜色
  * @retval 无
  */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  获取LCD当前层的文字背景颜色
  * @retval 文字背景颜色
  */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
 * @brief  设置LCD文字的颜色和背景的颜色
 * @param  TextColor: 指定文字颜色
 * @param  BackColor: 指定背景颜色
 * @retval 无
 */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor)
{
     LCD_SetTextColor (TextColor);
     LCD_SetBackColor (BackColor);
}

/**
  * @brief  设置字体格式(英文)
  * @param  fonts: 选择要设置的字体格式
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  DrawProp[ActiveLayer].pFont = fonts;
}

/**
  * @brief  获取LCD当前层显示的字体
  * @retval 字体类型
  */
sFONT *LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}

/**
  * @brief  显示一个字符
  * @param  Xpos: 显示字符的行位置
  * @param  Ypos: 列起始位置
  * @param  c: 指向字体数据的指针
  * @retval 无
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint32_t line;
  
  height = DrawProp[ActiveLayer].pFont->Height;//获取正在使用字体高度
  width  = DrawProp[ActiveLayer].pFont->Width; //获取正在使用字体宽度
  
  offset =  8 *((width + 7)/8) -  width ;//计算字符的每一行像素的偏移值，实际存储大小-字体宽度
  
  for(i = 0; i < height; i++)//遍历字体高度绘点
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);//计算字符的每一行像素的偏移地址
    
    switch(((width + 7)/8))//根据字体宽度来提取不同字体的实际像素值
    {
      
    case 1:
      line =  pchar[0];      //提取字体宽度小于8的字符的像素值
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //提取字体宽度大于8小于16的字符的像素值     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //提取字体宽度大于16小于24的字符的像素值     
      break;
    } 
    
    for (j = 0; j < width; j++)//遍历字体宽度绘点
    {
      if(line & (1 << (width- j + offset- 1))) //根据每一行的像素值及偏移位置按照当前字体颜色进行绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//如果这一行没有字体像素则按照背景颜色绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
  * @brief  显示一个字符
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Ascii: 字符 ascii 码,范围（ 0x20 —0x7E ）
  * @retval 无
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') *\
    DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  显示字符串
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标 
  * @param  Text: 字符串指针
  * @param  Mode: 显示对齐方式，可以是CENTER_MODE、RIGHT_MODE、LEFT_MODE
  * @retval None
  */
void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
  uint16_t ref_column = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = Text;
  
  /* 获取字符串大小 */
  while (*ptr++) size ++ ;
  
  /* 每一行可以显示字符的数量 */
  xsize = (LCD_GetXSize()/DrawProp[ActiveLayer].pFont->Width);
  
  switch (Mode)
  {
  case CENTER_MODE:
    {
      ref_column = Xpos + ((xsize - size)* DrawProp[ActiveLayer].pFont->Width) / 2;
      break;
    }
  case LEFT_MODE:
    {
      ref_column = Xpos;
      break;
    }
  case RIGHT_MODE:
    {
      ref_column = - Xpos + ((xsize - size)*DrawProp[ActiveLayer].pFont->Width);
      break;
    }    
  default:
    {
      ref_column = Xpos;
      break;
    }
  }
  
  /*检查起始行是否在显示范围内 */
  if ((ref_column < 1) || (ref_column >= 0x8000))
  {
    ref_column = 1;
  }

  /* 使用字符显示函数显示每一个字符*/
  while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF)\
			>= DrawProp[ActiveLayer].pFont->Width))
  {
    /* 显示一个字符 */
    LCD_DisplayChar(ref_column, Ypos, *Text);
    /* 根据字体大小计算下一个偏移位置 */
    ref_column += DrawProp[ActiveLayer].pFont->Width;
    /* 指针指向下一个字符 */
    Text++;
    i++;
  }  
}

/**
  * @brief  在指定行显示字符串(最多60个)
  * @param  Line: 显示的行
  * @param  ptr: 字符串指针
  * @retval 无
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}

/**
  * @brief  指定两点画一条线
  * @param  x1: 第一点X轴坐标
  * @param  y1: 第一点Y轴坐标
  * @param  x2: 第二点X轴坐标
  * @param  y2: 第二点Y轴坐标
  * @retval 无
  */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* 求x轴的绝对值 */
  deltay = ABS(y2 - y1);        /* 求y轴的绝对值 */
  x = x1;                       /* 第一个像素的x坐标起始值 */
  y = y1;                       /* 第一个像素的y坐标起始值 */
  
  if (x2 >= x1)                 /* x坐标值为递增 */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x坐标值为递减 */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y坐标值为递增 */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y坐标值为递减 */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* 每个 y 坐标值至少有一个x坐标值*/
  {
    xinc1 = 0;                  /* 当分子大于或等于分母时不要改变 x */
    yinc2 = 0;                  /* 不要为每次迭代更改 y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x比y多的值 */
  }
  else                          /* 每个 x 坐标值至少有一个y坐标值 */
  {
    xinc2 = 0;                  /* 不要为每次迭代更改 x */
    yinc1 = 0;                  /* 当分子大于或等于分母时不要改变 y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y比x多的值 */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);   /* 绘制当前像素点 */
    num += num_add;                            /* 在分数的基础上增加分子 */
    if (num >= den)                           /* 检查分子大于或等于分母 */
    {
      num -= den;                             /* 计算新的分子值 */
      x += xinc1;                             /* x值递增 */
      y += yinc1;                             /* y值递增 */
    }
    x += xinc2;                               /* y值递增 */
    y += yinc2;                               /* y值递增 */
  }
}

/**
  * @brief  绘制一个圆形
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  Radius: 圆的半径
  * @retval 无
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* 决策变量 */ 
  uint32_t  current_x;   /* 当前x坐标值 */
  uint32_t  current_y;   /* 当前y坐标值 */
  
  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;
  
  while (current_x <= current_y)
  {
    LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  } 
}
/**
  * @brief  清屏层2 需启用层2 填充为想要的颜色
  * @param  color:   RGB565颜色
  * @retval 无
  */
void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    for (index = 0x00; index < BUFFER_OFFSET; index++){
        *(__IO uint16_t*)(LCD_FRAME_BUFFER + BUFFER_OFFSET + (2*index)) = color;
    }
//    uint16_t i,j;
//    uint16_t *p = (uint16_t *)(LCD_FRAME_BUFFER+BUFFER_OFFSET);
//    for(j=0;j<ACTIVE_HEIGHT;j++)
//    {
//        for(i=0;i<ACTIVE_WIDTH;i++)
//        {
//            *p = color;
//            p++;
//        }
//    }

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
