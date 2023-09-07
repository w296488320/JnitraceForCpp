cmake_minimum_required(VERSION 3.4.1)


add_library(
        mapshidepath

        STATIC

        hide_maps_path.cpp

)
target_link_libraries(
        mapshidepath

        proc_maps_parser
)
target_link_libraries(
        mapshidepath

        ${log-lib}
)

