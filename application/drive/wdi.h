#ifndef WDI_H
#define WDI_H

#include "gd32f30x.h"
#include "systick.h"    /* 用于delay_1ms函数 */

/* WDI引脚定义 */
#define WDI_PIN         GPIO_PIN_8      // PC8引脚
#define WDI_GPIO_PORT   GPIOC           // GPIOC端口
#define WDI_CLK         RCU_GPIOC       // GPIOC时钟

/* 定时器配置 */
#define WDI_TIMER              TIMER2    // 使用定时器2
#define WDI_TIMER_CLK         RCU_TIMER2 // 定时器2时钟
#define WDI_TIMER_IRQn        TIMER2_IRQn // 定时器2中断号
#define WDI_TIMER_IRQHandler  TIMER2_IRQHandler // 定时器2中断处理函数

/* 函数声明 */
void WDI_Init(void);           // 初始化WDI引脚
void WDI_Feed(void);
void WDI_AutoFeed_Init(void);  // 初始化自动喂狗定时器
void WDI_AutoFeed_Start(void); // 启动自动喂狗
void WDI_AutoFeed_Stop(void);  // 停止自动喂狗

#endif /* WDI_H */
