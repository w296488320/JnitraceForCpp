//
// Created by zhenxi on 2021/11/7.
//

#include "includes/fileUtils.h"
#include "includes/appUtils.h"

#include <libgen.h>

using namespace std;


bool fileUtils::setFilePermissions(const std::string& filePath, std::filesystem::perms permissions) {
    // 检查文件是否存在
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return false;
    }

    // 设置文件权限
    std::filesystem::permissions(filePath, permissions);

    // 检查是否成功设置文件权限
    if (std::filesystem::status(filePath).permissions() != permissions) {
        std::cerr << "Failed to set file permissions: " << filePath << std::endl;
        return false;
    }

    // 返回成功
    return true;
}

/**
 * 替换文件里面的具体指令 。
 * @param filePath 文件路径
 * @param beginPos 开始位置
 * @param buffer buff 替换的内容地址
 * @param size  大小
 * @return
 */
bool fileUtils::replaceFileBytes(const std::string& filePath, std::streampos beginPos, void* buffer, std::streamsize size) {
// 打开文件
    std::fstream file(filePath, std::ios::binary | std::ios::in | std::ios::out);

    // 检查文件是否打开成功
    if (!file.is_open()) {
        LOG(ERROR)<< "Failed to open file: " << filePath ;
        return false;
    }

    // 跳转到指定位置
    file.seekp(beginPos);

    // 写入文件内容
    file.write(static_cast<const char*>(buffer), size);

    // 检查是否成功写入指定数量的字节
    if (!file.good()) {
        LOG(ERROR)<< "Failed to write " << size << " bytes to file: " << filePath ;
        file.close();
        return false;
    }

    // 关闭文件并返回成功
    file.close();
    return true;
}
/**
 * 读取文件的指令,需要free ,失败则返回null ;
 * @param filePath 文件路径
 * @param beginPos 开始位置index
 * @param size  大小
 * @return
 */
void* fileUtils::readFileBytes(const std::string& filePath, std::streampos beginPos, std::streamsize size) {
    // 打开文件
    std::ifstream file(filePath, std::ios::binary);

    // 检查文件是否打开成功
    if (!file.is_open()) {
        LOG(ERROR)<< "Failed to open file: " << filePath ;
        return nullptr;
    }

    // 跳转到指定位置
    file.seekg(beginPos);

    // 分配内存并读取文件内容
    char* buffer = static_cast<char*>(malloc(size));
    file.read(buffer, size);

    // 检查是否成功读取指定数量的字节
    if (file.gcount() != size) {
        LOG(ERROR)<< "Failed to read " << size << " bytes from file: " << filePath ;
        free(buffer);
        file.close();
        return nullptr;
    }

    // 关闭文件并返回读取的字节指针
    file.close();
    return static_cast<void*>(buffer);
}

bool fileUtils::copyFile(const std::string& sourcePath, const std::string& destinationPath) {
    // 打开源文件和目标文件
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    std::ofstream destinationFile(destinationPath, std::ios::binary);

    // 检查源文件是否打开成功
    if (!sourceFile.is_open()) {
        LOG(ERROR)<<"Failed to open source file: " << sourcePath;
        return false;
    }

    // 检查目标文件是否打开成功
    if (!destinationFile.is_open()) {
        LOG(ERROR)<< "Failed to open destination file: " << destinationPath ;
        sourceFile.close();
        return false;
    }

    // 拷贝源文件到目标文件
    destinationFile << sourceFile.rdbuf();

    // 关闭文件
    sourceFile.close();
    destinationFile.close();

    return true;
}
/**
 * 查看文件是否包含某个特征
 * 将特征内容写到buff
 */
bool fileUtils::file_include(const char* filePath, const std::vector<std::string>& list, std::string& buff) {
    // 打开文件
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG(ERROR)<< "file_include open file fail " << filePath << " " << strerror(errno) ;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << "file_include checking " << filePath << " " << line << std::endl;
        for (const auto& feature : list) {
            if (line.find(feature) != std::string::npos) {
                std::cout << "file_include find " << filePath << " " << line << std::endl;
                buff = line;
                file.close();
                return true;
            }
        }
    }

    file.close();
    //std::cerr << "file_include not find " << filePath << std::endl;
    return false;
}
/**
 * 根据fd获取path路径
 */
string fileUtils::get_file_name(int fd, pid_t pid) {
    if (fd <= 0) {
        LOGE("fileUtils get_file_name error fd == 0  -> %d ", fd);
        return {};
    }
    std::string path_string = "/proc/" +
            std::to_string(pid) + "/fd/" + std::to_string(fd);
    char file_path[PATH_MAX] = {};
    if (readlink(path_string.c_str(), file_path, sizeof(file_path) - 1) != -1) {
        return {file_path};
    }
    LOGE("fileUtils get_file_name not "
         "found pid-> %d fd ->%d path [%s] %s ",pid,fd, path_string.c_str(), strerror(errno));
    return {};
}

/*
 * 创建多级dir目录,防止创建文件之前找不到目录
 */
int fileUtils::makeDir(const char *path) {

    size_t beginCmpPath;
    size_t endCmpPath;
    size_t fullPathLen;
    size_t pathLen = strlen(path);
    char currentPath[128] = {0};
    char fullPath[128] = {0};
    //相对路径
    if ('/' != path[0]) {
        //获取当前路径
        getcwd(currentPath, sizeof(currentPath));
        strcat(currentPath, "/");
        beginCmpPath = strlen(currentPath);
        strcat(currentPath, path);
        if (path[pathLen] != '/') {
            strcat(currentPath, "/");
        }
        endCmpPath = strlen(currentPath);

    } else {
        //绝对路径
        strcpy(currentPath, path);
        if (path[strlen(path)] != '/') {
            strcat(currentPath, "/");
        }
        beginCmpPath = 1;
        endCmpPath = strlen(currentPath);
    }
    //创建各级目录
    for (int i = beginCmpPath; i < endCmpPath; i++) {
        if ('/' == currentPath[i]) {
            currentPath[i] = '\0';
            if (access(currentPath, NULL) != 0) {
                if (mkdir(currentPath, 0755) == -1) {
                    LOGE("fileUtils mkdir error ,currentPath -> %s  %s ", currentPath,
                         strerror(errno));
                    return -1;
                }
            }
            currentPath[i] = '/';
        }
    }
    return 0;
}


string fileUtils::readText(string file) {
    std::ifstream infile;
    infile.open(file.data());   //将文件流对象与文件连接起来
    if (!infile.is_open()) {
        //若失败,则输出错误消息,并终止程序运行
        //LOGE("fileUtils read text open file error %s ", file.c_str());
        return {};
    }

    string result;
    string lineStr;
    while (getline(infile, lineStr)) {
        result.append(lineStr);
    }
    infile.close();             //关闭文件输入流
    return result;
}

string fileUtils::readText(FILE *file) {
    string result;
    char buf[PATH_MAX] = {0};
    while (fgets(buf, PATH_MAX, file)) {
        result.append(buf);
    }
    return result;
}

void fileUtils::writeText(string file, const string& str, bool isAppend) {
    std::ofstream os;
    //保证文件存在
    makeDir(dirname(file.c_str()));
    if (isAppend) {
        //正常打开,末尾追加
        os.open(file.data(), ios::app);
    } else {
        //覆盖
        os.open(file.data(), ios::trunc);
    }
    os << str;
    os.close();             //关闭文件输入流
}

bool fileUtils::savefile(const char *savePath, size_t size, size_t start, bool isDele) {
    if (size == 0) {
        LOGE("savefile size == 0  ")
        return false;
    }
    if (savePath == nullptr) {
        LOGE("filePath == nullptr ")
        return false;
    }
    char *path = strdup(savePath);
    char *filepath = dirname(path);
    LOGI("fileUtils::savefile upper path -> %s ", filepath)
    if (access(filepath, 0) == -1) {
        LOGI("fileUtils::savefile not found upper path ,start create  %s", filepath)
        //如果父文件夹不存在递归创建多级
        int createret = fileUtils::makeDir(filepath);
        if (createret == -1) {
            LOGE("fileUtils::savefile create upper path error  -> %s  %s ", filepath,
                 strerror(errno))
            return false;
        }
    }
    FILE *file;
    if (isDele) {
        //http://www.wenjuntech.com/wp-content/uploads/2018/05/QQjt20180424183202-99.jpg
        //清除原有数据,不存在则创建
        file = fopen(savePath, "w+");
    } else {
        //不清除原有数据,不存在则创建
        file = fopen(savePath, "wt+");
    }
    if (file == nullptr) {
        LOGE("savefile fopen == null %s  ", strerror(errno))
        return false;
    }
    fseek(file, 0, SEEK_SET);

    LOGI("fileUtils::savefile save path -> start address ->0x%x  file end address -> 0x%x ", start,
         (start + size))
    //有的SO的内存段是不可读不可写,必须RWX 只R不行
    MPROTECT(start, size, MEMORY_RWX);

    size_t wirtesize = fwrite(reinterpret_cast<void *>(start), size, 1, file);

    fclose(file);
    if (path != nullptr) {
        free(path);
    }
    LOGI("fileUtils::savefile sucess ! save size %s  %zu fwrite size -> %lu ", path, size,
         wirtesize)
    return true;
}


bool fileUtils::isFileExists(const string& name) {
    try {
        return filesystem::exists(name);
    } catch (...) {
        return false;
    }
}