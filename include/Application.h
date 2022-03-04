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

#ifndef IMAGE_MORPHING_TOOL_APPLICATION_H
#define IMAGE_MORPHING_TOOL_APPLICATION_H

#include <set>
#include <list>

#include <QApplication>
#include <event_emitter.h>

#include "core/history/Snapshot.h"
#include "gui/MainWindow.h"
#include "core/Tool/ITool.h"
#include "core/history/History.h"
#include "events/LogEvent.h"
#include "core/drawables/Document.h"

class Application: public events::event_emitter {
public:
    static const char* LOG_EVENT;
protected:
    QApplication* qApplication;
    MainWindow* mainWindow;
    std::set<ITool*> registeredTools;
    History _history;
    Document* _document;
public:
    explicit Application(QApplication* qApplication);
    Application(const Application&) = delete;
    virtual ~Application();
public:
    Application& registerTool(ITool* tool) noexcept;
public:
    void log(const QString& message, LogEvent::LOG_LEVEL level);
public:
    Application& showGUI() noexcept;
    int exec();
public:
    [[nodiscard]] MainWindow& getMainWindow() const;
    [[nodiscard]] History& history() noexcept;
    [[nodiscard]] Document* document() noexcept;
};


#endif //IMAGE_MORPHING_TOOL_APPLICATION_H
