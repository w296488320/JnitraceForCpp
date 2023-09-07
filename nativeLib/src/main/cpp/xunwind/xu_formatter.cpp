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

#include "xu_formatter.h"

#include <ctype.h>
#include <dlfcn.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "xdl.h"
#include "xu_log.h"

#if defined(__LP64__)
#define XU_FORMATTER_ADDR "%016" PRIxPTR
#else
#define XU_FORMATTER_ADDR "%08" PRIxPTR
#endif

#define XU_FORMATTER_PREFIX "%s#%02zu pc " XU_FORMATTER_ADDR "  "

static int xu_formatter_maps_addr(uintptr_t pc, xdl_info_t *info, char *buf, size_t buf_len) {
  memset(info, 0, sizeof(xdl_info_t));

  FILE *fp = fopen("/proc/self/maps", "r");
  if (NULL == fp) return 0;

  int r = 0;
  while (fgets(buf, (int)buf_len, fp)) {
    uintptr_t lo, hi;
    int pos;
    if (2 == sscanf(buf, "%" SCNxPTR "-%" SCNxPTR " %*4s %*lx %*x:%*x %*d%n", &lo, &hi, &pos) && lo <= pc &&
        pc < hi) {
      while (isspace(buf[pos]) && pos < (int)(buf_len - 1)) pos++;
      if (pos >= (int)(buf_len - 1)) break;
      if (0 == strlen(buf + pos)) break;

      info->dli_fbase = (void *)lo;
      info->dli_fname = buf + pos;
      r = 1;
      break;
    }
  }

  fclose(fp);
  return r;
}

void xu_formatter_print(uintptr_t *frames, size_t frames_sz, const char *prefix, xu_printer_t *printer) {
  if (NULL == frames || 0 == frames_sz) return;

  if (NULL == prefix) prefix = "";

  void *cache = NULL;
  xdl_info_t info;
  for (size_t i = 0; i < frames_sz; i++) {
    memset(&info, 0, sizeof(xdl_info_t));
    int r = 0;

    if (0 != frames[i]) {
      // find info from linker
      r = xdl_addr((void *)(frames[i]), &info, &cache);

      // find info from maps
      char buf[512];
      if (0 == r || (uintptr_t)info.dli_fbase > frames[i])
        r = xu_formatter_maps_addr(frames[i], &info, buf, sizeof(buf));
    }

    // do print
    if (0 == r || (uintptr_t)info.dli_fbase > frames[i])
      xu_printer_append_format(printer, XU_FORMATTER_PREFIX "<unknown>\n", prefix, i, frames[i]);
    else if (NULL == info.dli_fname || '\0' == info.dli_fname[0])
      xu_printer_append_format(printer, XU_FORMATTER_PREFIX "<anonymous:" XU_FORMATTER_ADDR ">\n", prefix, i,
                               frames[i] - (uintptr_t)info.dli_fbase, (uintptr_t)info.dli_fbase);
    else if (NULL == info.dli_sname || '\0' == info.dli_sname[0])
      xu_printer_append_format(printer, XU_FORMATTER_PREFIX "%s\n", prefix, i,
                               frames[i] - (uintptr_t)info.dli_fbase, info.dli_fname);
    else if (0 == (uintptr_t)info.dli_saddr || (uintptr_t)info.dli_saddr > frames[i])
      xu_printer_append_format(printer, XU_FORMATTER_PREFIX "%s (%s)\n", prefix, i,
                               frames[i] - (uintptr_t)info.dli_fbase, info.dli_fname, info.dli_sname);
    else
      xu_printer_append_format(printer, XU_FORMATTER_PREFIX "%s (%s+%" PRIuPTR ")\n", prefix, i,
                               frames[i] - (uintptr_t)info.dli_fbase, info.dli_fname, info.dli_sname,
                               frames[i] - (uintptr_t)info.dli_saddr);
  }
  xdl_addr_clean(&cache);
}
