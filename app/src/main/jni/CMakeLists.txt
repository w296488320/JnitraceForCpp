cmake_minimum_required(VERSION 3.4.1)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -std=c++20")
#添加内联支持
enable_language(C ASM)

find_library(
        log-lib
        log
)


include_directories(
        ./
        ./log/
        ./Substrate/
        ./uitils/

)


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

        #Substrate
        Substrate/hde64.c
        Substrate/SubstrateDebug.cpp
        Substrate/SubstrateHook.cpp
        Substrate/SubstratePosixMemory.cpp
        Substrate/And64InlineHook.cpp

        uitils/parse.cpp
)

target_link_libraries(
        jnitrace

        ${log-lib}
)










