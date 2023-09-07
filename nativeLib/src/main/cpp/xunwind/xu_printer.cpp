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

#include "xu_printer.h"

#include <android/log.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "xu_util.h"

#define XU_PRINTER_DEFAULT_LOGTAG "Zhenxi"

void xu_printer_init_log(xu_printer_t *self, const char *logtag, android_LogPriority priority) {
  self->type = XU_PRINTER_TYPE_LOG;
  self->data.log.priority = (int)priority;
  self->data.log.logtag = (NULL == logtag ? XU_PRINTER_DEFAULT_LOGTAG : logtag);
}

void xu_printer_init_dump(xu_printer_t *self, int fd) {
  self->type = XU_PRINTER_TYPE_DUMP;
  self->data.dump.fd = fd;
}

void xu_printer_init_get(xu_printer_t *self) {
  self->type = XU_PRINTER_TYPE_GET;
  self->data.get.buf = NULL;
  self->data.get.buf_cap = 0;
  self->data.get.buf_sz = 0;
}

static int xu_printer_string_append_to_buf(xu_printer_t *self, const char *str) {
  char **buf = &(self->data.get.buf);
  size_t *buf_cap = &(self->data.get.buf_cap);
  size_t *buf_sz = &(self->data.get.buf_sz);

  if (NULL == str) return 0;

  size_t str_len = strlen(str);
  if (0 == str_len) return 0;

  if (*buf_cap - *buf_sz < str_len + 1) {
    size_t new_buf_cap = ((*buf_cap + str_len) & (~(0x400u - 1))) + 0x400u;
    char *new_buf = (char *)realloc(*buf, new_buf_cap);
    if (NULL == new_buf) return -1;
    *buf = new_buf;
    *buf_cap = new_buf_cap;
  }

  memcpy(*buf + *buf_sz, str, str_len);
  *(*buf + *buf_sz + str_len) = '\0';
  *buf_sz += str_len;
  return 0;
}

void xu_printer_append_string(xu_printer_t *self, const char *str) {
  if (XU_PRINTER_TYPE_LOG == self->type) {
    __android_log_print(self->data.log.priority, self->data.log.logtag, "%s", str);
  } else if (XU_PRINTER_TYPE_DUMP == self->type) {
    size_t len = strlen(str);
    if (len > 0) {
      xu_util_write(self->data.dump.fd, str, len);
      if ('\n' != str[len - 1]) xu_util_write(self->data.dump.fd, "\n", 1);
    }
  } else if (XU_PRINTER_TYPE_GET == self->type) {
    size_t len = strlen(str);
    if (len > 0) {
      xu_printer_string_append_to_buf(self, str);
      if ('\n' != str[len - 1]) xu_printer_string_append_to_buf(self, "\n");
    }
  }
}

void xu_printer_append_format(xu_printer_t *self, const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  char tmpbuf[1024];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  vsnprintf(tmpbuf, sizeof(tmpbuf), format, ap);
#pragma clang diagnostic pop

  va_end(ap);

  xu_printer_append_string(self, tmpbuf);
}

char *xu_printer_get(xu_printer_t *self) {
  if (XU_PRINTER_TYPE_GET != self->type) return NULL;

  return self->data.get.buf;
}
