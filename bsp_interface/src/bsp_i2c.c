#include "bsp_i2c.h"

#define BSP_I2C_DEFAULT_BAUDRATE        400000U              /* 默认 I2C 波特率 400KHz */

#define BSP_I2C_DEFAULT_PIN_MODE        PIN_MODE_2           /* 默认 SCL/SDA 引脚复用模式 */

#define BSP_I2C_REG_BUF_SIZE            16U                  /* 寄存器写操作临时缓冲区大小（含寄存器地址），超出请直接使用 bsp_i2c_write */

/**
 * @brief 构建 I2C 收发数据结构体
 *
 * @param [in] send_buf    发送缓冲区指针
 * @param [in] send_len    发送数据长度
 * @param [in] receive_buf 接收缓冲区指针
 * @param [in] receive_len 接收数据长度
 * @return 构造好的 i2c_data_t
 */
i2c_data_t bsp_i2c_set_data(uint8_t *send_buf, uint32_t send_len, uint8_t *receive_buf, uint32_t receive_len)
{
    i2c_data_t data =
    {
        .receive_buf = receive_buf,
        .receive_len = receive_len,
        .send_buf = send_buf,
        .send_len = send_len,
    };

    return data;
}

/**
 * @brief 初始化 I2C 主机
 *
 * 依次配置 SCL/SDA 引脚复用模式和 I2C 主机参数。
 *
 * @param [in] i2c_cfg I2C 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空或总线号非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_init(const bsp_i2c_cfg_t *i2c_cfg)
{
    errcode_t ret;
    pin_mode_t pin_mode;
    uint32_t baudrate;

    /* 参数合法性检查 */
    if((i2c_cfg == NULL) || (i2c_cfg->bus >= I2C_BUS_MAX_NUM))
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 未配置时使用默认值 */
    pin_mode = (i2c_cfg->pin_mode == PIN_MODE_0) ? BSP_I2C_DEFAULT_PIN_MODE : i2c_cfg->pin_mode;
    baudrate = (i2c_cfg->baudrate == 0) ? BSP_I2C_DEFAULT_BAUDRATE : i2c_cfg->baudrate;

    /* 配置 SCL 引脚复用模式 */
    ret = uapi_pin_set_mode(i2c_cfg->scl_pin, pin_mode);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 配置 SDA 引脚复用模式 */
    ret = uapi_pin_set_mode(i2c_cfg->sda_pin, pin_mode);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 初始化 I2C 主机 */
    ret = uapi_i2c_master_init(i2c_cfg->bus, baudrate, i2c_cfg->hscode);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 去初始化 I2C 主机
 *
 * @param [in] bus I2C 总线号
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 总线号非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_deinit(i2c_bus_t bus)
{
    errcode_t ret;

    /* 参数合法性检查 */
    if(bus >= I2C_BUS_MAX_NUM)
    {
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_i2c_deinit(bus);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief I2C 主机只发送数据
 *
 * @param [in] bus      I2C 总线号
 * @param [in] dev_addr 从机地址（7 位）
 * @param [in] send_buf 发送缓冲区指针
 * @param [in] send_len 发送数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_write(i2c_bus_t bus, uint16_t dev_addr, const uint8_t *send_buf, uint32_t send_len)
{
    errcode_t ret;
    i2c_data_t data;

    /* 参数合法性检查 */
    if((send_buf == NULL) || (send_len == 0))
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 只发送：接收缓冲区和长度留空 */
    data = bsp_i2c_set_data((uint8_t *)send_buf, send_len, NULL, 0);
    
    ret = uapi_i2c_master_write(bus, dev_addr, &data);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief I2C 主机只接收数据
 *
 * @param [in] bus         I2C 总线号
 * @param [in] dev_addr    从机地址（7 位）
 * @param [in] receive_buf 接收缓冲区指针
 * @param [in] receive_len 接收数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_read(i2c_bus_t bus, uint16_t dev_addr, uint8_t *receive_buf, uint32_t receive_len)
{
    errcode_t ret;
    i2c_data_t data;

    /* 参数合法性检查 */
    if((receive_buf == NULL) || (receive_len == 0))
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 只接收：发送缓冲区和长度留空 */
    data = bsp_i2c_set_data(NULL, 0, receive_buf, receive_len);

    ret = uapi_i2c_master_read(bus, dev_addr, &data);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief I2C 主机发送后接着接收（同一次传输，中间为重复起始信号）
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
                             uint8_t *receive_buf, uint32_t receive_len)
{
    errcode_t ret;
    i2c_data_t data;

    /* 参数合法性检查 */
    if((send_buf == NULL) || (send_len == 0) || (receive_buf == NULL) || (receive_len == 0))
    {
        return ERRCODE_INVALID_PARAM;
    }

    data = bsp_i2c_set_data((uint8_t *)send_buf, send_len, receive_buf, receive_len);

    ret = uapi_i2c_master_writeread(bus, dev_addr, &data);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 写从机寄存器
 *
 * 将寄存器地址和数据拼成一次发送。
 *
 * @param [in] bus      I2C 总线号
 * @param [in] dev_addr 从机地址（7 位）
 * @param [in] reg_addr 寄存器地址
 * @param [in] data     待写入数据指针
 * @param [in] data_len 待写入数据长度
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数非法或数据过长，ERRCODE_FAIL 失败
 */
errcode_t bsp_i2c_write_reg(i2c_bus_t bus, uint16_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint32_t data_len)
{
    uint8_t buf[BSP_I2C_REG_BUF_SIZE];
    uint32_t i;

    /* 参数合法性检查，临时缓冲区需容纳"寄存器地址 + 数据" */
    if((data == NULL) || (data_len == 0) || (data_len > (BSP_I2C_REG_BUF_SIZE - 1U)))
    {
        return ERRCODE_INVALID_PARAM;
    }

    buf[0] = reg_addr;
    for(i = 0; i < data_len; i++)
    {
        buf[i + 1U] = data[i];
    }

    return bsp_i2c_write(bus, dev_addr, buf, data_len + 1U);
}

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
errcode_t bsp_i2c_read_reg(i2c_bus_t bus, uint16_t dev_addr, uint8_t reg_addr, uint8_t *data, uint32_t data_len)
{
    uint8_t reg = reg_addr;

    /* 参数合法性检查 */
    if((data == NULL) || (data_len == 0))
    {
        return ERRCODE_INVALID_PARAM;
    }

    return bsp_i2c_write_read(bus, dev_addr, &reg, 1U, data, data_len);
}
