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

class Application: public events::event_emitter {
public:
    static const char* SNAPSHOT_CREATE_EVENT;
    static const char* SNAPSHOT_RESTORE_EVENT;
public:
    using history_t = std::list<Snapshot*>;
protected:
    QApplication* qApplication;
    MainWindow* mainWindow;
    std::set<ITool*> registeredTools;
    history_t _history;
public:
    Application(QApplication* qApplication);
    Application(const Application&) = delete;
    virtual ~Application();
public:
    const Snapshot* makeSnapshot();
    void rollbackToSnapshot(history_t::const_iterator snapshot);
public:
    Application& registerTool(ITool* tool) noexcept;
public:
    Application& showGUI() noexcept;
    int exec();
public:
    MainWindow& getMainWindow() const;
    const history_t& history() const noexcept;
};


#endif //IMAGE_MORPHING_TOOL_APPLICATION_H
