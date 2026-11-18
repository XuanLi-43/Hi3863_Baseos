#include "bsp_dev_info.h"

#define BSP_DEV_INFO_BYTE_NUM_DEFAULT       0u           /* efuse 读取起始字节偏移默认值，用户不配置 byte_num 时生效 */

#define BSP_DEV_INFO_DIE_ID_LEN             16u          /* DIE ID 长度（字节），len 未配置或小于该值时按该值兜底 */

#define BSP_DEV_INFO_SOC_ID_LEN             20u          /* SOC ID 长度（字节），len 未配置或小于该值时按该值兜底 */

/**
 * @brief 初始化设备信息模块（efuse）
 *
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_init(void)
{
    errcode_t ret;

    ret = uapi_efuse_init();
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 去初始化设备信息模块（efuse）
 *
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_deinit(void)
{
    errcode_t ret;

    ret = uapi_efuse_deinit();
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 读取芯片 DIE ID
 *
 * len 最小为 16 字节（未配置或小于 16 时按 16 处理），buf 缓冲区长度不能小于 16 字节
 *
 * @param [in] info_cfg 设备信息配置结构体指针（使用 buf、len 字段）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_get_die_id(const bsp_dev_info_cfg_t *info_cfg)
{
    errcode_t ret;
    uint16_t len = info_cfg->len;

    if(info_cfg == NULL)
    {
        return ERRCODE_FAIL;
    }

    /* 长度兜底：len 只能大于或等于 BSP_DEV_INFO_DIE_ID_LEN，未配置或过小时按最小长度处理 */
    if (len < BSP_DEV_INFO_DIE_ID_LEN)
    {
        len = BSP_DEV_INFO_DIE_ID_LEN;
    }

    ret = uapi_efuse_get_die_id(info_cfg->buf, len);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 读取芯片 SOC ID
 *
 * len 最小为 20 字节（未配置或小于 20 时按 20 处理），buf 缓冲区长度不能小于 20 字节
 *
 * @param [in] info_cfg 设备信息配置结构体指针（使用 buf、len 字段）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_get_soc_id(const bsp_dev_info_cfg_t *info_cfg)
{
    errcode_t ret;
    uint16_t len = info_cfg->len;

    if(info_cfg == NULL)
    {
        return ERRCODE_FAIL;
    }

    /* 长度兜底：len 只能大于或等于 BSP_DEV_INFO_SOC_ID_LEN，未配置或过小时按最小长度处理 */
    if (len < BSP_DEV_INFO_SOC_ID_LEN)
    {
        len = BSP_DEV_INFO_SOC_ID_LEN;
    }

    ret = uapi_soc_read_id(info_cfg->buf, len);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/**
 * @brief 读取 efuse 指定偏移的数据
 *
 * len 为要读取的数据长度（字节），buf 缓冲区长度不能小于 len；
 * byte_num 未配置（为 0）时使用默认偏移 0
 *
 * @param [in] info_cfg 设备信息配置结构体指针（使用 buf、len、byte_num 字段）
 * @return ERRCODE_SUCC 成功，ERRCODE_FAIL 失败
 */
errcode_t bsp_dev_info_get_byte_buffer(const bsp_dev_info_cfg_t *info_cfg)
{
    errcode_t ret;
    uint32_t byte_num = info_cfg->byte_num;

    if(info_cfg == NULL)
    {
        return ERRCODE_FAIL;
    }

    /* 用户未配置 byte_num（为 0）时，使用默认偏移 BSP_DEV_INFO_BYTE_NUM_DEFAULT */
    if (byte_num == 0u)
    {
        byte_num = BSP_DEV_INFO_BYTE_NUM_DEFAULT;
    }

    ret = uapi_efuse_read_buffer(info_cfg->buf, byte_num, info_cfg->len);
    if(ret != ERRCODE_SUCC)
    {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

