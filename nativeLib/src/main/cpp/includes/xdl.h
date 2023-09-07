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

// Created by caikelun on 2020-10-04.

//
// xDL version: 2.0.0
//
// xDL is an enhanced implementation of the Android DL series functions.
// For more information, documentation, and the latest version please check:
// https://github.com/hexhacking/xDL
//

#ifndef IO_GITHUB_HEXHACKING_XDL
#define IO_GITHUB_HEXHACKING_XDL

#include <dlfcn.h>
#include <link.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
//https://github.com/hexhacking/xDL/blob/master/README.zh-CN.md
typedef struct {
  // same as Dl_info:
  const char *dli_fname;  // Pathname of shared object that contains address.
  void *dli_fbase;        // Address at which shared object is loaded.
  const char *dli_sname;  // Name of nearest symbol with address lower than addr.
  void *dli_saddr;        // Exact address of symbol named in dli_sname.
  // added by xDL:
  size_t dli_ssize;             // Symbol size of nearest symbol with address lower than addr.
  const ElfW(Phdr) *dlpi_phdr;  // Pointer to array of ELF program headers for this object.
  size_t dlpi_phnum;            // Number of items in dlpi_phdr.
} xdl_info_t;

//
// Default value for flags in both xdl_open() and xdl_iterate_phdr().
//
#define XDL_DEFAULT 0x00

//
// Enhanced dlopen() / dlclose() / dlsym().
//
#define XDL_TRY_FORCE_LOAD    0x01
#define XDL_ALWAYS_FORCE_LOAD 0x02

//根据 flags 参数值的不同，xdl_open() 的行为会有一些差异：
//XDL_DEFAULT: 如果动态库已经被加载到内存中了，xdl_open() 不会再使用 dlopen() 加载它。（但依然会返回一个有效的 handle）
//XDL_TRY_FORCE_LOAD: 如果动态库还没有被加载到内存中，xdl_open() 将尝试使用 dlopen() 加载它。
//XDL_ALWAYS_FORCE_LOAD: xdl_open() 将总是使用 dlopen() 加载动态库。

void *xdl_open(const char *filename, int flags);
void *xdl_close(void *handle);

//如果 symbol_size 参数不为 NULL，它将被赋值为“符号对应的内容在 ELF 中占用的字节数
void *xdl_sym(void *handle, const char *symbol, size_t *symbol_size);
void *xdl_dsym(void *handle, const char *symbol, size_t *symbol_size);

void *getSymCompat(const char *filepath,const char *symbol);
void *getSymCompatForHandler(void *handler,const char *symbol);

// xdl_addr() 不仅能查询动态链接符号，还能查询调试符号。
// Enhanced dladdr().
// void *cache = NULL;
// xdl_info_t info;
// xdl_addr(addr_1, &info, &cache);
// xdl_addr(addr_2, &info, &cache);
// xdl_addr(addr_3, &info, &cache);
// xdl_addr_clean(&cache);
//
int xdl_addr(void *addr, xdl_info_t *info, void **cache);
void xdl_addr_clean(void **cache);

//
// Enhanced dl_iterate_phdr().
//
#define XDL_FULL_PATHNAME 0x01
int xdl_iterate_phdr(int (*callback)(struct dl_phdr_info *, size_t, void *), void *data, int flags);

//
// Custom dlinfo().
//
#define XDL_DI_DLINFO 1  // type of info: xdl_info_t
int xdl_info(void *handle, int request, void *info);

#ifdef __cplusplus
}
#endif

#endif
