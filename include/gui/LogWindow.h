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

#ifndef IMAGE_MORPHING_TOOL_LOGWINDOW_H
#define IMAGE_MORPHING_TOOL_LOGWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QColor>
#include <event_emitter.h>

#include "events/LogEvent.h"

class Application;

class LogWindow : public QWidget {
protected:
    static const QColor FATAL_ERROR_FONT_COLOR;
    static const QColor ERROR_FONT_COLOR;
    static const QColor WARNING_FONT_COLOR;
    static const QColor INFO_FONT_COLOR;
    static const QColor DEBUG_FONT_COLOR;
    static const QColor SILLY_FONT_COLOR;
private:
    events::event_emitter::event_handler_t eventLogCallback;
protected:
    Application* _application;
    QTextEdit* _textEdit;
public:
    LogWindow(Application* application, QWidget* parent = nullptr);
    ~LogWindow() override;
protected:
    virtual void handleLogEvent(LogEvent& event) noexcept;
};


#endif //IMAGE_MORPHING_TOOL_LOGWINDOW_H
