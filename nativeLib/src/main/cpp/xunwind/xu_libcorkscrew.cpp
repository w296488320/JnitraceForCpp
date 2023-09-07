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

// Android K
// https://android.googlesource.com/platform/system/core/+/android-4.4_r1/include/corkscrew/
// https://android.googlesource.com/platform/system/core/+/android-4.4_r1/libcorkscrew/

#include "xu_config.h"
#ifdef XU_CONFIG_CFI_WITH_LIBCORKSCREW

#include <android/log.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "xu_libcorkscrew.h"

#define XU_LIBCORKSCREW_MAX_DEPTH 32

typedef struct {
  uintptr_t placeholder[3];
} xu_libcorkscrew_frame_t;

typedef struct {
  uintptr_t placeholder[5];
} xu_libcorkscrew_symbol_t;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct {
  bool is_remote;
  pid_t pid;
  pid_t tid;
  xu_libcorkscrew_frame_t frames[XU_LIBCORKSCREW_MAX_DEPTH];
  size_t frames_sz;
  xu_libcorkscrew_symbol_t symbols[XU_LIBCORKSCREW_MAX_DEPTH];
  void *map;
} xu_libcorkscrew_t;
#pragma clang diagnostic pop

#define XU_LIBCORKSCREW_BASENAME             "libcorkscrew.so"
#define XU_LIBCORKSCREW_LOCAL_LOAD_MAP       "acquire_my_map_info_list"
#define XU_LIBCORKSCREW_LOCAL_FREE_MAP       "release_my_map_info_list"
#define XU_LIBCORKSCREW_LOCAL_UNWIND         "unwind_backtrace_thread"
#define XU_LIBCORKSCREW_LOCAL_UNWIND_CONTEXT "unwind_backtrace_signal_arch"
#define XU_LIBCORKSCREW_LOCAL_GET_SYM        "get_backtrace_symbols"
#define XU_LIBCORKSCREW_REMOTE_LOAD_MAP      "load_ptrace_context"
#define XU_LIBCORKSCREW_REMOTE_FREE_MAP      "free_ptrace_context"
#define XU_LIBCORKSCREW_REMOTE_UNWIND        "unwind_backtrace_ptrace"
#define XU_LIBCORKSCREW_REMOTE_GET_SYM       "get_backtrace_symbols_ptrace"
#define XU_LIBCORKSCREW_FREE_SYM             "free_backtrace_symbols"
#define XU_LIBCORKSCREW_FORMAT               "format_backtrace_line"

typedef void *(*xu_libcorkscrew_local_load_map_t)(void);
typedef void (*xu_libcorkscrew_local_free_map_t)(void *map);
typedef ssize_t (*xu_libcorkscrew_local_unwind_t)(pid_t maintid, xu_libcorkscrew_frame_t *bt, size_t ignore_depth,
                                                  size_t max_depth);
typedef ssize_t (*xu_libcorkscrew_local_unwind_context_t)(void *siginfo, void *context, const void *map,
                                                          xu_libcorkscrew_frame_t *bt, size_t ignore_depth,
                                                          size_t max_depth);
typedef void (*xu_libcorkscrew_local_get_sym_t)(const xu_libcorkscrew_frame_t *bt, size_t frames,
                                                xu_libcorkscrew_symbol_t *symbols);
typedef void *(*xu_libcorkscrew_remote_load_map_t)(pid_t pid);
typedef void (*xu_libcorkscrew_remote_free_map_t)(void *map);
typedef ssize_t (*xu_libcorkscrew_remote_unwind_t)(pid_t maintid, const void *map, xu_libcorkscrew_frame_t *bt,
                                                   size_t ignore_depth, size_t max_depth);
typedef void (*xu_libcorkscrew_remote_get_sym_t)(const void *map, const xu_libcorkscrew_frame_t *bt,
                                                 size_t frames, xu_libcorkscrew_symbol_t *symbols);
typedef void (*xu_libcorkscrew_free_sym_t)(xu_libcorkscrew_symbol_t *symbols, size_t frames);
typedef void (*xu_libcorkscrew_format_t)(unsigned frameNumber, const xu_libcorkscrew_frame_t *frame,
                                         const xu_libcorkscrew_symbol_t *symbol, char *buffer,
                                         size_t bufferSize);

static xu_libcorkscrew_local_load_map_t xu_libcorkscrew_local_load_map_ptr = NULL;
static xu_libcorkscrew_local_free_map_t xu_libcorkscrew_local_free_map_ptr = NULL;
static xu_libcorkscrew_local_unwind_t xu_libcorkscrew_local_unwind_ptr = NULL;
static xu_libcorkscrew_local_unwind_context_t xu_libcorkscrew_local_unwind_context_ptr = NULL;
static xu_libcorkscrew_local_get_sym_t xu_libcorkscrew_local_get_sym_ptr = NULL;
static xu_libcorkscrew_remote_load_map_t xu_libcorkscrew_remote_load_map_ptr = NULL;
static xu_libcorkscrew_remote_free_map_t xu_libcorkscrew_remote_free_map_ptr = NULL;
static xu_libcorkscrew_remote_unwind_t xu_libcorkscrew_remote_unwind_ptr = NULL;
static xu_libcorkscrew_remote_get_sym_t xu_libcorkscrew_remote_get_sym_ptr = NULL;
static xu_libcorkscrew_free_sym_t xu_libcorkscrew_free_sym_ptr = NULL;
static xu_libcorkscrew_format_t xu_libcorkscrew_format_ptr = NULL;

int xu_libcorkscrew_init(void) {
  int ret = -1;

  void *handle = dlopen(XU_LIBCORKSCREW_BASENAME, RTLD_NOW);
  if (NULL == handle) return -1;

  if (NULL == (xu_libcorkscrew_local_load_map_ptr =
                   (xu_libcorkscrew_local_load_map_t)dlsym(handle, XU_LIBCORKSCREW_LOCAL_LOAD_MAP)))
    goto end;
  if (NULL == (xu_libcorkscrew_local_free_map_ptr =
                   (xu_libcorkscrew_local_free_map_t)dlsym(handle, XU_LIBCORKSCREW_LOCAL_FREE_MAP)))
    goto end;
  if (NULL == (xu_libcorkscrew_local_unwind_ptr =
                   (xu_libcorkscrew_local_unwind_t)dlsym(handle, XU_LIBCORKSCREW_LOCAL_UNWIND)))
    goto end;
  if (NULL == (xu_libcorkscrew_local_unwind_context_ptr = (xu_libcorkscrew_local_unwind_context_t)dlsym(
                   handle, XU_LIBCORKSCREW_LOCAL_UNWIND_CONTEXT)))
    goto end;
  if (NULL == (xu_libcorkscrew_local_get_sym_ptr =
                   (xu_libcorkscrew_local_get_sym_t)dlsym(handle, XU_LIBCORKSCREW_LOCAL_GET_SYM)))
    goto end;
  if (NULL == (xu_libcorkscrew_remote_load_map_ptr =
                   (xu_libcorkscrew_remote_load_map_t)dlsym(handle, XU_LIBCORKSCREW_REMOTE_LOAD_MAP)))
    goto end;
  if (NULL == (xu_libcorkscrew_remote_free_map_ptr =
                   (xu_libcorkscrew_remote_free_map_t)dlsym(handle, XU_LIBCORKSCREW_REMOTE_FREE_MAP)))
    goto end;
  if (NULL == (xu_libcorkscrew_remote_unwind_ptr =
                   (xu_libcorkscrew_remote_unwind_t)dlsym(handle, XU_LIBCORKSCREW_REMOTE_UNWIND)))
    goto end;
  if (NULL == (xu_libcorkscrew_remote_get_sym_ptr =
                   (xu_libcorkscrew_remote_get_sym_t)dlsym(handle, XU_LIBCORKSCREW_REMOTE_GET_SYM)))
    goto end;
  if (NULL ==
      (xu_libcorkscrew_free_sym_ptr = (xu_libcorkscrew_free_sym_t)dlsym(handle, XU_LIBCORKSCREW_FREE_SYM)))
    goto end;
  if (NULL == (xu_libcorkscrew_format_ptr = (xu_libcorkscrew_format_t)dlsym(handle, XU_LIBCORKSCREW_FORMAT)))
    goto end;
  ret = 0;

end:
  // dlclose(handle);
  return ret;
}

void *xu_libcorkscrew_create(pid_t pid, pid_t maintid) {
  xu_libcorkscrew_t *self = (xu_libcorkscrew_t *)calloc(1, sizeof(xu_libcorkscrew_t));
  if (NULL == self) return NULL;

  self->is_remote = (pid == getpid() ? false : true);
  self->pid = pid;
  self->maintid = maintid;
  return (void *)self;
}

void xu_libcorkscrew_destroy(void *self) {
  xu_libcorkscrew_t *obj = (xu_libcorkscrew_t *)self;

  if (NULL != obj->map) {
    if (!obj->is_remote)
      xu_libcorkscrew_local_free_map_ptr(obj->map);
    else
      xu_libcorkscrew_remote_free_map_ptr(obj->map);
  }

  if (obj->frames_sz > 0) xu_libcorkscrew_free_sym_ptr(obj->symbols, obj->frames_sz);

  free(self);
}

size_t xu_libcorkscrew_unwind(void *self, size_t num_ignore_frames, void *context) {
  xu_libcorkscrew_t *obj = (xu_libcorkscrew_t *)self;

  if (!obj->is_remote)  // local
  {
    ssize_t frames_sz;
    if (NULL != context && obj->maintid == gettid())  // with context (in current thread)
    {
      obj->map = xu_libcorkscrew_local_load_map_ptr();
      if (NULL == obj->map) return 0;

      frames_sz = xu_libcorkscrew_local_unwind_context_ptr(NULL, context, obj->map, obj->frames,
                                                           num_ignore_frames, XU_LIBCORKSCREW_MAX_DEPTH);
    } else  // without context
    {
      frames_sz = xu_libcorkscrew_local_unwind_ptr(obj->maintid, obj->frames, num_ignore_frames,
                                                   XU_LIBCORKSCREW_MAX_DEPTH);
    }

    if (frames_sz <= 0) return 0;
    obj->frames_sz = (size_t)frames_sz;

    xu_libcorkscrew_local_get_sym_ptr(obj->frames, obj->frames_sz, obj->symbols);
  } else  // remote
  {
    obj->map = xu_libcorkscrew_remote_load_map_ptr(obj->pid);
    if (NULL == obj->map) return 0;

    ssize_t frames_sz = xu_libcorkscrew_remote_unwind_ptr(obj->maintid, obj->map, obj->frames, num_ignore_frames,
                                                          XU_LIBCORKSCREW_MAX_DEPTH);

    if (frames_sz <= 0) return 0;
    obj->frames_sz = (size_t)frames_sz;

    xu_libcorkscrew_remote_get_sym_ptr(obj->map, obj->frames, obj->frames_sz, obj->symbols);
  }

  return obj->frames_sz;
}

void xu_libcorkscrew_format(void *self, size_t frame_num, char *buf, size_t buf_cap) {
  xu_libcorkscrew_t *obj = (xu_libcorkscrew_t *)self;

  xu_libcorkscrew_format_ptr((unsigned)frame_num, &obj->frames[frame_num], &obj->symbols[frame_num], buf,
                             buf_cap);
}

#else
typedef int make_iso_happy;
#endif
