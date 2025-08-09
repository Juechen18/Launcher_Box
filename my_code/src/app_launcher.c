#include "app_launcher.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int app_launcher_start(const char *exec_path, pid_t *out_pid)
{
    if (exec_path == NULL || out_pid == NULL)
    {
        return -1;
    }

    if (access(exec_path, X_OK) != 0)
    {
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        return -1;
    }
    else if (pid == 0)
    {
        // 子进程
        char *argv[] = {(char *)exec_path, NULL};
        execvp(exec_path, argv);
        exit(EXIT_FAILURE);
    }
    else
    {
        *out_pid = pid;
        return 0;
    }
}