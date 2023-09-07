

#ifndef QCONTAINER_PRO_APPUTILS_H
#define QCONTAINER_PRO_APPUTILS_H


#include "parse.h"

typedef size_t Addr;


class Jnitrace {
    public:
        /**
         * start jni trace
         *
         * @param env  jniEnv
         * @param hookAll  lister all so jni
         * @param forbid_list  not lister so list
         * @param filter_list  lister so list
         * @param os using the list to save the entire collection,
         *           the jnitrace only handles the so name inside the list 。
         *           No file save occurs if os == nullptr
         */
        static void startjnitrace(JNIEnv *env,
                                  bool hookAll,
                                  const std::list<string> &forbid_list,
                                  const std::list<string> &filter_list,
                                  std::ofstream * os);

        /**
         * stop jni trace
         */
        [[maybe_unused]] static void stopjnitrace();

        private:
                static void init(JNIEnv *env);
};


#endif //QCONTAINER_PRO_APPUTILS_H
