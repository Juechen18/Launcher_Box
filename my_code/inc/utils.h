/**
 * @file utils.h
 * @brief 工具函数声明
 */

#ifndef UTILS_H
#define UTILS_H

#include <lvgl.h>

/**
 * @brief 从文件加载图像
 * @param path 图像文件路径
 * @return 成功返回LVGL图像对象指针，失败返回NULL
 */
lv_obj_t *utils_load_image(const char *path);

/**
 * @brief 读取文本文件内容
 * @param path 文本文件路径
 * @return 成功返回包含文件内容的缓冲区指针，失败返回NULL
 */
char *utils_read_text_file(const char *path);

#endif