/**
 * @file launcher.c
 * @brief 项目启动器核心功能实现
 *
 * 本模块负责扫描项目目录、管理项目列表以及启动项目进程。
 * 使用信号量(g_sem)控制并发启动，防止资源竞争。
 */

#include "launcher.h"
#include "config.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static sem_t *g_sem = NULL; ///< 全局信号量指针，用于进程启动同步控制

/**
 * @brief 设置全局信号量
 * @param[in] sem 外部传入的信号量指针
 * @note 必须在调用launcher_start_project()前初始化
 */
void launcher_set_semaphore(sem_t *sem)
{
    g_sem = sem;
}

/**
 * @brief 扫描项目目录并构建项目列表
 * @param[out] list 输出的项目信息数组指针(需调用者释放)
 * @return int 找到的有效项目数量
 * @retval 0 无有效项目或打开目录失败
 * @retval >0 实际找到的项目数量
 * @warning 返回的数组必须通过free()释放
 */
int launcher_scan_projects(project_info_t **list)
{
    DIR *dir = opendir(PROJECTS_DIR);
    if (!dir)
        return 0;

    struct dirent *entry;
    int count = 0;
    project_info_t *arr = NULL;

    /* 遍历目录项 */
    while ((entry = readdir(dir)) != NULL)
    {
        /* 只处理常规目录且排除"."和".." */
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0)
        {
            // 检查是否存在可执行文件
            if (config_project_exec_exists(entry->d_name))
            {
                arr = realloc(arr, sizeof(project_info_t) * (count + 1));
                strncpy(arr[count].name, entry->d_name, sizeof(arr[count].name));

                // 构建完整路径
                config_build_project_exec_path(entry->d_name, arr[count].exec_path, sizeof(arr[count].exec_path));

                // 设置默认描述
                snprintf(arr[count].desc, sizeof(arr[count].desc), "项目: %s", entry->d_name);

                count++;
            }
        }
    }
    closedir(dir);
    *list = arr;
    return count;
}

/**
 * @brief 启动指定项目
 * @param[in] exec_path 项目可执行文件的完整路径
 * @note 使用fork()+execl()启动新进程
 * @warning 需要先通过launcher_set_semaphore()设置信号量
 *
 * @details 启动流程:
 * 1. 获取信号量(g_sem)
 * 2. 创建子进程
 * 3. 在子进程中执行目标程序
 * 4. 错误处理并释放信号量
 *
 * @par 并发控制:
 * 使用信号量确保同一时间只有一个项目启动过程
 */
void launcher_start_project(const char *exec_path)
{
    // 1. 获取信号量(g_sem)
    if (!g_sem)
        return;
    sem_wait(g_sem);

    // 2. 创建子进程
    pid_t pid = fork();
    if (pid == 0)
    {
        /* 子进程 */
        // 3. 在子进程中执行目标程序
        execl(exec_path, exec_path, NULL);
        perror("execl");
        exit(1);
    }
    else if (pid < 0)
    {
        /* 父进程 */
        // 4. 错误处理并释放信号量
        perror("fork");
        sem_post(g_sem);
    }
}