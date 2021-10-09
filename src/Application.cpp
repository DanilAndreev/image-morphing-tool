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

#include "Application.h"

#include <QApplication>

#include "events/SnapshotCreateEvent.h"
#include "events/SnapshotRestoreEvent.h"

const char* Application::SNAPSHOT_CREATE_EVENT = "snapshot_create";
const char* Application::SNAPSHOT_RESTORE_EVENT = "snapshot_restore";


Application::Application(QApplication* qApplication): registeredTools{}, qApplication(qApplication) {
    this->mainWindow = new MainWindow{this};
}

Application &Application::showGUI() noexcept {
    this->mainWindow->resize(800, 600);
    this->mainWindow->show();
    return *this;
}

int Application::exec() {
    return this->qApplication->exec();
}

Application::~Application() {
    for (ITool* tool : this->registeredTools) {
        tool->uninitialize(this);
    }
    delete this->mainWindow;
    for (const auto& snapshot : this->_history) {
        delete snapshot;
    }
}

Application &Application::registerTool(ITool *tool) noexcept {
    tool->initialize(this);
    this->registeredTools.insert(tool);
    return *this;
}

MainWindow& Application::getMainWindow() const {
    return *mainWindow;
}

const Snapshot *Application::makeSnapshot() {
    SnapshotCreateEvent event{new Snapshot{}};
    this->emit_event(Application::SNAPSHOT_CREATE_EVENT, event);
    this->_history.emplace_front(event.snapshot);
    return event.snapshot;
}

void Application::rollbackToSnapshot(history_t::const_iterator snapshot) {
    if (snapshot == this->_history.end()) {
        //TODO: throw an exception;
        return;
    }
    SnapshotRestoreEvent event{*snapshot};
    this->emit_event(Application::SNAPSHOT_RESTORE_EVENT, event);
    this->_history.erase(this->_history.cbegin(), ++snapshot);
    this->mainWindow->getViewport()->update();
}

const Application::history_t &Application::history() const noexcept {
    return this->_history;
}
