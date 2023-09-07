cmake_minimum_required(VERSION 3.4.1)
#sandhook inlinehook新版本 采用C++ 17编译,优先采用17编译。
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -std=c++20")
#添加内联支持
enable_language(C ASM)


add_subdirectory(xDL)


add_library(
        dlfc

        STATIC

        elf_util.cpp

)

target_link_libraries(
        dlfc
        ${log-lib}
)










