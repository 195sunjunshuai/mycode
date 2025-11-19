#ifndef __LCD_H_
#define __LCD_H_

#include "stm32f10x.h"


#define USE_FSMC   1   //0 表示不使用FSMC  1 表示使用FSMC


#if (USE_FSMC==1)
	//LCD的FSMC地址结构体
	typedef struct {
		vu16 LCD_REG;  //命令
		vu16 LCD_RAM;  //数据
	} LCD_TypeDef;
	//使用NOR/SRAM的 Bank1.sector4(6C000000 -- 6FFFFFFF),地址位HADDR[27,26]=11 A10作为数据命令区分线 
	//注意设置时STM32内部会右移一位对其! 			    
	#define LCD_BASE        ((u32)(0x6C000000 | 0x000007FE))   //0x6C0007FE  
	#define LCD             ((LCD_TypeDef *) LCD_BASE)	  //REG  0x6C0007FE   RAM  0x6C000800
#elif (USE_FSMC==0)
	#define	  D0_GPIO_Port   GPIOD
	#define		D1_GPIO_Port   GPIOD 
	#define		D2_GPIO_Port   GPIOD
	#define		D3_GPIO_Port   GPIOD
	#define		D4_GPIO_Port   GPIOE
	#define		D5_GPIO_Port   GPIOE
	#define		D6_GPIO_Port   GPIOE
	#define		D7_GPIO_Port   GPIOE
	#define		D8_GPIO_Port   GPIOE
	#define		D9_GPIO_Port   GPIOE
	#define		D10_GPIO_Port   GPIOE
	#define		D11_GPIO_Port   GPIOE
	#define		D12_GPIO_Port   GPIOE
	#define		D13_GPIO_Port		GPIOD
	#define		D14_GPIO_Port		GPIOD
	#define		D15_GPIO_Port		GPIOD
	
	

	#define	  D0_Pin         GPIO_Pin_14
	#define		D1_Pin         GPIO_Pin_15         
	#define		D2_Pin         GPIO_Pin_0        
	#define		D3_Pin         GPIO_Pin_1        
	#define		D4_Pin         GPIO_Pin_7         
	#define		D5_Pin         GPIO_Pin_8         
	#define		D6_Pin         GPIO_Pin_9         
	#define		D7_Pin         GPIO_Pin_10         
	#define		D8_Pin         GPIO_Pin_11         
	#define		D9_Pin         GPIO_Pin_12         	
	#define		D10_Pin         GPIO_Pin_13	
	#define		D11_Pin         GPIO_Pin_14	
	#define		D12_Pin         GPIO_Pin_15	
	#define		D13_Pin         GPIO_Pin_8	
	#define		D14_Pin         GPIO_Pin_9	
	#define		D15_Pin         GPIO_Pin_10	
	
#endif

//LCD参数
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;					//LCD ID
	u8  dir;				//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令 
}_lcd_dev; 	 


//LCD屏幕扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//LCD 背光控制
#define	LCD_LED(x)	(x?(GPIO_SetBits(GPIOB, GPIO_Pin_0)):(GPIO_ResetBits(GPIOB, GPIO_Pin_0)))	

#if (USE_FSMC==0)
//宏定义部分，使代码更容易理解
#define LCD_CS_LOW()           GPIO_WriteBit(GPIOG,GPIO_Pin_12,Bit_RESET)   //片选引脚   CS
#define LCD_CS_HIGH()          GPIO_WriteBit(GPIOG,GPIO_Pin_12,Bit_SET)
#define LCD_COMMAND()          GPIO_WriteBit(GPIOG,GPIO_Pin_0,Bit_RESET)   //数据命令选择线 D/CX
#define LCD_DATA()             GPIO_WriteBit(GPIOG,GPIO_Pin_0,Bit_SET)
#define LCD_R_DATA_ENABLE()    GPIO_WriteBit(GPIOD,GPIO_Pin_4,Bit_RESET)  //读使能
#define LCD_R_DATA_DISABLE()   GPIO_WriteBit(GPIOD,GPIO_Pin_4,Bit_SET)
#define LCD_W_DATA_ENABLE()    GPIO_WriteBit(GPIOD,GPIO_Pin_5,Bit_RESET)  //写使能
#define LCD_W_DATA_DISABLE()   GPIO_WriteBit(GPIOD,GPIO_Pin_5,Bit_SET)
#endif


//画笔颜色  RGB565
#define WHITE         	 	0xFFFF
#define BLACK         	 	0x0000	  
#define BLUE         	 		0x001F  
#define BRED             	0XF81F
#define GRED 			 				0XFFE0
#define GBLUE			 				0X07FF
#define RED           	 	0xF800
#define MAGENTA       	 	0xF81F
#define GREEN         	 	0x07E0
#define CYAN          	 	0x7FFF
#define YELLOW        	 	0xFFE0
#define BROWN 			 			0XBC40 //棕色
#define BRRED 			 			0XFC07 //棕红色
#define GRAY  			 			0X8430 //灰色



extern _lcd_dev lcddev;

#if (USE_FSMC==1)
void LCD_GPIOConfig(void);
void LCD_FSMCConfig(void);
#elif (USE_FSMC==0)
void LCD_GPIOConfig(uint8_t IO_Sate);
#endif

void LCD_WR_REG(u16 regval);
void LCD_WR_DATA(u16 data);
void LCD_WriteRAM_Prepare(void);
u16 LCD_RD_DATA(void);
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue);
void LCD_ReadID(void);
void LCD_Config(void);
void LCD_Scan_Dir(u8 dir);
void LCD_Display_Dir(u8 dir);
void LCD_Clear(u16 color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_Init(void);
void LCD_DrawPoint(u16 x,u16 y);
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fast_DrawPoint2(u16 x,u16 y,u16 *color);
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowPhoto(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *p);
void LCD_ShowHz(uint16_t x, uint16_t y, uint16_t backColor, uint16_t fontColor, uint8_t buff[]);
void LCD_ShowChineseStr(uint16_t x, uint16_t y, uint16_t backColor, uint16_t fontColor, char str[]);
#endif
