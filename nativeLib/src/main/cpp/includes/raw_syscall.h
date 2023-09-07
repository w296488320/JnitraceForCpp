//
// Created by Zhenxi on 2022/8/22.
//

#ifndef HUNTER_RAW_SYSCALL_H
#define HUNTER_RAW_SYSCALL_H

#include <syscall.h>

#define INLINE __always_inline
extern "C" {
INLINE  extern inline long raw_syscall(long __number, ...);
}
#endif //HUNTER_RAW_SYSCALL_H
