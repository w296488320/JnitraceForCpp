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

#include "xunwind.h"

#include <android/log.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "xdl.h"
#include "xu_cfi.h"
#include "xu_eh.h"
#include "xu_formatter.h"
#include "xu_fp.h"
#include "xu_log.h"
#include "xu_printer.h"
#include "xu_util.h"

void xunwind_cfi_log(pid_t pid, pid_t tid, void *context, const char *logtag, android_LogPriority priority,
                     const char *prefix) {
  if (priority < ANDROID_LOG_VERBOSE || ANDROID_LOG_FATAL < priority) return;

  if (0 != xu_cfi_init()) return;

  xu_printer_t printer;
  xu_printer_init_log(&printer, logtag, priority);

  xu_cfi_print(pid, tid, context, prefix, &printer);
}

void xunwind_cfi_dump(pid_t pid, pid_t tid, void *context, int fd, const char *prefix) {
  if (fd < 0) return;

  if (0 != xu_cfi_init()) return;

  xu_printer_t printer;
  xu_printer_init_dump(&printer, fd);

  xu_cfi_print(pid, tid, context, prefix, &printer);
}

char *xunwind_cfi_get(pid_t pid, pid_t tid, void *context, const char *prefix) {
  if (0 != xu_cfi_init()) return NULL;

  xu_printer_t printer;
  xu_printer_init_get(&printer);

  xu_cfi_print(pid, tid, context, prefix, &printer);
  return xu_printer_get(&printer);
}

size_t xunwind_fp_unwind(uintptr_t *frames, size_t frames_cap, void *context) {
  if (0 != xu_fp_init()) return 0;

  return xu_fp_unwind(frames, frames_cap, context);
}

size_t xunwind_eh_unwind(uintptr_t *frames, size_t frames_cap, void *context) {
  return xu_eh_unwind(frames, frames_cap, context);
}

void xunwind_frames_log(uintptr_t *frames, size_t frames_sz, const char *logtag, android_LogPriority priority,
                        const char *prefix) {
  if (priority < ANDROID_LOG_VERBOSE || ANDROID_LOG_FATAL < priority) return;

  xu_printer_t printer;
  xu_printer_init_log(&printer, logtag, priority);

  xu_formatter_print(frames, frames_sz, prefix, &printer);
}

void xunwind_frames_dump(uintptr_t *frames, size_t frames_sz, int fd, const char *prefix) {
  if (fd < 0) return;

  xu_printer_t printer;
  xu_printer_init_dump(&printer, fd);

  xu_formatter_print(frames, frames_sz, prefix, &printer);
}

char *xunwind_frames_get(uintptr_t *frames, size_t frames_sz, const char *prefix) {
  xu_printer_t printer;
  xu_printer_init_get(&printer);

  xu_formatter_print(frames, frames_sz, prefix, &printer);
  return xu_printer_get(&printer);
}
