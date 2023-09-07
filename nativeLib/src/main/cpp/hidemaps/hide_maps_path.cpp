#include <sys/mman.h>
#include <dlfcn.h>
#include <link.h>
#include <string>
#include <iostream>
#include <elf_util.h>
#include <set>
#include <list>
#include <cinttypes>
#include <sys/mman.h>
#include <set>
#include <string_view>
#include <pmparser.h>


#include "AllInclude.h"
#include "ZhenxiLog.h"
#include "pmparser.h"
#include "common_macros.h"
#include "mylibc.h"
#include "hide_maps_path.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <appUtils.h>

namespace ZhenxiRuntime::MapsItemHide {


#ifdef __LP64__
#define LIB_PATH "/system/lib64/"
#else
#define LIB_PATH "/system/lib/"
#endif

    struct hide_struct {
        procmaps_struct *original;
        uintptr_t backup_address;
    };

    static int get_prot(const procmaps_struct *procstruct) {
        int prot = 0;
        if (procstruct->is_r) {
            prot |= PROT_READ;
        }
        if (procstruct->is_w) {
            prot |= PROT_WRITE;
        }
        if (procstruct->is_x) {
            prot |= PROT_EXEC;
        }
        return prot;
    }

#define FAILURE_RETURN(exp, failure_value) ({   \
    __typeof__(exp) _rc;                    \
    _rc = (exp);                            \
    if (_rc == failure_value) {             \
        LOGE("%s: %s", #exp, strerror(errno)); \
        return 1;                           \
    }                                       \
    _rc; })

    static int do_hide_org(hide_struct *data) {
        auto procstruct = data->original;
        auto start = (uintptr_t) procstruct->addr_start;
        auto end = (uintptr_t) procstruct->addr_end;
        auto length = end - start;
        int prot = get_prot(procstruct);

        // backup
        data->backup_address = (uintptr_t) FAILURE_RETURN(
                mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0),
                MAP_FAILED);
//        LOGD("%" PRIxPTR"-%" PRIxPTR" %s %ld %s is backup to %" PRIxPTR, start, end, procstruct->perm,
//             procstruct->offset,
//             procstruct->pathname, data->backup_address);

        if (!procstruct->is_r) {
            //LOGD("mprotect +r");
            FAILURE_RETURN(mprotect((void *) start, length, prot | PROT_READ), -1);
        }
        //LOGD("memcpy -> backup %s",procstruct->pathname)
        memcpy((void *) data->backup_address, (void *) start, length);

        // munmap original
        //LOGD("munmap original %s",procstruct->pathname)
        FAILURE_RETURN(munmap((void *) start, length), -1);

        // restore
        //LOGD("mmap original %s",procstruct->pathname)
        FAILURE_RETURN(mmap((void *) start, length, prot, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0),
                       MAP_FAILED);
        //LOGD("mprotect +w");
        FAILURE_RETURN(mprotect((void *) start, length, prot | PROT_WRITE), -1);
        //LOGD("memcpy -> original");
        memcpy((void *) start, (void *) data->backup_address, length);
        if (!procstruct->is_w) {
            //LOGD("mprotect -w");
            FAILURE_RETURN(mprotect((void *) start, length, prot), -1);
        }
        return 0;
    }

#if defined(__arm__)
#ifndef __NR_mmap2
#define __NR_mmap2 192
#endif
#define MMAP_NR __NR_mmap2
#elif defined(__aarch64__)
#ifndef __NR_mmap
#define __NR_mmap 222
#endif
#define MMAP_NR __NR_mmap
#endif

    static int do_hide(hide_struct *data) {
        auto procstruct = data->original;
        auto start = (uintptr_t) procstruct->addr_start;
        auto end = (uintptr_t) procstruct->addr_end;
        auto length = end - start;
        int prot = get_prot(procstruct);

        // backup
        data->backup_address = reinterpret_cast<uintptr_t>(FAILURE_RETURN(
                (void *) raw_syscall(MMAP_NR, nullptr, length, PROT_READ | PROT_WRITE,
                                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0),
                MAP_FAILED));

        if (!procstruct->is_r) {
            FAILURE_RETURN(raw_syscall(SYS_mprotect, (void *) start, length, prot | PROT_READ), -1);
        }
        my_memcpy((void *) data->backup_address, (void *) start, length);

        // munmap original
        FAILURE_RETURN(raw_syscall(SYS_munmap, (void *) start, length), -1);

        // restore
        FAILURE_RETURN((void *) raw_syscall(MMAP_NR, (void *) start, length, prot,
                                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0),
                       MAP_FAILED);
        FAILURE_RETURN(raw_syscall(SYS_mprotect, (void *) start, length, prot | PROT_WRITE), -1);
        my_memcpy((void *) start, (void *) data->backup_address, length);
        if (!procstruct->is_w) {
            FAILURE_RETURN(raw_syscall(SYS_mprotect, (void *) start, length, prot), -1);
        }
        return 0;
    }

    bool clear_elf_header(const std::set<std::string_view> &names, CLEAN_ELF_HEADER_TYPE type) {
        procmaps_iterator *maps = pmparser_parse(-1);
        if (maps == nullptr) {
            LOGE("clear_elf_header cannot parse the memory map");
            return false;
        }

        bool success = false;
        procmaps_struct *maps_tmp;
        while ((maps_tmp = pmparser_next(maps)) != nullptr) {
            bool matched;
            matched = names.count(maps_tmp->pathname);
            if (!matched) continue;
            void *addr = reinterpret_cast<void *>(maps_tmp->addr_start);
            unsigned int size = 0;
#if defined(__aarch64__)
            auto *elf_hdr = reinterpret_cast<Elf64_Ehdr *>(addr);
            if (type == CLEAN_ELF_HEADER_TYPE::CLEAN_ELF) {
                size = sizeof(elf_hdr->e_ident);
            } else if (type == CLEAN_ELF_HEADER_TYPE::CLEAN_ELF_HREADER) {
                size = sizeof(Elf64_Ehdr);
            }
#else
            auto *elf_hdr = reinterpret_cast<Elf64_Ehdr *>(addr);
            if(type==CLEAN_ELF_HEADER_TYPE::CLEAN_ELF){
                size = sizeof(elf_hdr->e_ident);
            }else if(type==CLEAN_ELF_HEADER_TYPE::CLEAN_ELF_HREADER){
                size = sizeof(Elf32_Ehdr);
            }
#endif
            //需要先修改权限,否则读取elf头可能会sign11
            bool ret = mProtectDef((size_t) addr, size);
            if (!ret) {
                LOGE("mprotect elf header fail %s", maps_tmp->pathname)
                success = false;
                continue;
            }
            //是elf头
            if (
                    (((char *) addr)[0] == ELFMAG0) &&
                    ((char *) addr)[1] == ELFMAG1 &&
                    ((char *) addr)[2] == ELFMAG2 &&
                    ((char *) addr)[3] == ELFMAG3) {
                //抹掉elf头信息
                std::memset(addr, 0, size);
                LOGI("clean elf header success ! %s ", maps_tmp->pathname)
            }
            //权限复原
            success = mProtectUserDefined((size_t) addr, size,
                                          (maps_tmp->is_r ? PROT_READ : 0) |
                                          (maps_tmp->is_w ? PROT_WRITE : 0) |
                                          (maps_tmp->is_x ? PROT_EXEC : 0));
        }
        pmparser_free(maps);
        return success;
    }

    void hide_elf_header(const std::set<std::string_view> &names, CLEAN_ELF_HEADER_TYPE type) {
        clear_elf_header(names, type);
    }

    bool riru_hide(const std::set<std::string_view> &names) {
        procmaps_iterator *maps = pmparser_parse(-1);
        if (maps == nullptr) {
            LOGE("cannot parse the memory map");
            return false;
        }
        void *libc = dlopen("libc.so", RTLD_NOW);
        if (libc == nullptr) {
            LOGE("cannot get dlopen libc.so ");
            return false;
        }
        //间接调用,而非直接调用。防止隐藏libc.so时候出现问题。
        auto realloc_sym = (void *(*)(void *, size_t)) dlsym(libc, "realloc");
        auto malloc_sym = (void *(*)(size_t)) dlsym(libc, "malloc");

        //char buf[PATH_MAX];
        hide_struct *data = nullptr;
        size_t data_count = 0;
        procmaps_struct *maps_tmp;
        while ((maps_tmp = pmparser_next(maps)) != nullptr) {
            bool matched = names.count(maps_tmp->pathname);
            if (!matched) continue;
            if (maps_tmp->is_r) {
                if (data) {
                    data = (hide_struct *) realloc_sym(data,
                                                       sizeof(hide_struct) * (data_count + 1));
                } else {
                    data = (hide_struct *) malloc_sym(sizeof(hide_struct));
                }
                data[data_count].original = maps_tmp;
                data_count += 1;
            }
//            LOGI("%" PRIxPTR"-%" PRIxPTR" %s %ld %s", start, end, maps_tmp->perm, maps_tmp->offset,
//                 maps_tmp->pathname);
        }

        for (int i = 0; i < data_count; ++i) {
            do_hide(&data[i]);
            LOGI("maps do_hide success !  %s  [%s] ", data[i].original->pathname, getprogname())
        }

        if (data) free(data);
        dlclose(libc);
        pmparser_free(maps);

        return true;
    }

}  // namespace Hide
