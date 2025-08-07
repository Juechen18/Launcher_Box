/*app_list.c*/
#include "app_list.h"
#include <stdio.h>
#include <unistd.h>

// 静态应用列表（模拟本地应用数据，可扩展为从配置文件读取）
static AppInfo g_app_list[] =
    {
        {
            .name = "钢琴块",
            .icon_path = "",
            .description = "经典钢琴块游戏，点击下落的黑色方块或者长按灰色方块获得分数",
            .exec_path = ""
        }
        // ,
        // {.name = "打地鼠",
        //  .icon_path = NULL,
        //  .description = "打地鼠游戏，点击地鼠获得分数",
        //  .exec_path = NULL
        // },
        // {.name = "笨鸟先飞",
        //  .icon_path = NULL,
        //  .description = "笨鸟先飞游戏，点击小鸟飞跃障碍物获得分数",
        //  .exec_path = NULL
        // }
};

// 静态应用列表
static AppInfo path_list[] =
    {
        {
            .name = "钢琴块",
            .icon_path = "assets/app_icons/piano_blocks.png",
            .exec_path = "bin/projects/piano_blocks"
        }
        // {.name = "打地鼠",
        //  .icon_path = ,
        //  .exec_path = 
        // },
        // {.name = "笨鸟先飞",
        //  .icon_path = ,
        //  .exec_path = 
        // }
};

void path_init(void)
{
    int i;
    for (i = 0; i < app_list_get_count(); i++)
    {
        get_path(&g_app_list[i], &path_list[i]);
    }

}

void get_path(AppInfo *app, AppInfo *path)
{
    // 获取当进程前工作路径
    char *ret;
    char buf[256];
    ret = getcwd(buf, sizeof(buf));
    if (ret == NULL)
    {
        perror("getcwd() error");
        return;
    }
    // 拼接路径
    snprintf(app->exec_path, sizeof(app->exec_path), "%s/%s", buf, path->exec_path);
    snprintf(app->icon_path, sizeof(app->icon_path), "%s/%s", buf, path->icon_path);
}

const AppInfo *app_list_get_all(void)
{
    return g_app_list;
}

int app_list_get_count(void)
{
    return sizeof(g_app_list) / sizeof(g_app_list[0]);
}

const AppInfo *app_list_get_by_index(int idx)
{
    if (idx < 0 || idx >= app_list_get_count())
    {
        return NULL;
    }
    return &g_app_list[idx];
}