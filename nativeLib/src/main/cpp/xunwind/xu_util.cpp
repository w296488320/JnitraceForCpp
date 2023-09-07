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

#include "xu_util.h"

#include <android/api-level.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "xdl.h"

static bool xu_util_starts_with(const char *str, const char *start) {
  while (*str && *str == *start) {
    str++;
    start++;
  }

  return '\0' == *start;
}

static int xu_util_get_api_level_from_build_prop(void) {
  char buf[128];
  int api_level = -1;

  FILE *fp = fopen("/system/build.prop", "r");
  if (NULL == fp) goto end;

  while (fgets(buf, sizeof(buf), fp)) {
    if (xu_util_starts_with(buf, "ro.build.version.sdk=")) {
      api_level = atoi(buf + 21);
      break;
    }
  }
  fclose(fp);

end:
  return (api_level > 0) ? api_level : -1;
}

int xu_util_get_api_level(void) {
  static int xu_util_api_level = -1;

  if (xu_util_api_level < 0) {
    int api_level = android_get_device_api_level();
    if (api_level < 0) api_level = xu_util_get_api_level_from_build_prop();  // compatible with unusual models
    if (api_level < __ANDROID_API_J__) api_level = __ANDROID_API_J__;

    __atomic_store_n(&xu_util_api_level, api_level, __ATOMIC_SEQ_CST);
  }

  return xu_util_api_level;
}

static void xu_util_get_process_or_thread_name(const char *path, char *buf, size_t buf_len, pid_t id) {
  bool ok = false;
  FILE *fp = NULL;
  if (NULL == (fp = fopen(path, "r"))) {
    if (!ok) snprintf(buf, buf_len, "[unknown-%d]", id);
    return;
  }
  if (NULL == fgets(buf, (int)buf_len, fp)) {
    if (NULL != fp) fclose(fp);
    if (!ok) snprintf(buf, buf_len, "[unknown-%d]", id);
    return;
  }

  size_t len = strlen(buf);
  if (0 == len) goto end;
  if ('\n' == buf[len - 1]) {
    if (1 == len) goto end;
    buf[len - 1] = '\0';
  }

  ok = true;

end:
  if (NULL != fp) fclose(fp);
  if (!ok) snprintf(buf, buf_len, "[unknown-%d]", id);
}

void xu_util_get_process_name(pid_t pid, char *buf, size_t buf_len) {
  char path[64];
  snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
  xu_util_get_process_or_thread_name(path, buf, buf_len, pid);
}

void xu_util_get_thread_name(pid_t pid, pid_t tid, char *buf, size_t buf_len) {
  char path[64];
  snprintf(path, sizeof(path), "/proc/%d/task/%d/comm", pid, tid);
  xu_util_get_process_or_thread_name(path, buf, buf_len, tid);
}

pid_t *xu_util_get_thread_ids(pid_t pid, size_t *tids_cnt) {
  pid_t *tids = NULL;
  *tids_cnt = 0;
  size_t tids_cap = 0;

  char path[64];
  snprintf(path, sizeof(path), "/proc/%d/task", pid);

  DIR *dir = opendir(path);
  if (NULL == dir) return NULL;

  struct dirent *ep;
  while (NULL != (ep = readdir(dir))) {
    pid_t tid = -1;
    sscanf(ep->d_name, "%d", &tid);
    if (tid < 0) continue;

    if (tids_cap - *tids_cnt < 1) {
      size_t new_tids_cap = tids_cap + 512;
      pid_t *new_tids = (pid_t *)realloc(tids, new_tids_cap * (sizeof(pid_t)));
      if (NULL == new_tids) {
        closedir(dir);
        if (NULL != tids) free(tids);
        *tids_cnt = 0;
        return NULL;
      }
      tids = new_tids;
      tids_cap = new_tids_cap;
    }

    tids[(*tids_cnt)++] = tid;
  }
  closedir(dir);

  return tids;
}

int xu_util_ptrace_attach(pid_t tid) {
  if (0 != ptrace(PTRACE_ATTACH, tid, NULL, NULL)) return -1;

  errno = 0;
  while (waitpid(tid, NULL, __WALL) < 0) {
    if (EINTR != errno) {
      ptrace(PTRACE_DETACH, tid, NULL, NULL);
      return -1;
    }
  }
  return 0;
}

void xu_util_ptrace_detach(pid_t tid) {
  ptrace(PTRACE_DETACH, tid, NULL, NULL);
}

int xu_util_write(int fd, const char *buf, size_t buf_len) {
  if (fd < 0) return -1;

  const char *ptr = buf;
  size_t nleft = buf_len;

  while (nleft > 0) {
    errno = 0;
    ssize_t nwritten = write(fd, ptr, nleft);
    if (nwritten <= 0) {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;  // call write() again
      else
        return -1;  // error
    }
    nleft -= (size_t)nwritten;
    ptr += nwritten;
  }

  return 0;
}
