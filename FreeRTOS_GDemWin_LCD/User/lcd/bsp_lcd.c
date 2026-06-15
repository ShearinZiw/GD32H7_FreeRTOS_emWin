/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  embedfire
  * @version V1.0
  * @date    2024
  * @brief   LCDӦ�ú����ӿڣ�Ŀǰ��֧��RGB565(����������ʾ)
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨��Ұ�� GDH759IMK6 GD32������ 
  * ��̳      ��http://www.firebbs.cn
  * ����      ��https://embedfire.com/
  * �Ա�      ��https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */
  
#include "lcd/bsp_lcd.h"

/* Always needed - used by LCD_LayerInit for frame buffer init */
uint16_t  *framebuffer = (uint16_t *)LCD_FRAME_BUFFER;

/* Default LCD configuration with LCD Layer 0 */
static uint32_t ActiveLayer = 0;

/* When USE_GDEMWIN is defined (emWin project), exclude drawing functions
 * that conflict with emWin's built-in LCD_* API. Keep only HW init. */
#ifndef USE_GDEMWIN
#include "./fonts/font24.c"
#include "./fonts/font20.c"
#include "./fonts/font16.c"
#include "./fonts/font12.c"
#include "./fonts/font8.c"

#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y))
#define ABS(X)  ((X) > 0 ? (X) : -(X))

static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];
#endif /* !USE_GDEMWIN */

 /**
  * @brief  ��ʼ������LCD��IO
  * @param  ��
  * @retval ��
  */
void LCD_GPIO_Config(void)
{ 

    /* ʹ��LCDʹ�õ�������ʱ�� */
    //��ɫ������
    LTDC_R3_GPIO_CLK_ENABLE(LTDC_R3_GPIO_CLK);
    LTDC_R4_GPIO_CLK_ENABLE(LTDC_R4_GPIO_CLK);
    LTDC_R5_GPIO_CLK_ENABLE(LTDC_R5_GPIO_CLK);
    LTDC_R6_GPIO_CLK_ENABLE(LTDC_R6_GPIO_CLK);
    LTDC_R7_GPIO_CLK_ENABLE(LTDC_R7_GPIO_CLK);
    //��ɫ������
    LTDC_G2_GPIO_CLK_ENABLE(LTDC_G2_GPIO_CLK);
    LTDC_G3_GPIO_CLK_ENABLE(LTDC_G3_GPIO_CLK);
    LTDC_G4_GPIO_CLK_ENABLE(LTDC_G4_GPIO_CLK);
    LTDC_G5_GPIO_CLK_ENABLE(LTDC_G5_GPIO_CLK);
    LTDC_G6_GPIO_CLK_ENABLE(LTDC_G6_GPIO_CLK);
    LTDC_G7_GPIO_CLK_ENABLE(LTDC_G7_GPIO_CLK);
    //��ɫ������
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

    /* GPIO���� */
    /* ��ɫ������ */  

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
    
    //��ɫ������
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
  
    //��ɫ������
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
  
    //�����ź���
  
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
  
    //����BL                            
    gpio_mode_set(LTDC_BL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LTDC_BL_GPIO_PIN);
    gpio_output_options_set(LTDC_BL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, LTDC_BL_GPIO_PIN);
    gpio_bit_set(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN);
    
}

/*!
    \brief      LCD ��ʼ������
    \param[in]  ��
    \param[out] ��
    \retval     ��
*/
void BSP_LCD_Init(void)
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

    /* ����TLI�����ṹ */
    /*�źż�������*/
    /* ��ͬ���źż��� */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;                                                   //ˮƽͬ�����ԣ����õ͵�ƽ���Ǹߵ�ƽ��Ч
    /* ��ֱͬ���źż��� */  
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;                                                   //��ֱͬ������
    /* ����ʹ���źż��� */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;                                                     //����ʹ�ܼ���
    /* ����ͬ��ʱ�Ӽ��� */ 
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;                                               //����ʱ�Ӽ���
    /* LCD��ʾ��ʱ���� */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;                                             //ˮƽͬ������
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;                                               //��ֱͬ������
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;                   //ˮƽͬ�����ؿ���
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;                       //��ֱͬ�����ظ߶�
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH- 1;     //��Ч����
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT -1;        //��Ч�߶�
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;    //�ܿ���
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;         //�ܸ߶�
    
    tli_init_struct.backcolor_red =     0xFF;			//��Ļ�������ɫ����
    tli_init_struct.backcolor_green =   0xFF;			//��Ļ��������ɫ����
    tli_init_struct.backcolor_blue =    0xFF;			//��Ļ����ɫ��ɫ����
    
    tli_init(&tli_init_struct);

}



/*!
    \brief      ��������ɫ����
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

    /* ����TLI�����ṹ */
    /*�źż�������*/
    /* ��ͬ���źż��� */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;                                                   //ˮƽͬ�����ԣ����õ͵�ƽ���Ǹߵ�ƽ��Ч
    /* ��ֱͬ���źż��� */  
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;                                                   //��ֱͬ������
    /* ����ʹ���źż��� */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;                                                     //����ʹ�ܼ���
    /* ����ͬ��ʱ�Ӽ��� */ 
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;                                               //����ʱ�Ӽ���
    /* LCD��ʾ��ʱ���� */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;                                             //ˮƽͬ������
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;                                               //��ֱͬ������
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;                   //ˮƽͬ�����ؿ���
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;                       //��ֱͬ�����ظ߶�
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH- 1;     //��Ч����
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT -1;        //��Ч�߶�
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;    //�ܿ���
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;         //�ܸ߶�
    
    tli_init_struct.backcolor_red =     redkey;			    //��Ļ�������ɫ����
    tli_init_struct.backcolor_green =   greenkey;			//��Ļ��������ɫ����
    tli_init_struct.backcolor_blue =    bluekey;			//��Ļ����ɫ��ɫ����
    
    tli_init(&tli_init_struct);

}


/**
  * @brief ��ʼ��LTD�� �� ����
  *           - �����Դ�ռ�
  *           - ���÷ֱ���
  * @param  None
  * @retval None
  */
/* When emWin is used, its drawing functions replace all below */
/*!
    \brief      Initialize frame buffer with default color
    \param[in]  color: RGB565 color value
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

void LCD_LayerInit(void)
{
    
    tli_layer_parameter_struct         tli_layer_init_struct;
    
    /* TLI layer0���� */
    /* TLI���ڴ�С���� */
    tli_layer_init_struct.layer_window_leftpos = 00+0+lcd_param[cur_lcd].hsw + lcd_param[cur_lcd].hbp;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + lcd_param[cur_lcd].hsw + lcd_param[cur_lcd].hbp - 1);
    tli_layer_init_struct.layer_window_toppos = 00+0+lcd_param[cur_lcd].vsw + lcd_param[cur_lcd].vbp;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + 0 + lcd_param[cur_lcd].vsw + lcd_param[cur_lcd].vbp - 1);
    /* TLI�������ظ�ʽ���� */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;     //TLI_LxPPF  PPF[2:0]  010��RGB565
    /* TLI����ָ��alpha���� */
    tli_layer_init_struct.layer_sa = 0xff;                   //��͸���� 255Ϊ��ȫ��͸��
    /* TLI��Ĭ��alpha R��G��Bֵ���� */
    tli_layer_init_struct.layer_default_blue = 0x00;        //�ò���ʾ��Χ�����ɫ
    tli_layer_init_struct.layer_default_green = 0x00;       
    tli_layer_init_struct.layer_default_red = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0;
    /* TLI���ڻ������ */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;       //����ģʽ ��һ�������� Alpha ���Թ�һ���ĺ㶨 Alpha
    tli_layer_init_struct.layer_acf2 = LAYER_ACF1_SA;       //111����һ�������� Alpha ���Թ�һ���ĺ㶨 Alpha       
    /* TLI��֡�����ַ���� */
    /* �ó�ԱӦд��(һ����������ռ�õ��ֽ���+3)
    Line Lenth = ����Ч���ظ��� x ÿ�����ص��ֽ��� + 3 
    ����Ч���ظ��� = LCD_PIXEL_WIDTH 
    ÿ�����ص��ֽ��� = 2��RGB565/RGB1555��/ 3 (RGB888)/ 4��ARGB8888�� */
    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER;     //�����ַ
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);       //�г��� ���ֵΪһ�е��ֽ���+3
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);       //����ƫ�� �����˴�ĳ����ʼ������һ����ʼ��֮����ֽ���
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;            //������ ������һ֡����
    tli_layer_init(LAYER0, &tli_layer_init_struct);
    //�رն�������Щ��ɫ���ܲ���rgb565��Χ�ڣ��������ܴ��Ժ�ͻ�ȥ��������ɫȥ�ƽ��Ǹ���ɫ����Ļ�ߴ��ͬʱ�����ܶ��ֱȽ�С�Ļ������ܻ��п�������
    tli_dither_config(TLI_DITHER_DISABLE);
    /* ���Ϊ��ɫ */
    framebuffer_init(LCD_COLOR565_WHITE);
    
    tli_layer_enable(LAYER0);
    /* ���¼������� */
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();    
    
//    /*���õ� 2 �㣬��û����дĳ����Ա��ֵ����ó�Աʹ�ø���1��һ�������� */
//    /* ���ñ�����Դ��׵�ַ�����������������ڵ�1��ĺ���*/     
//    tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER + BUFFER_OFFSET;
//    
//    /* ���û�����ӣ�ʹ������Alpha������ */  
//    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;       //����ģʽ ��һ�������� Alpha ���Թ�һ���ĺ㶨 Alpha
//    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;       //111����һ�������� Alpha ���Թ�һ���ĺ㶨 Alpha 
//    /* ��ʼ����2�� */
//    tli_layer_init(LAYER1, &tli_layer_init_struct);
//    /* ���Ϊ��ɫ */
//    lcd_clear(LCD_COLOR565_YELLOW);
//     /*ʹ��ǰ���� */
//    tli_layer_enable(LAYER1);
//    tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);

//    tli_reload_config(TLI_REQUEST_RELOAD_EN);
//    tli_enable();    
    
}



/* ��ͬҺ�����Ĳ��� */
const LCD_PARAM_TypeDef lcd_param[LCD_TYPE_NUM]={

  /* 5�������� */
  {
    /*����Һ�������ֲ�Ĳ�������*/
    .hbp = 46,  //HSYNC�����Ч����
    .vbp = 23,  //VSYNC�����Ч����

    .hsw = 1,   //HSYNC����
    .vsw = 3,   //VSYNC����

    .hfp = 40,  //HSYNCǰ����Ч����
    .vfp = 13,  //VSYNCǰ����Ч����
    
    .comment_clock_2byte = 33, //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    .comment_clock_4byte = 21, //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��

    
    .lcd_pixel_width = ((uint16_t)800),//Һ���ֱ��ʣ���
    .lcd_pixel_height = ((uint16_t)480),//Һ���ֱ��ʣ���

  },
  
   /* 7������������5��һ���� */
  {
    /*����Һ�������ֲ�Ĳ�������*/
    .hbp = 46,  //HSYNC�����Ч����
    .vbp = 23,  //VSYNC�����Ч����

    .hsw = 1,  	//HSYNC����
    .vsw = 1,   //VSYNC����

    .hfp = 22,  	//HSYNCǰ����Ч����
    .vfp = 22,  	//VSYNCǰ����Ч����
    
    .comment_clock_2byte = 33, //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    .comment_clock_4byte = 21, //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��

    
    .lcd_pixel_width = ((uint16_t)800),//Һ���ֱ��ʣ���
    .lcd_pixel_height = ((uint16_t)480),//Һ���ֱ��ʣ���
  
  },

  /* 4.3�������� */
  {
      /*����Һ�������ֲ�Ĳ�������*/
    .hbp = 8,  //HSYNC�����Ч����
    .vbp = 2,  //VSYNC�����Ч����

    .hsw = 41,  	//HSYNC����
    .vsw = 10,   //VSYNC����

    .hfp = 4,  	//HSYNCǰ����Ч����
    .vfp = 4,  	//VSYNCǰ����Ч����
    
    .comment_clock_2byte = 15, //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    .comment_clock_4byte = 15, //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    
    .lcd_pixel_width = 480,//Һ���ֱ��ʣ���
    .lcd_pixel_height = 272,//Һ���ֱ��ʣ���
  }
};
LCD_TypeDef cur_lcd = INCH_5;


#ifndef USE_GDEMWIN


/*!
    \brief      ����Ļ�ϻ�һ����
    \param[in]  x: ���x����
    \param[in]  y: ���y����
    \param[in]  color: RGB565��ʽ�ĵ����ɫ
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
    \brief      ʹ��Ĭ����ɫ��ʼ��֡������
    \param[in]  none
    \param[out] none
    \retval     none
*/

/**
  * @brief  ��ȡLCD��ǰ��Ч��X��Ĵ�С
  * @retval X��Ĵ�С
  */
uint32_t LCD_GetXSize(void)
{
  return ACTIVE_WIDTH;
}

/**
  * @brief  ��ȡLCD��ǰ��Ч��Y��Ĵ�С
  * @retval Y��Ĵ�С
  */
uint32_t LCD_GetYSize(void)
{
  return ACTIVE_HEIGHT;
}

/**
  * @brief  ����LCD��ǰ��������ɫ
  * @param  Color: ������ɫ
  * @retval ��
  */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  ��ȡLCD��ǰ��������ɫ
  * @retval ������ɫ
  */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  ����LCD��ǰ������ֱ�����ɫ
  * @param  Color: ���ֱ�����ɫ
  * @retval ��
  */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  ��ȡLCD��ǰ������ֱ�����ɫ
  * @retval ���ֱ�����ɫ
  */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
 * @brief  ����LCD���ֵ���ɫ�ͱ�������ɫ
 * @param  TextColor: ָ��������ɫ
 * @param  BackColor: ָ��������ɫ
 * @retval ��
 */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor)
{
     LCD_SetTextColor (TextColor);
     LCD_SetBackColor (BackColor);
}

/**
  * @brief  ���������ʽ(Ӣ��)
  * @param  fonts: ѡ��Ҫ���õ������ʽ
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  DrawProp[ActiveLayer].pFont = fonts;
}

/**
  * @brief  ��ȡLCD��ǰ����ʾ������
  * @retval ��������
  */
sFONT *LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}

/**
  * @brief  ��ʾһ���ַ�
  * @param  Xpos: ��ʾ�ַ�����λ��
  * @param  Ypos: ����ʼλ��
  * @param  c: ָ���������ݵ�ָ��
  * @retval ��
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint32_t line;
  
  height = DrawProp[ActiveLayer].pFont->Height;//��ȡ����ʹ������߶�
  width  = DrawProp[ActiveLayer].pFont->Width; //��ȡ����ʹ���������
  
  offset =  8 *((width + 7)/8) -  width ;//�����ַ���ÿһ�����ص�ƫ��ֵ��ʵ�ʴ洢��С-�������
  
  for(i = 0; i < height; i++)//��������߶Ȼ��
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);//�����ַ���ÿһ�����ص�ƫ�Ƶ�ַ
    
    switch(((width + 7)/8))//���������������ȡ��ͬ�����ʵ������ֵ
    {
      
    case 1:
      line =  pchar[0];      //��ȡ�������С��8���ַ�������ֵ
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //��ȡ������ȴ���8С��16���ַ�������ֵ     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //��ȡ������ȴ���16С��24���ַ�������ֵ     
      break;
    } 
    
    for (j = 0; j < width; j++)//����������Ȼ��
    {
      if(line & (1 << (width- j + offset- 1))) //����ÿһ�е�����ֵ��ƫ��λ�ð��յ�ǰ������ɫ���л��
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//�����һ��û�������������ձ�����ɫ���
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
  * @brief  ��ʾһ���ַ�
  * @param  Xpos: X����ʼ����
  * @param  Ypos: Y����ʼ����
  * @param  Ascii: �ַ� ascii ��,��Χ�� 0x20 ��0x7E ��
  * @retval ��
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') *\
    DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  ��ʾ�ַ���
  * @param  Xpos: X����ʼ����
  * @param  Ypos: Y����ʼ���� 
  * @param  Text: �ַ���ָ��
  * @param  Mode: ��ʾ���뷽ʽ��������CENTER_MODE��RIGHT_MODE��LEFT_MODE
  * @retval None
  */
void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
  uint16_t ref_column = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = Text;
  
  /* ��ȡ�ַ�����С */
  while (*ptr++) size ++ ;
  
  /* ÿһ�п�����ʾ�ַ������� */
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
  
  /*�����ʼ���Ƿ�����ʾ��Χ�� */
  if ((ref_column < 1) || (ref_column >= 0x8000))
  {
    ref_column = 1;
  }

  /* ʹ���ַ���ʾ������ʾÿһ���ַ�*/
  while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF)\
			>= DrawProp[ActiveLayer].pFont->Width))
  {
    /* ��ʾһ���ַ� */
    LCD_DisplayChar(ref_column, Ypos, *Text);
    /* ���������С������һ��ƫ��λ�� */
    ref_column += DrawProp[ActiveLayer].pFont->Width;
    /* ָ��ָ����һ���ַ� */
    Text++;
    i++;
  }  
}

/**
  * @brief  ��ָ������ʾ�ַ���(���60��)
  * @param  Line: ��ʾ����
  * @param  ptr: �ַ���ָ��
  * @retval ��
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}

/**
  * @brief  ָ�����㻭һ����
  * @param  x1: ��һ��X������
  * @param  y1: ��һ��Y������
  * @param  x2: �ڶ���X������
  * @param  y2: �ڶ���Y������
  * @retval ��
  */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* ��x��ľ���ֵ */
  deltay = ABS(y2 - y1);        /* ��y��ľ���ֵ */
  x = x1;                       /* ��һ�����ص�x������ʼֵ */
  y = y1;                       /* ��һ�����ص�y������ʼֵ */
  
  if (x2 >= x1)                 /* x����ֵΪ���� */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x����ֵΪ�ݼ� */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y����ֵΪ���� */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y����ֵΪ�ݼ� */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* ÿ�� y ����ֵ������һ��x����ֵ*/
  {
    xinc1 = 0;                  /* �����Ӵ��ڻ���ڷ�ĸʱ��Ҫ�ı� x */
    yinc2 = 0;                  /* ��ҪΪÿ�ε������� y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x��y���ֵ */
  }
  else                          /* ÿ�� x ����ֵ������һ��y����ֵ */
  {
    xinc2 = 0;                  /* ��ҪΪÿ�ε������� x */
    yinc1 = 0;                  /* �����Ӵ��ڻ���ڷ�ĸʱ��Ҫ�ı� y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y��x���ֵ */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);   /* ���Ƶ�ǰ���ص� */
    num += num_add;                            /* �ڷ����Ļ��������ӷ��� */
    if (num >= den)                           /* �����Ӵ��ڻ���ڷ�ĸ */
    {
      num -= den;                             /* �����µķ���ֵ */
      x += xinc1;                             /* xֵ���� */
      y += yinc1;                             /* yֵ���� */
    }
    x += xinc2;                               /* yֵ���� */
    y += yinc2;                               /* yֵ���� */
  }
}

/**
  * @brief  ����һ��Բ��
  * @param  Xpos:   X������
  * @param  Ypos:   Y������
  * @param  Radius: Բ�İ뾶
  * @retval ��
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* ���߱��� */ 
  uint32_t  current_x;   /* ��ǰx����ֵ */
  uint32_t  current_y;   /* ��ǰy����ֵ */
  
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
  * @brief  ������2 �����ò�2 ���Ϊ��Ҫ����ɫ
  * @param  color:   RGB565��ɫ
  * @retval ��
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

#endif /* !USE_GDEMWIN */
