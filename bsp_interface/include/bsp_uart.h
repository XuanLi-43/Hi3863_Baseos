#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "pinctrl.h"
#include "uart.h"

typedef struct bsp_uart_rx_buf_cfg
{
    uint8_t     *rx_buffer;

    size_t      rx_buffer_size;

} bsp_uart_rx_buf_cfg_t;

typedef struct bsp_uart_cfg                               /* UART 基础配置，只包含引脚和串口参数 */
{
    uart_bus_t           bus;                             /* 选择串口号 */

    uint32_t             baudrate;                        /* 设置波特率 */

    uart_data_bit_t      data_bits;                       /* 设置数据位 */

    uart_stop_bit_t      stop_bits;                       /* 设置停止位*/

    uart_parity_t        parity;                          /* 设置校验位 */

    uart_flow_ctrl_t     flow_ctrl;                       /* 设置流控类型 */

    pin_t                tx_pin;                          /* 选择 TX 引脚编号 */

    pin_t                rx_pin;                          /* 选择 RX 引脚编号 */

    pin_mode_t           tx_pin_mode;                     /* 设置 TX 引脚复用模式，按开发板引脚复用表填写 */

    pin_mode_t           rx_pin_mode;                     /* 设置 RX 引脚复用模式，按开发板引脚复用表填写 */

    bsp_uart_rx_buf_cfg_t rx_buf_cfg;

} bsp_uart_cfg_t;

/**
 * @brief 初始化 UART
 *
 * 依次配置 TX/RX 引脚复用模式、串口参数和接收缓冲区，初始化完成后重新下发一次串口属性，
 * 保证流控等参数生效。已初始化过的串口会先关闭再重新初始化。
 *
 * 使用示例：
 * @code
 * static uint8_t g_uart_rx_buff[128];
 * bsp_uart_cfg_t cfg = {
 *     .bus = UART_BUS_1,
 *     .baudrate = 115200,
 *     .data_bits = UART_DATA_BIT_8,
 *     .stop_bits = UART_STOP_BIT_1,
 *     .parity = UART_PARITY_NONE,
 *     .flow_ctrl = UART_FLOW_CTRL_NONE,
 *     .tx_pin = BSP_UART_TX_PIN,
 *     .rx_pin = BSP_UART_RX_PIN,
 *     .tx_pin_mode = PIN_MODE_1,
 *     .rx_pin_mode = PIN_MODE_1,
 *     .rx_buf_cfg = { .rx_buffer = g_uart_rx_buff, .rx_buffer_size = sizeof(g_uart_rx_buff) },
 * };
 * bsp_uart_init(&cfg);
 * @endcode
 *
 * @param [in] uart_cfg UART 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空、总线号非法或未指定接收缓冲区，ERRCODE_FAIL 失败
 */
errcode_t bsp_uart_init(const bsp_uart_cfg_t *uart_cfg);

/**
 * @brief 去初始化 UART
 *
 * 注销接收回调并关闭串口，释放串口占用的引脚。
 *
 * @param [in] uart_cfg UART 配置结构体指针（使用其中的总线号）
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空或总线号非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_uart_deinit(const bsp_uart_cfg_t *uart_cfg);


#endif /* __BSP_UART_H__ */
