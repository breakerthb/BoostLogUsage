#include <stdarg.h>
#include <iostream>
#include "logger.h"

#include "boost/log/common.hpp"
#include <boost/log/expressions.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>

#include <boost/log/sources/logger.hpp>

#include <boost/log/support/date_time.hpp>

using namespace std;

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

// Here we define our application severity levels.
enum severity_level
{
    normal,
    info,
    warning,
    error,
    debug,
    critical
};

// The formatting logic for the severity level
template<typename CharT, typename TraitsT>
inline basic_ostream<CharT, TraitsT>& operator << (
    basic_ostream<CharT, TraitsT>& strm, severity_level lvl)
{
    static const char* const str[] =
    {
        "normal",
        " info",
        " warn",
        "error",
        "debug",
        "critical"
    };
    if (static_cast<size_t>(lvl) < (sizeof(str) / sizeof(*str)))
    {
        strm << str[lvl];
    }
    else
    {
        strm << static_cast<int>(lvl);
    }
    return strm;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(_severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(_timestamp, "TimeStamp", boost::posix_time::ptime)

#define DEF_LOG_NAME "BoostLog.log"

class Logger
{
public:
    Logger(void);
    ~Logger(void);
    void Init();
    void Init(string strLogName);
    void Start();
    void Stop();

private:
    string m_strLogFileName;
};


#define MAX_MESSAGE_LEN					2048

Logger::Logger(void)
{
    this->m_strLogFileName = DEF_LOG_NAME;
}

Logger::~Logger(void)
{
}

void Logger::Init(string strLogName)
{
    m_strLogFileName = strLogName;

    auto pSink = logging::add_file_log
        (
        m_strLogFileName.c_str(),
        keywords::filter = expr::attr< severity_level >("Severity") >= info,
        keywords::format = expr::stream
        << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
        << " <" << expr::attr< severity_level >("Severity") << ">"
        << " <" << expr::attr< boost::thread::id >("ThreadID") << "> "
        << expr::message
        );

    // Also let's add some commonly used attributes, like timestamp and record counter.
    logging::add_common_attributes();
    pSink->locked_backend()->auto_flush(true);
}

void Logger::Init()
{
    this->Init(DEF_LOG_NAME);
}

void Logger::Start()
{
    logging::core::get()->set_logging_enabled(true);
}

void Logger::Stop()
{
    logging::core::get()->set_logging_enabled(false);
}

/////////////////////////////////////////////////////////////////
/// Implement APIs
///
Logger* g_pLogger = NULL;
int LogInit(const char* strLogName)
{
    if (g_pLogger != NULL)
    {
        g_pLogger->Stop();
        delete g_pLogger;
    }

    g_pLogger = new Logger();
    g_pLogger->Init(strLogName);
    return 0;
}

int LogInit()
{
    LogInit("MyLog.log");
    return 0;
}

int LogDeInit()
{
    if (g_pLogger != NULL)
    {
        delete g_pLogger;
    }
    return 0;
}

int log_error(const char *function_name, const char *msg, ...)
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    src::severity_logger< severity_level > slg;
    char error_msg[MAX_MESSAGE_LEN + 1] = "";
    va_list args;
    va_start(args, msg);
    vsnprintf(error_msg, MAX_MESSAGE_LEN, msg, args);
    va_end(args);

    string str(function_name);
    str.append(": ");
    str.append(error_msg);
    BOOST_LOG_SEV(slg, error) << str;
    return 0;
}
int log_info(const char *function_name, const char *msg, ...)
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    src::severity_logger< severity_level > slg;
    char error_msg[MAX_MESSAGE_LEN + 1] = "";
    va_list args;
    va_start(args, msg);
    vsnprintf(error_msg, MAX_MESSAGE_LEN, msg, args);
    va_end(args);

    std::string str(function_name);
    str.append(": ");
    str.append(error_msg);
    BOOST_LOG_SEV(slg, info) << str;
    return 0;
}
int log_warning(const char *function_name, const char *msg, ...)
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    src::severity_logger< severity_level > slg;
    char error_msg[MAX_MESSAGE_LEN + 1] = "";
    va_list args;
    va_start(args, msg);
    vsnprintf(error_msg, MAX_MESSAGE_LEN, msg, args);
    va_end(args);

    std::string str(function_name);
    str.append(": ");
    str.append(error_msg);
    BOOST_LOG_SEV(slg, warning) << str;
    return 0;
}

bool debug_enable = false;
int log_debug(const char* function_name, const char* msg, ...)
{
    if (!debug_enable)
    {
        return 0;
    }

    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    src::severity_logger< severity_level > slg;
    char error_msg[MAX_MESSAGE_LEN + 1] = "";
    va_list args;
    va_start(args, msg);
    vsnprintf(error_msg, MAX_MESSAGE_LEN, msg, args);
    va_end(args);

    std::string str;
    if (function_name != NULL)
    {
        str.append(function_name);
        str.append(": ");
    }
    str.append(error_msg);
    BOOST_LOG_SEV(slg, debug) << str;
    return 0;
}

void LogStart()
{
    g_pLogger->Start();
}
void LogStop()
{
    g_pLogger->Stop();
}

void LogEnableDebug()
{
    debug_enable = true;
}

