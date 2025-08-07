/*ui_manager.h*/
#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "lvgl/lvgl.h"

/**
 * @brief 初始化UI系统（LVGL库+主界面布局）
 */
void ui_manager_init(void);

/**
 * @brief 应用列表项选中事件回调
 * @param e LVGL事件对象（包含事件上下文）
 */
void ui_manager_on_list_item_selected(lv_event_t *e);

/**
 * @brief 启动按钮点击事件回调
 * @param e LVGL事件对象（包含事件上下文）
 */
void ui_manager_on_launch_btn_clicked(lv_event_t *e);

#endif // UI_MANAGER_H