#ifndef _WHALE_BASE_LOGGING_H_
#define _WHALE_BASE_LOGGING_H_

#include <string>
#include <sstream>

#include "macros.h"

#ifdef __ANDROID__

#include <android/log.h>

#endif


#define CHECK(x) \
  if (UNLIKELY(!(x))) /*  NOLINT*/ \
    w::LogMessageFatal("", __LINE__).stream() \
        << "Check failed: " #x << " "

#define CHECK_OP(LHS, RHS, OP) \
  for (auto _values = zhenxi::MakeEagerEvaluator(LHS, RHS); \
       UNLIKELY(!(_values.lhs OP _values.rhs)); /* empty */) \
        zhenxi::LogMessage("", __LINE__).stream() \
        << "Check failed: " << #LHS << " " << #OP << " " << #RHS \
        << " (" #LHS "=" << _values.lhs << ", " #RHS "=" << _values.rhs << ") "

#define CHECK_EQ(x, y) CHECK_OP(x, y, ==)
#define CHECK_NE(x, y) CHECK_OP(x, y, !=)
#define CHECK_LE(x, y) CHECK_OP(x, y, <=)
#define CHECK_LT(x, y) CHECK_OP(x, y, <)
#define CHECK_GE(x, y) CHECK_OP(x, y, >=)
#define CHECK_GT(x, y) CHECK_OP(x, y, >)

#define CHECK_STROP(s1, s2, sense) \
  if (UNLIKELY((strcmp(s1, s2) == 0) != sense)) \
    LOG(FATAL) << "Check failed: " \
               << "\"" << s1 << "\"" \
               << (sense ? " == " : " != ") \
               << "\"" << s2 << "\""

#define CHECK_STREQ(s1, s2) CHECK_STROP(s1, s2, true)
#define CHECK_STRNE(s1, s2) CHECK_STROP(s1, s2, false)

#define CHECK_CONSTEXPR(x, out, dummy) \
  (UNLIKELY(!(x))) ? (LOG(FATAL) << "Check failed: " << #x out, dummy) :

#ifndef NDEBUG

#define DCHECK(x) CHECK(x)
#define DCHECK_EQ(x, y) CHECK_EQ(x, y)
#define DCHECK_NE(x, y) CHECK_NE(x, y)
#define DCHECK_LE(x, y) CHECK_LE(x, y)
#define DCHECK_LT(x, y) CHECK_LT(x, y)
#define DCHECK_GE(x, y) CHECK_GE(x, y)
#define DCHECK_GT(x, y) CHECK_GT(x, y)
#define DCHECK_STREQ(s1, s2) CHECK_STREQ(s1, s2)
#define DCHECK_STRNE(s1, s2) CHECK_STRNE(s1, s2)
#define DCHECK_CONSTEXPR(x, out, dummy) CHECK_CONSTEXPR(x, out, dummy)

#else  // NDEBUG

#define DCHECK(condition) \
  while (false) \
    CHECK(condition)

#define DCHECK_EQ(val1, val2) \
  while (false) \
    CHECK_EQ(val1, val2)

#define DCHECK_NE(val1, val2) \
  while (false) \
    CHECK_NE(val1, val2)

#define DCHECK_LE(val1, val2) \
  while (false) \
    CHECK_LE(val1, val2)

#define DCHECK_LT(val1, val2) \
  while (false) \
    CHECK_LT(val1, val2)

#define DCHECK_GE(val1, val2) \
  while (false) \
    CHECK_GE(val1, val2)

#define DCHECK_GT(val1, val2) \
  while (false) \
    CHECK_GT(val1, val2)

#define DCHECK_STREQ(str1, str2) \
  while (false) \
    CHECK_STREQ(str1, str2)

#define DCHECK_STRNE(str1, str2) \
  while (false) \
    CHECK_STRNE(str1, str2)

#define DCHECK_CONSTEXPR(x, out, dummy) \
  (false && (x)) ? (dummy) :

#endif



//#ifdef ZHENXI_BUILD_TYPE_NOLOG
//
//#define LOG_INFO    zhenxi::NOLogMessage()
//#define LOG_WARNING zhenxi::NOLogMessage()
//#define LOG_ERROR   zhenxi::NOLogMessage()
//#define LOG_DEBUG   zhenxi::NOLogMessage()
//
//#else

#define LOG_INFO zhenxi::LogMessage(ANDROID_LOG_INFO, __LINE__)
#define LOG_WARNING zhenxi::LogMessage(ANDROID_LOG_WARN, __LINE__)
#define LOG_ERROR zhenxi::LogMessage(ANDROID_LOG_ERROR, __LINE__)
#define LOG_DEBUG zhenxi::LogMessage(ANDROID_LOG_DEBUG, __LINE__)

//#endif


#define TAG "Zhenxi"
#define LOG(severity) LOG_ ## severity.stream()

#define LOG_QFATAL LOG_FATAL


#define VLOG(x) if ((x) > 0) {} else LOG_INFO.stream() // NOLINT

namespace zhenxi {

    template<typename LHS, typename RHS>
    struct EagerEvaluator {
        EagerEvaluator(LHS lhs, RHS rhs) : lhs(lhs), rhs(rhs) {}

        LHS lhs;
        RHS rhs;
    };

    template<typename LHS, typename RHS>
    EagerEvaluator<LHS, RHS> MakeEagerEvaluator(LHS lhs, RHS rhs) {
        return EagerEvaluator<LHS, RHS>(lhs, rhs);
    }

#define EAGER_PTR_EVALUATOR(T1, T2) \
  template <> struct EagerEvaluator<T1, T2> { \
    EagerEvaluator(T1 lhs, T2 rhs) \
        : lhs(reinterpret_cast<const void*>(lhs)), \
          rhs(reinterpret_cast<const void*>(rhs)) { } \
    const void* lhs; \
    const void* rhs; \
  }

    EAGER_PTR_EVALUATOR(const char*, const char*);

    EAGER_PTR_EVALUATOR(const char*, char*);

    EAGER_PTR_EVALUATOR(char*, const char*);

    EAGER_PTR_EVALUATOR(char*, char*);

    EAGER_PTR_EVALUATOR(const unsigned char*, const unsigned char*);

    EAGER_PTR_EVALUATOR(const unsigned char*, unsigned char*);

    EAGER_PTR_EVALUATOR(unsigned char*, const unsigned char*);

    EAGER_PTR_EVALUATOR(unsigned char*, unsigned char*);

    EAGER_PTR_EVALUATOR(const signed char*, const signed char*);

    EAGER_PTR_EVALUATOR(const signed char*, signed char*);

    EAGER_PTR_EVALUATOR(signed char*, const signed char*);

    EAGER_PTR_EVALUATOR(signed char*, signed char*);

    class NOLogMessage {

    public:
        NOLogMessage() {

        }

        NOLogMessage(const NOLogMessage &) = delete;

        NOLogMessage &operator=(const NOLogMessage &) = delete;

        void Flush() {

        }

        std::ostream &stream() { return str_; }

    private:
        std::ostringstream str_;
    };


    class LogMessage {

    public:
        LogMessage(android_LogPriority type, int line) :
                flushed_(false), type__(type) {
        }

        LogMessage(const LogMessage &) = delete;

        LogMessage &operator=(const LogMessage &) = delete;

        void Flush() {
            std::string s = str_.str();
            size_t n = s.size();
#ifdef __ANDROID__


#ifdef ZHENXI_BUILD_TYPE_NOLOG

#else
            __android_log_write(type__, TAG, s.c_str());
#endif


#else
            fwrite(s.data(), 1, n, stderr);
#endif
            flushed_ = true;
        }

        virtual ~LogMessage() {
            if (!flushed_) {
                Flush();
            }
        }

        std::ostream &stream() { return str_; }

    private:
        android_LogPriority type__;
        bool flushed_;
        std::ostringstream str_;
    };


    class LogMessageFatal : public LogMessage {

    public:
        LogMessageFatal(android_LogPriority type, int line)
                : LogMessage(type, line) {

        }

        LogMessageFatal(const LogMessageFatal &) = delete;

        LogMessageFatal &operator=(const LogMessageFatal &) = delete;

        NO_RETURN ~LogMessageFatal() override {
            Flush();
            abort();
        }
    };

}

#endif  // _WHALE_BASE_LOGGING_H_
