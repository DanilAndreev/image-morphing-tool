// Copyright (c) 2021.
// License: CC0 1.0 Universal
// Permissions:
// - Commercial use
// - Modification
// - Distribution
// - Private use
//
// Limitations:
// - Liability
// - Trademark use
// - Patent use
// - Warranty
//
// Author: Danil Andreev | danssg08@gmail.com | https://github.com/DanilAndreev

#ifndef IMAGE_MORPHING_TOOL_LOGEVENT_H
#define IMAGE_MORPHING_TOOL_LOGEVENT_H

#include <QString>
#include <utility>
#include <event_emitter.h>

class LogEvent : public events::event_base {
public:
    enum class LOG_LEVEL {
        FATAL_ERROR = 0,
        ERROR = 1,
        WARNING = 2,
        INFO = 3,
        DEBUG = 4,
        SILLY = 5,
    };
protected:
    const QString _message;
    LOG_LEVEL _level;
public:
    explicit LogEvent(QString message, LOG_LEVEL level = LOG_LEVEL::SILLY) noexcept: events::event_base(),
                                                                                     _message(std::move(message)),
                                                                                     _level(level) {}

    LogEvent(const LogEvent &reference) noexcept: events::event_base(reference),
                                                             _message(reference._message),
                                                             _level(reference._level) {}
    ~LogEvent() override = default;
public:
    const QString& message() const noexcept { return this->_message; }
    const LOG_LEVEL& level() const noexcept { return this->_level; }
};


#endif //IMAGE_MORPHING_TOOL_LOGEVENT_H
