//
// Created by Ssss on 2022/9/21.
//

#ifndef HUNTER_SOLIST_UTIL_H
#define HUNTER_SOLIST_UTIL_H

#include "elf_util.h"
#include <list>
#include <set>
#include <string>

namespace solist {
    struct soinfo {
        soinfo *next() {
            return *(soinfo **) ((uintptr_t) this + solist_next_offset);
        }

        void next(soinfo *si) {
            *(soinfo **) ((uintptr_t) this + solist_next_offset) = si;
        }

        const char *get_realpath() {
            return get_realpath_sym ? get_realpath_sym(this) : ((std::string *) (
                    (uintptr_t) this + solist_realpath_offset))->c_str();

        }

        static bool setup(const SandHook::ElfImg &linker);

#ifdef __LP64__
        constexpr static size_t solist_realpath_offset = 0x1a8;
        inline static size_t solist_next_offset = 0x30;
#else
        constexpr static size_t solist_realpath_offset = 0x174;
        inline static size_t solist_next_offset = 0xa4;
#endif
        // since Android 8
        inline static const char *(*get_realpath_sym)(soinfo *);
    };
    std::list<soinfo *> linker_get_solist();
    void RemoveFromSoList(const std::set<std::string_view> &names);
}

#endif //HUNTER_SOLIST_UTIL_H
