/*app_list.h*/
#ifndef APP_LIST_H
#define APP_LIST_H

/**
 * @brief 应用信息结构体（存储应用的核心属性）
 */
typedef struct
{
    const char *name;        // 应用名称（只读）
    char icon_path[256];     // 图标完整路径（可修改，足够大的缓冲区）
    const char *description; // 应用描述（只读）
    char exec_path[256];     // 可执行文件完整路径（可修改，足够大的缓冲区）
} AppInfo;

/**
 * @brief 获取应用列表（静态数组，不可修改）
 * @return 应用列表的首地址
 */
const AppInfo *app_list_get_all(void);

/**
 * @brief 获取应用数量
 * @return 应用列表中的应用数量
 */
int app_list_get_count(void);

/**
 * @brief 根据索引获取应用信息
 * @param idx 应用索引（0-based，需小于应用数量）
 * @return 应用信息指针（索引无效时返回NULL）
 */
const AppInfo *app_list_get_by_index(int idx);

/**
 * @brief 获取应用路径
 * @param app 绝对路径指针
 * @param path 相对路径指针
 */
void get_path(AppInfo *app, AppInfo *path);

/**
 * @brief 初始化应用路径
 */
void path_init(void);

#endif // APP_LIST_H