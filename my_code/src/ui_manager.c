/**
 * @file ui_manager.c
 * @brief 用户界面管理实现
 * @details 提供项目选择界面、项目信息展示和启动功能
 */

#include "ui_manager.h"
#include "config.h"
#include "lv_my_font.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

static lv_obj_t *menu_list;             /**< 左侧菜单列表 */
static lv_obj_t *right_panel;           /**< 右侧内容面板 */
static lv_obj_t *title_label;           /**< 标题标签 */
static lv_obj_t *content_label;         /**< 内容显示标签 */
static project_info_t *projects = NULL; /**< 项目列表 */
static int project_count = 0;           /**< 项目数量 */
static int selected_index = -1;         /**< 当前选中项目索引 */

/* 内部函数声明 */
static void show_project_info(int idx);
static void hide_ui(void);
static void show_ui(void);
static void create_title_label(void);
static void create_menu_list(void);

/**
 * @brief 项目按钮事件回调
 * @param e LVGL事件对象
 */
static void project_btn_event_cb(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || idx >= project_count)
        return;
    selected_index = idx;
    show_project_info(idx);
}

/**
 * @brief 启动按钮事件回调
 * @param e LVGL事件对象
 */
static void start_btn_event_cb(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || idx >= project_count)
        return;
    hide_ui();
    launcher_start_project(projects[idx].exec_path);
}

/**
 * @brief 退出按钮事件回调
 * @param e LVGL事件对象
 */
static void exit_btn_event_cb(lv_event_t *e)
{
    exit(0);
}

/**
 * @brief 隐藏UI界面
 */
static void hide_ui(void)
{
    lv_obj_add_flag(menu_list, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(right_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(title_label, LV_OBJ_FLAG_HIDDEN);
}

/**
 * @brief 显示UI界面
 */
static void show_ui(void)
{
    lv_obj_remove_flag(menu_list, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(right_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(title_label, LV_OBJ_FLAG_HIDDEN);
}

/**
 * @brief 显示项目详细信息
 * @param idx 项目索引
 */
static void show_project_info(int idx)
{
    // 清除旧内容
    if (content_label)
    {
        lv_obj_delete(content_label);
        content_label = NULL;
    }

    // 创建内容标签
    content_label = lv_label_create(right_panel);
    lv_obj_set_width(content_label, lv_pct(95));
    lv_label_set_long_mode(content_label, LV_LABEL_LONG_WRAP);

    // 设置样式
    static lv_style_t content_style;
    lv_style_init(&content_style);
    lv_style_set_text_font(&content_style, &lv_my_font);
    lv_style_set_text_line_space(&content_style, 8);
    lv_obj_add_style(content_label, &content_style, LV_STATE_DEFAULT);

    // 设置文本内容
    char buffer[512];
    snprintf(buffer, sizeof(buffer),
             "项目名称: %s\n\n"
             "项目描述: %s\n\n",
             projects[idx].name,
             projects[idx].desc);
    lv_label_set_text(content_label, buffer);
    lv_obj_align(content_label, LV_ALIGN_CENTER, 10, 10);

    // 创建启动按钮
    lv_obj_t *start_btn = lv_button_create(right_panel);
    lv_obj_set_size(start_btn, 120, 50);
    lv_obj_align(start_btn, LV_ALIGN_BOTTOM_MID, 0, -20);

    lv_obj_t *btn_label = lv_label_create(start_btn);

    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_font(&label_style, &lv_my_font);
    lv_style_set_text_color(&label_style, lv_color_hex(0x000000));
    lv_style_set_text_align(&label_style, LV_TEXT_ALIGN_CENTER);

    lv_obj_add_style(btn_label, &label_style, LV_STATE_DEFAULT);

    lv_label_set_text(btn_label, "启动");
    lv_obj_center(btn_label);
    lv_obj_add_event_cb(start_btn, start_btn_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
}

/**
 * @brief 创建标题标签
 */
static void create_title_label(void)
{
    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, &lv_my_font);
    lv_style_set_text_color(&title_style, lv_color_hex(0x000000));
    lv_style_set_text_align(&title_style, LV_TEXT_ALIGN_CENTER);

    title_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(title_label, &title_style, LV_STATE_DEFAULT);
    lv_label_set_text(title_label, "软件盒子");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
}

/**
 * @brief 创建菜单列表
 */
static void create_menu_list(void)
{
    menu_list = lv_list_create(lv_scr_act());
    lv_obj_set_size(menu_list, 200, 480);
    lv_obj_align(menu_list, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_scroll_dir(menu_list, LV_DIR_VER);

    for (int i = 0; i < project_count; i++)
    {
        lv_obj_t *btn = lv_list_add_btn(menu_list, LV_SYMBOL_FILE, projects[i].name);
        lv_obj_set_height(btn, 80);
        lv_obj_add_event_cb(btn, project_btn_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
}

/**
 * @brief 初始化用户界面
 */
void ui_create_all(void)
{
    project_count = launcher_scan_projects(&projects);

    // 创建右侧面板
    right_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(right_panel, 600, 480);
    lv_obj_align(right_panel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_scroll_dir(right_panel, LV_DIR_VER);

    // 创建UI组件
    create_title_label();
    create_menu_list();

    // 创建退出按钮
    lv_obj_t *exit_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(exit_btn, 80, 40);
    lv_obj_align(exit_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t *exit_label = lv_label_create(exit_btn);

    static lv_style_t exit_style;
    lv_style_init(&exit_style);
    lv_style_set_text_font(&exit_style, &lv_my_font);
    lv_style_set_text_color(&exit_style, lv_color_hex(0x000000));
    lv_style_set_text_align(&exit_style, LV_TEXT_ALIGN_CENTER);

    lv_obj_add_style(exit_label, &exit_style, LV_STATE_DEFAULT);

    lv_label_set_text(exit_label, "退出");
    lv_obj_center(exit_label);
    lv_obj_add_event_cb(exit_btn, exit_btn_event_cb, LV_EVENT_CLICKED, NULL);
}

/**
 * @brief 显示菜单界面
 */
void show_menu_screen(void)
{
    show_ui();
    if (content_label)
    {
        lv_obj_delete(content_label);
        content_label = NULL;
    }
}

/**
 * @brief 处理子程序退出事件
 */
void ui_on_child_exit(void)
{
    show_ui();
    if (selected_index >= 0 && selected_index < project_count)
    {
        show_project_info(selected_index);
    }
}