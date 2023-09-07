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

#include "xu_cfi.h"

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
#include "xu_libbacktrace.h"
#include "xu_libcorkscrew.h"
#include "xu_libcxx.h"
#include "xu_log.h"
#include "xu_printer.h"
#include "xu_util.h"
#include "xunwind.h"

#define XU_CFI_MAX_FRAME             256
#define XU_CFI_FORMAT_PROCESS_HEADER "----- pid %d -----\nCmd line: %s"
#define XU_CFI_FORMAT_THREAD_HEADER  "\n\"%s\" sysTid=%d"
#define XU_CFI_FORMAT_PROCESS_FOOTER "\n----- end %d -----"

int xu_cfi_init(void) {
  static int init_status = -1;  // -1: not init  0: init OK  1: init failed
  if (init_status >= 0) return init_status;

  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&lock);

  if (init_status < 0) {
#ifdef XU_CONFIG_CFI_WITH_LIBCORKSCREW
    if (xu_util_get_api_level() < __ANDROID_API_L__)
      init_status = (0 == xu_libcorkscrew_init() ? 0 : 1);
    else
#endif
      init_status = ((0 == xu_libcxx_init() && 0 == xu_libbacktrace_init()) ? 0 : 1);
  }

  pthread_mutex_unlock(&lock);

  return init_status;
}

#ifdef XU_CONFIG_CFI_WITH_LIBCORKSCREW
static void xu_cfi_thread_libcorkscrew(pid_t pid, pid_t maintid, void *context, size_t ignore_depth,
                                       const char *prefix, xu_printer_t *printer) {
  // create corkscrew object
  void *cs = xu_libcorkscrew_create(pid, maintid);
  if (NULL == cs) return;

  // unwind
  size_t frames_sz = xu_libcorkscrew_unwind(cs, ignore_depth, context);
  if (0 == frames_sz) goto end;

  char frame_line[800];
  for (size_t i = 0; i < frames_sz; i++) {
    // format the frame line
    frame_line[0] = '\0';
    xu_libcorkscrew_format(cs, i, frame_line, sizeof(frame_line));

    // check line length
    if (strlen(frame_line) < 5) break;

    // dump
    xu_printer_append_format(printer, "%s%s", NULL != prefix ? prefix : "", frame_line);
  }

end:
  // destroy the corkscrew object
  xu_libcorkscrew_destroy(cs);
}
#endif

static void xu_cfi_thread_libbacktrace(pid_t pid, pid_t tid, void *context, size_t ignore_depth,
                                       bool is_remote, const char *prefix, xu_printer_t *printer) {
  // create backtrace object
  void *bt = xu_libbacktrace_create(pid, tid);
  if (NULL == bt) return;

  if (is_remote) {
    // unwind remote process
    if (!xu_libbacktrace_remote_unwind(bt, ignore_depth, context)) goto end;
  } else {
    // unwind current process
    if (!xu_libbacktrace_local_unwind(bt, ignore_depth, context)) goto end;
  }

  for (size_t i = 0; i < XU_CFI_MAX_FRAME; i++) {
    // format the frame line
    xu_libcxx_string_t frame_line_str = xu_libbacktrace_format(bt, i);

    // c++ string to c string
    const char *frame_line_c_str = xu_libcxx_string_c_str(&frame_line_str);

    // check line length
    if (strlen(frame_line_c_str) < 5) {
      xu_libcxx_string_dtor_d1(&frame_line_str);
      break;
    }

    // dump
    xu_printer_append_format(printer, "%s%s", NULL != prefix ? prefix : "", frame_line_c_str);

    // free the c++ string object on stack
    xu_libcxx_string_dtor_d1(&frame_line_str);
  }

end:
  // free the backtrace object on stack
  xu_libbacktrace_dtor_d1(bt);
}

static void xu_cfi_thread(pid_t pid, pid_t tid, void *context, size_t ignore_depth, bool is_remote,
                          const char *prefix, xu_printer_t *printer) {
#ifdef XU_CONFIG_CFI_WITH_LIBCORKSCREW
  if (xu_util_get_api_level() < __ANDROID_API_L__)
    xu_cfi_thread_libcorkscrew(pid, maintid, context, ignore_depth, prefix, printer);
  else
#endif
    xu_cfi_thread_libbacktrace(pid, tid, context, ignore_depth, is_remote, prefix, printer);
}

void xu_cfi_print(pid_t pid, pid_t tid, void *context, const char *prefix, xu_printer_t *printer) {
  pid_t cur_pid = getpid();
  if (XUNWIND_CURRENT_PROCESS == pid) pid = cur_pid;

  bool is_remote = (pid != cur_pid);

  if (tid == XUNWIND_ALL_THREADS) {
    // get all thread IDs
    size_t tids_cnt = 0;
    pid_t *tids = xu_util_get_thread_ids(pid, &tids_cnt);
    if (NULL == tids) return;

    // get & print PID + process-name
    char pname[128];
    xu_util_get_process_name(pid, pname, sizeof(pname));
    xu_printer_append_format(printer, XU_CFI_FORMAT_PROCESS_HEADER, pid, pname);

    // ptrace attach all threads in remote process
    if (is_remote) {
      for (size_t i = 0; i < tids_cnt; i++) {
        if (0 != xu_util_ptrace_attach(tids[i])) tids[i] = -1;
      }
    }

    // for each threads
    for (size_t i = 0; i < tids_cnt; i++) {
      if (tids[i] < 0) continue;

      // get & print TID + thread-name
      char thread_name[128];
      xu_util_get_thread_name(pid, tids[i], thread_name, sizeof(thread_name));
      xu_printer_append_format(printer, XU_CFI_FORMAT_THREAD_HEADER, thread_name, tids[i]);

      // print thread's backtrace
      xu_cfi_thread(pid, tids[i], NULL, 0, is_remote, prefix, printer);
    }

    // ptrace detach all threads in remote process
    if (is_remote) {
      for (size_t i = 0; i < tids_cnt; i++) {
        if (tids[i] < 0) continue;
        xu_util_ptrace_detach(tids[i]);
      }
    }

    // print footer
    xu_printer_append_format(printer, XU_CFI_FORMAT_PROCESS_FOOTER, pid);
  } else {
    // ptrace attach target thread in remote process
    if (is_remote) {
      if (0 != xu_util_ptrace_attach(tid)) return;
    }

    // print thread's backtrace
    if (XUNWIND_CURRENT_THREAD == tid) tid = gettid();
    xu_cfi_thread(pid, tid, context, 0, is_remote, prefix, printer);

    // ptrace detach target thread in remote process
    if (is_remote) {
      xu_util_ptrace_detach(tid);
    }
  }
}
