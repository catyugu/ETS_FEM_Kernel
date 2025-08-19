# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称和 C++ 标准
project(ETS_FEM_Kernel CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

# --- 依赖管理 ---
# --- Google Test (使用现代的 FetchContent 方法) ---
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# 对于 Visual Studio 用户，强制 gtest 使用与主项目相同的运行时库
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 查找 Eigen3 库
# 建议使用 find_package，这更符合 CMake 的现代用法。
# 您可以通过设置 CMAKE_PREFIX_PATH 环境变量或在 CMake GUI/命令行中指定 -DEigen3_DIR
# 来告诉 CMake 在哪里找到 Eigen3。
set(EIGEN3_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/share/eigen)
if(NOT EXISTS ${EIGEN3_INCLUDE_DIR})
    message(STATUS "本地未找到 Eigen，正在从远程下载...")
    execute_process(
            COMMAND git clone --depth 1 --branch 3.4.0 https://gitlab.com/libeigen/eigen.git ${EIGEN3_INCLUDE_DIR}
            RESULT_VARIABLE GIT_CLONE_RESULT
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "克隆 Eigen 仓库失败")
    endif()
    message(STATUS "Eigen 下载完成")
endif()

# --- 定义项目结构 ---

# 定义头文件目录
# 所有子目录都可以访问这个路径
include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/fem
        ${EIGEN3_INCLUDE_DIR}
)

# --- 构建核心库 (fem_library) ---

# 自动收集所有位于 /fem 目录下的源文件
file(GLOB_RECURSE FEM_SOURCES "fem/**/*.cpp" "utils/*.cpp")

# 创建一个静态库，包含所有核心FEM逻辑
add_library(fem_library STATIC ${FEM_SOURCES})

# 为我们的库链接它所需要的依赖
# PUBLIC 表示任何链接到 fem_library 的目标也会自动包含这些头文件目录
target_include_directories(fem_library PUBLIC
        ${EIGEN3_INCLUDE_DIRS}
)

# --- 构建主应用程序 ---

# 收集应用层源文件
file(GLOB APP_SOURCES "app/*.cpp")

# 创建可执行文件
add_executable(solver_app ${APP_SOURCES})

# 链接主程序到我们的核心库
# 这会自动处理 fem_library 的所有公共依赖 (比如 Eigen)
target_link_libraries(solver_app PRIVATE fem_library)


# 自动收集所有测试源文件
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

# 创建测试可执行文件
add_executable(fem_tests ${TEST_SOURCES})

# 链接测试程序到 GoogleTest 和我们的核心库
target_link_libraries(fem_tests PRIVATE fem_library gtest_main)

# 自动将测试添加到 CTest
include(GoogleTest)
gtest_discover_tests(fem_tests)

# --- 输出信息 ---
message(STATUS "Eigen3 include dirs: ${EIGEN3_INCLUDE_DIRS}")
message(STATUS "Building application 'solver_app' and tests 'fem_tests'...")
if(MSVC)
    # 禁用 C4819 警告
    target_compile_options(fem_tests PRIVATE /wd4819)
    target_compile_options(fem_library PRIVATE /wd4819)
    target_compile_options(solver_app PRIVATE /wd4819)
endif()