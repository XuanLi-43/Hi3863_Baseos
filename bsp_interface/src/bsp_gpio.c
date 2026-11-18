#include "bsp_gpio.h"

/**
 * @brief 初始化GPIO
 */
void bsp_gpio_init(void)
{
    uapi_gpio_init();
}

/**
 * @brief 去初始化GPIO, 释放GPIO
 */
void bsp_gpio_deinit(void)
{
    uapi_gpio_deinit();
}

/**
 * @brief 设置 GPIO 基础配置
 *
 * 依次配置引脚复用模式、上下拉类型和输入输出方向。
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_set_config(const bsp_gpio_cfg_t *gpio_cfg)
{
    errcode_t ret;

    /* 参数合法性检查 */
    if(gpio_cfg == NULL)
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 配置引脚复用模式 */
    ret = uapi_pin_set_mode(gpio_cfg->pin, gpio_cfg->mode);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 配置引脚上下拉 */
    ret = uapi_pin_set_pull(gpio_cfg->pin, gpio_cfg->pull);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    /* 配置引脚输入输出方向 */
    ret = uapi_gpio_set_dir(gpio_cfg->pin, gpio_cfg->dir);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 读取 GPIO 当前电平
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @return GPIO 当前电平（GPIO_LEVEL_LOW/GPIO_LEVEL_HIGH）
 */
gpio_level_t ws63_gpio_get_val(const bsp_gpio_cfg_t *gpio_cfg)
{
    /* 参数合法性检查，非法时返回低电平 */
    if(gpio_cfg == NULL)
    {
        return GPIO_LEVEL_LOW;
    }

    return uapi_gpio_get_val(gpio_cfg->pin);
}

/**
 * @brief 设置 GPIO 输出电平
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @param [in] level 目标电平（GPIO_LEVEL_LOW/GPIO_LEVEL_HIGH）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_set_val(const bsp_gpio_cfg_t *gpio_cfg, gpio_level_t level)
{
    errcode_t ret;

    /* 参数合法性检查 */
    if(gpio_cfg == NULL)
    {
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_gpio_set_val(gpio_cfg->pin, level);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 翻转 GPIO 输出电平
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_toggle(const bsp_gpio_cfg_t *gpio_cfg)
{
    errcode_t ret;

    /* 参数合法性检查 */
    if(gpio_cfg == NULL)
    {
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_gpio_toggle(gpio_cfg->pin);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 设置 GPIO 中断
 *
 * @param [in]  gpio_cfg     GPIO 配置结构体指针
 * @param [in]  gpio_isr_cfg 中断配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_isr_set_config(const bsp_gpio_cfg_t *gpio_cfg, const bsp_gpio_isr_cfg_t *gpio_isr_cfg)
{
    errcode_t ret;

    pin_t pin;

    /* 参数合法性检查 */
    if((gpio_cfg == NULL) || (gpio_isr_cfg == NULL))
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 以基础配置中的引脚为准 */
    pin = gpio_cfg->pin;

    /* 注册中断服务函数 */
    ret = uapi_gpio_register_isr_func(pin, gpio_isr_cfg->trigger_num, gpio_isr_cfg->callback_func);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 取消 GPIO 中断
 *
 * @param [in]  gpio_cfg GPIO 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_isr_unset_config(const bsp_gpio_cfg_t *gpio_cfg)
{
    errcode_t ret;

    pin_t pin;

    /* 参数合法性检查 */
    if((gpio_cfg == NULL))
    {
        return ERRCODE_INVALID_PARAM;
    }

    /* 以基础配置中的引脚为准 */
    pin = gpio_cfg->pin;

    /* 注销中断服务函数 */
    ret = uapi_gpio_unregister_isr_func(pin);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
