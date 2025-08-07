/*app_launcher.h*/
#ifndef APP_LAUNCHER_H
#define APP_LAUNCHER_H

/**
 * @brief 启动外部应用程序
 * @param exec_path 应用可执行文件路径（相对于项目根目录或绝对路径）
 * @return 成功返回0，失败返回-1（如文件不存在、fork失败）
 */
int app_launcher_start(const char *exec_path);

#endif // APP_LAUNCHER_H