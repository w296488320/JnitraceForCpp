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

#ifndef IO_GITHUB_HEXHACKING_XUNWIND_CONFIG
#define IO_GITHUB_HEXHACKING_XUNWIND_CONFIG

// Do not open the log when running online.
//
// (Default: close)
//#define XU_CONFIG_WITH_LOG

// We only use libcorkscrew.so for CFI unwinding on Android 4.x with arm32/x86.
//
// (Default: dynamically specified, generally do not need to modify it)
#if !defined __LP64__ && __ANDROID_API__ < 21
#define XU_CONFIG_CFI_WITH_LIBCORKSCREW
#endif

// syscall-checking is a dangerous thing.
// It needs to read data from unknown memory addresses, which may cause a crash.
// Unless your usage scenario really needs it, don't use it.
//
// (Default: close)
//#define XU_CONFIG_FP_CHECK_SYSCALL

// EH unwinding may not always be necessary,
// and removing it can reduce the file size by nearly half.
//
// (Default: open)
#define XU_CONFIG_EH_ENABLE

#endif
