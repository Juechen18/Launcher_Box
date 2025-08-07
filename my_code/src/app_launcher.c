/*app_launcher.c*/
#include "app_launcher.h"
#include "utils.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int app_launcher_start(const char *exec_path)
{
    // 参数校验
    if (exec_path == NULL)
    {
        // print_error("app_launcher_start: 可执行文件路径为空");
        return -1;
    }

    // 检查文件是否存在且可执行
    if (access(exec_path, X_OK) != 0)
    {
        // print_error("app_launcher_start: 可执行文件不存在或无权限:%s", exec_path);
        return -1;
    }

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1)
    {
        // print_error("app_launcher_start: 创建子进程失败(fork())");
        return -1;
    }
    else if (pid == 0)
    {
        // 子进程:执行应用(替换为新进程)
        char *argv[] = {(char *)exec_path, NULL};
        execvp(exec_path, argv);
        // 若execvp返回，说明失败
        // print_error("app_launcher_start: 执行应用失败(execvp()):%s", exec_path);
        exit(EXIT_FAILURE); // 强制退出子进程
    }
    else
    {
        // 父进程:非阻塞等待子进程结束(不影响主界面)
        waitpid(pid, NULL, WNOHANG);
        // print_info("app_launcher_start: 应用启动成功(PID: %d):%s", pid, exec_path);
        return 0;
    }
}