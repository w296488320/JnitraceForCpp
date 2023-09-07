cmake_minimum_required(VERSION 3.4.1)
enable_language(C ASM)






if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(RAW_SOURCE syscall64.S)
elseif (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
    set(RAW_SOURCE syscall32.S)
endif ()

add_library(
        raw_syscall

        STATIC

        ${RAW_SOURCE}
)

target_link_libraries(
        raw_syscall

        ${log-lib}
)


#find_library(
#        log-lib
#        log
#)








