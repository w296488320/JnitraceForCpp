//
// Created by zhenxi on 2022/1/21.
//

#ifndef QCONTAINER_PRO_LINKERHANDLER_H
#define QCONTAINER_PRO_LINKERHANDLER_H

#include <set>
namespace ZhenxiRunTime {
/**
    * linker 加载 so 回调实现类。
    * 可以在需求的代码块里面进行对应逻辑的设计 。
    */
    class LinkerLoadCallBack {
        public:
            /**
             * so load 之前
             * @param name so path
             */
            virtual void loadBefore(const char *name) const = 0;

            /**
             * so load 之后
             * @param name so path
             */
            virtual void loadAfter(const char *path,const char *redirect_path, void *ret) const = 0;

        protected:

    };

    class LinkerIORedirect {
        public:
            virtual std::string linkerRedirect(const char *path) const = 0;
        protected:
    };

    class linkerHandler {
    public:
        /**
         * 初始化&hook linker
         * @return
         */
        static void init();

        /**
         * 添加linker加载的回调
         */
        static void addLinkerCallBack(LinkerLoadCallBack *callBack);
        /**
         * 处理linker io重定向逻辑:
         * 注:
         * 该方法只能被调用一次
         * @param callBack
         * @return
         */
        static void linkerIORedirect(LinkerIORedirect *callBack);

        /**
         * 清除掉全部的callback
         */
        static void cleanCallBack();

        /**
         * 获取linker加载的全部elf文件
         */
        static std::list<std::string> getlinkerLoadList();

        static void removelinkerList(const std::set<std::string_view> &names);

    private:

    };
}

#endif
