/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_LOGGER_H
#define IVW_LOGGER_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/util/singleton.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/stringconversion.h>

#include <ostream>
#include <string>
#include <vector>

namespace inviwo {

class Processor;

enum class LogLevel : int { Info, Warn, Error };
enum class LogAudience : int { User, Developer };
enum class MessageBreakLevel : int { Off, Error, Warn, Info };

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss, LogLevel ll) {
    switch (ll) {
        case LogLevel::Info:
            ss << "Info";
            break;
        case LogLevel::Warn:
            ss << "Warn";
            break;
        case LogLevel::Error:
            ss << "Error";
            break;
    }
    return ss;
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss, LogAudience la) {
    switch (la) {
        case LogAudience::User:
            ss << "User";
            break;
        case LogAudience::Developer:
            ss << "Developer";
            break;
    }
    return ss;
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss,
                                             MessageBreakLevel ll) {
    switch (ll) {
        case MessageBreakLevel::Info:
            ss << "Info";
            break;
        case MessageBreakLevel::Warn:
            ss << "Warn";
            break;
        case MessageBreakLevel::Error:
            ss << "Error";
            break;
        case MessageBreakLevel::Off:
            ss << "Off";
            break;
    }
    return ss;
}

#define LogInfo(message)                                                                       \
    {                                                                                          \
        std::ostringstream stream__;                                                           \
        stream__ << message;                                                                   \
        inviwo::LogCentral::getPtr()->log(                                                     \
            inviwo::parseTypeIdName(std::string(typeid(this).name())), inviwo::LogLevel::Info, \
            inviwo::LogAudience::Developer, __FILE__, __FUNCTION__, __LINE__, stream__.str()); \
    }
#define LogWarn(message)                                                                       \
    {                                                                                          \
        std::ostringstream stream__;                                                           \
        stream__ << message;                                                                   \
        inviwo::LogCentral::getPtr()->log(                                                     \
            inviwo::parseTypeIdName(std::string(typeid(this).name())), inviwo::LogLevel::Warn, \
            inviwo::LogAudience::Developer, __FILE__, __FUNCTION__, __LINE__, stream__.str()); \
    }
#define LogError(message)                                                                       \
    {                                                                                           \
        std::ostringstream stream__;                                                            \
        stream__ << message;                                                                    \
        inviwo::LogCentral::getPtr()->log(                                                      \
            inviwo::parseTypeIdName(std::string(typeid(this).name())), inviwo::LogLevel::Error, \
            inviwo::LogAudience::Developer, __FILE__, __FUNCTION__, __LINE__, stream__.str());  \
    }

#define LogInfoCustom(source, message)                                                            \
    {                                                                                             \
        std::ostringstream stream__;                                                              \
        stream__ << message;                                                                      \
        inviwo::LogCentral::getPtr()->log(source, inviwo::LogLevel::Info,                         \
                                          inviwo::LogAudience::Developer, __FILE__, __FUNCTION__, \
                                          __LINE__, stream__.str());                              \
    }
#define LogWarnCustom(source, message)                                                            \
    {                                                                                             \
        std::ostringstream stream__;                                                              \
        stream__ << message;                                                                      \
        inviwo::LogCentral::getPtr()->log(source, inviwo::LogLevel::Warn,                         \
                                          inviwo::LogAudience::Developer, __FILE__, __FUNCTION__, \
                                          __LINE__, stream__.str());                              \
    }
#define LogErrorCustom(source, message)                                                           \
    {                                                                                             \
        std::ostringstream stream__;                                                              \
        stream__ << message;                                                                      \
        inviwo::LogCentral::getPtr()->log(source, inviwo::LogLevel::Error,                        \
                                          inviwo::LogAudience::Developer, __FILE__, __FUNCTION__, \
                                          __LINE__, stream__.str());                              \
    }

#define LogProcessorInfo(message)                                                             \
    {                                                                                         \
        std::ostringstream stream__;                                                          \
        stream__ << message;                                                                  \
        inviwo::LogCentral::getPtr()->logProcessor(this, inviwo::LogLevel::Info,              \
                                                   inviwo::LogAudience::User, stream__.str(), \
                                                   __FILE__, __FUNCTION__, __LINE__);         \
    }

#define LogProcessorWarn(message)                                                             \
    {                                                                                         \
        std::ostringstream stream__;                                                          \
        stream__ << message;                                                                  \
        inviwo::LogCentral::getPtr()->logProcessor(this, inviwo::LogLevel::Warn,              \
                                                   inviwo::LogAudience::User, stream__.str(), \
                                                   __FILE__, __FUNCTION__, __LINE__);         \
    }

#define LogProcessorError(message)                                                            \
    {                                                                                         \
        std::ostringstream stream__;                                                          \
        stream__ << message;                                                                  \
        inviwo::LogCentral::getPtr()->logProcessor(this, inviwo::LogLevel::Error,             \
                                                   inviwo::LogAudience::User, stream__.str(), \
                                                   __FILE__, __FUNCTION__, __LINE__);         \
    }

#define LogNetworkInfo(message)                                                             \
    {                                                                                       \
        std::ostringstream stream__;                                                        \
        stream__ << message;                                                                \
        inviwo::LogCentral::getPtr()->logNetwork(inviwo::LogLevel::Info,                    \
                                                 inviwo::LogAudience::User, stream__.str(), \
                                                 __FILE__, __FUNCTION__, __LINE__);         \
    }

#define LogNetworkWarn(message)                                                             \
    {                                                                                       \
        std::ostringstream stream__;                                                        \
        stream__ << message;                                                                \
        inviwo::LogCentral::getPtr()->logNetwork(inviwo::LogLevel::Warn,                    \
                                                 inviwo::LogAudience::User, stream__.str(), \
                                                 __FILE__, __FUNCTION__, __LINE__);         \
    }

#define LogNetworkError(message)                                                            \
    {                                                                                       \
        std::ostringstream stream__;                                                        \
        stream__ << message;                                                                \
        inviwo::LogCentral::getPtr()->logNetwork(inviwo::LogLevel::Error,                   \
                                                 inviwo::LogAudience::User, stream__.str(), \
                                                 __FILE__, __FUNCTION__, __LINE__);         \
    }

class IVW_CORE_API Logger {
public:
    Logger() = default;
    virtual ~Logger() = default;

    virtual void log(std::string logSource, LogLevel logLevel, LogAudience audience,
                     const char* file, const char* function, int line, std::string msg) = 0;

    virtual void logProcessor(Processor* processor, LogLevel level, LogAudience audience,
                              std::string msg, const char* file, const char* function, int line);

    virtual void logNetwork(LogLevel level, LogAudience audience, std::string msg, const char* file,
                            const char* function, int line);

    virtual void logAssertion(const char* file, const char* function, int line, std::string msg);
};

class IVW_CORE_API LogCentral : public Singleton<LogCentral> {
public:
    LogCentral();
    virtual ~LogCentral() = default;

    void setLogLevel(LogLevel logLevel) { logLevel_ = logLevel; }
    LogLevel getLogLevel() { return logLevel_; }

    /**
     * \brief Register logger for use. LogCentral does not take ownership
     * of registered loggers.
     * @param logger Logger to register.
     */
    void registerLogger(std::weak_ptr<Logger> logger);

    void log(std::string source, LogLevel level, LogAudience audience, const char* file,
             const char* function, int line, std::string msg);

    void logProcessor(Processor* processor, LogLevel level, LogAudience audience, std::string msg,
                      const char* file = "", const char* function = "", int line = 0);

    void logNetwork(LogLevel level, LogAudience audience, std::string msg, const char* file = "",
                    const char* function = "", int line = 0);

    void logAssertion(const char* file, const char* function, int line, std::string msg);

    void setLogStacktrace(const bool& logStacktrace = true);
    bool getLogStacktrace() const;

    void setMessageBreakLevel(MessageBreakLevel level);
    MessageBreakLevel getMessageBreakLevel() const;

private:
    friend Singleton<LogCentral>;
    static LogCentral* instance_;

    LogLevel logLevel_;
#include <warn/push>
#include <warn/ignore/dll-interface>
    std::vector<std::weak_ptr<Logger>> loggers_;
#include <warn/pop>
    bool logStacktrace_ = false;
    MessageBreakLevel breakLevel_ = MessageBreakLevel::Off;
};

namespace util {

IVW_CORE_API void log(ExceptionContext context, std::string message,
                      LogLevel level = LogLevel::Info,
                      LogAudience audience = LogAudience::Developer);

}  // namespace util

}  // namespace inviwo

#endif  // IVW_LOGGER_H
