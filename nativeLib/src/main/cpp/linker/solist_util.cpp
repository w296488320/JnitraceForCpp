//
// Created by Ssss on 2022/9/21.
//

#include <sys/mman.h>
#include <dlfcn.h>
#include <link.h>
#include <string>
#include <iostream>
#include <set>

#include "solist_util.h"
#include "ZhenxiLog.h"



namespace solist {
    class ProtectedDataGuard {

    public:
        ProtectedDataGuard() {
            if (ctor != nullptr)
                (this->*ctor)();
        }

        ~ProtectedDataGuard() {
            if (dtor != nullptr)
                (this->*dtor)();
        }

        static bool setup(const SandHook::ElfImg &linker) {
            ctor = MemFunc{.data = {.p = reinterpret_cast<void *>(linker.getSymbAddress(
                    "__dl__ZN18ProtectedDataGuardC2Ev")),
                    .adj = 0}}
                    .f;
            dtor = MemFunc{.data = {.p = reinterpret_cast<void *>(linker.getSymbAddress(
                    "__dl__ZN18ProtectedDataGuardD2Ev")),
                    .adj = 0}}
                    .f;
            return ctor != nullptr && dtor != nullptr;
        }

        ProtectedDataGuard(const ProtectedDataGuard &) = delete;

        void operator=(const ProtectedDataGuard &) = delete;

    private:
        using FuncType = void (ProtectedDataGuard::*)();

        static FuncType ctor;
        static FuncType dtor;

        union MemFunc {
            FuncType f;

            struct {
                void *p;
                std::ptrdiff_t adj;
            } data;
        };
    };

    ProtectedDataGuard::FuncType ProtectedDataGuard::ctor = nullptr;
    ProtectedDataGuard::FuncType ProtectedDataGuard::dtor = nullptr;

    struct soinfo;

    soinfo *solist = nullptr;
    soinfo **sonext = nullptr;
    soinfo *somain = nullptr;

    template<typename T>
    inline T *getStaticVariable(const SandHook::ElfImg &linker, std::string_view name) {
        auto *addr = reinterpret_cast<T **>(linker.getSymbAddress(name.data()));
        return addr == nullptr ? nullptr : *addr;
    }

    bool soinfo::setup(const SandHook::ElfImg &linker) {
        get_realpath_sym = reinterpret_cast<decltype(get_realpath_sym)>(linker.getSymbAddress(
                "__dl__ZNK6soinfo12get_realpathEv"));
        auto vsdo = getStaticVariable<soinfo>(linker, "__dl__ZL4vdso");
        for (size_t i = 0; i < 1024 / sizeof(void *); i++) {
            auto *possible_next = *(void **) ((uintptr_t) solist + i * sizeof(void *));
            if (possible_next == somain || (vsdo != nullptr && possible_next == vsdo)) {
                solist_next_offset = i * sizeof(void *);
                return get_realpath_sym != nullptr;
            }
        }
        LOGW("failed to search next offset");
        // shortcut
        return get_realpath_sym != nullptr;
    }

    bool solist_remove_soinfo(soinfo *si) {
        soinfo *prev = nullptr, *trav;
        for (trav = solist; trav != nullptr; trav = trav->next()) {
            if (trav == si) {
                break;
            }
            prev = trav;
        }

        if (trav == nullptr) {
            // si was not in solist
            LOGE("name \"%s\"@%p is not in solist!", si->get_realpath(), si);
            return false;
        }

        // prev will never be null, because the first entry in solist is
        // always the static libdl_info.
        prev->next(si->next());
        if (si == *sonext) {
            *sonext = prev;
        }

        //LOGI("removed soinfo: %s", si->get_realpath());

        return true;
    }

    // init_array 中执行
    const auto initialized = []() {
        SandHook::ElfImg linker("/linker");
        return ProtectedDataGuard::setup(linker) &&
               (solist = getStaticVariable<soinfo>(linker, "__dl__ZL6solist")) != nullptr &&
               (sonext = linker.getSymbAddress<soinfo **>("__dl__ZL6sonext")) != nullptr &&
               (somain = getStaticVariable<soinfo>(linker, "__dl__ZL6somain")) != nullptr &&
               soinfo::setup(linker);
    }();

    std::list<soinfo *> linker_get_solist() {
        std::list<soinfo *> linker_solist{};
        for (auto *iter = solist; iter; iter = iter->next()) {
            linker_solist.push_back(iter);
        }
        return linker_solist;
    }

    void RemovePathsFromSolist(const std::set<std::string_view> &names) {
        if (!initialized) {
            LOGE("not initialized")
            return;
        }
        ProtectedDataGuard g;
        for (const auto &soinfo: linker_get_solist()) {
            const auto &real_path = soinfo->get_realpath();
            if (real_path && names.count(real_path)) {
                bool isSuccess = solist_remove_soinfo(soinfo);
                if(isSuccess){
                    //LOGI("remove success ! %s",real_path)
                } else{
                    LOGE("remove fail ! %s",real_path)
                }
            }
        }
    }

    void RemoveFromSoList(const std::set<std::string_view> &names) {
        RemovePathsFromSolist(names);
    }
}  // namespace Hide
