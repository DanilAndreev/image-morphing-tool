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

#include "core/Tool/ToolSnapshotEvents.h"
#include "Application.h"

ToolSnapshotEvents::ToolSnapshotEvents() {
    this->eventSnapshotCreateCallback = [this](events::event_base &event) {
        this->snapshotCreateEventHandler(dynamic_cast<SnapshotCreateEvent &>(event));
    };
    this->eventSnapshotRestoreCallback = [this](events::event_base &event) {
        this->snapshotRestoreEventHandler(dynamic_cast<SnapshotRestoreEvent &>(event));
    };
}

void ToolSnapshotEvents::initialize(Application *application) {
    History& history = application->history();
    history.add_listener(History::SNAPSHOT_CREATE_EVENT, &this->eventSnapshotCreateCallback);
    history.add_listener(History::SNAPSHOT_RESTORE_EVENT, &this->eventSnapshotRestoreCallback);
}

void ToolSnapshotEvents::uninitialize(Application *application) noexcept {
    History& history = application->history();
    history.remove_listener(History::SNAPSHOT_CREATE_EVENT, &this->eventSnapshotCreateCallback);
    history.remove_listener(History::SNAPSHOT_RESTORE_EVENT, &this->eventSnapshotRestoreCallback);
}
