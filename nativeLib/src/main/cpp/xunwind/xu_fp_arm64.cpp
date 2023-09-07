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

#ifdef __aarch64__

#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <ucontext.h>
#include <unistd.h>

#include "xdl.h"
#include "xu_config.h"
#include "xu_fp.h"
#include "xu_log.h"

#define XU_FP_VDSO_PATHNAME  "[vdso]"
#define XU_FP_VDSO_SIGRETURN "__kernel_rt_sigreturn"

static uintptr_t xu_fp_sigreturn_addr = 0;
static uintptr_t xu_fp_main_thread_stack_low = 0;
static uintptr_t xu_fp_main_thread_stack_high = 0;

typedef struct {
  uintptr_t fp;
  uintptr_t lr;
} xu_fp_frame_record_t;

static int xu_fp_set_sigreturn_addr(void) {
  void *handle = xdl_open(XU_FP_VDSO_PATHNAME, XDL_DEFAULT);
  if (NULL == handle) return -1;

  void *addr = xdl_sym(handle, XU_FP_VDSO_SIGRETURN, NULL);
  xdl_close(handle);
  if (NULL == addr) return -1;

  xu_fp_sigreturn_addr = (uintptr_t)addr;
  return 0;
}

static int xu_fp_set_main_thread_stack_range(void) {
  char line[512];
  FILE *fp;

  // get stack size limit
  struct rlimit stack_limit;
  if (0 != getrlimit(RLIMIT_STACK, &stack_limit)) return -1;
  if (RLIM_INFINITY == stack_limit.rlim_cur) stack_limit.rlim_cur = 8 * 1024 * 1024;

  // get startstack
  uintptr_t start_stack = 0;
  if (NULL != (fp = fopen("/proc/self/stat", "re"))) {
    if (fgets(line, sizeof(line), fp)) {
      const char *end_of_comm = strrchr((const char *)line, (int)')');
      if (1 != sscanf(end_of_comm + 1,
                      " %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %*u "
                      "%*d %*u %*u %*u %" SCNuPTR,
                      &start_stack))
        start_stack = 0;
    }
    fclose(fp);
  }
  if (0 == start_stack) return -1;

  // get main thread stack high
  uintptr_t stack_high = 0;
  if (NULL != (fp = fopen("/proc/self/maps", "r"))) {
    while (fgets(line, sizeof(line), fp)) {
      uintptr_t lo, hi;
      if (2 == sscanf(line, "%" SCNxPTR "-%" SCNxPTR " ", &lo, &hi) && lo <= start_stack &&
          start_stack <= hi) {
        stack_high = hi;
        break;
      }
    }
    fclose(fp);
  }
  if (0 == stack_high) return -1;
  if (stack_high < stack_limit.rlim_cur) return -1;

  xu_fp_main_thread_stack_low = stack_high - stack_limit.rlim_cur;
  xu_fp_main_thread_stack_high = stack_high;
  return 0;
}

int xu_fp_init(void) {
  static int init_status = -1;  // -1: not init  0: init OK  1: init failed
  if (init_status >= 0) return init_status;

  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&lock);

  if (init_status < 0) {
    init_status = 1;

    if (0 != xu_fp_set_sigreturn_addr()) goto end;
    XU_LOG("xu_fp_init: sigreturn %p", (void *)xu_fp_sigreturn_addr);

    if (0 != xu_fp_set_main_thread_stack_range()) goto end;
    XU_LOG("xu_fp_init: main thread stack range %p - %p, sz: %zx", (void *)xu_fp_main_thread_stack_low,
           (void *)xu_fp_main_thread_stack_high,
           (size_t)(xu_fp_main_thread_stack_high - xu_fp_main_thread_stack_low));

    init_status = 0;  // OK
  }

end:
  pthread_mutex_unlock(&lock);
  return init_status;
}

static void xu_fp_get_thread_stack(uintptr_t *low, uintptr_t *high) {
  if (getpid() == gettid()) {
    *low = xu_fp_main_thread_stack_low;
    *high = xu_fp_main_thread_stack_high;
  } else {
    pthread_t thrd = pthread_self();

    // thread attributes
    pthread_attr_t attr;
    if (0 != pthread_getattr_np(thrd, &attr)) return;

    // thread stack
    uintptr_t stack_low, stack_high;
    size_t stack_sz;
    if (0 != pthread_attr_getstack(&attr, (void **)(&stack_low), &stack_sz)) return;
    stack_high = (uintptr_t)stack_low + (uintptr_t)stack_sz;

    // thread stack guard
    uintptr_t guard_sz = 0;
    if (0 != pthread_attr_getguardsize(&attr, &guard_sz)) guard_sz = 0x1000;

    pthread_attr_destroy(&attr);

    *low = (stack_low + guard_sz);
    *high = stack_high;
  }
}

static inline bool xu_fp_is_sigreturn(uintptr_t addr) {
  return xu_fp_sigreturn_addr == addr;
  //    if(0 != xu_fp_sigreturn_addr)
  //    {
  //        return xu_fp_sigreturn_addr == addr;
  //    }
  //    else
  //    {
  //        // __kernel_rt_sigreturn in vDSO:
  //        // 0xd2801168   mov x8, #0x8b
  //        // 0xd4000001   svc #0x0
  //        return (0xd4000001d2801168ULL == *((uint64_t *)addr)) ? true : false;
  //    }
}

#ifdef XU_CONFIG_FP_CHECK_SYSCALL

typedef struct {
  siginfo_t info;
  ucontext_t uc;
  xu_fp_frame_record_t record;
} xu_fp_sigframe_t;

static inline bool xu_fp_is_syscall(uintptr_t addr) {
  // syscall in libc.so:
  // 0xd280???8 	mov	x8, #0x???
  // 0xd4000001 	svc	#0x0
  return (0xd4000001d2800008ULL == (*((uint64_t *)addr) & 0xffffffffffff000fULL)) ? true : false;
}

static xu_fp_sigframe_t *xu_fp_find_sigframe_addr(uintptr_t stack_low, uintptr_t stack_high, size_t stack_sz,
                                                  uintptr_t begin) {
  if (begin >= stack_high) return 0;
  for (; stack_high - begin >= sizeof(xu_fp_sigframe_t); begin += sizeof(uintptr_t)) {
    xu_fp_sigframe_t *sigframe = (xu_fp_sigframe_t *)begin;
    if (0 == sigframe->info.si_signo || sigframe->info.si_signo > 63) continue;
    if ((void *)stack_low != sigframe->uc.uc_stack.ss_sp) continue;
    if (stack_sz != sigframe->uc.uc_stack.ss_size) continue;
    return sigframe;
  }
  return NULL;
}

#endif

static bool xu_fp_append_frame(uintptr_t *frames, size_t frames_cap, size_t *frames_sz, uintptr_t addr) {
  if (*frames_sz >= frames_cap) return false;

  frames[*frames_sz] = addr & 0x7FFFFFFFFFULL;  // clear PAC bits
  *frames_sz += 1;
  XU_LOG("xu_fp_unwind: append frame #%02zu %p", *frames_sz - 1, (void *)frames[*frames_sz - 1]);

  return *frames_sz >= frames_cap ? false : true;
}

size_t xu_fp_unwind(uintptr_t *frames, size_t frames_cap, void *context) {
  if (NULL == frames || 0 == frames_cap) return 0;

  size_t frames_sz = 0;

  // get current FP
  uintptr_t cur_fp;
  if (NULL == context) {
    // from current FP
    cur_fp = (uintptr_t)(__builtin_frame_address(0));
  } else {
    // from FP in signal context
    mcontext_t *mc = &(((ucontext_t *)context)->uc_mcontext);
    cur_fp = mc->regs[29];

    // append frame for "PC in signal context"
    if (!xu_fp_append_frame(frames, frames_cap, &frames_sz, mc->pc)) return frames_sz;

#ifdef XU_CONFIG_FP_CHECK_SYSCALL
    // append frame for previous-PC if "PC in signal context" is a syscall()
    if (xu_fp_is_syscall(mc->pc - 8))
      if (!xu_fp_append_frame(frames, frames_cap, &frames_sz, mc->regs[30] < 4 ? 0 : mc->regs[30] - 4))
        return frames_sz;
#endif
  }

  // current signal-stack info
  bool unwind_on_signal_stack = false;
  stack_t ss;
  uintptr_t signal_stack_low = 0, signal_stack_high = 0;
  size_t signal_stack_sz = 0;
  if (0 == sigaltstack(NULL, &ss) && ss.ss_flags & SS_ONSTACK) {
    unwind_on_signal_stack = true;
    signal_stack_low = (uintptr_t)ss.ss_sp;
    signal_stack_high = (uintptr_t)ss.ss_sp + (uintptr_t)ss.ss_size;
    signal_stack_sz = (size_t)ss.ss_size;
    XU_LOG("xu_fp_unwind: signal stack %p - %p, sz: %zx", (void *)signal_stack_low, (void *)signal_stack_high,
           signal_stack_sz);
  }

  // current thread-stack info
  uintptr_t thread_stack_low = 0, thread_stack_high = 0;
  xu_fp_get_thread_stack(&thread_stack_low, &thread_stack_high);
  XU_LOG("xu_fp_unwind: thread stack %p - %p, sz: %zx", (void *)thread_stack_low, (void *)thread_stack_high,
         thread_stack_high - thread_stack_low);

  // current stack
  uintptr_t cur_stack_low;
  uintptr_t cur_stack_high;
  if (unwind_on_signal_stack) {
    cur_stack_low = signal_stack_low;
    cur_stack_high = signal_stack_high;
  } else {
    cur_stack_low = thread_stack_low;
    cur_stack_high = thread_stack_high;
  }

  // walk the stack
  while (1) {
    uintptr_t prev_fp;
    xu_fp_frame_record_t *record = (xu_fp_frame_record_t *)cur_fp;

    if (unwind_on_signal_stack && xu_fp_is_sigreturn(record->lr)) {
      // handle sigreturn()
      XU_LOG("xu_fp_unwind: handle sigreturn at %p", (void *)(record->lr));

      if (!xu_fp_append_frame(frames, frames_cap, &frames_sz, record->lr)) return frames_sz;
      prev_fp = record->fp;
      if (prev_fp & 0xfu) break;

#ifdef XU_CONFIG_FP_CHECK_SYSCALL
      // append frame by syscall()
      xu_fp_sigframe_t *sigframe =
          xu_fp_find_sigframe_addr(signal_stack_low, signal_stack_high, signal_stack_sz, cur_fp);
      if (NULL != sigframe) {
        uintptr_t sigframe_pc = sigframe->uc.uc_mcontext.pc;
        if (xu_fp_is_syscall(sigframe_pc - 8)) {
          if (!xu_fp_append_frame(frames, frames_cap, &frames_sz, sigframe_pc < 4 ? 0 : sigframe_pc - 4))
            return frames_sz;
          prev_fp = record->fp;
          if (prev_fp & 0xfu) break;
        }
      }
#endif
    } else {
      // handle situations other than sigreturn()
      if (!xu_fp_append_frame(frames, frames_cap, &frames_sz, record->lr < 4 ? 0 : record->lr - 4))
        return frames_sz;
      prev_fp = record->fp;
      if (prev_fp & 0xfu) break;
    }

    if (unwind_on_signal_stack && (prev_fp < signal_stack_low || signal_stack_high <= prev_fp)) {
      // change current stack from signal-stack to thread-stack, do this at most once
      unwind_on_signal_stack = false;
      cur_stack_low = thread_stack_low;
      cur_stack_high = thread_stack_high;
      XU_LOG("xu_fp_unwind: signal-stack -> thread-stack %p - %p, sz: %zx", (void *)thread_stack_low,
             (void *)thread_stack_high, thread_stack_high - thread_stack_low);
    } else {
      // ensure to walk FP from low address to high address
      if (prev_fp < cur_fp + sizeof(xu_fp_frame_record_t)) break;
    }

    // ensure FP is within the stack range
    if (prev_fp < cur_stack_low || cur_stack_high <= prev_fp) break;

    // walk to the previous FP
    cur_fp = prev_fp;
  }

  return frames_sz;
}

#else
typedef int make_iso_happy;
#endif
