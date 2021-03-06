#include "util/Logger.h"

#include "util/LogAppenders.h"
#include "util/Stopwatch.h"
#include "util/StrUtil.h"
#include "util/stdex.h"

#include <cstring>
#include <memory>
#include <string>

using namespace std;

std::mutex Loggers::m_lock;
Loggers Log::loggers;


Loggers::Loggers()
{
    m_init = true;
    setRoot();
}

Loggers::~Loggers()
{
    clear();
    m_init = false;
}

void Loggers::clearAppenderUnchecked(LogAppender* logAppender)
{
    for (auto& logger : m_loggers) {
        logger.second->clearAppender(logAppender);
    }
}

void Loggers::clear()
{
    if (m_init) {
        lock_guard<mutex> lock(m_lock);

        // Break ties to all LogAppenders, so that destruction order of statics does not matter.
        clearAppenderUnchecked(nullptr);
        m_loggers.clear();
    }
}

void Loggers::clearAppender(LogAppender* logAppender)
{
    if (m_init) {
        lock_guard<mutex> lock(m_lock);

        clearAppenderUnchecked(logAppender);
    }
}

void Loggers::setRoot()
{
    assert(m_init);
    if (m_init) {
        auto root = std::unique_ptr<Logger>(new Logger(this, nullptr, "", 0));
        root->setLevel(Log::Warn);

        {
            lock_guard<mutex> lock(m_lock);
            m_loggers[root->getName()] = std::move(root);
        }
    }
}

Logger* Loggers::get(const char* name)
{
    if (!m_init)
        return nullptr;

    m_lock.lock();
    Logger* logger = m_loggers[name].get();
    m_lock.unlock();

    if (!logger) {
        unsigned int nameLen = strlen(name);
        const char* end;
        unsigned int searchOffset = 0;
        // Root logger should have been created in setRoot.
        assert(nameLen > 0);

        lock_guard<mutex> lock(m_lock);
        Logger* parent = m_loggers[""].get();
        assert(parent);
        do {
            unsigned int subnameLen;
            end = strchr(name + searchOffset, '.');
            if (end) {
                subnameLen = end - name;
                searchOffset = subnameLen + 1;
            } else {
                subnameLen = nameLen;
            }
            std::string subname(name, subnameLen);
            logger = m_loggers[subname].get();
            if (!logger) {
                logger = new Logger(this, parent, subname);
                m_loggers[subname] = std::unique_ptr<Logger>(logger);
            }
            parent = logger;
        } while (end);
    }
    return logger;
}

std::string Log::getLogDirectory()
{
    // TODO
    return "/tmp";
}

void Log::reset()
{
    loggers.clear();
    loggers.setRoot();
}

Logger* Log::get(const char* name)
{
    return loggers.get(name);
}

void Log::log(const char* name, Log::Level level, const char* fmt, va_list args)
{
    auto logger = get(name);
    if (logger)
        logger->log(level, fmt, args);
}

void Log::log(const char* name, Log::Level level, const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    auto logger = get(name);
    if (logger)
        logger->log(level, fmt, ap);
    va_end(ap);
}

Logger::Logger(Loggers* loggers, Logger* parent, std::string& name) :
    m_loggers(loggers),
    m_parent(parent),
    m_name(name),
    m_level(Log::Unset)
{
}

Logger::Logger(Loggers* loggers, Logger* parent, const char* name, int32_t nameLen) :
    m_loggers(loggers),
    m_parent(parent),
    m_name(name, nameLen),
    m_level(Log::Unset)
{
}

void Logger::setLevel(Log::Level level)
{
    if (m_parent || level != Log::Unset)  // Can't unset root logger
        m_level = level;
}

Log::Level Logger::getLevel() const
{
    if (m_level != Log::Unset)
        return m_level;
    return m_parent->getLevel();
}

Logger* Logger::getParent()
{
    return m_parent;
}

void Logger::setAppender(LogAppender* a)
{
    assert(a);

    lock_guard<mutex> lock(m_loggers->m_lock);
    auto iter = m_appenders.find(a);
    if (iter != m_appenders.end())
        m_appenders.erase(iter);
    m_appenders.insert(a);
    a->setLoggers(m_loggers);
}

void Logger::clearAppender(LogAppender* logAppender)
{
    if (logAppender) {
        auto iter = m_appenders.find(logAppender);
        if (iter != m_appenders.end())
            m_appenders.erase(iter);
    } else {
        for (auto a : m_appenders) {
            a->setLoggers(nullptr);
        }
        m_appenders.clear();
    }
}

void Logger::append(Log::Level level, const std::string& s)
{
    {
        lock_guard<mutex> lock(m_loggers->m_lock);
        for (auto a : m_appenders) {
            a->append(s);
        }
    }

    if (m_parent)
        m_parent->append(level, s);
}

static const char levelChar[] = {
    'T',
    'D',
    'I',
    'W',
    'E',
    'F'
};

void Logger::log(Log::Level level, const char* fmt, va_list ap)
{
    static Stopwatch sw;
    try {
        if (getLevel() <= level) {
            std::string s(format("%c %" PRIu64 " %s ", levelChar[level - 1], sw.elapsedUSec() / 1000, m_name.c_str()));
            appendFormatList(s, fmt, ap);
            s += "\n";
            append(level, s);
        }
    } catch (...) {
        // Logging is best effort.
    }
}

void Logger::log(Log::Level level, const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log(level, fmt, ap);
    va_end(ap);
}
