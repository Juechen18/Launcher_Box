/**
 * @file launcher.h
 * @brief 项目启动器功能声明
 * 
 * 提供扫描和启动项目的函数接口
 */

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <semaphore.h>

/**
 * @brief 项目信息结构体
 */
typedef struct
{
    char name[64];          /**< 项目名称 */
    char desc[256];         /**< 项目描述 */
    char desc_path[256];    /**< 描述文件路径 */
    char exec_path[256];    /**< 可执行文件完整路径 */
} project_info_t;

/**
 * @brief 设置进程同步信号量
 * @param sem 信号量指针
 */
void launcher_set_semaphore(sem_t *sem);

/**
 * @brief 扫描项目目录并构建可用项目列表
 * @param list 项目列表输出参数
 * @return 找到的项目数量
 */
int launcher_scan_projects(project_info_t **list);

/**
 * @brief 启动项目可执行文件
 * @param exec_path 可执行文件路径
 */
void launcher_start_project(const char *exec_path);

#endif