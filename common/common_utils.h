#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#include <stddef.h>

/**
 * @brief 根据成员指针获取所属结构体的指针。
 *
 * @param ptr    指向有效 type 对象中 member 成员的指针，不允许为 NULL。
 * @param type   所属结构体类型。
 * @param member 成员名称。
 *
 * @note 调用者须保证指针与成员类型匹配，且所属对象仍然有效。
 *       const 对象请使用 container_of_const，避免丢失只读限定。
 *
 * 示例：device_t *dev = container_of(node, device_t, node);
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)));

/** @brief 根据只读成员指针获取所属结构体的只读指针 */
#define container_of_const(ptr, type, member) \
    ((const type *)((const char *)(ptr) - offsetof(type, member)));

#endif /* __COMMON_UTILS_H__ */
