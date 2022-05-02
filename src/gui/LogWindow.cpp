// Copyright (c) 2021-2021.
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

#include "gui/LogWindow.h"

#include <QBoxLayout>
#include <QDateTime>

#include "Application.h"

const QColor LogWindow::FATAL_ERROR_FONT_COLOR = {"#f00"};
const QColor LogWindow::ERROR_FONT_COLOR = {"#c00"};
const QColor LogWindow::WARNING_FONT_COLOR = {"#ff0"};
const QColor LogWindow::INFO_FONT_COLOR = {"#0f0"};
const QColor LogWindow::DEBUG_FONT_COLOR = {"#00f"};
const QColor LogWindow::SILLY_FONT_COLOR = {"#000"};

LogWindow::LogWindow(Application *application, QWidget *parent) : QWidget(parent), _application(application) {
    this->eventLogCallback = [this](events::event_base &event) {
        this->handleLogEvent(dynamic_cast<LogEvent &>(event));
    };
    this->setWindowIcon(QIcon{"icons/TerminalIcon.png"});
    this->setWindowTitle("Console");

    this->_application->add_listener(Application::LOG_EVENT, &this->eventLogCallback);
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);

    this->_textEdit = new QTextEdit();
    this->_textEdit->setReadOnly(true);
    layout->addWidget(_textEdit);
    this->setLayout(layout);
    this->resize(800, 600);
}

LogWindow::~LogWindow() {
    this->_application->remove_listener(Application::LOG_EVENT, &this->eventLogCallback);
}

void LogWindow::handleLogEvent(LogEvent &event) noexcept {
    QString prefix{};
    switch (event.level()) {
        case LogEvent::LOG_LEVEL::FATAL_ERROR:
            prefix = "Fatal Error: ";
            this->_textEdit->setTextColor(LogWindow::FATAL_ERROR_FONT_COLOR);
            break;
        case LogEvent::LOG_LEVEL::ERROR:
            prefix = "Error: ";
            this->_textEdit->setTextColor(LogWindow::ERROR_FONT_COLOR);
            break;
        case LogEvent::LOG_LEVEL::WARNING:
            prefix = "Warning: ";
            this->_textEdit->setTextColor(LogWindow::WARNING_FONT_COLOR);
            break;
        case LogEvent::LOG_LEVEL::INFO:
            prefix = "Info: ";
            this->_textEdit->setTextColor(LogWindow::INFO_FONT_COLOR);
            break;
        case LogEvent::LOG_LEVEL::DEBUG:
            prefix = "Debug: ";
            this->_textEdit->setTextColor(LogWindow::DEBUG_FONT_COLOR);
            break;
        case LogEvent::LOG_LEVEL::SILLY:
            prefix = "Silly: ";
            this->_textEdit->setTextColor(LogWindow::SILLY_FONT_COLOR);
            break;
        default:
            break;
    }
    this->_textEdit->append("[" + QDateTime::currentDateTime().toString() + "] " + prefix + event.message());
}
