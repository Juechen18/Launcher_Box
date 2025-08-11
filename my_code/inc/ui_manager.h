/**
 * @file ui_manager.h
 * @brief 用户界面管理头文件
 * 
 * 提供创建和管理启动器用户界面的函数
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "launcher.h"
#include <lvgl.h>

/**
 * @brief 创建所有UI组件
 */
void ui_create_all(void);

/**
 * @brief 显示菜单界面
 */
void show_menu_screen(void);

/**
 * @brief 处理子进程退出事件
 */
void ui_on_child_exit(void);

#endif