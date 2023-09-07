set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fdiagnostics-show-option -Wextra -Wredundant-decls -pedantic -Wwrite-strings -Wunused")
include_directories(.)
ENABLE_LANGUAGE(ASM)

set(uccontext_src
        breakpad_getcontext.S)

set(uccontext_src_32
        arm/getcontext.S
        arm/makecontext.c
        arm/setcontext.S
        arm/swapcontext.S
        )

set(uccontext_src_64
        aarch64/getcontext.S
        aarch64/makecontext.c
        aarch64/setcontext.S
        aarch64/sysdep.c)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    set(uccontext_src ${uccontext_src} ${uccontext_src_32})
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
    set(uccontext_src ${uccontext_src} ${uccontext_src_64})
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64)")
    add_library(uccontext ${uccontext_src})
    target_link_libraries(uccontext android)
endif ()
