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

#ifndef IO_GITHUB_HEXHACKING_XUNWIND_LOG
#define IO_GITHUB_HEXHACKING_XUNWIND_LOG

#include <stdarg.h>

#include "xu_config.h"

#ifdef XU_CONFIG_WITH_LOG

#include <android/log.h>

#define XU_LOG_TAG      "xunwind_tag"
#define XU_LOG_PRIORITY ANDROID_LOG_INFO

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#define XU_LOG(fmt, ...) __android_log_print(XU_LOG_PRIORITY, XU_LOG_TAG, fmt, ##__VA_ARGS__)
#pragma clang diagnostic pop

#else
#define XU_LOG(fmt, ...)
#endif

#endif
