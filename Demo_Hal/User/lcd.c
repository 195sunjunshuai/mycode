#include "lcd.h"
#include "stdio.h"
//#include "delay.h"
#include "font.h"


//LCD的画笔颜色和背景色	   
u16 POINT_COLOR = 0x0000;	//画笔颜色
u16 BACK_COLOR = 0xFFFF;  //背景色 

//LCD参数定义
_lcd_dev lcddev = {0};


/*-------------------------------封装接口函数------------------------*/

//写寄存器函数  命令
//regval:寄存器值
void LCD_WR_REG(u16 regval)
{   
	LCD->LCD_REG = regval;	//写入要写的寄存器序号	 
}

//写LCD数据
//data:要写入的值
void LCD_WR_DATA(u16 data)
{	 
	LCD->LCD_RAM = data;		 
}

//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
	vu16 ram;			
	ram = LCD->LCD_RAM;	
	return ram;	 
}	

//像寄存器内写数据
//LCD_Reg:寄存器地址(命令)
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}


//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG = lcddev.wramcmd;	  
}	

/*-------------------------------验证通信是否正常------------------------*/
void LCD_ReadID(void)
{
	//尝试9341 ID的读取		
	LCD_WR_REG(0XD3);				   
	lcddev.id=LCD_RD_DATA();	//dummy read 	
	lcddev.id=LCD_RD_DATA();	//读到0X00
	lcddev.id=LCD_RD_DATA();   	//读取93								   
	lcddev.id<<=8;
	lcddev.id|=LCD_RD_DATA();  	//读取41	
	printf("lcd id：%X\r\n", lcddev.id);	
}

/*-------------------------------LCD屏幕初始化(参考厂家的代码)------------------------*/
void LCD_Config(void)
{
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0xC1); 
	LCD_WR_DATA(0X30);
	
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81);
	
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
	
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 
	
	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA(0x48); 
	
	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 
	
	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 
	
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 
	
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 
	
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x54); 
	LCD_WR_DATA(0XA9); 
	LCD_WR_DATA(0x43); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 	
	
	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x15); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x2B); 
	LCD_WR_DATA(0x56); 
	LCD_WR_DATA(0x3C); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x0F); 
	
	LCD_WR_REG(0x2B);   //设置屏幕宽和高 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);   //0x13F  319
	
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);	  //0xEF  239
	
	LCD_WR_REG(0x11); //Exit Sleep
//	Delay_ms(120);
	HAL_Delay(120);
	LCD_WR_REG(0x29); //display on		
}


//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试	   	   

	//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;   
	if(lcddev.dir==1)//横屏时
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 	
	if(lcddev.id==0x9341)//9341
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		dirreg=0X36;
		regval|=0X08;//5310/5510/1963不需要BGR      
		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)//1963不做坐标处理
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}else  
			{
				if(lcddev.width>lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}  
		}
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
  }
}
 



//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{	
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;	
		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;		
	}
	else
	{
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;	
		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;		
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}


/*---------------------应用层面函数----------------*/

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
#if (USE_FSMC==1)
		LCD->LCD_RAM=color;	
#elif (USE_FSMC==0)
		LCD_WR_DATA(color);
#endif
	}
}  


//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
	LCD_WR_REG(lcddev.setxcmd); //0x2A
	LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);
  	
	LCD_WR_REG(lcddev.setycmd); //0x2B
	LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
} 

void LCD_Init(void)
{
#if (USE_FSMC==1)
//	LCD_GPIOConfig();   
//	LCD_FSMCConfig();
#elif (USE_FSMC==0)
	LCD_GPIOConfig(0); 	//配置成输出模式
#endif
//	Delay_ms(50);
	HAL_Delay(50);
	LCD_ReadID();
	LCD_Config();
	LCD_Display_Dir(0);		//默认为竖屏
	LCD_LED(1);				//点亮背光
	LCD_Clear(WHITE);
}


//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
//void LCD_SetCursor(u16 Xpos, u16 Ypos)
//{	 
//	LCD_WR_REG(lcddev.setxcmd); //0x2A
//	LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);
//  	
//	LCD_WR_REG(lcddev.setycmd); //0x2B
//	LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
//} 
	
	LCD_WriteRAM_Prepare();	//开始写入GRAM
#if (USE_FSMC==1)
		LCD->LCD_RAM=POINT_COLOR; 
#elif (USE_FSMC==0)
		LCD_WR_DATA(POINT_COLOR);
#endif	
}


//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);  			 
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 	
#if (USE_FSMC==1)	
	LCD->LCD_REG=lcddev.wramcmd; 
	LCD->LCD_RAM=color; 
#elif (USE_FSMC==0)	
	LCD_WR_REG(lcddev.wramcmd);
	LCD_WR_DATA(color);
#endif
}


//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(sx>>8); 
	LCD_WR_DATA(sx&0XFF);	 
	LCD_WR_DATA(twidth>>8); 
	LCD_WR_DATA(twidth&0XFF);  
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(sy>>8); 
	LCD_WR_DATA(sy&0XFF); 
	LCD_WR_DATA(theight>>8); 
	LCD_WR_DATA(theight&0XFF); 
}


//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(sx,i);      				//设置光标位置 
		LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
		for(j=0;j<xlen;j++)
		{			
#if (USE_FSMC==1)
				LCD->LCD_RAM=color;	//显示颜色  
#elif (USE_FSMC==0)
				LCD_WR_DATA(color);
#endif				
		}			
	} 
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width; 
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)
		{
#if (USE_FSMC==1)
				LCD->LCD_RAM=color[i*width+j];//写入数据 
#elif (USE_FSMC==0)
				LCD_WR_DATA(color[i*width+j]);
#endif				
		}
	}		  
}  

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
// 局部清屏：清 (x,y) 到 (x+w-1,y+h-1)
static inline void LCD_ClearRect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
    LCD_Set_Window(x, y, x + w - 1, y + h - 1); // 你文件里的设置窗口函数
    LCD_WriteRAM_Prepare();                     // 准备连续写GRAM
    for (u32 i = 0; i < (u32)w * h; i++)
#if (USE_FSMC==1)
        LCD->LCD_RAM = color;
#else
        LCD_WR_DATA(color);
#endif
}

// 在 (x,y) 以 size 字高显示一整行字符串（先清再画）
void LCD_ShowStringBox(u16 x, u16 y, u8 size, const char *s)
{
    u16 len = 0;
    const char *p = s;
    while(*p) { len++; p++; }

    u16 w = len * (size/2);  // 你的字库宽度= size/2
    u16 h = size;

    LCD_ClearRect(x, y, w, h, BACK_COLOR);         // 1) 只清这一小块
    LCD_ShowString(x, y, w, h, size, (u8*)s);     // 2) 透明模式重新画
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
	while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
	{       
			if(x>=width){x=x0;y+=size;}
			if(y>=height)break;//退出
			LCD_ShowChar(x,y,*p,size,0);
			x+=size/2;
			p++;
	}  
}


//显示图片
//x,y:起点坐标
//width,height:区域大小
//*p:图片起始地址
void LCD_ShowPhoto(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *p)
{
	uint16_t i=0, j=0;
	uint16_t pcolor = 0;
	LCD_Set_Window(x, y, width, height);
#if (USE_FSMC==1)
				LCD->LCD_REG=lcddev.wramcmd;  
#elif (USE_FSMC==0)
				LCD_WR_REG(lcddev.wramcmd);
#endif	
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			pcolor = (*p<<8)|(*(p+1));		
#if (USE_FSMC==1)
				LCD->LCD_RAM=pcolor;  //写显示颜色
#elif (USE_FSMC==0)
				LCD_WR_DATA(pcolor);
#endif				
			p+=2;
		}
	}
	LCD_Set_Window(x, y, lcddev.width, lcddev.height);
}

uint8_t chen[]={0x00,0x40,0x78,0x40,0x48,0x40,0x57,0xFE,0x50,0x80,0x61,0x20,0x51,0x20,0x4A,0x20,0x4B,0xFC,0x48,0x20,0x69,0x28,0x51,0x24,0x42,0x22,0x44,0x22,0x40,0xA0,0x40,0x40};/*"陈",0*/

//显示汉字
//只显示16*16的汉字
void LCD_ShowHz(uint16_t x, uint16_t y, uint16_t backColor, uint16_t fontColor, uint8_t buff[])
{
	uint16_t cnt = 0;
	for(uint8_t i=0;i<32;i++) {
		for(uint8_t j=0;j<8;j++) {
			if((buff[i] & (1<<(7-j))) != 0) {
				LCD_Fast_DrawPoint(cnt%16+x, cnt/16+y, fontColor);	
			}
			else {
				LCD_Fast_DrawPoint(cnt%16+x, cnt/16+y, backColor);
			}
			cnt++;	
		}
	}
}
void LCD_DrawLine(int x0,int y0,int x1,int y1,uint16_t color)
{
	int dx=x1-x0;
	int dy=y1-y0;
	int sx=dx>=0?1:0;
	int sy=dy>=0?1:0;
	dx=dx>=0?dx:-dx;
	dy=dy>=0?dy:-dy;
	if(dx==0){
		if(y0>y1){	int t=y0;y0=y1;y1=t;}
		for(int y=y0;y<=y1;y++)
		{
			LCD_Fast_DrawPoint(x0,y,color);
		}
		return ;
	}
	if(dy==0)
	{
		if(x0>x1){	int t=x0;x0=x1;x1=t;}
		for(int x=x0;x<=x1;x++)
		{
			LCD_Fast_DrawPoint(x,y0,color);
		}
		return ;
	}
	int err=dy-dx;
	while(1)
	{
		if(x0==x1&&y0==y1) break;
		LCD_Fast_DrawPoint(x0,y0,color);
		int e2=err*2;
		if(e2>-dy) {err-=dy;x0+=sx;}
		if(e2<dx) {err+=dx;y0+=sy;}
	}
}



//可以显示汉字的字符串
char hz1616[] = "孙军帅年月日时分秒";

uint8_t Hz_16_16[][32] = {
{0x00,0x20,0x7E,0x20,0x02,0x20,0x04,0x20,0x08,0x20,0x08,0xA8,0x0A,0xA4,0x0C,0xA4,0x39,0x22,0xC9,0x22,0x0A,0x22,0x08,0x20,0x08,0x20,0x08,0x20,0x28,0xA0,0x10,0x40},/*"孙",0*/
{0x00,0x00,0x7F,0xFE,0x40,0x02,0x82,0x04,0x02,0x00,0x3F,0xF8,0x04,0x00,0x09,0x00,0x11,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00},/*"军",1*/
{0x08,0x20,0x08,0x20,0x48,0x20,0x48,0x20,0x49,0xFC,0x49,0x24,0x49,0x24,0x49,0x24,0x49,0x24,0x49,0x24,0x49,0x24,0x09,0x34,0x11,0x28,0x10,0x20,0x20,0x20,0x40,0x20},/*"帅",2*/
{0x10,0x00,0x10,0x00,0x1F,0xFC,0x20,0x80,0x20,0x80,0x40,0x80,0x1F,0xF8,0x10,0x80,0x10,0x80,0x10,0x80,0xFF,0xFE,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80},/*"年",0*/
{0x00,0x00,0x1F,0xF8,0x10,0x08,0x10,0x08,0x10,0x08,0x1F,0xF8,0x10,0x08,0x10,0x08,0x10,0x08,0x1F,0xF8,0x10,0x08,0x10,0x08,0x20,0x08,0x20,0x08,0x40,0x28,0x80,0x10},/*"月",1*/
{0x00,0x00,0x1F,0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10},/*"日",2*/
{0x00,0x08,0x00,0x08,0x7C,0x08,0x44,0x08,0x45,0xFE,0x44,0x08,0x44,0x08,0x7C,0x08,0x44,0x88,0x44,0x48,0x44,0x48,0x44,0x08,0x7C,0x08,0x44,0x08,0x00,0x28,0x00,0x10},/*"时",3*/
{0x00,0x40,0x04,0x40,0x04,0x20,0x08,0x20,0x10,0x10,0x20,0x08,0x40,0x04,0x9F,0xE2,0x04,0x20,0x04,0x20,0x04,0x20,0x08,0x20,0x08,0x20,0x10,0x20,0x21,0x40,0x40,0x80},/*"分",4*/
{0x08,0x20,0x1C,0x20,0xF0,0x20,0x10,0xA8,0x10,0xA4,0xFC,0xA2,0x11,0x22,0x31,0x20,0x3A,0x24,0x54,0x24,0x54,0x28,0x90,0x08,0x10,0x10,0x10,0x20,0x10,0xC0,0x13,0x00},/*"秒",5*/
};




void LCD_ShowChineseStr(uint16_t x, uint16_t y, uint16_t backColor, uint16_t fontColor, char str[])
{
	uint16_t i=0;
	while(str[i] != '\0')
	{
		uint16_t j=0;
		//逐个汉字的对比 -- 两个字节 两个字节的对比
		while(hz1616[j] != '\0')
		{
			if(str[i]==hz1616[j] && str[i+1]==hz1616[j+1])
			{
				LCD_ShowHz(x, y, backColor, fontColor, Hz_16_16[j/2]);
				x+=16;
				if(lcddev.width-x<16) {	
					y+=16;
					if(lcddev.height-y<16) {
						y=0;
					}
					x=0;
				}
				break;
			}
			j+=2;
		}
		i+=2;
	}
}





