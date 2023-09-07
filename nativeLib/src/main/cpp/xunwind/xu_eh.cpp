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

#include "xu_eh.h"

#include <stddef.h>

#include "xu_config.h"

#ifdef XU_CONFIG_EH_ENABLE

#include <stdbool.h>
#include <string.h>
#include <ucontext.h>
#include <unwind.h>

typedef struct {
  uintptr_t *frames;
  size_t frames_cap;
  size_t frames_sz;
  uintptr_t prev_sp;
  ucontext_t *uc;
} xu_eh_info_t;

static _Unwind_Reason_Code xu_eh_unwind_cb(struct _Unwind_Context *unw_ctx, void *arg) {
  uintptr_t pc = _Unwind_GetIP(unw_ctx);
  uintptr_t sp = _Unwind_GetCFA(unw_ctx);

  xu_eh_info_t *info = (xu_eh_info_t *)arg;
  if (NULL != info->uc) {
#if defined(__arm__)
    uintptr_t ctx_pc = (uintptr_t)info->uc->uc_mcontext.arm_pc;
    uintptr_t ctx_lr = (uintptr_t)info->uc->uc_mcontext.arm_lr;
#elif defined(__aarch64__)
    uintptr_t ctx_pc = (uintptr_t)info->uc->uc_mcontext.pc;
    uintptr_t ctx_lr = (uintptr_t)info->uc->uc_mcontext.regs[30];
#elif defined(__i386__)
    uintptr_t ctx_pc = (uintptr_t)info->uc->uc_mcontext.gregs[REG_EIP];
    uintptr_t ctx_lr = 0;
#elif defined(__x86_64__)
    uintptr_t ctx_pc = (uintptr_t)info->uc->uc_mcontext.gregs[REG_RIP];
    uintptr_t ctx_lr = 0;
#endif

    if ((ctx_pc >= sizeof(uintptr_t) && pc <= ctx_pc + sizeof(uintptr_t) &&
         pc >= ctx_pc - sizeof(uintptr_t)) ||
        (ctx_lr >= sizeof(uintptr_t) && pc <= ctx_lr + sizeof(uintptr_t) && pc >= ctx_lr - sizeof(uintptr_t)))
      info->uc = NULL;  // we found the context PC
    else
      return _URC_NO_REASON;  // skip & continue
  }

  if (info->frames_sz > 0 && pc == info->frames[info->frames_sz - 1] && sp == info->prev_sp)
    return _URC_END_OF_STACK;  // stop

  info->frames[info->frames_sz++] = pc;

  if (info->frames_sz >= info->frames_cap) return _URC_END_OF_STACK;  // stop

  info->prev_sp = sp;
  return _URC_NO_REASON;  // continue
}

size_t xu_eh_unwind(uintptr_t *frames, size_t frames_cap, void *context) {
  if (NULL == frames || 0 == frames_cap) return 0;

  xu_eh_info_t info;
  info.frames = frames;
  info.frames_cap = frames_cap;
  info.frames_sz = 0;
  info.prev_sp = 0;
  info.uc = (ucontext_t *)context;

  _Unwind_Backtrace(xu_eh_unwind_cb, &info);

  return info.frames_sz;
}

#else

size_t xu_eh_unwind(uintptr_t* frames, size_t frames_cap, void* context) {
  (void)frames, (void)frames_cap, (void)context;
  return 0;
}

#endif
