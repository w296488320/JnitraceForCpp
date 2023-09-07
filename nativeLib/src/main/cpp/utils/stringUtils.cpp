//
// Created by zhenxi on 2021/12/8.
//

#include "stringUtils.h"
#include "mylibc.h"


bool StringUtils::compare_pred(unsigned char a, unsigned char b) {
    return std::tolower(a) == std::tolower(b);
}

bool StringUtils::equals(const std::string &str1, const std::string &str2) {
    return my_strcmp(str1.c_str(),str2.c_str())==0;
}
bool StringUtils::isNumeric(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}
bool StringUtils::endsWith(const std::string &str, const std::string &suffix) {

    if (str.size() < suffix.size()) {
        return false;
    }

    std::string tstr = str.substr(str.size() - suffix.size());

    if (tstr.length() == suffix.length()) {
        return std::equal(suffix.begin(), suffix.end(), tstr.begin(), compare_pred);
    } else {
        return false;
    }
}

bool StringUtils::endsWith(const char *s, const std::string &suffix) {
    std::string str(s);
    return endsWith(str,suffix);
}

bool StringUtils::startWith(const std::string &str, const std::string &suffix) {

    if (str.size() < suffix.size()) {
        return false;
    }

    std::string tstr = str.substr(0, suffix.length());

    if (tstr.length() == suffix.length()) {
        return std::equal(suffix.begin(), suffix.end(), tstr.begin(), compare_pred);
    } else {
        return false;
    }
}

bool StringUtils::startWith(const char *s, const std::string &suffix) {
    std::string str(s);
    return startWith(str,suffix);
}

std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower_str;
}

std::string to_upper(const std::string& str) {
    std::string upper_str = str;
    std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return upper_str;
}

bool StringUtils::containsInsensitive(const std::string& str, const std::string& substr) {
    std::string str_lower = to_lower(str);
    std::string substr_lower = to_lower(substr);
    return str_lower.find(substr_lower) != std::string::npos;
}

bool StringUtils::contains(const std::string &str, const std::string &suffix) {
    if (str.find(suffix) == std::string::npos){
        return false;
    }
    return true;
}
bool StringUtils::contains(const char *s, const std::string &suffix) {
    std::string str(s);
    return contains(str,suffix);
}
std::string StringUtils::binaryToHex(const std::string &binaryStr) {
    std::string ret;
    static const char *hex = "0123456789ABCDEF";
    for (auto c: binaryStr) {
        ret.push_back(hex[(c >> 4) & 0xf]); //取二进制高四位
        ret.push_back(hex[c & 0xf]);        //取二进制低四位
    }
    return ret;
}
std::string StringUtils::toUpperCase(const std::string& str) {
    std::string result(str);
    std::transform(str.begin(), str.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}
std::string StringUtils::toLowerCase(const std::string& str) {
    std::string result(str);
    std::transform(str.begin(), str.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}
int StringUtils::HexToInt(const std::string &str) {

    std::string indexstr("0x");
    indexstr.append(str);
    int sum = 0;
    for (size_t i = indexstr.size() - 1, base = 1; i > 1; i--, base *= 16) {
        if (isdigit(indexstr[i]))
            sum += (indexstr[i] - '0') * base;
        else
            sum += (indexstr[i] - 'A' + 10) * base;
    }
    return sum;
}
/**
 * 将str里面的pattern替换成newpat
 * 返回替换成功的个数
 */
int StringUtils::replace(std::string &str, const std::string &pattern, const std::string &newpat) {
    int count = 0;
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();
    for (size_t pos = str.find(pattern, 0);
         pos != std::string::npos; pos = str.find(pattern, pos + nsize)) {
         str.replace(pos, psize, newpat);
         count++;
    }
    return count;
}

