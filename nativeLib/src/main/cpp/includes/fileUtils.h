//
// Created by zhenxi on 2021/11/7.
//


#ifndef QCONTAINER_PRO_FILEUTILS_H
#define QCONTAINER_PRO_FILEUTILS_H


#include <cstdio>
#include "AllInclude.h"

class fileUtils {
public:
    static bool setFilePermissions(const std::string& filePath, std::filesystem::perms permissions);
    static bool replaceFileBytes(const std::string& filePath, std::streampos beginPos, void* buffer, std::streamsize size);
    static void* readFileBytes(const std::string& filePath, std::streampos beginPos, std::streamsize size);
    static bool copyFile(const std::string& sourcePath, const std::string& destinationPath);
    static int makeDir(const char* path);
    static string readText(string file);
    static string readText(FILE *file);
    static void writeText(string file,const string& str,bool isAppend);
    static bool savefile(const char* filePath,size_t size,size_t start,bool isDele);
    static string get_file_name(int fd,pid_t pid);
    static bool isFileExists(const string& name);
    static bool file_include(const char* filePath, const std::vector<std::string>& list, std::string& buff);
};


#endif //QCONTAINER_PRO_FILEUTILS_H
