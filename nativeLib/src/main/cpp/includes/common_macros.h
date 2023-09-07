//
// Created by Zhenxi on 2023/4/23.
//

#ifndef ZHENXIRUNTIME_COMMON_MACROS_H
#define ZHENXIRUNTIME_COMMON_MACROS_H

#include <sstream>


#define PRINTF_LIST(list) \
    do { \
        std::stringstream ss; \
        ss << #list << ": ["; \
        for (const auto &item : list) { \
            ss << item << " "; \
        } \
        ss << "]"; \
        LOGI(" %s", ss.str().c_str());  \
    } while (0)

#define PRINTF_LIST_LINE(list) \
    do { \
        for (const auto &item : list) { \
            LOGI(" %s", item.c_str()); \
        } \
        LOGI("-------------------------");\
    } while (0)     \

#define PRINTF_SET_LINE(set) \
    do { \
        std::stringstream ss; \
        for (const auto &item : set) { \
            ss << item << " "; \
            LOGI(" %s", ss.str().c_str());                       \
        } \
        LOGI("-------------------------");\
    } while (0) \


#define PRINT_OPENED_FILES_INFO(pid) \
    do { \
        std::string fd_dir = "/proc/" + std::to_string(pid) + "/fd"; \
        DIR *dir = opendir(fd_dir.c_str()); \
        if (dir != nullptr) { \
            struct dirent *entry; \
            while ((entry = readdir(dir)) != nullptr) { \
                char *endptr; \
                int fd = strtol(entry->d_name, &endptr, 10); \
                if (*endptr == '\0') { \
                    std::string fd_path = fd_dir + "/" + entry->d_name; \
                    char target_path[PATH_MAX]; \
                    ssize_t len = readlink(fd_path.c_str(), target_path, sizeof(target_path) - 1); \
                    if (len != -1) { \
                        target_path[len] = '\0'; \
                        LOG(INFO) << "FD: " << fd << " Path: " << target_path ; \
                    } \
                } \
            } \
            closedir(dir); \
        } \
    } while (0)




#define FIND_PACKAGE_ITEMS_IN_MAPS(PID, KEYWORD)                        \
    do {                                                                \
        std::string maps_file_path = "/proc/" + std::to_string(PID) + "/maps"; \
        std::ifstream maps_file(maps_file_path);                       \
        if (!maps_file.is_open()) {                                    \
            LOG(ERROR) << "Error opening maps file: " << maps_file_path ; \
        } else {                                                        \
            std::string line;                                          \
            while (std::getline(maps_file, line)) {                    \
                if (StringUtils::containsInsensitive(line,KEYWORD)) {  \
                    LOG(INFO)<<"match item info -> "<< line ;           \
                }                                                       \
            }                                                           \
            LOGI("-------------------------");                        \
            maps_file.close();                                         \
        }                                                               \
    } while (false)


#define FIND_PACKAGE_ITEMS_IN_MAPS_SELF(KEYWORD)                        \
    do {                                                                \
        std::string maps_file_path = "/proc/self/maps"; \
        std::ifstream maps_file(maps_file_path);                       \
        if (!maps_file.is_open()) {                                    \
            LOG(ERROR) << "Error opening maps file: " << maps_file_path ; \
        } else {                                                        \
            std::string line;                                          \
            while (std::getline(maps_file, line)) {                    \
                if (StringUtils::containsInsensitive(line,KEYWORD)) {  \
                    LOG(INFO)<<"match item info -> "<< line ;           \
                }                                                       \
            }                                                           \
            LOGI("-------------------------");                        \
            maps_file.close();                                         \
        }                                                               \
    } while (false)

#define LIST2STRING(args) \
std::string list2str; \
for (size_t i = 0; i < args.size(); ++i) {\
    list2str += args[i];\
    if (i != args.size() - 1) {\
        list2str += ",";\
    }\
} \

#define IS_NULL_STR(str) (str == nullptr?"":str) \

#endif //ZHENXIRUNTIME_COMMON_MACROS_H
