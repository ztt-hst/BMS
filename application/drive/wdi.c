#include "wdi.h"

/* 记录当前的喂狗阶段 */
volatile static uint8_t watchdog_state = 0;  // 0: 等待喂狗, 1: 10ms内拉低

/**
  * @brief  初始化WDI引脚
  * @note   配置PC8为推挽输出，初始电平为低
  * @param  无
  * @retval 无
  */
void WDI_Init(void)
{
    /* 使能GPIO时钟 */
    rcu_periph_clock_enable(WDI_CLK);
    
    /* 配置WDI引脚为推挽输出模式 */
    gpio_init(WDI_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, WDI_PIN);
    
    /* 初始设置WDI为低电平 */
    gpio_bit_reset(WDI_GPIO_PORT, WDI_PIN);
}

/**
  * @brief  初始化自动喂狗定时器
  * @note   配置TIMER2为10kHz计数频率，初始周期1秒
  * @param  无
  * @retval 无
  */
void WDI_AutoFeed_Init(void)
{
    /* 定时器参数结构体 */
    timer_parameter_struct timer_initpara;

    /* 使能定时器时钟 */
    rcu_periph_clock_enable(WDI_TIMER_CLK);

    /* 定时器基础配置 */
    timer_deinit(WDI_TIMER); 
    
    /* 配置定时器参数 */
    timer_initpara.prescaler         = 11999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 10000;  //10000*0.1ms=1s
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(WDI_TIMER, &timer_initpara);

    /* 使能定时器中断 */
    timer_interrupt_enable(WDI_TIMER, TIMER_INT_UP);
    
    /* 配置NVIC，优先级为2 */
    nvic_irq_enable(WDI_TIMER_IRQn, 2, 0);
}

/**
  * @brief  启动自动喂狗
  * @note   使能定时器开始自动喂狗
  * @param  无
  * @retval 无
  */
void WDI_AutoFeed_Start(void)
{
    watchdog_state = 0;
    timer_enable(WDI_TIMER);
}

/**
  * @brief  停止自动喂狗
  * @note   关闭定时器并将WDI引脚置低
  * @param  无
  * @retval 无
  */
void WDI_AutoFeed_Stop(void)
{
    timer_disable(WDI_TIMER);
    gpio_bit_reset(WDI_GPIO_PORT, WDI_PIN);
}

/**
  * @brief  定时器2中断服务函数
  * @note   实现自动喂狗的时序控制
  *         - 1秒定时到，输出高电平并设置10ms定时
  *         - 10ms定时到，输出低电平并设置1秒定时
  * @param  无
  * @retval 无
  */
void TIMER2_IRQHandler(void)
{
    if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) != RESET)
    {
        /* 清除中断标志 */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
        
        if(watchdog_state == 0)
        {
            /* 第一次触发（1s到期）：拉高WDI */
            gpio_bit_set(WDI_GPIO_PORT, WDI_PIN);
            watchdog_state = 1;
            TIMER_CAR(TIMER2) = 100;    // 设置10ms (100 * 0.1ms)
        }
        else
        {
            /* 第二次触发（10ms到期）：拉低WDI */
            gpio_bit_reset(WDI_GPIO_PORT, WDI_PIN);
            watchdog_state = 0;
            TIMER_CAR(TIMER2) = 10000;  // 设置1s (1000 * 0.1ms)
        }
    }
}
