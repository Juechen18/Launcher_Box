/**
 * @file main.c
 * @brief 主程序入口
 * @details 负责初始化系统、信号处理和主循环
 */

#include "ui_manager.h"
#include "launcher.h"
#include "config.h"
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

static sem_t *g_sem = NULL;                    ///< 全局信号量
volatile sig_atomic_t g_child_exited_flag = 0; ///< 子进程退出标志

/**
 * @brief SIGCHLD信号处理函数
 * @param signo 信号编号
 */
static void sigchld_handler(int signo)
{
    int status;
    pid_t pid;
    /* 回收所有已退出的子进程 */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // V操作
        if (g_sem)
            sem_post(g_sem);
        g_child_exited_flag = 1; /* 在主循环中处理 UI 恢复 */
    }
}

/**
 * @brief 主函数
 * @return 程序退出状态
 */
int main(void)
{
    /* 1. 初始化 LVGL（确保 lv_drivers 已正确集成）*/
    lv_init();
    lv_display_t *disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
    lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");

    /* 2. 初始化命名信号量 */
    g_sem = sem_open("/launcher_sem", O_CREAT, 0666, 1);
    if (g_sem == SEM_FAILED)
    {
        perror("sem_open");
        return -1;
    }
    launcher_set_semaphore(g_sem);

    /* 3. 安装 SIGCHLD 处理器 */
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
    }

    /* 4. 创建 UI */
    ui_create_all();
    show_menu_screen();

    /* 5. 主循环：处理 LVGL 任务，并在检测到子进程退出后恢复 UI（安全） */
    while (1)
    {
        lv_timer_handler();

        if (g_child_exited_flag)
        {
            g_child_exited_flag = 0;
            ui_on_child_exit();
        }

        usleep(5000);
    }

    /* 清理（通常不可达） */
    sem_close(g_sem);
    sem_unlink("/launcher_sem");
    return 0;
}