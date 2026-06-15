#ifndef __BSP_LCD_H
#define	__BSP_LCD_H

#include "gd32h7xx.h"
#include <stdio.h>
#include "fonts/fonts.h"

#define HORIZONTAL_SYNCHRONOUS_PULSE  1
#define HORIZONTAL_BACK_PORCH         46
#define ACTIVE_WIDTH                  800
#define HORIZONTAL_FRONT_PORCH        40

#define VERTICAL_SYNCHRONOUS_PULSE    3
#define VERTICAL_BACK_PORCH           23
#define ACTIVE_HEIGHT                 480
#define VERTICAL_FRONT_PORCH          13

#define LCD_FRAME_BUFFER             (0xC0000000)    //��һ���׵�ַ
#define BUFFER_OFFSET                (800*480*2)     //һ��Һ����������
#define LCD_PIXCELS                  (800*480) 

#define POINT_SIZE                    1

typedef struct 
{ 
  uint32_t TextColor; 
  uint32_t BackColor;  
  sFONT    *pFont;
}LCD_DrawPropTypeDef;   
   
typedef struct 
{
  int16_t X;
  int16_t Y;
}Point, * pPoint; 

/** 
  * @brief  �������ģʽ  
  */ 
typedef enum
{
  CENTER_MODE             = 0x01,    /* ���ж��� */
  RIGHT_MODE              = 0x02,    /* �Ҷ���   */
  LEFT_MODE               = 0x03     /* �����   */
}Text_AlignModeTypdef;

#define MAX_LAYER_NUMBER            (2)
#define LTDC_ACTIVE_LAYER	        (1) /* Layer 1 */


/**
  * @brief  LCD color
  */
/*
 *   ʹ������������ɫ�����LCD_LayerInit()�����ж�ÿ������ö�Ӧ����Ȼ�ᵼ����ɫ��ʾ����
 * �ر�Ϊʵ������ͬʱ������ʾ��Ҫ���ϲ�(LCD_FOREGROUND_LAYER)����ΪLTDC_Pixelformat_ARGB1555
 * ��ʽ����Ӧ�ı���ʹ��LCD_COLOR1555_XXX��ɫ���ø�ʽ��1λ͸������λ������ʵ��û��ʵ����ɫ��
 * ������ص�ͨ͸��ʾ,�Ӷ�ʹ����ʾ�²�(LCD_BACKGROUND_LAYER)��Ϊ���ܡ�����ϲ���ʹ��
 * LTDC_Pixelformat_ARGB565��ʽ�������޷�������ʾ˫��ġ�
 *   ��Ȼ�����԰��²�����ΪLTDC_Pixelformat_ARGB1555��ʽ��ʹ����LCD_COLOR1555_XXXҲ�ǿ�����
 * ����ʾ�ġ�
 * �ر�ע�⣺ARGB1555���е�A��Ӧ������Һ����˵����Ϊ1��ʾ��͸��������Ϊ0��͸����
*/
                                                   //RGB565
#define LCD_COLOR565_WHITE           0xFFFF        //11111 111111 11111
#define LCD_COLOR565_BLACK           0x0000        //00000 000000 00000
#define LCD_COLOR565_GREY            0xF7DE        //11110 111110 11110
#define LCD_COLOR565_BLUE            0x001F        //00000 000000 11111
#define LCD_COLOR565_BLUE2           0x051F        //00000 101000 11111
#define LCD_COLOR565_RED             0xF800        //11111 000000 00000
#define LCD_COLOR565_MAGENTA         0xF81F        //11111 000000 11111
#define LCD_COLOR565_GREEN           0x07E0        //00000 111111 00000
#define LCD_COLOR565_CYAN            0x7FFF        //01111 111111 11111
#define LCD_COLOR565_YELLOW          0xFFE0        //11111 111111 00000


//��ɫ������

#define LTDC_R3_GPIO_PORT        	    GPIOA
#define LTDC_R3_GPIO_CLK        	    RCU_GPIOA
#define LTDC_R3_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_R3_GPIO_PIN         	    GPIO_PIN_15
#define LTDC_R3_GPIO_AF			        GPIO_AF_9

#define LTDC_R4_GPIO_PORT        	    GPIOH
#define LTDC_R4_GPIO_CLK       	        RCU_GPIOH
#define LTDC_R4_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_R4_GPIO_PIN         	    GPIO_PIN_10
#define LTDC_R4_GPIO_AF			        GPIO_AF_14

#define LTDC_R5_GPIO_PORT        	    GPIOH
#define LTDC_R5_GPIO_CLK       	        RCU_GPIOH
#define LTDC_R5_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_R5_GPIO_PIN         	    GPIO_PIN_11
#define LTDC_R5_GPIO_AF			        GPIO_AF_14

#define LTDC_R6_GPIO_PORT        	    GPIOA
#define LTDC_R6_GPIO_CLK        	    RCU_GPIOA
#define LTDC_R6_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_R6_GPIO_PIN         	    GPIO_PIN_8
#define LTDC_R6_GPIO_AF			        GPIO_AF_14

#define LTDC_R7_GPIO_PORT        	    GPIOG
#define LTDC_R7_GPIO_CLK       	        RCU_GPIOG
#define LTDC_R7_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_R7_GPIO_PIN         	    GPIO_PIN_6
#define LTDC_R7_GPIO_AF			        GPIO_AF_14

//��ɫ������
#define LTDC_G2_GPIO_PORT        	    GPIOC
#define LTDC_G2_GPIO_CLK        	    RCU_GPIOC
#define LTDC_G2_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_G2_GPIO_PIN         	    GPIO_PIN_0
#define LTDC_G2_GPIO_AF			        GPIO_AF_11

#define LTDC_G3_GPIO_PORT        	    GPIOG
#define LTDC_G3_GPIO_CLK        	    RCU_GPIOG
#define LTDC_G3_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_G3_GPIO_PIN         	    GPIO_PIN_10
#define LTDC_G3_GPIO_AF			        GPIO_AF_9

#define LTDC_G4_GPIO_PORT        	    GPIOH
#define LTDC_G4_GPIO_CLK       	        RCU_GPIOH
#define LTDC_G4_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_G4_GPIO_PIN         	    GPIO_PIN_15
#define LTDC_G4_GPIO_AF			        GPIO_AF_14

#define LTDC_G5_GPIO_PORT        	    GPIOH
#define LTDC_G5_GPIO_CLK       	        RCU_GPIOH
#define LTDC_G5_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_G5_GPIO_PIN         	    GPIO_PIN_4
#define LTDC_G5_GPIO_AF			        GPIO_AF_9

#define LTDC_G6_GPIO_PORT        	    GPIOC
#define LTDC_G6_GPIO_CLK        	        RCU_GPIOC
#define LTDC_G6_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_G6_GPIO_PIN         	    GPIO_PIN_7
#define LTDC_G6_GPIO_AF			        GPIO_AF_14

#define LTDC_G7_GPIO_PORT        	    GPIOD
#define LTDC_G7_GPIO_CLK       	        RCU_GPIOD
#define LTDC_G7_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_G7_GPIO_PIN         	    GPIO_PIN_3
#define LTDC_G7_GPIO_AF			        GPIO_AF_14

//��ɫ������
#define LTDC_B3_GPIO_PORT        	    GPIOG
#define LTDC_B3_GPIO_CLK       	        RCU_GPIOG
#define LTDC_B3_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_B3_GPIO_PIN         	    GPIO_PIN_11
#define LTDC_B3_GPIO_AF			        GPIO_AF_14

#define LTDC_B4_GPIO_PORT        	    GPIOG
#define LTDC_B4_GPIO_CLK       	        RCU_GPIOG
#define LTDC_B4_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_B4_GPIO_PIN         	    GPIO_PIN_12
#define LTDC_B4_GPIO_AF			        GPIO_AF_9

#define LTDC_B5_GPIO_PORT        	    GPIOB
#define LTDC_B5_GPIO_CLK       	        RCU_GPIOB
#define LTDC_B5_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_B5_GPIO_PIN         	    GPIO_PIN_5
#define LTDC_B5_GPIO_AF			        GPIO_AF_3

#define LTDC_B6_GPIO_PORT        	    GPIOB
#define LTDC_B6_GPIO_CLK      	        RCU_GPIOB
#define LTDC_B6_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_B6_GPIO_PIN         	    GPIO_PIN_8
#define LTDC_B6_GPIO_AF		            GPIO_AF_14

#define LTDC_B7_GPIO_PORT        	    GPIOB
#define LTDC_B7_GPIO_CLK        	    RCU_GPIOB
#define LTDC_B7_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_B7_GPIO_PIN         	    GPIO_PIN_9
#define LTDC_B7_GPIO_AF			        GPIO_AF_14

//�����ź���
/*����ʱ��CLK*/
#define LTDC_CLK_GPIO_PORT              GPIOB
#define LTDC_CLK_GPIO_CLK       	    RCU_GPIOB
#define LTDC_CLK_GPIO_CLK_ENABLE        rcu_periph_clock_enable
#define LTDC_CLK_GPIO_PIN               GPIO_PIN_3
#define LTDC_CLK_GPIO_AF			    GPIO_AF_2
/*ˮƽͬ���ź�HSYNC*/
#define LTDC_HSYNC_GPIO_PORT            GPIOC
#define LTDC_HSYNC_GPIO_CLK             RCU_GPIOC
#define LTDC_HSYNC_GPIO_CLK_ENABLE      rcu_periph_clock_enable
#define LTDC_HSYNC_GPIO_PIN             GPIO_PIN_6
#define LTDC_HSYNC_GPIO_AF			    GPIO_AF_14
/*��ֱͬ���ź�VSYNC*/
#define LTDC_VSYNC_GPIO_PORT            GPIOA
#define LTDC_VSYNC_GPIO_CLK             RCU_GPIOA
#define LTDC_VSYNC_GPIO_CLK_ENABLE      rcu_periph_clock_enable
#define LTDC_VSYNC_GPIO_PIN             GPIO_PIN_4
#define LTDC_VSYNC_GPIO_AF			    GPIO_AF_14

/*����ʹ���ź�DE*/
#define LTDC_DE_GPIO_PORT               GPIOF
#define LTDC_DE_GPIO_CLK                RCU_GPIOF
#define LTDC_DE_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_DE_GPIO_PIN                GPIO_PIN_10
#define LTDC_DE_GPIO_AF			        GPIO_AF_14

/*Һ���������źţ��ߵ�ƽʹ��*/
#define LTDC_BL_GPIO_PORT               GPIOG
#define LTDC_BL_GPIO_CLK                RCU_GPIOG
#define LTDC_BL_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_BL_GPIO_PIN                GPIO_PIN_3
#define LTDC_BL_GPIO_AF			        GPIO_AF_14


/** 
  * @brief  LCDҺ������
  */   
typedef enum
{ 
  INCH_5  = 0x00, /* Ұ��5���� */
  INCH_7,     /* Ұ��7���� */
  INCH_4_3,  /* Ұ��4.3���� */
  
  LCD_TYPE_NUM /* LCD��������*/
}LCD_TypeDef;

/* ��ǰʹ�õ�LCD��Ĭ��Ϊ5���� */
extern LCD_TypeDef cur_lcd;

/** 
  * @brief  LCDҺ������
  */   
typedef struct
{
  /*����Һ�������ֲ�Ĳ�������*/
  uint8_t hbp;  //HSYNC�����Ч����
  uint8_t vbp;  //VSYNC�����Ч����

  uint8_t hsw;  	//HSYNC����
  uint8_t vsw;   //VSYNC����

  uint8_t hfp;  	//HSYNCǰ����Ч����
  uint8_t vfp;  	//VSYNCǰ����Ч����
  
  uint8_t comment_clock_2byte; //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
  uint8_t comment_clock_4byte; //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��

  uint16_t lcd_pixel_width; //Һ���ֱ��ʣ���
  uint16_t lcd_pixel_height;//Һ���ֱ��ʣ���

}LCD_PARAM_TypeDef;

/* LCD �������ش�С (���Ⱥ͸߶�) */
#define  LCD_PIXEL_WIDTH          lcd_param[cur_lcd].lcd_pixel_width
#define  LCD_PIXEL_HEIGHT         lcd_param[cur_lcd].lcd_pixel_height

/*����Һ�������ֲ�Ĳ�������*/
#define HBP  lcd_param[cur_lcd].hbp		//HSYNC�����Ч����
#define VBP  lcd_param[cur_lcd].vbp		//VSYNC�����Ч����

#define HSW  lcd_param[cur_lcd].hsw		//HSYNC����
#define VSW  lcd_param[cur_lcd].vsw		//VSYNC����

#define HFP  lcd_param[cur_lcd].hfp		//HSYNCǰ����Ч����
#define VFP  lcd_param[cur_lcd].vfp		//VSYNCǰ����Ч����

/* ��ǰʹ�õ�LCD��Ĭ��Ϊ5���� */
extern LCD_TypeDef cur_lcd;
/* ��ͬҺ�����Ĳ��� */
extern const LCD_PARAM_TypeDef lcd_param[];

/** 
  * @brief LCD default font 
  */ 
#define LCD_DEFAULT_FONT         Font24 


void LCD_GPIO_Config(void);
void BSP_LCD_Init(void);
void LCD_LayerInit(void);
void Colour_Set(uint32_t layerx,uint8_t redkey,uint8_t greenkey,uint8_t bluekey);


#ifndef USE_GDEMWIN

void draw_point(uint16_t x, uint16_t y, uint16_t color);
void framebuffer_init(uint16_t color);

void LCD_SetTextColor(uint32_t Color);
uint32_t LCD_GetTextColor(void);
void LCD_SetBackColor(uint32_t Color);
uint32_t LCD_GetBackColor(void);
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);

void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
#endif /* !USE_GDEMWIN */


void lcd_clear(uint16_t color);

#endif /* __BSP_LCD_H */
