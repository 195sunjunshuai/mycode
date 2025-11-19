#include "rtctime.h"
// 简易局部清除：在 (x,y) 大小 w*h 内填充背景色
void LCD_FillRect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
    u16 x2 = x + w - 1;
    u16 y2 = y + h - 1;
    if (x2 >= lcddev.width)  x2 = lcddev.width - 1;
    if (y2 >= lcddev.height) y2 = lcddev.height - 1;

    LCD_Set_Window(x, y, x2, y2); // 你的设置窗口函数
    LCD_WriteRAM_Prepare();
    for (u32 i = 0; i < (u32)w * h; i++)
    {
#if (USE_FSMC==1)
        LCD->LCD_RAM = color;
#else
        LCD_WR_DATA(color);
#endif
    }
    LCD_Set_Window(0,0,lcddev.width-1,lcddev.height-1); // 还原全窗
}
// 画大标题
void UI_DrawTitle(const char* title)
{
    LCD_FillRect(0, 0, lcddev.width, ROW_H, BG_COLOR);
    LCD_ShowString(COL0_X, 0, lcddev.width, ROW_H, FONT_H, (u8*)title);
}

// 局部更新一行文字：先清行再写
void UI_PrintLine(u16 x, u16 y, const char* s)
{
    LCD_FillRect(x, y, AREA_W, AREA_H, BG_COLOR);
    LCD_ShowString(x, y, AREA_W, AREA_H, FONT_H, (u8*)s);
}

// 刷新时钟界面
void UI_RefreshClock(void)
{
    // 每秒触发：RTC_Second_FLAG 在主循环里判断
    uint32_t ts = RTC_GetCounter();
    struct tm* t = localtime((time_t*)&ts);

    char now_time[16];
    char now_date[16];
    sprintf(now_time, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    sprintf(now_date, "%04d-%02d-%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday);

    if (strcmp(now_time, last_time) != 0) {
        strcpy(last_time, now_time);
        UI_PrintLine(COL0_X, ROW_H*1, now_time);
    }
    if (strcmp(now_date, last_date) != 0) {
        strcpy(last_date, now_date);
        UI_PrintLine(COL0_X, ROW_H*2, now_date);
    }

    // 串口输出准确时间
    printf("TIME %s %s\r\n", now_date, now_time);
}

// 刷新秒表界面（按需刷新，避免无谓重绘）
void UI_RefreshStopwatch(uint8_t force)
{
    // sw_ms -> mm:ss.ms(两位)
    uint32_t ms  = sw_ms % 1000;
    uint32_t sec = (sw_ms / 1000) % 60;
    uint32_t min = (sw_ms / 60000);

    static uint32_t last_show = 0xFFFFFFFF;

    // 每 10ms 更新一次显示（或被 force 强制）
    uint32_t pack = (min<<16) | (sec<<8) | (ms/10);
    if (!force && pack == last_show) return;
    last_show = pack;

    char line[20];
    sprintf(line, "%02lu:%02lu.%02lu", (unsigned long)min, (unsigned long)sec, (unsigned long)(ms/10));
    UI_PrintLine(COL0_X, ROW_H*1, line);

    UI_PrintLine(COL0_X, ROW_H*2, sw_running ? "RUN" : "PAUSE");
}
void UI_SwitchTo(ui_mode_t m)
{
    gMode = m;
    LCD_FillRect(0, 0, lcddev.width, lcddev.height, BG_COLOR);
    if (gMode == MODE_CLOCK) {
        UI_DrawTitle("Clock");
        // 立即强刷一次
        last_time[0] = 0; last_date[0] = 0;
        UI_RefreshClock();
    } else {
        UI_DrawTitle("Stopwatch");
        UI_RefreshStopwatch(1);
    }
}
uint8_t Keys_Process_FLAG;
void Keys_Process(void)
{
    if (Keys_Process_FLAG==1) {
        UI_SwitchTo(gMode == MODE_CLOCK ? MODE_SW : MODE_CLOCK);
    }
    if (gMode == MODE_SW) {
        if (Keys_Process_FLAG==2) sw_running = !sw_running;
        if (Keys_Process_FLAG==3) { sw_running = 0; sw_ms = 0; UI_RefreshStopwatch(1); }
    }
}