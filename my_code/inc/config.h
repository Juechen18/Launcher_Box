/**
 * @file config.h
 * @brief 项目配置管理头文件
 *
 * 定义项目目录结构及提供访问项目资源的函数
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define PROJECTS_DIR "./projects"    /**< 项目根目录路径 */
#define PROJECT_EXEC_FILE "main"     /**< 可执行文件名 */
// #define PROJECT_DESC_FILE "desc.txt" /**< 项目描述文件名（可选） */
// #define PROJECT_ICON_FILE "icon.png" /**< 项目图标文件名（可选） */

/**
 * @brief 获取项目根目录路径
 * @return 返回项目根目录的常量字符串
 */
const char *config_get_projects_dir(void);

/**
 * @brief 构建项目可执行文件路径
 * @param proj 项目名称
 * @param out 输出路径缓冲区
 * @param outsz 缓冲区大小
 * @return 成功返回0，失败返回-1
 */
int config_build_project_exec_path(const char *proj, char *out, size_t outsz);

/**
 * @brief 构建项目描述文件路径
 * @param proj 项目名称
 * @param out 输出路径缓冲区
 * @param outsz 缓冲区大小
 * @return 成功返回0，失败返回-1
 */
int config_build_project_desc_path(const char *proj, char *out, size_t outsz);

/**
 * @brief 检查项目可执行文件是否存在并可执行
 * @param proj 项目名称
 * @return 存在且可执行返回1，否则返回0
 */
int config_project_exec_exists(const char *proj);

#endif /* CONFIG_H */