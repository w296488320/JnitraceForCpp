cmake_minimum_required(VERSION 3.22.1)
aux_source_directory(. xdl_src_list)


include_directories(
        ./
        ${HomePath}/nativeLib/src/main/cpp/includes/
)


add_library(
        xdl

        STATIC

        ${xdl_src_list}
)

target_link_libraries(
        xdl

        ${log-lib}
)