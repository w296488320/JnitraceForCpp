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

// Android L
// https://android.googlesource.com/platform/system/core/+/android-5.0.0_r1/include/backtrace/
// https://android.googlesource.com/platform/system/core/+/android-5.0.0_r1/libbacktrace/

// Android M
// https://android.googlesource.com/platform/system/core/+/android-6.0.0_r1/include/backtrace/
// https://android.googlesource.com/platform/system/core/+/android-6.0.0_r1/libbacktrace/

// Android P
// https://android.googlesource.com/platform/system/core/+/android-9.0.0_r1/libbacktrace/include/backtrace
// https://android.googlesource.com/platform/system/core/+/android-9.0.0_r1/libbacktrace/

#include "xu_libbacktrace.h"

#include <android/api-level.h>
#include <android/log.h>
#include <stdbool.h>
#include <stddef.h>

#include "xdl.h"
#include "xu_libcxx.h"
#include "xu_util.h"

#ifndef __LP64__
#define XU_LIBBACKTRACE_PATHNAME_L      "/system/lib/libbacktrace_libc++.so"
#define XU_LIBBACKTRACE_PATHNAME_M      "/system/lib/libbacktrace.so"
#define XU_LIBBACKTRACE_FORMAT          "_ZN9Backtrace15FormatFrameDataEj"
#define XU_LIBBACKTRACE_LOCAL_UNWIND_L  "_ZN9Backtrace6UnwindEjP8ucontext"
#define XU_LIBBACKTRACE_LOCAL_UNWIND_M  "_ZN16BacktraceCurrent6UnwindEjP8ucontext"
#define XU_LIBBACKTRACE_LOCAL_UNWIND_P  "_ZN16BacktraceCurrent6UnwindEjPv"
#define XU_LIBBACKTRACE_REMOTE_UNWIND_L "_ZN12UnwindPtrace6UnwindEjP8ucontext"
#define XU_LIBBACKTRACE_REMOTE_UNWIND_P "_ZN17UnwindStackPtrace6UnwindEjPv"
#else
#define XU_LIBBACKTRACE_PATHNAME_L      "/system/lib64/libbacktrace_libc++.so"
#define XU_LIBBACKTRACE_PATHNAME_M      "/system/lib64/libbacktrace.so"
#define XU_LIBBACKTRACE_FORMAT          "_ZN9Backtrace15FormatFrameDataEm"
#define XU_LIBBACKTRACE_LOCAL_UNWIND_L  "_ZN9Backtrace6UnwindEmP8ucontext"
#define XU_LIBBACKTRACE_LOCAL_UNWIND_M  "_ZN16BacktraceCurrent6UnwindEmP8ucontext"
#define XU_LIBBACKTRACE_LOCAL_UNWIND_P  "_ZN16BacktraceCurrent6UnwindEmPv"
#define XU_LIBBACKTRACE_REMOTE_UNWIND_L "_ZN12UnwindPtrace6UnwindEmP8ucontext"
#define XU_LIBBACKTRACE_REMOTE_UNWIND_P "_ZN17UnwindStackPtrace6UnwindEmPv"
#endif
#define XU_LIBBACKTRACE_CREATE  "_ZN9Backtrace6CreateEiiP12BacktraceMap"
#define XU_LIBBACKTRACE_DTOR_D1 "_ZN9BacktraceD1Ev"

typedef void *(*xu_libbacktrace_create_t)(pid_t pid, pid_t tid, void *map);
typedef void (*xu_libbacktrace_dtor_d1_t)(void *self);
typedef xu_libcxx_string_t (*xu_libbacktrace_format_t)(void *self, size_t frame_num);
typedef bool (*xu_libbacktrace_unwind_t)(void *self, size_t num_ignore_frames, void *context);

static xu_libbacktrace_create_t xu_libbacktrace_create_ptr = NULL;
static xu_libbacktrace_dtor_d1_t xu_libbacktrace_dtor_d1_ptr = NULL;
static xu_libbacktrace_format_t xu_libbacktrace_format_ptr = NULL;
static xu_libbacktrace_unwind_t xu_libbacktrace_local_unwind_ptr = NULL;
static xu_libbacktrace_unwind_t xu_libbacktrace_remote_unwind_ptr = NULL;

int xu_libbacktrace_init(void) {
  int api_level = xu_util_get_api_level();
  const char *sym = NULL;
  int ret = -1;

  void *handle =
      xdl_open(api_level <= __ANDROID_API_L_MR1__ ? XU_LIBBACKTRACE_PATHNAME_L : XU_LIBBACKTRACE_PATHNAME_M,
               XDL_DEFAULT);
  if (NULL == handle) return -1;

  if (NULL ==
      (xu_libbacktrace_create_ptr = (xu_libbacktrace_create_t)xdl_sym(handle, XU_LIBBACKTRACE_CREATE, NULL)))
    goto end;
  if (NULL == (xu_libbacktrace_dtor_d1_ptr =
                   (xu_libbacktrace_dtor_d1_t)xdl_sym(handle, XU_LIBBACKTRACE_DTOR_D1, NULL)))
    goto end;
  if (NULL ==
      (xu_libbacktrace_format_ptr = (xu_libbacktrace_format_t)xdl_sym(handle, XU_LIBBACKTRACE_FORMAT, NULL)))
    goto end;

  if (__ANDROID_API_L__ <= api_level && api_level <= __ANDROID_API_L_MR1__)
    sym = XU_LIBBACKTRACE_LOCAL_UNWIND_L;
  else if (__ANDROID_API_M__ <= api_level && api_level <= __ANDROID_API_O_MR1__)
    sym = XU_LIBBACKTRACE_LOCAL_UNWIND_M;
  else if (__ANDROID_API_P__ <= api_level)
    sym = XU_LIBBACKTRACE_LOCAL_UNWIND_P;
  if (NULL == (xu_libbacktrace_local_unwind_ptr = (xu_libbacktrace_unwind_t)xdl_sym(handle, sym, NULL)))
    goto end;

  if (__ANDROID_API_L__ <= api_level && api_level <= __ANDROID_API_O_MR1__)
    sym = XU_LIBBACKTRACE_REMOTE_UNWIND_L;
  else if (__ANDROID_API_P__ <= api_level)
    sym = XU_LIBBACKTRACE_REMOTE_UNWIND_P;
  if (NULL == (xu_libbacktrace_remote_unwind_ptr = (xu_libbacktrace_unwind_t)xdl_sym(handle, sym, NULL)))
    goto end;

  ret = 0;

end:
  xdl_close(handle);
  return ret;
}

void *xu_libbacktrace_create(pid_t pid, pid_t tid) {
  return xu_libbacktrace_create_ptr(pid, tid, NULL);
}

void xu_libbacktrace_dtor_d1(void *self) {
  xu_libbacktrace_dtor_d1_ptr(self);
}

bool xu_libbacktrace_local_unwind(void *self, size_t num_ignore_frames, void *context) {
  return xu_libbacktrace_local_unwind_ptr(self, num_ignore_frames, context);
}

bool xu_libbacktrace_remote_unwind(void *self, size_t num_ignore_frames, void *context) {
  // libunwind.so in Android [5.0, 8.1] do NOT support unwinding from a specified context
  if (NULL != context) {
    int api_level = xu_util_get_api_level();
    if (__ANDROID_API_L__ <= api_level && api_level <= __ANDROID_API_O_MR1__) context = NULL;
  }

  return xu_libbacktrace_remote_unwind_ptr(self, num_ignore_frames, context);
}

xu_libcxx_string_t xu_libbacktrace_format(void *self, size_t frame_num) {
  return xu_libbacktrace_format_ptr(self, frame_num);
}
