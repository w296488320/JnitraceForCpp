cmake_minimum_required(VERSION 3.4.1)


add_library(
        proc_maps_parser

        STATIC

        pmparser.cpp

)

target_link_libraries(
        proc_maps_parser

        ${log-lib}
)

