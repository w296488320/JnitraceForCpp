//
// Created by zhenxi on 2021/12/12.
//

//#include <VAJni.h>

#include <utility>

#include "includes/appUtils.h"
#include "includes/HookUtils.h"
#include "includes/libpath.h"

using namespace StringUtils;
using namespace ZhenxiRunTime;

bool isSystemFile(const string& path){
    return endsWith(path, "/libc.so") || //允许libc调用
           startWith(path, "/system/") ||
           startWith(path, "/apex/") ;
}


string get_process_name_by_pid(int pid) {
    // 构造进程文件名
    std::stringstream filename;
    filename << "/proc/" << pid << "/cmdline";
    // 打开进程文件并读取进程名
    std::ifstream file(filename.str());
    if (file.is_open()) {
        std::string process_name;
        std::getline(file, process_name, '\0'); // 读取进程名直到遇到空字符 '\0'
        file.close();
        return process_name;
    }
    // 如果无法获取当前进程名，则返回空字符串
    return {};
}


void getAllOpenFdInfo(){
#define F_LEN 1024

    struct dirent *dirent;
    char path[PATH_MAX];
    size_t status;
    DIR *dirp;

    /* Open the virtual directory "/proc/$pid/fd". */
    dirp = opendir("/proc/");
    if (dirp == nullptr)
        return ;
    while ((dirent = readdir(dirp)) != nullptr) {
        long procPid = strtol(dirent->d_name, nullptr, 10);
        if (procPid){
            char filename[F_LEN]={};
            char buf[F_LEN]={};
            snprintf(filename, F_LEN, "/proc/self/fd/%s",  dirent->d_name);
            status = readlink(filename, buf, F_LEN);
            if (status < 0 || status >= PATH_MAX){
                LOGW(">>>>>>>>> find fd error filename [%s] %s",filename, strerror(errno))
                continue;
            }
            path[status] = '\0';
            LOGI(">>>>>>>>> find fd %s [%s] %s",dirent->d_name,buf,filename)
        }
    }
    closedir(dirp);
}


void inline findSymBySo(size_t address, MapAddresInfo* info) {

}


/**
 * 根据一个地址获取所在的so信息
 */
string getSymbolforAddress(size_t address,pid_t pid) {
    if (address == 0) {
        LOGE("getSymbolforAddress  address == null ")
        return {};
    }
    FILE *fp = fopen(string("/proc/").append(
            to_string(pid)).append("/maps").c_str(),"r");
    char *line = nullptr;
    char perm[12] = {'\0'}, dev[12] = {'\0'},mapname[PATH_MAX] = {'\0'};
    size_t begin, end, inode, foo;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {
        sscanf(line, "%lx-%lx %s %lx %s %ld %s",
               &begin, &end, perm, &foo, dev, &inode, mapname);
        if (address >= begin && address <= end) {
            syscall(__NR_close, fp);
            return {mapname};
        }
    }
    LOGI("getSymbolforAddress not found address in maps   0x%zx ", address)
    //找不到也需要关闭
    syscall(__NR_close, fp);
    return {};
}



//将内存指定地址设置可读,可执行,
//但是不可写
bool MPROTECT(size_t addr,size_t size,int __prot){
    //计算扇叶
    auto alignment = (unsigned) ((unsigned long long) addr % sysconf(_SC_PAGESIZE));
    //保证开始地址 减去扇叶大小
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + size),__prot);
    if (i == 0) {
        return true;
    }
    LOGE("Helper mprotect error   %s ", strerror(errno))
    return false;
}
#define BUF_SIZE 1024


MapItemInfo getSoBaseAddress(const char *name) {
    MapItemInfo info{0};
    if(name== nullptr){
        return info;
    }
    size_t start = 0 ;
    size_t end = 0 ;
    bool isFirst = true;
    size_t len = 0;
    char buffer[PATH_MAX] = {0};
    //读取的Maps不进行IO重定向
    FILE *fp =fopen("/proc/self/abcd", "r");
    if(fp == nullptr){
        //找不到用原始文件
        fp = fopen("/proc/self/maps", "r");
        if(fp== nullptr){
            return info;
        }
    }
    char *line = nullptr;
    while (getline(&line, &len, fp) != -1) {
        if (line!= nullptr&&strstr(line, name)) {
            sscanf(line, "%lx-%lx %*s %*s %*s %*s %*s %*s", &start, &end);
            //start 只有第一次赋值
            if(isFirst){
                info.start = start;
                isFirst= false;
            }
        }
    }
    info.end = end;
    if (line != nullptr) {
        free(line);
    }
    fclose(fp);
    //LOGE("get maps info start -> 0x%zx  end -> 0x%zx ",info.start,info.end);
    return info;
}




bool getpData(char temp[],const void *p, size_t size) {
    memset(temp,0,strlen(temp));
    int i;
    int len = 0;
    for (i = 0; i < size; i++) {
        len += sprintf(temp + len, "%02X", ((char *) p)[i]);
    }
    return true;
}

string getpData(const void *p,size_t tempSize) {
    char temp[tempSize*2];
    memset(temp,0,strlen(temp));
    int i;
    int len = 0;
    for (i = 0; i < tempSize; i++) {
        len += sprintf(temp + len, "%02X", ((char *) p)[i]);
    }
    return string {temp};
}
bool mProtectDef(size_t addr, size_t len) {
    auto alignment = (unsigned) ((unsigned long long) addr % sysconf(_SC_PAGESIZE));
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),
                     PROT_READ | PROT_WRITE | PROT_EXEC);
    if (i == -1) {
        return false;
    }
    return true;
}
/**
 * PROT_READ | PROT_WRITE | PROT_EXEC
 */
bool mProtectUserDefined(size_t addr, size_t len,int prot) {
    auto alignment = (unsigned) ((unsigned long long) addr % sysconf(_SC_PAGESIZE));
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),prot);
    if (i == -1) {
        return false;
    }
    return true;
}