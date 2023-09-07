// Copyright (c) 2020-2023 HexHacking Team
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// Created by caikelun on 2020-10-21.

//
// xUnwind version: 2.0.0
//
// xUnwind is a collection of Android native stack unwinding solutions.
// For more information, documentation, and the latest version please check:
// https://github.com/hexhacking/xUnwind
//

#ifndef IO_GITHUB_HEXHACKING_XUNWIND
#define IO_GITHUB_HEXHACKING_XUNWIND

#include <android/log.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define XUNWIND_CURRENT_PROCESS (-1)
#define XUNWIND_CURRENT_THREAD  (-1)
#define XUNWIND_ALL_THREADS     (-2)

#ifdef __cplusplus
extern "C" {
#endif

void xunwind_cfi_log(pid_t pid, pid_t tid, void *context, const char *logtag, android_LogPriority priority,
                     const char *prefix);
void xunwind_cfi_dump(pid_t pid, pid_t tid, void *context, int fd, const char *prefix);
char *xunwind_cfi_get(pid_t pid, pid_t tid, void *context, const char *prefix);

size_t xunwind_fp_unwind(uintptr_t *frames, size_t frames_cap, void *context);
size_t xunwind_eh_unwind(uintptr_t *frames, size_t frames_cap, void *context);

void xunwind_frames_log(uintptr_t *frames, size_t frames_sz, const char *logtag, android_LogPriority priority,
                        const char *prefix);
void xunwind_frames_dump(uintptr_t *frames, size_t frames_sz, int fd, const char *prefix);
char *xunwind_frames_get(uintptr_t *frames, size_t frames_sz, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif
