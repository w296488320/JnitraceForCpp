#ifndef DETECTFRIDA_MYLIBC_H
#define DETECTFRIDA_MYLIBC_H
/*
 * These functions are copied from glibc, android libc, apple libc open source code.
 * This is to avoid easy bypass through libc functions
 */
//#include "raw_syscall.h"
#include "logging.h"



__attribute__((always_inline))
static inline size_t
my_strlcpy(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    /* Copy as many bytes as will fit */
    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }
    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';		/* NUL-terminate dst */
        while (*s++)
            ;
    }
    return(s - src - 1);	/* count does not include NUL */
}

__attribute__((always_inline))
static inline
size_t my_strlen(const char *s)
{
    if(s == nullptr){
        return 0;
    }
    size_t len = 0;
    while(*s++) len++;
    return len;
}

__attribute__((always_inline))
static inline int
my_strncmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0)
        return (0);
    do {
        if (*s1 != *s2++)
            return (*(unsigned char *)s1 - *(unsigned char *)--s2);
        if (*s1++ == 0)
            break;
    } while (--n != 0);
    return (0);
}

__attribute__((always_inline))
static inline char *
my_strstr(const char *s, const char *find)
{
    if(find == nullptr||s == nullptr){
        return nullptr;
    }
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
        len = my_strlen(find);
        do {
            do {
                if ((sc = *s++) == '\0')
                    return (NULL);
            } while (sc != c);
        } while (my_strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

__attribute__((always_inline))
static inline
void*  my_memset(void*  dst, int c, size_t n)
{
    char*  q   = (char*)dst;
    char*  end = q + n;
    for (;;) {
        if (q >= end) break; *q++ = (char) c;
        if (q >= end) break; *q++ = (char) c;
        if (q >= end) break; *q++ = (char) c;
        if (q >= end) break; *q++ = (char) c;
    }
    return dst;
}

__attribute__((always_inline))
static inline int
my_strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2++)
        if (*s1++ == 0)
            return (0);
    return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

__attribute__((always_inline))
static inline int my_atoi(const char *s)
{
    int n=0, neg=0;
    while (isspace(*s)) s++;
    switch (*s) {
        case '-': neg=1;
        case '+': s++;
    }
    /* Compute n as a negative number to avoid overflow on INT_MIN */
    while (isdigit(*s))
        n = 10*n - (*s++ - '0');
    return neg ? n : -n;
}

//__attribute__((always_inline))
//static inline int  my_openat(int __dir_fd, const void* __path, int __flags, int __mode ){
//    return (int)raw_syscall(__NR_openat, __dir_fd, (long)__path, __flags, __mode);
//}
//
//__attribute__((always_inline))
//static inline int  my_open(const void* __path, int __flags, int __mode ){
//    return (int)raw_syscall(__NR_openat, AT_FDCWD, (long)__path, __flags, __mode);
//}
//
//__attribute__((always_inline))
//static inline ssize_t my_read(int __fd, void* __buf, size_t __count){
//    return raw_syscall(__NR_read, __fd, (long)__buf, (long)__count);
//}
//
//__attribute__((always_inline))
//static inline off_t my_lseek(int __fd, off_t __offset, int __whence){
//    return raw_syscall(__NR_lseek, __fd, __offset, __whence);
//}
//
//__attribute__((always_inline))
//static inline int my_close(int __fd){
//    return (int)raw_syscall(__NR_close, __fd);
//}
//
//__attribute__((always_inline))
//static inline int my_nanosleep(const struct timespec* __request, struct timespec* __remainder){
//    return (int)raw_syscall(__NR_nanosleep, (long)__request, (long)__remainder);
//}
//
//__attribute__((always_inline))
//static inline ssize_t my_readlinkat(int __dir_fd, const char* __path, char* __buf, size_t __buf_size){
//    return raw_syscall(__NR_readlinkat, __dir_fd, (long)__path, (long)__buf,(long)__buf_size);
//}
//
////Not Used
//__attribute__((always_inline))
//static inline int my_tgkill(int __tgid, int __tid, int __signal){
//    return (int)raw_syscall(__NR_tgkill, __tgid, __tid, __signal);
//}

//__attribute__((always_inline))
//static inline void* my_mmap(void* __addr, size_t __size, int __prot, int __flags, int __fd, off_t __offset){
//    return (void*)raw_syscall(__NR_mmap, __addr, __size, __prot,__flags,__fd,__offset);
//}

//Not Used
//__attribute__((always_inline))
//static inline void my_exit(int __status){
//    raw_syscall(__NR_exit, __status);
//}
__attribute__((always_inline))
static inline char* int_to_str(int num) {
    bool is_negative = false;
    if (num < 0) {
        is_negative = true;
        num = -num;
    }

    int len = 0;
    int temp = num;
    do {
        temp /= 10;
        len++;
    } while (temp > 0);

    if (is_negative) {
        len++;
    }

    char* str = (char*)malloc(len + 1);
    str[len] = '\0';

    int i = len - 1;
    do {
        str[i--] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    if (is_negative) {
        str[0] = '-';
    }

    return str;
}

__attribute__((always_inline))
static inline char *my_stpcpy(char*  d,const char * s)
{
#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    word *wd;
    const word *ws;
    if ((uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN) {
        for (; (uintptr_t)s % ALIGN; s++, d++)
            if (!(*d=*s)) return d;
        wd=(word *)d; ws=(const word *)s;
        for (; !HASZERO(*ws); *wd++ = *ws++);
        d=(char *)wd; s=(const char *)ws;
    }
#endif
    for (; (*d=*s); s++, d++);

    return d;
}

__attribute__((always_inline)) static inline
char *my_strcat(char * dest, const char * src){
    my_stpcpy(dest + my_strlen(dest), src);
    return dest;
}
__attribute__((always_inline)) static inline
void *my_memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = static_cast<unsigned char *>(dest);
    const unsigned char *s = static_cast<const unsigned char *>(src);

    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }

    return dest;
}



#endif //DETECTFRIDA_MYLIBC_H
