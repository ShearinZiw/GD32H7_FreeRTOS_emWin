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

#define LCD_FRAME_BUFFER             (0xC0000000)    //第一层首地址
#define BUFFER_OFFSET                (800*480*2)     //一层液晶的数据量
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
  * @brief  字体对齐模式  
  */ 
typedef enum
{
  CENTER_MODE             = 0x01,    /* 居中对齐 */
  RIGHT_MODE              = 0x02,    /* 右对齐   */
  LEFT_MODE               = 0x03     /* 左对齐   */
}Text_AlignModeTypdef;

#define MAX_LAYER_NUMBER            (2)
#define LTDC_ACTIVE_LAYER	        (1) /* Layer 1 */


/**
  * @brief  LCD color
  */
/*
 *   使用以下两组颜色必须跟LCD_LayerInit()函数中对每层的设置对应，不然会导致颜色显示错误。
 * 特别为实现两层同时正常显示需要把上层(LCD_FOREGROUND_LAYER)设置为LTDC_Pixelformat_ARGB1555
 * 格式，对应的必须使用LCD_COLOR1555_XXX颜色，该格式在1位透明控制位，可以实现没有实际颜色填
 * 充的像素点通透显示,从而使得显示下层(LCD_BACKGROUND_LAYER)成为可能。如果上层是使用
 * LTDC_Pixelformat_ARGB565格式设置是无法正常显示双层的。
 *   当然，可以把下层设置为LTDC_Pixelformat_ARGB1555格式，使用用LCD_COLOR1555_XXX也是可以正
 * 常显示的。
 * 特别注意：ARGB1555其中的A对应开发板液晶来说设置为1表示不透明，设置为0表透明。
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


//红色数据线

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

//绿色数据线
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

//蓝色数据线
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

//控制信号线
/*像素时钟CLK*/
#define LTDC_CLK_GPIO_PORT              GPIOB
#define LTDC_CLK_GPIO_CLK       	    RCU_GPIOB
#define LTDC_CLK_GPIO_CLK_ENABLE        rcu_periph_clock_enable
#define LTDC_CLK_GPIO_PIN               GPIO_PIN_3
#define LTDC_CLK_GPIO_AF			    GPIO_AF_2
/*水平同步信号HSYNC*/
#define LTDC_HSYNC_GPIO_PORT            GPIOC
#define LTDC_HSYNC_GPIO_CLK             RCU_GPIOC
#define LTDC_HSYNC_GPIO_CLK_ENABLE      rcu_periph_clock_enable
#define LTDC_HSYNC_GPIO_PIN             GPIO_PIN_6
#define LTDC_HSYNC_GPIO_AF			    GPIO_AF_14
/*垂直同步信号VSYNC*/
#define LTDC_VSYNC_GPIO_PORT            GPIOA
#define LTDC_VSYNC_GPIO_CLK             RCU_GPIOA
#define LTDC_VSYNC_GPIO_CLK_ENABLE      rcu_periph_clock_enable
#define LTDC_VSYNC_GPIO_PIN             GPIO_PIN_4
#define LTDC_VSYNC_GPIO_AF			    GPIO_AF_14

/*数据使能信号DE*/
#define LTDC_DE_GPIO_PORT               GPIOF
#define LTDC_DE_GPIO_CLK                RCU_GPIOF
#define LTDC_DE_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_DE_GPIO_PIN                GPIO_PIN_10
#define LTDC_DE_GPIO_AF			        GPIO_AF_14

/*液晶屏背光信号，高电平使能*/
#define LTDC_BL_GPIO_PORT               GPIOG
#define LTDC_BL_GPIO_CLK                RCU_GPIOG
#define LTDC_BL_GPIO_CLK_ENABLE         rcu_periph_clock_enable
#define LTDC_BL_GPIO_PIN                GPIO_PIN_3
#define LTDC_BL_GPIO_AF			        GPIO_AF_14


/** 
  * @brief  LCD液晶类型
  */   
typedef enum
{ 
  INCH_5  = 0x00, /* 野火5寸屏 */
  INCH_7,     /* 野火7寸屏 */
  INCH_4_3,  /* 野火4.3寸屏 */
  
  LCD_TYPE_NUM /* LCD类型总数*/
}LCD_TypeDef;

/* 当前使用的LCD，默认为5寸屏 */
extern LCD_TypeDef cur_lcd;

/** 
  * @brief  LCD液晶参数
  */   
typedef struct
{
  /*根据液晶数据手册的参数配置*/
  uint8_t hbp;  //HSYNC后的无效像素
  uint8_t vbp;  //VSYNC后的无效行数

  uint8_t hsw;  	//HSYNC宽度
  uint8_t vsw;   //VSYNC宽度

  uint8_t hfp;  	//HSYNC前的无效像素
  uint8_t vfp;  	//VSYNC前的无效行数
  
  uint8_t comment_clock_2byte; //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
  uint8_t comment_clock_4byte; //Argb8888等四字节像素时推荐使用的液晶时钟频率

  uint16_t lcd_pixel_width; //液晶分辨率，宽
  uint16_t lcd_pixel_height;//液晶分辨率，高

}LCD_PARAM_TypeDef;

/* LCD 物理像素大小 (宽度和高度) */
#define  LCD_PIXEL_WIDTH          lcd_param[cur_lcd].lcd_pixel_width
#define  LCD_PIXEL_HEIGHT         lcd_param[cur_lcd].lcd_pixel_height

/*根据液晶数据手册的参数配置*/
#define HBP  lcd_param[cur_lcd].hbp		//HSYNC后的无效像素
#define VBP  lcd_param[cur_lcd].vbp		//VSYNC后的无效行数

#define HSW  lcd_param[cur_lcd].hsw		//HSYNC宽度
#define VSW  lcd_param[cur_lcd].vsw		//VSYNC宽度

#define HFP  lcd_param[cur_lcd].hfp		//HSYNC前的无效像素
#define VFP  lcd_param[cur_lcd].vfp		//VSYNC前的无效行数

/* 当前使用的LCD，默认为5寸屏 */
extern LCD_TypeDef cur_lcd;
/* 不同液晶屏的参数 */
extern const LCD_PARAM_TypeDef lcd_param[];

/** 
  * @brief LCD default font 
  */ 
#define LCD_DEFAULT_FONT         Font24 


void LCD_GPIO_Config(void);
void LCD_Init(void);
void LCD_LayerInit(void);
void Colour_Set(uint32_t layerx,uint8_t redkey,uint8_t greenkey,uint8_t bluekey);

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

void lcd_clear(uint16_t color);

#endif /* __BSP_LCD_H */
