/*app_launcher.h*/
#ifndef APP_LAUNCHER_H
#define APP_LAUNCHER_H

#include <unistd.h>

/**
 * @brief 启动外部应用程序
 * @param exec_path 应用可执行文件路径
 * @param out_pid 输出参数，用于获取子进程PID
 * @return 成功返回0，失败返回-1
 */
int app_launcher_start(const char *exec_path, pid_t *out_pid);

#endif // APP_LAUNCHER_H