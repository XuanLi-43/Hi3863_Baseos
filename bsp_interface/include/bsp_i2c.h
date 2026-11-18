#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include "pinctrl.h"
#include "i2c.h"

typedef struct bsp_i2c_cfg                               /* I2C 基础配置 */
{
    i2c_bus_t            bus;                            /* 选择 I2C 总线号 */

    uint32_t             baudrate;                       /* 设置 I2C 波特率，为 0 时使用默认值 400KHz */

    uint8_t              hscode;                         /* 高速模式主机码（0~7），仅高速模式需要配置，其余填 0 */

    pin_t                scl_pin;                        /* 选择 SCL 引脚编号 */

    pin_t                sda_pin;                        /* 选择 SDA 引脚编号 */

    pin_mode_t           pin_mode;                       /* 设置 SCL/SDA 引脚复用模式，为 PIN_MODE_0 时使用默认值 PIN_MODE_2 */

} bsp_i2c_cfg_t;

/**
 * @brief 初始化 I2C 主机
 *
 * 依次配置 SCL/SDA 引脚复用模式和 I2C 主机参数。
 *
 * 使用示例：
 * @code
 * bsp_i2c_cfg_t cfg = {
 *     .bus = I2C_BUS_1,
 *     .scl_pin = BSP_I2C_SCL_PIN,
 *     .sda_pin = BSP_I2C_SDA_PIN,
 * };
 * bsp_i2c_init(&cfg);
 *
 * uint8_t cmd[] = {0x00, 0xAE};
 * bsp_i2c_write(cfg.bus, 0x3C, cmd, sizeof(cmd));          // 只发送
 *
 * uint8_t id = 0;
 * bsp_i2c_read_reg(cfg.bus, 0x3C, 0xD0, &id, 1);           // 写寄存器地址后接收
 * @endcode
 *
 * @param [in] i2c_cfg I2C 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空或总线号非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_init(const bsp_i2c_cfg_t *i2c_cfg);

/**
 * @brief 去初始化 I2C 主机
 *
 * @param [in] bus I2C 总线号
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 总线号非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_deinit(i2c_bus_t bus);

/**
 * @brief 构建 I2C 收发数据结构体
 *
 * 底层辅助接口，只需发送时 receive_buf 传 NULL、receive_len 传 0；
 * 只需接收时 send_buf 传 NULL、send_len 传 0。一般无需直接调用，优先使用下面的封装接口。
 *
 * @param [in] send_buf    发送缓冲区指针
 * @param [in] send_len    发送数据长度
 * @param [in] receive_buf 接收缓冲区指针
 * @param [in] receive_len 接收数据长度
 * @return 构造好的 i2c_data_t
 */
i2c_data_t bsp_i2c_set_data(uint8_t *send_buf, uint32_t send_len, uint8_t *receive_buf, uint32_t receive_len);

/**
 * @brief I2C 主机只发送数据
 *
 * @param [in] bus      I2C 总线号
 * @param [in] dev_addr 从机地址（7 位）
 * @param [in] send_buf 发送缓冲区指针
 * @param [in] send_len 发送数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_write(i2c_bus_t bus, uint16_t dev_addr, const uint8_t *send_buf, uint32_t send_len);

/**
 * @brief I2C 主机只接收数据
 *
 * @param [in] bus         I2C 总线号
 * @param [in] dev_addr    从机地址（7 位）
 * @param [in] receive_buf 接收缓冲区指针
 * @param [in] receive_len 接收数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_read(i2c_bus_t bus, uint16_t dev_addr, uint8_t *receive_buf, uint32_t receive_len);

/**
 * @brief I2C 主机发送后接着接收（同一次传输，中间为重复起始信号）
 *
 * 常用于"先写寄存器地址再读数据"的传感器时序。
 *
 * @param [in]  bus         I2C 总线号
 * @param [in]  dev_addr    从机地址（7 位）
 * @param [in]  send_buf    发送缓冲区指针
 * @param [in]  send_len    发送数据长度
 * @param [out] receive_buf 接收缓冲区指针
 * @param [in]  receive_len 接收数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_write_read(i2c_bus_t bus, uint16_t dev_addr, const uint8_t *send_buf, uint32_t send_len,
                             uint8_t *receive_buf, uint32_t receive_len);

/**
 * @brief 写从机寄存器
 *
 * 将寄存器地址和数据拼成一次发送，数据长度不能超过 BSP_I2C_REG_BUF_SIZE - 1，超出请自行拼接后调用 bsp_i2c_write。
 *
 * @param [in] bus      I2C 总线号
 * @param [in] dev_addr 从机地址（7 位）
 * @param [in] reg_addr 寄存器地址
 * @param [in] data     待写入数据指针
 * @param [in] data_len 待写入数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法或数据过长，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_write_reg(i2c_bus_t bus, uint16_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint32_t data_len);

/**
 * @brief 读从机寄存器
 *
 * 先发送寄存器地址再紧接着接收数据。
 *
 * @param [in]  bus      I2C 总线号
 * @param [in]  dev_addr 从机地址（7 位）
 * @param [in]  reg_addr 寄存器地址
 * @param [out] data     接收数据缓冲区指针
 * @param [in]  data_len 接收数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_read_reg(i2c_bus_t bus, uint16_t dev_addr, uint8_t reg_addr, uint8_t *data, uint32_t data_len);

#endif /* __BSP_I2C_H__ */
