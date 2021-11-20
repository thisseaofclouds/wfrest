//
// Created by Chanchan on 11/18/21.
//

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <functional>

#include "LogStream.h"
#include "Timestamp.h"

namespace wfrest
{
class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    // Two std::funcion set output location
    using OutputFunc = std::function<void(const char *msg, int len)>;
    using FlushFunc = std::function<void()>;

    static void set_log_output(const OutputFunc &output_func, const FlushFunc &flush_func);

    static void set_log_output(OutputFunc &&output_func, FlushFunc &&flush_func);

    // compile time calculation of basename of source file
    class SourceFile
    {
    public:
        template<int N>
        explicit SourceFile(const char (&arr)[N]);

        explicit SourceFile(const char *filename);

    public:
        const char *data_;
        int size_;
    };

public:
    Logger(SourceFile file, int line);

    Logger(SourceFile file, int line, LogLevel level);

    Logger(SourceFile file, int line, LogLevel level, const char *func);

    Logger(SourceFile file, int line, bool toAbort);

    ~Logger();

    LogStream &stream()
    { return impl_.stream_; }

    static LogLevel log_level()
    { return log_level_(); }

    static void set_log_level(LogLevel level)
    { log_level_() = level; }

protected:
    static LogLevel &log_level_();

    // default write to stdout
    // Can set to file
    static void default_output(const char *msg, int len);

    static void default_flush();

    static OutputFunc &output_func_();

    static FlushFunc &flush_func_();

private:
    class Impl
    {
    public:
        using LogLevel = Logger::LogLevel;

        Impl(LogLevel level, int old_errno, const SourceFile &file, int line);

        void formatTime();

        void finish();

    public:
        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    } impl_;
};


#define LOG_TRACE if (wfrest::Logger::logLevel() <= wfrest::Logger::TRACE) \
  wfrest::Logger(__FILE__, __LINE__, wfrest::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (wfrest::Logger::logLevel() <= wfrest::Logger::DEBUG) \
  wfrest::Logger(__FILE__, __LINE__, wfrest::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (wfrest::Logger::logLevel() <= wfrest::Logger::INFO) \
  wfrest::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN wfrest::Logger(__FILE__, __LINE__, wfrest::Logger::WARN).stream()
#define LOG_ERROR wfrest::Logger(__FILE__, __LINE__, wfrest::Logger::ERROR).stream()
#define LOG_FATAL wfrest::Logger(__FILE__, __LINE__, wfrest::Logger::FATAL).stream()
#define LOG_SYSERR wfrest::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL wfrest::Logger(__FILE__, __LINE__, true).stream()

// note: LOG_TRACE << "the server has read the client message";
// equals wfrest::Logger(__FILE__,__LINE__,wfrest::Logger::TRACE,__func__).stream()<<"the server has read the client message";

}  // namespace wfrest



#endif //_LOGGER_H_