cmake_minimum_required(VERSION 3.4.1)
ENABLE_LANGUAGE(ASM)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -std=c++17")



add_subdirectory(exts/vixl)
add_subdirectory(exts/distorm)
add_subdirectory(exts/ucontext)

include_directories(
        ${HomePath}/nativeLib/src/main/cpp/
        ${HomePath}/nativeLib/src/main/cpp/includes/
        ${HomePath}/nativeLib/src/main/cpp/hook/signhook/include/
)

include_directories(
        ./
        common
        exts
        exts/distorm
        exts/ucontext
        exts/vixl
        include
        inline
        inline/arm32/
        inline/arm64/

        instrcache
        instrcache/arm32
        instrcache/arm64

        memory
        platform

)

set(ARCH_SRC_AARCH32
        instrcache/arm32/translator.cc
        inline/arm32/inline_hook_arm32.cpp
        )
set(ARCH_SRC_AARCH64
        instrcache/arm64/translator.cc
        inline/arm64/inline_hook_arm64.cc
        )

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
set(ARCH_SOURCES ${ARCH_SOURCES} ${ARCH_SRC_AARCH32})
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
set(ARCH_SOURCES ${ARCH_SOURCES} ${ARCH_SRC_AARCH64})
endif ()

add_library(
        # runtime在使用这个库
        signhook

        STATIC

        ./common/types.cc
        ./common/dlfcn_nougat.cc
        ./common/elf_util.cc
        ./common/elf.cpp
        ./inline/inline_hook.cc
        ./platform/memory.cc
        ./memory/code_buffer.cc

        ${ARCH_SOURCES}
)


target_link_libraries(signhook distorm vixl uccontext android log)

