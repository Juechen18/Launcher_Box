/**
 * @file utils.c
 * @brief 工具函数实现
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

lv_obj_t *utils_load_image(const char *path)
{
    // 使用LVGL图像解码器加载图像
    // 如果是PNG/JPEG格式，需要开启对应的解码器支持
    LV_LOG_USER("从 %s 加载图像", path);
    return NULL;
}

char *utils_read_text_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
        return NULL;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, fp);
    buf[size] = '\0';
    fclose(fp);
    return buf;
}