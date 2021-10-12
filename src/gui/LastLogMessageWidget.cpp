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

#include "gui/LastLogMessageWidget.h"
#include "Application.h"

#include <QTextEdit>

const char* LastLogMessageWidget::BACKGROUND_FATAL_ERROR_STYLE = "background: #f00;";
const char* LastLogMessageWidget::BACKGROUND_ERROR_STYLE = "background: #f00;";
const char* LastLogMessageWidget::BACKGROUND_WARNING_STYLE = "background: #ff0;";
const char* LastLogMessageWidget::BACKGROUND_INFO_STYLE = "background: none;";

LastLogMessageWidget::LastLogMessageWidget(Application *application, QWidget *parent)
        : QLineEdit(parent), _application(application) {
    this->setReadOnly(true);
    this->eventSnapshotCreateCallback = [this](events::event_base &event) {
        this->handleLogEvent(dynamic_cast<LogEvent &>(event));
    };
    this->_application->add_listener(Application::LOG_EVENT, &this->eventSnapshotCreateCallback);
}

LastLogMessageWidget::~LastLogMessageWidget() {
    this->_application->remove_listener(Application::LOG_EVENT, &this->eventSnapshotCreateCallback);
}

void LastLogMessageWidget::handleLogEvent(LogEvent &event) noexcept {
    this->_message = event.message();
    this->_level = event.level();
    QString prefix{};
    switch (this->_level) {
        case LogEvent::LOG_LEVEL::FATAL_ERROR:
            this->appendStylesheet(LastLogMessageWidget::BACKGROUND_FATAL_ERROR_STYLE);
            prefix = "Fatal Error: ";
            break;
        case LogEvent::LOG_LEVEL::ERROR:
            this->appendStylesheet(LastLogMessageWidget::BACKGROUND_ERROR_STYLE);
            prefix = "Error: ";
            break;
        case LogEvent::LOG_LEVEL::WARNING:
            this->appendStylesheet(LastLogMessageWidget::BACKGROUND_WARNING_STYLE);
            prefix = "Warning: ";
            break;
        case LogEvent::LOG_LEVEL::INFO:
            this->appendStylesheet(LastLogMessageWidget::BACKGROUND_INFO_STYLE);
            prefix = "Info: ";
            break;
        default:
            break;
    }
    this->setText(prefix + this->_message);
}

void LastLogMessageWidget::appendStylesheet(const char* stylesheet) noexcept {
    this->setStyleSheet(this->styleSheet().append(stylesheet));
};
