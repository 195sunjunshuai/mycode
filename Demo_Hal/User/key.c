#include "key.h" 
uint16_t KEY_Period[2]={0,2000}; 
//按键扫描函数    
/*返回值 
0  
无按键按下 
1  
按键1短按并松手 
2  
按键2短按并松手 
3  
按键3短按并松手 
4  
5  
6  
7  
8  
按键4短按并松手 
按键1长按并松手 
按键2长按并松手 
按键3长按并松手 
按键4长按并松手 
*/ 
uint8_t KEY_SCAN(void) 
{ 
	uint8_t KEY_State=0;  
	static uint16_t KEY_Cnt[4]={0};  //统计按下的时间 
	// if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET) 
	if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_SET) 
	{//检测到按键按下 
	KEY_Cnt[0]++;  //统计按下的时间++ 
	} 
	else  
	{//已经松手 
	if(KEY_Cnt[0]<=1) 
	{//误触发，不要有效按下 
	KEY_Cnt[0]=0; 
	} 
	else if(KEY_Cnt[0]>=2&&KEY_Cnt[0]<100) 
	{//短按，按下时间1s以内 
		KEY_Cnt[0]=0; 
		 KEY_State=1; 
		 return KEY_State; 
		} 
		else if(KEY_Cnt[0]>=100) 
		{//长按，按下时间大于1s 
		 KEY_Cnt[0]=0; 
		 KEY_State=5; 
		 return KEY_State;    
		} 
	 } 
// if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==Bit_RESET) 
 if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET)  
 {//检测到按键按下 
  KEY_Cnt[1]++;  //统计按下的时间++ 
 } 
 else  
 {//已经松手 
  if(KEY_Cnt[1]<=1) 
  {//误触发，不要有效按下 
   KEY_Cnt[1]=0; 
  } 
  else if(KEY_Cnt[1]>=2&&KEY_Cnt[1]<100) 
  {//短按，按下时间1s以内 
   KEY_Cnt[1]=0; 
   KEY_State=2; 
   return KEY_State; 
  } 
  else if(KEY_Cnt[1]>=100) 
  {//长按，按下时间大于1s 
   KEY_Cnt[1]=0; 
   KEY_State=6; 
   return KEY_State;    
  } 
 } 
// if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==Bit_RESET) 
 if(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==GPIO_PIN_RESET)  
 {//检测到按键按下 
  KEY_Cnt[2]++;  //统计按下的时间++ 
 } 
 else  
 {//已经松手 
  if(KEY_Cnt[2]<=1) 
  {//误触发，不要有效按下 
   KEY_Cnt[2]=0; 
  }
	else if(KEY_Cnt[2]>=2&&KEY_Cnt[2]<100) 
  {//短按，按下时间1s以内 
   KEY_Cnt[2]=0; 
   KEY_State=3; 
   return KEY_State; 
  } 
  else if(KEY_Cnt[2]>=100) 
  {//长按，按下时间大于1s 
   KEY_Cnt[2]=0; 
   KEY_State=7; 
   return KEY_State;    
  } 
 }  
// if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==Bit_RESET) 
 if(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==GPIO_PIN_RESET)   
 {//检测到按键按下 
  KEY_Cnt[3]++;  //统计按下的时间++ 
 } 
 else  
 {//已经松手 
  if(KEY_Cnt[3]<=1) 
  {//误触发，不要有效按下 
   KEY_Cnt[3]=0; 
  } 
  else if(KEY_Cnt[3]>=2&&KEY_Cnt[3]<100) 
  {//短按，按下时间1s以内 
   KEY_Cnt[3]=0; 
   KEY_State=4; 
   return KEY_State; 
  } 
  else if(KEY_Cnt[3]>=100) 
  {//长按，按下时间大于1s 
   KEY_Cnt[3]=0; 
   KEY_State=8; 
   return KEY_State;    
  } 
 }  
 return KEY_State;   
} 
 
 
 
//按键处理函数
void KEY_Handle(void) 
{  
 uint8_t KEY_State=0;
	KEY_State=KEY_SCAN();  //调用按键扫描函数 
	switch(KEY_State) 
	{ 
	case 0: //无按键按下 
	break; 
	case 1: //按键 1 短按并松手 
	HAL_GPIO_WritePin(GPIOE, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin, GPIO_PIN_SET); 
	break;   
	case 2: //按键 2 短按并松手 
	HAL_GPIO_WritePin(GPIOE, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin, GPIO_PIN_RESET); 
	break;   
	case 3: //按键 3 短按并松手 
	break;   
	case 4: //按键 4 短按并松手 
	break; 
	case 5: //按键 1 长按并松手 
	break;   
	case 6: //按键 2 长按并松手 
	break;   
	case 7: //按键 3 长按并松手 
	break;   
	case 8: //按键 4 长按并松手 
	break;   
	default: 
	break; 
	} 
}