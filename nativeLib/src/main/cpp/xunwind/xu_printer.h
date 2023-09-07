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

// Created by caikelun on 2020-11-29.

#ifndef IO_GITHUB_HEXHACKING_XUNWIND_PRINTER
#define IO_GITHUB_HEXHACKING_XUNWIND_PRINTER

#include <android/log.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XU_PRINTER_TYPE_LOG  0
#define XU_PRINTER_TYPE_DUMP 1
#define XU_PRINTER_TYPE_GET  2

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct {
  int type;

  union {
    struct {
      const char *logtag;
      int priority;
    } log;

    struct {
      int fd;
    } dump;

    struct {
      char *buf;
      size_t buf_cap;
      size_t buf_sz;
    } get;
  } data;
} xu_printer_t;
#pragma clang diagnostic pop

void xu_printer_init_log(xu_printer_t *self, const char *logtag, android_LogPriority priority);
void xu_printer_init_dump(xu_printer_t *self, int fd);
void xu_printer_init_get(xu_printer_t *self);

void xu_printer_append_string(xu_printer_t *self, const char *str);
void xu_printer_append_format(xu_printer_t *self, const char *format, ...);

char *xu_printer_get(xu_printer_t *self);

#ifdef __cplusplus
}
#endif

#endif
