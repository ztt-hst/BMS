#include "gd32f30x.h"
#include "gd32f303e_eval.h"

typedef struct {
    uint32_t        port;
    uint32_t        pin;
    rcu_periph_enum clk;
} LED_ConfigTypeDef;

static const LED_ConfigTypeDef s_led_config[LEDn] = {
    [LED2] = {
        .port = LED2_GPIO_PORT,
        .pin  = LED2_PIN,
        .clk  = LED2_GPIO_CLK
    },
    [LED3] = {
        .port = LED3_GPIO_PORT,
        .pin  = LED3_PIN,
        .clk  = LED3_GPIO_CLK
    },
    [LED4] = {
        .port = LED4_GPIO_PORT,
        .pin  = LED4_PIN,
        .clk  = LED4_GPIO_CLK
    },
    [LED5] = {
        .port = LED5_GPIO_PORT,
        .pin  = LED5_PIN,
        .clk  = LED5_GPIO_CLK
    }
};

void gd_eval_led_init(led_typedef_enum lednum)
{
    const LED_ConfigTypeDef *p_led = &s_led_config[lednum];

    rcu_periph_clock_enable(p_led->clk);

    gpio_init(p_led->port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, p_led->pin);

    GPIO_BC(p_led->port) = p_led->pin;
}

void gd_eval_led_on(led_typedef_enum lednum)
{
    const LED_ConfigTypeDef *p_led = &s_led_config[lednum];
    GPIO_BOP(p_led->port) = p_led->pin;
}

void gd_eval_led_off(led_typedef_enum lednum)
{
    const LED_ConfigTypeDef *p_led = &s_led_config[lednum];
    GPIO_BC(p_led->port) = p_led->pin;
}


void gd_eval_led_toggle(led_typedef_enum lednum)
{
    const LED_ConfigTypeDef *p_led = &s_led_config[lednum];

    gpio_bit_write(
        p_led->port,
        p_led->pin,
        (bit_status)(1U - gpio_input_bit_get(p_led->port, p_led->pin))
    );
}

typedef struct {
    uint32_t        port;
    uint32_t        pin;
    rcu_periph_enum clk;
    exti_line_enum  exti_line;
    uint8_t         port_source;
    uint8_t         pin_source;
    uint8_t         irqn;
} KEY_ConfigTypeDef;

static const KEY_ConfigTypeDef s_key_config[KEYn] = {
    [KEY_WAKEUP] = {
        .port        = WAKEUP_KEY_GPIO_PORT,
        .pin         = WAKEUP_KEY_PIN,
        .clk         = WAKEUP_KEY_GPIO_CLK,
        .exti_line   = WAKEUP_KEY_EXTI_LINE,
        .port_source = WAKEUP_KEY_EXTI_PORT_SOURCE,
        .pin_source  = WAKEUP_KEY_EXTI_PIN_SOURCE,
        .irqn        = WAKEUP_KEY_EXTI_IRQn
    },
    [KEY_TAMPER] = {
        .port        = TAMPER_KEY_GPIO_PORT,
        .pin         = TAMPER_KEY_PIN,
        .clk         = TAMPER_KEY_GPIO_CLK,
        .exti_line   = TAMPER_KEY_EXTI_LINE,
        .port_source = TAMPER_KEY_EXTI_PORT_SOURCE,
        .pin_source  = TAMPER_KEY_EXTI_PIN_SOURCE,
        .irqn        = TAMPER_KEY_EXTI_IRQn
    },
    [KEY_USER1] = {
        .port        = USER_KEY1_GPIO_PORT,
        .pin         = USER_KEY1_PIN,
        .clk         = USER_KEY1_GPIO_CLK,
        .exti_line   = USER_KEY1_EXTI_LINE,
        .port_source = USER_KEY1_EXTI_PORT_SOURCE,
        .pin_source  = USER_KEY1_EXTI_PIN_SOURCE,
        .irqn        = USER_KEY1_EXTI_IRQn
    },
    [KEY_USER2] = {
        .port        = USER_KEY2_GPIO_PORT,
        .pin         = USER_KEY2_PIN,
        .clk         = USER_KEY2_GPIO_CLK,
        .exti_line   = USER_KEY2_EXTI_LINE,
        .port_source = USER_KEY2_EXTI_PORT_SOURCE,
        .pin_source  = USER_KEY2_EXTI_PIN_SOURCE,
        .irqn        = USER_KEY2_EXTI_IRQn
    }
};

void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode)
{
    const KEY_ConfigTypeDef *p_key = &s_key_config[key_num];

    rcu_periph_clock_enable(p_key->clk);
    rcu_periph_clock_enable(RCU_AF);

    gpio_init(p_key->port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, p_key->pin);

    if (KEY_MODE_EXTI == key_mode) {
        nvic_irq_enable(p_key->irqn, 2U, 0U);

        gpio_exti_source_select(p_key->port_source, p_key->pin_source);

        exti_init(p_key->exti_line, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(p_key->exti_line);
    }
}

uint8_t gd_eval_key_state_get(key_typedef_enum key_num)
{
    const KEY_ConfigTypeDef *p_key = &s_key_config[key_num];
    return (uint8_t)gpio_input_bit_get(p_key->port, p_key->pin);
}

typedef struct {
    rcu_periph_enum gpio_clk;
    rcu_periph_enum usart_clk;
    uint32_t        tx_pin;
    uint32_t        rx_pin;
    uint32_t        gpio_port;
} COM_ConfigTypeDef;

static const COM_ConfigTypeDef s_com_config[COMn] = {
    [0] = {
        .gpio_clk  = EVAL_COM1_GPIO_CLK,
        .usart_clk = EVAL_COM1_CLK,
        .tx_pin    = EVAL_COM1_TX_PIN,
        .rx_pin    = EVAL_COM1_RX_PIN,
        .gpio_port = EVAL_COM1_GPIO_PORT
    },
    [1] = {
        .gpio_clk  = EVAL_COM2_GPIO_CLK,
        .usart_clk = EVAL_COM2_CLK,
        .tx_pin    = EVAL_COM2_TX_PIN,
        .rx_pin    = EVAL_COM2_RX_PIN,
        .gpio_port = EVAL_COM2_GPIO_PORT
    }
};

void gd_eval_com_init(uint32_t com)
{
    uint32_t com_id = (EVAL_COM1 == com) ? 0U : 1U;

    const COM_ConfigTypeDef *p_com = &s_com_config[com_id];

    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(p_com->gpio_clk);
    rcu_periph_clock_enable(p_com->usart_clk);

    gpio_init(p_com->gpio_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, p_com->tx_pin);
    gpio_init(p_com->gpio_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, p_com->rx_pin);

    usart_deinit(com);
    usart_baudrate_set(com, 115200U);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);
    usart_enable(com);
}

void gd_eval_com_deinit(uint32_t com)
{
    uint32_t com_id = (EVAL_COM1 == com) ? 0U : 1U;
    const COM_ConfigTypeDef *p_com = &s_com_config[com_id];

    rcu_periph_clock_disable(p_com->gpio_clk);
    rcu_periph_clock_disable(p_com->usart_clk);

    usart_deinit(com);
}

void usartSend(uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        usart_data_transmit(USART0, (uint8_t)buffer[i]);
        while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
            ;
    }
}

#if defined(__GNUC__)
int _write(int fd, char *ptr, int len)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ptr[0]);
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE))
        ;
    return fd;
}
#endif
