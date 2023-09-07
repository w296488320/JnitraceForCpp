cmake_minimum_required(VERSION 3.4.1)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -std=c++20")
#添加内联支持
enable_language(C ASM)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -std=c++20")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmacro-redefined")

add_definitions(-DCORE_SO_NAME="libjnitrace.so")

find_library(
        log-lib
        log
)


message(STATUS "HomePath ->${HomePath} PROJECT_SOURCE_DIR -> ${PROJECT_SOURCE_DIR} ")

include_directories(
        ./
        ./includes/

        ${HomePath}/nativeLib/src/main/cpp/
        ${HomePath}/nativeLib/src/main/cpp/includes/
)

add_subdirectory(${HomePath}/nativeLib/src/main/cpp/ nativeLib.out)

add_library(
        tracetest

        SHARED

        test/test.cpp
)
target_link_libraries(
        tracetest

        ${log-lib}
)


add_library(
        jnitrace

        SHARED

        main.cpp
        JnitraceForC.cpp
        stringHandler.cpp

)

target_link_libraries(
        jnitrace

        adapter
        dlfc
        runtimehook
        libpath

        utils
        raw_syscall
        xunwind
        xdl
        linkerHandler#处理linker相关的方法
        findsyscall

        ${log-lib}
)










