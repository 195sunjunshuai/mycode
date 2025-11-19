#ifndef _DOWNLOAD_FONT_H_ 
#define _DOWNLOAD_FONT_H_ 
 
#include "stm32f10x.h" 
 
/*W25Q64    64MBit=8MByte   地址范围  0x000000--0x7FFFFF 
存储结构 
页(256Byte)     扇区(16页=16*256=4096字节)      块(16扇区=16*256*256)     
最小的擦除单位是扇区,写入之前必须擦除 
 
16号字库  
大小994319字节,994319/4096=242.8,所以分配243个扇区(扇区0--扇区242(0xF2000)) 
 
24号字库 
大小1878629字节,1878629/4096=458.6,所以分配459个扇区(扇区239(0xEF000)--扇区697(0x2B9000)) 
*/ 
 
#define   FONT_Down_Num         0    //16 下载16号字库   24 下载24号字库 
 
#define   FONT_16_Addr          0        //16号字库的起始地址 
#define   FONT_16_Sector_NUM    239      //16号字库占用的扇区数量 
 
#define   FONT_24_Addr          0xEF000     //24号字库的起始地址 
#define   FONT_24_Sector_NUM    459      //24号字库占用的扇区数量 
 
extern uint32_t Font_Rec_Cont; 
extern uint8_t  Font_Rec_Ing;     //0表示未接收   1表示正在接收 
extern uint8_t  Font_Rec_Timeout; //超时时间 
 
 
#endif 
