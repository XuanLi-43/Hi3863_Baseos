#include "bsp_uart.h"

#define BSP_UART_DEFAULT_BAUDRATE       115200U             /* 默认波特率 115200 */

/**
 * @brief 初始化 UART
 *
 * 依次配置 TX/RX 引脚复用模式、串口参数和接收缓冲区，
 * 初始化完成后重新下发一次串口属性，保证流控等参数生效。
 *
 * @param [in] uart_cfg UART 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空、总线号非法或未指定接收缓冲区，ERRCODE_FAIL 失败
 */
errcode_t bsp_uart_init(const bsp_uart_cfg_t *uart_cfg)
{
    errcode_t ret;
    uint32_t baudrate;

    uart_attr_t attr;
    uart_pin_config_t pin_config;
    uart_buffer_config_t buffer_config;

    /* 参数合法性检查，接收缓冲区必须由用户提供 */
    if((uart_cfg == NULL) || (uart_cfg->bus >= UART_BUS_MAX_NUM) ||
       (uart_cfg->rx_buf_cfg.rx_buffer == NULL) || (uart_cfg->rx_buf_cfg.rx_buffer_size == 0))
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 未配置时使用默认值 */
    baudrate = (uart_cfg->baudrate == 0) ? BSP_UART_DEFAULT_BAUDRATE : uart_cfg->baudrate;

    /* 已初始化的串口不会被重新配置，先关闭再重新初始化，保证新配置生效 */
    (void)uapi_uart_deinit(uart_cfg->bus);

    /* 配置 TX 引脚复用模式 */
    ret = uapi_pin_set_mode(uart_cfg->tx_pin, uart_cfg->tx_pin_mode);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 配置 RX 引脚复用模式 */
    ret = uapi_pin_set_mode(uart_cfg->rx_pin, uart_cfg->rx_pin_mode);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 串口基本参数 */
    attr.baud_rate = baudrate;
    attr.data_bits = uart_cfg->data_bits;
    attr.parity    = uart_cfg->parity;
    attr.flow_ctrl = uart_cfg->flow_ctrl;
    attr.stop_bits = uart_cfg->stop_bits;

    /* 串口引脚，未使用硬件流控时 RTS/CTS 引脚留空 */
    pin_config.tx_pin  = uart_cfg->tx_pin;
    pin_config.rx_pin  = uart_cfg->rx_pin;
    pin_config.cts_pin = PIN_NONE;
    pin_config.rts_pin = PIN_NONE;

    /* 接收缓冲区 */
    buffer_config.rx_buffer      = uart_cfg->rx_buf_cfg.rx_buffer;
    buffer_config.rx_buffer_size = uart_cfg->rx_buf_cfg.rx_buffer_size;

    /* 初始化串口 */
    ret = uapi_uart_init(uart_cfg->bus, &pin_config, &attr, NULL, &buffer_config);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 初始化时未开启流控配置项，这里重新下发属性，使 flow_ctrl 等参数生效 */
    ret = uapi_uart_set_attr(uart_cfg->bus, &attr);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 去初始化 UART
 *
 * 注销接收回调并关闭串口，释放串口占用的引脚。
 *
 * @param [in] uart_cfg UART 配置结构体指针（使用其中的总线号）
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空或总线号非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_uart_deinit(const bsp_uart_cfg_t *uart_cfg)
{
    errcode_t ret;

    /* 参数合法性检查 */
    if((uart_cfg == NULL) || (uart_cfg->bus >= UART_BUS_MAX_NUM))
    {
        return ERRCODE_INVALID_PARAM;
    }

#if defined(CONFIG_UART_SUPPORT_RX)
    /* 注销接收回调，避免回调访问已释放的接收缓冲区 */
    uapi_uart_unregister_rx_callback(uart_cfg->bus);
#endif  /* CONFIG_UART_SUPPORT_RX */

    ret = uapi_uart_deinit(uart_cfg->bus);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
