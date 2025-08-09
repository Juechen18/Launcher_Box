/*main.c*/
#include "lvgl/lvgl.h"  // LVGL核心库
#include "ui_manager.h" // UI层：界面布局与交互
#include <unistd.h>     // 用于主循环睡眠（usleep）

int main()
{
    /************************** 1. LVGL库初始化 **************************/
    lv_init(); // 必须先初始化LVGL，否则无法创建显示/输入设备

    lv_fs_posix_init(); // 注册POSIX文件系统驱动（处理本地文件路径）

    /************************** 2. 显示设备初始化（Framebuffer） **************************/
    lv_display_t *disp = lv_linux_fbdev_create(); // 创建Linux Framebuffer显示设备
    if (!disp)
    {
        return 1;
    } // 简单错误处理（无日志）
    lv_linux_fbdev_set_file(disp, "/dev/fb0"); // 指定Framebuffer设备文件（如/dev/fb0）

    /************************** 3. 输入设备初始化（鼠标/触摸） **************************/
    lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0"); // 创建输入设备（指针类型）
    if (!indev)
    {
        return 1;
    } // 简单错误处理（无日志）

    /************************** 4. 初始化UI界面（布局+交互） **************************/
    ui_manager_init(); // UI层负责：创建主界面（列表+详情面板）、绑定事件（点击/选中）

    /************************** 5. LVGL主循环（界面刷新与事件处理） **************************/
    while (1)
    {
        lv_timer_handler();            // 处理LVGL内部事件（动画、输入、定时器）
        ui_manager_check_app_status(); // 新增：定期检查应用状态
        usleep(5000);                  // 睡眠5ms，降低CPU占用（可调整）
    }

    return 0;
}