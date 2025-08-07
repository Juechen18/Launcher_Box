/*ui_manager.c*/
#include "ui_manager.h"
#include "app_list.h"
#include "app_launcher.h"
#include "lvgl/lvgl.h"
#include "lv_mygec_font.h" // 使用统一的字体头文件
#include <stdio.h>

// 全局UI组件（仅本文件可见）
static lv_obj_t *g_main_screen;       // 主屏幕
static lv_obj_t *g_app_list;          // 左侧应用列表
static lv_obj_t *g_detail_panel;      // 右侧详情面板
static lv_obj_t *g_detail_icon;       // 详情面板-图标
static lv_obj_t *g_detail_name_label; // 详情面板-名称
static lv_obj_t *g_detail_desc_label; // 详情面板-描述
static lv_obj_t *g_launch_btn;        // 详情面板-启动按钮
static int g_selected_app_idx = -1;   // 当前选中的应用索引（-1表示未选中）

// 定义统一的文本样式
static lv_style_t g_text_style;

#if 0
// 声明鼠标光标资源（需在mouse_cursor_icon.c中定义）
extern const lv_img_dsc_t mouse_cursor_icon;
#endif

/**
 * @brief 更新详情面板内容（根据选中的应用）
 */
static void update_detail_panel(const AppInfo *app)
{
    lv_img_set_src(g_detail_icon, app->icon_path);            // 设置图标
    lv_label_set_text(g_detail_name_label, app->name);        // 设置名称
    lv_label_set_text(g_detail_desc_label, app->description); // 设置描述
    lv_obj_clear_flag(g_detail_panel, LV_OBJ_FLAG_HIDDEN);    // 显示详情面板
}

/**
 * @brief 创建主界面布局（左侧列表+右侧详情面板）
 */
static void create_main_layout(void)
{
    // 初始化应用路径
    path_init();

    // 初始化文本样式（使用自定义字体）
    lv_style_init(&g_text_style);
    lv_style_set_text_font(&g_text_style, &lv_mygec_font); // 统一使用自定义字体

    // 获取主屏幕（LVGL当前显示的屏幕）
    g_main_screen = lv_scr_act();
    lv_obj_set_style_bg_color(g_main_screen, lv_color_hex(0x2D3748), LV_PART_MAIN); // 深灰色背景

    // 左侧：应用列表（宽度220px，占满屏幕高度）
    g_app_list = lv_list_create(g_main_screen);
    lv_obj_set_size(g_app_list, 220, LV_VER_RES);
    lv_obj_set_style_bg_color(g_app_list, lv_color_hex(0x1A202C), LV_PART_MAIN); // 更深的灰色背景
    lv_obj_set_scrollbar_mode(g_app_list, LV_SCROLLBAR_MODE_AUTO);               // 自动显示滚动条

    // 添加应用列表项（循环数据层的应用数据）
    const AppInfo *apps = app_list_get_all();
    int app_count = app_list_get_count();
    for (int i = 0; i < app_count; i++)
    {
        lv_obj_t *item = lv_list_add_btn(g_app_list, apps[i].icon_path, apps[i].name);                   // 图标+名称
        lv_obj_add_style(item, &g_text_style, LV_PART_MAIN);                                             // 应用文本样式
        lv_obj_set_style_text_color(item, lv_color_hex(0xFFFFFF), LV_PART_MAIN);                         // 白色文本
        lv_obj_add_event_cb(item, ui_manager_on_list_item_selected, LV_EVENT_CLICKED, (void *)&apps[i]); // 绑定选中事件
    }

    // 右侧：详情面板（宽度=屏幕宽度-220px，占满屏幕高度）
    g_detail_panel = lv_obj_create(g_main_screen);
    lv_obj_set_size(g_detail_panel, LV_HOR_RES - 220, LV_VER_RES);
    lv_obj_align(g_detail_panel, LV_ALIGN_RIGHT_MID, 0, 0); // 右对齐
    lv_obj_add_flag(g_detail_panel, LV_OBJ_FLAG_HIDDEN);    // 初始隐藏（未选中应用时不显示）

    // 详情面板-应用图标（120x120px，顶部居中）
    g_detail_icon = lv_img_create(g_detail_panel);
    lv_obj_set_size(g_detail_icon, 120, 120);
    lv_obj_align(g_detail_icon, LV_ALIGN_TOP_MID, 0, 60);

    // 详情面板-应用名称（图标下方居中）
    g_detail_name_label = lv_label_create(g_detail_panel);
    lv_obj_align_to(g_detail_name_label, g_detail_icon, LV_ALIGN_BOTTOM_MID, 0, 20);
    lv_obj_add_style(g_detail_name_label, &g_text_style, LV_PART_MAIN);                     // 应用文本样式
    lv_obj_set_style_text_color(g_detail_name_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // 白色文本

    // 详情面板-应用描述（名称下方居中，限制宽度500px，自动换行）
    g_detail_desc_label = lv_label_create(g_detail_panel);
    lv_obj_set_width(g_detail_desc_label, 500);
    lv_obj_align_to(g_detail_desc_label, g_detail_name_label, LV_ALIGN_BOTTOM_MID, 0, 20);
    lv_obj_add_style(g_detail_desc_label, &g_text_style, LV_PART_MAIN);                     // 应用文本样式
    lv_obj_set_style_text_color(g_detail_desc_label, lv_color_hex(0xA0AEC0), LV_PART_MAIN); // 浅灰色文本

    // 详情面板-启动按钮（180x50px，蓝色背景，底部居中）
    g_launch_btn = lv_btn_create(g_detail_panel);
    lv_obj_set_size(g_launch_btn, 180, 50);
    lv_obj_align(g_launch_btn, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_set_style_bg_color(g_launch_btn, lv_color_hex(0x3182CE), LV_PART_MAIN); // 蓝色背景

    // 按钮文本（白色）
    lv_obj_t *btn_label = lv_label_create(g_launch_btn);
    lv_label_set_text(btn_label, "启动应用");
    lv_obj_add_style(btn_label, &g_text_style, LV_PART_MAIN);                     // 应用文本样式
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // 白色文本
    lv_obj_center(btn_label);                                                     // 文本居中

    // 绑定启动按钮点击事件
    lv_obj_add_event_cb(g_launch_btn, ui_manager_on_launch_btn_clicked, LV_EVENT_CLICKED, NULL);
}

/**
 * @brief UI初始化（仅负责界面布局与鼠标光标设置）
 */
void ui_manager_init(void)
{
#if 0
    // 设置鼠标光标（需提前将光标图标编译为LVGL资源）
    lv_img_set_src(lv_mouse_get_cursor(), &mouse_cursor_icon);
#endif

    // 创建主界面布局（左侧列表+右侧详情面板）
    create_main_layout();
}

/**
 * @brief 应用列表项选中事件回调（更新详情面板）
 */
void ui_manager_on_list_item_selected(lv_event_t *e)
{
    const AppInfo *app = (const AppInfo *)lv_event_get_user_data(e);
    if (!app)
        return;

    // 查找应用索引（用于启动按钮点击时获取应用）
    const AppInfo *apps = app_list_get_all();
    int app_count = app_list_get_count();
    for (int i = 0; i < app_count; i++)
    {
        if (&apps[i] == app)
        {
            g_selected_app_idx = i;
            break;
        }
    }

    // 更新详情面板内容
    update_detail_panel(app);
}

/**
 * @brief 启动按钮点击事件回调（调用逻辑层启动应用）
 */
void ui_manager_on_launch_btn_clicked(lv_event_t *e)
{
    if (g_selected_app_idx == -1)
        return;

    const AppInfo *app = app_list_get_by_index(g_selected_app_idx);
    if (!app)
        return;

    app_launcher_start(app->exec_path); // 调用逻辑层启动应用
}