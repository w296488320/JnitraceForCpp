cmake_minimum_required(VERSION 3.4.1)


include_directories(
        ${HomePath}/nativeLib/src/main/cpp/includes/
)

add_library(
        adapter

        STATIC

        adapter.cpp

        )

target_link_libraries(
        adapter

        ${log-lib}
)

