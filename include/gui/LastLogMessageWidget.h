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

#ifndef IMAGE_MORPHING_TOOL_LASTLOGMESSAGEWIDGET_H
#define IMAGE_MORPHING_TOOL_LASTLOGMESSAGEWIDGET_H

#include <QLineEdit>
#include <QString>
#include <event_emitter.h>

#include "events/LogEvent.h"

class Application;

class LastLogMessageWidget : public QLineEdit {
private:
    static const char* BACKGROUND_FATAL_ERROR_STYLE;
    static const char* BACKGROUND_ERROR_STYLE;
    static const char* BACKGROUND_WARNING_STYLE;
    static const char* BACKGROUND_INFO_STYLE;
private:
    events::event_emitter::event_handler_t eventLogCallback;
protected:
    Application* _application;
    QString _message;
    LogEvent::LOG_LEVEL _level;
public:
    explicit LastLogMessageWidget(Application* application, QWidget* parent = nullptr);
    ~LastLogMessageWidget() override;
protected:
    virtual void handleLogEvent(LogEvent& event) noexcept;
private:
    inline void appendStylesheet(const char* stylesheet) noexcept;
};


#endif //IMAGE_MORPHING_TOOL_LASTLOGMESSAGEWIDGET_H
