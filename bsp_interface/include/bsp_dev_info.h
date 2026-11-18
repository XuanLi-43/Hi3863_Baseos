#ifndef __BSP_DEV_INFO_H__
#define __BSP_DEV_INFO_H__

#include "pinctrl.h"
#include "efuse.h"

typedef struct bsp_dev_info_cfg                          /* 设备信息配置结构体 */
{
    uint8_t *buf;                                        /* 读取数据的输出缓冲区，缓冲区长度不能小于 len */

    uint16_t len;                                        /* 要读取的数据长度（字节），只能大于或等于最小长度：DIE ID 为 16，SOC ID 为 20；未配置或过小时按最小长度处理 */

    uint32_t byte_num;                                   /* efuse 读取起始字节偏移，为 0 时表示用户未配置，使用默认偏移 0x00 */

} bsp_dev_info_cfg_t;

/**
 * @brief 初始化设备信息模块（efuse）
 *
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_init(void);

/**
 * @brief 去初始化设备信息模块（efuse）
 *
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_deinit(void);

/**
 * @brief 读取芯片 DIE ID
 *
 * len 最小为 16 字节（未配置或小于 16 时按 16 处理），buf 缓冲区长度不能小于 16 字节
 *
 * @param [in] info_cfg 设备信息配置结构体指针（使用 buf、len 字段）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_get_die_id(const bsp_dev_info_cfg_t *info_cfg);

/**
 * @brief 读取芯片 SOC ID
 *
 * len 最小为 20 字节（未配置或小于 20 时按 20 处理），buf 缓冲区长度不能小于 20 字节
 *
 * @param [in] info_cfg 设备信息配置结构体指针（使用 buf、len 字段）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_get_soc_id(const bsp_dev_info_cfg_t *info_cfg);

/**
 * @brief 读取 efuse 指定偏移的数据
 *
 * len 为要读取的数据长度（字节），buf 缓冲区长度不能小于 len；
 * byte_num 为 0 时表示用户未配置，使用默认偏移 0x00
 *
 * @param [in] info_cfg 设备信息配置结构体指针（使用 buf、len、byte_num 字段）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_get_byte_buffer(const bsp_dev_info_cfg_t *info_cfg);

#endif /* __BSP_DEV_INFO_H__ */
