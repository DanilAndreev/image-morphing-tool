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

Application::Application(int argc, char *argv[]): registeredTools{} {
    this->qApplication = new QApplication{argc, argv};
    this->mainWindow = new MainWindow{};
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
    delete this->qApplication;
    delete this->mainWindow;
    for (const auto& snapshot : this->history) {
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
    this->emit_event("snapshot_create", event);
    this->history.emplace_front(event.snapshot);
    return event.snapshot;
}

void Application::rollbackToSnapshot(const history_t::iterator& snapshot) {
    this->history.erase(this->history.begin(), snapshot);
    SnapshotRestoreEvent event{*snapshot};
    this->emit_event("snapshot_restore", event);
}
