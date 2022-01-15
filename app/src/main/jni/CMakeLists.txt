cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 14)
#添加内联支持
enable_language(C ASM)


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
        Jnitrace.cpp

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


find_library(
        log-lib
        log
)








