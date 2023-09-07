//
// Created by SwiftGan on 2020/8/20.
//

#include <sys/mman.h>
#include "memory.h"

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/vm_map.h>
#include <libkern/OSCacheControl.h>
#include <sys/sysctl.h>
#endif

#if __APPLE__

extern "C" kern_return_t mach_vm_remap(vm_map_t, mach_vm_address_t *, mach_vm_size_t,
                                       mach_vm_offset_t, int, vm_map_t, mach_vm_address_t,
                                       boolean_t, vm_prot_t *, vm_prot_t *, vm_inherit_t);

#endif

using namespace SandHook;


void *Platform::MirrorRWMemory(void *base, size_t size) {
#if ANDROID
    return base;
#else
    kern_return_t ret;
    mach_vm_address_t mirror;
    vm_prot_t cur_prot, max_prot;

    mirror = 0;
    ret = mach_vm_remap(mach_task_self(),
                        &mirror,
                        size,
                        0,
                        VM_FLAGS_ANYWHERE | VM_FLAGS_RANDOM_ADDR,
                        mach_task_self(),
                        (mach_vm_address_t)base,
                        false,
                        &cur_prot,
                        &max_prot,
                        VM_INHERIT_DEFAULT
    );
    if (ret != KERN_SUCCESS) {
        return NULL;
    }
    mprotect((void*) mirror, size, VM_PROT_READ | VM_PROT_WRITE);
    return reinterpret_cast<void *>(mirror);
#endif
}

void *Platform::MapExecutableMemory(size_t size, VAddr addr) {
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (addr != 0) {
        flags |= MAP_FIXED;
    }
#if ANDROID
    return mmap(reinterpret_cast<void *>(addr), size, PROT_READ | PROT_WRITE | PROT_EXEC,
                flags,
                -1, 0);

#else
    return mmap(reinterpret_cast<void *>(addr), size, PROT_READ | PROT_EXEC,
                flags,
                -1, 0);
#endif
}

void *Platform::MapCowMemory(size_t size, VAddr addr) {
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (addr != 0) {
        flags |= MAP_FIXED;
    }
    return mmap(reinterpret_cast<void *>(addr), size, PROT_READ | PROT_WRITE,
                flags,
                -1, 0);
}

void *Platform::MapFile(int fd, size_t size, size_t offset) {
    int flags = MAP_PRIVATE;
    auto ret = mmap(0, size, PROT_READ | PROT_WRITE, flags, fd, offset);
    if (ret == MAP_FAILED) {
        return nullptr;
    }
    return ret;
}

void Platform::UnMapMemory(VAddr addr, size_t size) {
    munmap(reinterpret_cast<void *>(addr), size);
}

void Platform::ClearICache(VAddr start, size_t size) {
#ifdef __APPLE__
    sys_icache_invalidate(reinterpret_cast<char *>(start), size);
#else
    __builtin___clear_cache(reinterpret_cast<char *>(start),
                            reinterpret_cast<char *>(start + size));
#endif
}

void Platform::ClearDCache(VAddr start, size_t size) {
#ifdef __APPLE__
    sys_dcache_flush(reinterpret_cast<char *>(start), size);
#endif
}

bool Platform::MakeRWX(VAddr addr, size_t size) {
    size = (((addr + size) / page_size) - (addr / page_size) + 1) * page_size;
#ifdef __APPLE__
    return false;
#else
    return mprotect(reinterpret_cast<void *>(AlignDown(addr, page_size)), size, PROT_READ | PROT_WRITE | PROT_EXEC) >= 0;
#endif
}

