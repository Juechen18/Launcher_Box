cmake_minimum_required(VERSION 3.10)

# 设置交叉编译
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
# 这里指定的路径必须是绝对路径
set(CMAKE_C_COMPILER /usr/local/arm/6.5.0/bin/arm-linux-gcc)
set(CMAKE_CXX_COMPILER /usr/local/arm/6.5.0/bin/arm-linux-g++)

# 项目名称设置为软件盒子
project(Launcher_Box)

set(CMAKE_C_STANDARD 99)  # C99
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 包含自定义头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/my_code/inc
    ${PROJECT_SOURCE_DIR}/lvgl
    ${PROJECT_SOURCE_DIR}  # 添加项目根目录，确保能找到自定义字体
)

# 包含LVGL库
add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

# 收集主启动器源文件
file(GLOB_RECURSE LAUNCHER_SOURCES
    ${PROJECT_SOURCE_DIR}/my_code/src/*.c
)

# 添加主启动器可执行文件
add_executable(launcher
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/mouse_cursor_icon.c
    ${LAUNCHER_SOURCES}
)

# 链接库 - 仅保留必要的库
target_link_libraries(launcher lvgl m pthread)

# 自定义运行目标
add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/launcher DEPENDS launcher)