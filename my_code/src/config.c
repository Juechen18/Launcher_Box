/**
 * @file config.c
 * @brief 项目配置管理实现
 */

#include "my_code/inc/config.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

const char *config_get_projects_dir(void)
{
    return PROJECTS_DIR;
}

/**
 * @brief 安全的字符串格式化函数
 * @param out 输出缓冲区
 * @param outsz 缓冲区大小
 * @param fmt 格式字符串
 * @return 成功返回0，截断或错误返回-1
 */
static int safe_snprintf(char *out, size_t outsz, const char *fmt, ...)
{
    if (!out || outsz == 0)
        return -1;
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(out, outsz, fmt, ap);
    va_end(ap);
    if (r < 0)
        return -1;
    if ((size_t)r >= outsz)
        return -1;
    return 0;
}

int config_build_project_exec_path(const char *proj, char *out, size_t outsz)
{
    if (!proj || !out)
        return -1;
    return safe_snprintf(out, outsz, "%s/%s/%s",
                         PROJECTS_DIR, proj, PROJECT_EXEC_FILE);
}

int config_project_exec_exists(const char *proj)
{
    if (!proj)
        return 0;
    char path[PATH_MAX];
    if (config_build_project_exec_path(proj, path, sizeof(path)) != 0)
        return 0;
    return (access(path, X_OK) == 0) ? 1 : 0;
}