cmake_minimum_required(VERSION 3.10)

# 设置交叉编译
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
# 这里指定的路径必须是绝对路径
set(CMAKE_C_COMPILER /usr/local/arm/6.5.0/bin/arm-linux-gcc)
set(CMAKE_CXX_COMPILER /usr/local/arm/6.5.0/bin/arm-linux-g++)

project(Launcher_Box)

set(CMAKE_C_STANDARD 99)  # C99 # lvgl officially support C99 and above
set(CMAKE_CXX_STANDARD 17) # C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 包含自定义头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/my_code/inc
    ${PROJECT_SOURCE_DIR}/lvgl
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
    ${LAUNCHER_SOURCES}
)

target_link_libraries(launcher lvgl m pthread)

# 添加子项目构建
# 钢琴块项目
add_subdirectory(projects/piano_blocks)

# 其他项目示例（按需添加）
# add_subdirectory(projects/calculator)
# add_subdirectory(projects/my_app1)

# 自定义目标用于运行主程序
add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/launcher DEPENDS launcher)

# 安装目标（可选）
install(TARGETS launcher 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)