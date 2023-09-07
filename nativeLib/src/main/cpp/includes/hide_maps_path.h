#ifndef HIDE_MAPS_PATH_H
#define HIDE_MAPS_PATH_H

#include <string>

namespace ZhenxiRuntime {

    enum CLEAN_ELF_HEADER_TYPE {
        CLEAN_ELF,//清理头部elf标识
        CLEAN_ELF_HREADER,//清理elf_header
    };
    namespace MapsItemHide {
        bool riru_hide(const std::set<std::string_view> &names);
        void hide_elf_header(const std::set<std::string_view> &names,CLEAN_ELF_HEADER_TYPE type);
    }
}
#endif //HIDE_MAPS_PATH_H
