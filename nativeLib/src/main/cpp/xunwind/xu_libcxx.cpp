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

// https://android.googlesource.com/platform/external/libcxx/+/refs/heads/master/include/string
// https://en.cppreference.com/w/cpp/string/basic_string

#include "xu_libcxx.h"

#include <stddef.h>

#include "xdl.h"

#ifndef __LP64__
#define XU_LIBCXX_PATHNAME    "/system/lib/libc++.so"
#define XU_LIBCXX_STRING_COPY "_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE4copyEPcjj"
#define XU_LIBCXX_STRING_AT   "_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE2atEj"
#else
#define XU_LIBCXX_PATHNAME    "/system/lib64/libc++.so"
#define XU_LIBCXX_STRING_COPY "_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE4copyEPcmm"
#define XU_LIBCXX_STRING_AT   "_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE2atEm"
#endif

#define XU_LIBCXX_STRING_DTOR_D1 "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev"

typedef size_t (*xu_libcxx_string_copy_t)(void *self, char *s, size_t len, size_t pos);
typedef const char *(*xu_libcxx_string_at_t)(void *self, size_t pos);
typedef void (*xu_libcxx_string_dtor_d1_t)(void *self);

static xu_libcxx_string_copy_t xu_libcxx_string_copy_ptr = NULL;
static xu_libcxx_string_at_t xu_libcxx_string_at_ptr = NULL;
static xu_libcxx_string_dtor_d1_t xu_libcxx_string_dtor_d1_ptr = NULL;

int xu_libcxx_init(void) {
  int ret = -1;

  void *handle = xdl_open(XU_LIBCXX_PATHNAME, XDL_DEFAULT);
  if (NULL == handle) return -1;

  if (NULL ==
      (xu_libcxx_string_copy_ptr = (xu_libcxx_string_copy_t)xdl_sym(handle, XU_LIBCXX_STRING_COPY, NULL)))
    goto end;
  if (NULL == (xu_libcxx_string_at_ptr = (xu_libcxx_string_at_t)xdl_sym(handle, XU_LIBCXX_STRING_AT, NULL)))
    goto end;
  if (NULL == (xu_libcxx_string_dtor_d1_ptr =
                   (xu_libcxx_string_dtor_d1_t)xdl_sym(handle, XU_LIBCXX_STRING_DTOR_D1, NULL)))
    goto end;

  ret = 0;

end:
  xdl_close(handle);
  return ret;
}

const char *xu_libcxx_string_c_str(xu_libcxx_string_t *self) {
  char buf[2] = {'\0', '\0'};
  size_t len = xu_libcxx_string_copy_ptr((void *)self, buf, 1, 0);
  return (len > 0) ? xu_libcxx_string_at_ptr(self, 0) : "";
}

void xu_libcxx_string_dtor_d1(xu_libcxx_string_t *self) {
  xu_libcxx_string_dtor_d1_ptr((void *)self);
}
