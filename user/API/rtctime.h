#ifndef __RTCTIME_H_ 
#define __RTCTIME_H_ 
 
#include "stm32f10x.h"
#include "lcd.h"
#include "rtc.h"
#include "time.h"
#include "stdio.h"
#include "string.h"

// ========== 可调区域 ==========
#define FONT_H   24        // 字号：12/16/24/32 ——与你字库一致
#define ROW_H    (FONT_H+2)
#define COL0_X   10
#define COL1_X   160
#define AREA_W   150
#define AREA_H   (ROW_H)

// 颜色（按你的定义来）
#define FG_COLOR WHITE
#define BG_COLOR BLACK

// ========== 状态 ==========
typedef enum { MODE_CLOCK = 0, MODE_SW = 1 } ui_mode_t;
static volatile ui_mode_t gMode = MODE_CLOCK;
extern uint8_t Keys_Process_FLAG;
// 秒表计数（单位 ms）
static volatile uint32_t sw_ms = 0;
static volatile uint8_t  sw_running = 0;

// RTC 显示缓存，避免重复重绘
static char last_time[16] = "00:00:00";
static char last_date[16] = "2000-01-01";

// ========== 你已有的标志 ==========


// ========== 简单的按键桩函数（自行替换）==========
int KEY_MODE(void);   // 切换时钟/秒表
int KEY_START(void);  // 秒表开始/暂停
int KEY_RESET(void);  // 秒表清零
extern void Keys_Process(void);
extern void UI_SwitchTo(ui_mode_t m);
void UI_RefreshStopwatch(uint8_t force);
void UI_PrintLine(u16 x, u16 y, const char* s);
void UI_DrawTitle(const char* title);
void LCD_FillRect(u16 x, u16 y, u16 w, u16 h, u16 color);
void UI_RefreshClock(void);
#endif