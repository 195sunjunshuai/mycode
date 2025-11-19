#include "Fontdownload.h" 
 
uint32_t Font_Rec_Cont=0;    //已经接收到的字库字节数 
uint8_t  Font_Rec_Ing=0;     //0表示未接收   1表示正在接收 
uint8_t  Font_Rec_Timeout=0; //超时时间 