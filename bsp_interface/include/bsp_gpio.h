#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "pinctrl.h"
#include "gpio.h"

typedef struct bsp_gpio_cfg                              /* GPIO 基础配置结构体 */
{
    pin_t                pin;                            /* 选择配置的引脚 */

    pin_mode_t           mode;                           /* 选择引脚模式，具体查看引脚模式规则 */

    gpio_direction_t     dir;                            /* 选择引脚输入输出方向 */

    pin_pull_t           pull;                           /* 设置引脚上下拉类型 */

} bsp_gpio_cfg_t;

/**
 * @brief 初始化GPIO
 */
void bsp_gpio_init(void);

/**
 * @brief 去初始化GPIO
 */
void bsp_gpio_deinit(void);

/**
 * @brief 设置 GPIO 基础配置
 *
 * 依次配置引脚复用模式、上下拉类型和输入输出方向
 * 
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_set_config(const bsp_gpio_cfg_t *gpio_cfg);

/**
 * @brief 读取 GPIO 当前电平
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @return GPIO 当前电平（GPIO_LEVEL_LOW/GPIO_LEVEL_HIGH），参数为空时返回 GPIO_LEVEL_LOW
 */
gpio_level_t ws63_gpio_get_val(const bsp_gpio_cfg_t *gpio_cfg);

/**
 * @brief 设置 GPIO 输出电平
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @param [in] level 目标电平（GPIO_LEVEL_LOW/GPIO_LEVEL_HIGH）
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_set_val(const bsp_gpio_cfg_t *gpio_cfg, gpio_level_t level);

/**
 * @brief 翻转 GPIO 输出电平
 *
 * @param [in] gpio_cfg GPIO 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_toggle(const bsp_gpio_cfg_t *gpio_cfg);

typedef struct bsp_gpio_isr_cfg                          /* GPIO 中断配置结构体 */
{
    uint32_t            trigger_num;                     /* 中断触发方式, 1:上升沿中断, 2:下降沿中断, 3:双边沿中断, 4:低电平中断, 8:高电平中断 */

    gpio_callback_t     callback_func;                   /* 中断回调函数, 中断时实现的功能 */

} bsp_gpio_isr_cfg_t;                                    

/**
 * @brief 设置 GPIO 中断
 *
 * @param [in]  gpio_cfg     GPIO 配置结构体指针
 * @param [in]  gpio_isr_cfg 中断配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_isr_set_config(const bsp_gpio_cfg_t *gpio_cfg, const bsp_gpio_isr_cfg_t *gpio_isr_cfg);

/**
 * @brief 取消 GPIO 中断
 *
 * @param [in]  gpio_cfg GPIO 配置结构体指针
 * @return ERRCODE_SUCC 成功，ERRCODE_INVALID_PARAM 参数为空，ERRCODE_FAIL 失败
 */
errcode_t bsp_gpio_isr_unset_config(const bsp_gpio_cfg_t *gpio_cfg);

#endif /* __BSP_GPIO_H__ */

