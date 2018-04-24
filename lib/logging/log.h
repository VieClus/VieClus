/*
    Bernhard Schuster
    Implementation and Evaluation of Approximation Algorithms for Noah's Ark Problems
    Master's Thesis
    University of Vienna
    2017
*/



/**
 * Taken from:
 * http://www.drdobbs.com/cpp/logging-in-c/201804215
 * Code in .zip:
 * http://www.drdobbs.com/cpp/logging-in-c/cpp/sourcecode/logging-in-c/30100117
 */

#ifndef __LOG_H__
#define __LOG_H__

#include "bexception.h"
#include <sstream>
#include <string>
#include <stdio.h>

// either set with pre-processor at compile time (-D),
// or it is set here
// or in program code: FILELog::ReportingLevel() = ...
#ifndef D_FILELOG_MAX_LEVEL
#define D_FILELOG_MAX_LEVEL logDEBUG4
#endif

/// Log to file, if "level" is less or equal to the logging level
/// This define should be optimised out by the compiler,
/// if "level" is greater than D_FILELOG_MAX_LEVEL
#define FILE_LOG(level) \
    if (level > D_FILELOG_MAX_LEVEL) ;\
    else if (level > FILELog::ReportingLevel() || !Output2FILE::Stream()) ; \
    else FILELog().Get(level)


/// Some defines that are used very often to reduce writing/coding effort for the programmer.
/// Log "msg" to FILE_LOG(logERROR) with info about file, line and function
#define LOG_ERR(msg) \
            FILE_LOG(logERROR) << __FILE__ <<  ":" << __LINE__ << ": " << __func__ << "(): " << msg;

/// Log "msg" to FILE_LOG(logERROR) with info about file, line and function
/// and return "ret"
#define LOG_ERR_RET(msg, ret) \
            LOG_ERR(msg); \
            return ret;

/// Log "msg" to FILE_LOG(logERROR) with info about file, line and function
/// and return "ret"
/// only if a condition is TRUE
#define LOG_ERR_RET_IF(cond, msg, ret) \
            if (cond) { LOG_ERR_RET(msg, ret); }
//#define LOG_ERROR2(msg, ret) SimpleLog::error(msg, ret, __func__, __FILE__, __LINE__)

/// Log "msg" to FILE_LOG(logWARNING) with info about file, line and function
#define LOG_WARN(msg) \
            FILE_LOG(logWARNING) << __FILE__ <<  ":" << __LINE__ << ": " << __func__ << "(): " << msg;

/// Log "msg" to FILE_LOG(logWARNING) with info about file, line and function,
/// if "cond" is TRUE
#define LOG_WARN_IF(cond, msg) \
            if (cond) { LOG_WARN(msg); }

/// Throw an exception with current file, line and function info
#define THR_EXP(msg) \
            throw BException(msg, __FILE__, __LINE__, __func__);

/// Throw an exception with current file, line and function info
/// only if a condition is TRUE
#define THR_EXP_IF(cond, msg) \
            if (cond) { THR_EXP(msg); }

/// Check Memory Allocation
/// Log bad allocation exception to FILE_LOG(logERROR) with info about file, line and function,
/// C++ throws an exception, if the memory allocation fails
#define CMA(stmt) \
    try \
    { \
        stmt; \
    } \
    catch (std::bad_alloc &exp)\
    { \
        FILE_LOG(logERROR) << __FILE__ <<  ":" << __LINE__ << ": " << __func__ << "(): " \
        << exp.what() << " " << "Memory allocation failed."; \
        throw;\
    }

#define CHECK_ERROR(code, val, msg) \
    if(code != val) { SimpleLog::error(msg, code, __func__, __FILE__, __LINE__); }
#define CHECK_ERR_RET(code, val, msg, ret) \
    if(code != val) { SimpleLog::error(msg, code, __func__, __FILE__, __LINE__); return ret; }


inline std::string NowTime();

enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4};

template <typename T>
class Log
{
    public:
        Log();
        virtual ~Log();
        std::ostringstream& Get(TLogLevel level = logINFO);
    public:
        static TLogLevel& ReportingLevel();
        static std::string ToString(TLogLevel level);
        static TLogLevel FromString(const std::string& level);
    protected:
        std::ostringstream os;
    private:
        Log(const Log&);
        Log& operator =(const Log&);
};

template <typename T>
Log<T>::Log()
{
}

template <typename T>
std::ostringstream& Log<T>::Get(TLogLevel level)
{
    os  << NowTime();
    os << " " << ToString(level) << ":\t";
    // intendation with tabs according to debug level
    os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
    return os;
}

template <typename T>
Log<T>::~Log()
{
    os << std::endl;
    T::Output(os.str());
}

template <typename T>
TLogLevel& Log<T>::ReportingLevel()
{
    static TLogLevel reportingLevel = logDEBUG4;
    return reportingLevel;
}

template <typename T>
std::string Log<T>::ToString(TLogLevel level)
{
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
    return buffer[level];
}

template <typename T>
TLogLevel Log<T>::FromString(const std::string& level)
{
    if (level == "DEBUG4")
        return logDEBUG4;
    if (level == "DEBUG3")
        return logDEBUG3;
    if (level == "DEBUG2")
        return logDEBUG2;
    if (level == "DEBUG1")
        return logDEBUG1;
    if (level == "DEBUG")
        return logDEBUG;
    if (level == "INFO")
        return logINFO;
    if (level == "WARNING")
        return logWARNING;
    if (level == "ERROR")
        return logERROR;
    Log<T>().Get(logWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return logINFO;
}


class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
    static FILE* pStream = stdout;
    return pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   if defined (BUILDING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllexport)
#   elif defined (USING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllimport)
#   else
#       define FILELOG_DECLSPEC
#   endif // BUILDING_DBSIMPLE_DLL
#else
#   define FILELOG_DECLSPEC
#endif // _WIN32


//class FILELOG_DECLSPEC FILELog : public Log<Output2FILE> {};
typedef Log<Output2FILE> FILELog;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

inline std::string NowTime()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
                       "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}

#else

#include <sys/time.h>

inline std::string NowTime()
{
    char result[100] = {0};
    char buffer[11];
    struct timeval tv;
    time_t t;
    struct tm r = {0};

    time(&t);
    strftime(buffer, 11, "%X", localtime_r(&t, &r));
    gettimeofday(&tv, 0);

    sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
    return result;
}

#endif //WIN32

#endif //__LOG_H__
