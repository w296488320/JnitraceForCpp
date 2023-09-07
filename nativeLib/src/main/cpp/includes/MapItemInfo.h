//
// Created by Zhenxi on 2023/1/1.
//

#ifndef ZHENXIRUNTIME_MAPITEMINFO_H
#define ZHENXIRUNTIME_MAPITEMINFO_H

namespace ZhenxiRunTime {
    struct MapAddresInfo {
        /**
         * 函数的符号
         */
        char *sym = nullptr;
        /**
         * 函数在文件路径
         */
        char *fname = nullptr;

        /**
         * 所在函数的基地址
         */
        size_t sym_base = 0;
        /**
         * 文件基地址
         */
        size_t fbase = 0;

        /**
         * 传入地址,相对于so的偏移
         */
        size_t offset = 0;
    };

    struct MapItemInfo {
        /**
         * item开始位置
         */
        size_t start;

        /**
         * item结束位置
         */
        size_t end;
    };
}

#endif //ZHENXIRUNTIME_MAPITEMINFO_H
