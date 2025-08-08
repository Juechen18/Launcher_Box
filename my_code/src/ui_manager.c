#include "ui_manager.h"
#include "app_list.h"
#include "app_launcher.h"
#include "lvgl/lvgl.h"
#include "lv_mygec_font.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>

// UI组件
static lv_obj_t *g_main_screen;
static lv_obj_t *g_app_list;
static lv_obj_t *g_detail_panel;
static lv_obj_t *g_detail_icon;
static lv_obj_t *g_detail_name_label;
static lv_obj_t *g_detail_desc_label;
static lv_obj_t *g_launch_btn;
static int g_selected_app_idx = -1;

// 状态管理
static pid_t g_running_pid = -1;
static bool g_app_running = false;

// 文本样式
static lv_style_t g_text_style;

// 监控线程函数
static void *app_monitor_thread(void *arg)
{
    int status;
    pid_t pid = (pid_t)(intptr_t)arg;

    waitpid(pid, &status, 0);

    g_app_running = false;
    g_running_pid = -1;
    lv_obj_clear_flag(g_launch_btn, LV_OBJ_FLAG_HIDDEN);

    return NULL;
}

static void update_detail_panel(const AppInfo *app)
{
    if (!app)
        return;

    // 设置图标（仅当路径有效时）
    if (app->icon_path[0] != '\0')
    {
        lv_img_set_src(g_detail_icon, app->icon_path);
    }

    // 设置文本
    lv_label_set_text(g_detail_name_label, app->name);
    lv_label_set_text(g_detail_desc_label, app->description);

    // 根据运行状态更新按钮
    if (g_app_running)
    {
        lv_obj_add_flag(g_launch_btn, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_clear_flag(g_launch_btn, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_clear_flag(g_detail_panel, LV_OBJ_FLAG_HIDDEN);
}

static void create_main_layout(void)
{
    // 初始化路径
    path_init();

    // 初始化样式
    lv_style_init(&g_text_style);
    lv_style_set_text_font(&g_text_style, &lv_mygec_font);

    // 主屏幕
    g_main_screen = lv_scr_act();
    lv_obj_set_style_bg_color(g_main_screen, lv_color_hex(0x2D3748), LV_PART_MAIN);

    // 应用列表
    g_app_list = lv_list_create(g_main_screen);
    lv_obj_set_size(g_app_list, 220, LV_VER_RES);
    lv_obj_set_style_bg_color(g_app_list, lv_color_hex(0x1A202C), LV_PART_MAIN);

    // 添加应用项
    const AppInfo *apps = app_list_get_all();
    for (int i = 0; i < app_list_get_count(); i++)
    {
        lv_obj_t *item = lv_list_add_btn(g_app_list, NULL, apps[i].name);
        lv_obj_add_style(item, &g_text_style, LV_PART_MAIN);
        lv_obj_add_event_cb(item, ui_manager_on_list_item_selected, LV_EVENT_CLICKED, (void *)&apps[i]);
    }

    // 详情面板
    g_detail_panel = lv_obj_create(g_main_screen);
    lv_obj_set_size(g_detail_panel, LV_HOR_RES - 220, LV_VER_RES);
    lv_obj_align(g_detail_panel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_flag(g_detail_panel, LV_OBJ_FLAG_HIDDEN);

    // 图标
    g_detail_icon = lv_img_create(g_detail_panel);
    lv_obj_set_size(g_detail_icon, 120, 120);
    lv_obj_align(g_detail_icon, LV_ALIGN_TOP_MID, 0, 60);

    // 名称标签
    g_detail_name_label = lv_label_create(g_detail_panel);
    lv_obj_align_to(g_detail_name_label, g_detail_icon, LV_ALIGN_BOTTOM_MID, 0, 20);
    lv_obj_add_style(g_detail_name_label, &g_text_style, LV_PART_MAIN);

    // 描述标签
    g_detail_desc_label = lv_label_create(g_detail_panel);
    lv_obj_set_width(g_detail_desc_label, 500);
    lv_obj_align_to(g_detail_desc_label, g_detail_name_label, LV_ALIGN_BOTTOM_MID, 0, 20);
    lv_obj_add_style(g_detail_desc_label, &g_text_style, LV_PART_MAIN);

    // 启动按钮
    g_launch_btn = lv_btn_create(g_detail_panel);
    lv_obj_set_size(g_launch_btn, 180, 50);
    lv_obj_align(g_launch_btn, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_t *btn_label = lv_label_create(g_launch_btn);
    lv_label_set_text(btn_label, "启动");
    lv_obj_center(btn_label);
    lv_obj_add_style(btn_label, &g_text_style, LV_PART_MAIN);
    lv_obj_set_style_text_color(btn_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_launch_btn, lv_color_hex(0x3182CE), LV_PART_MAIN);
    lv_obj_add_event_cb(g_launch_btn, ui_manager_on_launch_btn_clicked, LV_EVENT_CLICKED, NULL);
}

void ui_manager_init(void)
{
    create_main_layout();
}

void ui_manager_on_list_item_selected(lv_event_t *e)
{
    const AppInfo *app = (const AppInfo *)lv_event_get_user_data(e);
    if (!app)
        return;

    const AppInfo *apps = app_list_get_all();
    for (int i = 0; i < app_list_get_count(); i++)
    {
        if (&apps[i] == app)
        {
            g_selected_app_idx = i;
            break;
        }
    }

    update_detail_panel(app);
}

void ui_manager_on_launch_btn_clicked(lv_event_t *e)
{
    if (g_selected_app_idx == -1)
        return;

    const AppInfo *app = app_list_get_by_index(g_selected_app_idx);
    if (!app)
        return;

    // 隐藏按钮
    lv_obj_add_flag(g_launch_btn, LV_OBJ_FLAG_HIDDEN);
    g_app_running = true;

    // 启动子程序
    pid_t pid = fork();
    if (pid == 0)
    {
        char *argv[] = {(char *)app->exec_path, NULL};
        execvp(app->exec_path, argv);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        g_running_pid = pid;
        // 创建监控线程
        pthread_t tid;
        pthread_create(&tid, NULL, app_monitor_thread, (void *)(intptr_t)pid);
        pthread_detach(tid);
    }
}

void ui_manager_check_app_status(void)
{
    if (g_running_pid != -1)
    {
        int status;
        pid_t result = waitpid(g_running_pid, &status, WNOHANG);
        if (result > 0)
        {
            g_app_running = false;
            g_running_pid = -1;
            lv_obj_clear_flag(g_launch_btn, LV_OBJ_FLAG_HIDDEN);
        }
    }
}