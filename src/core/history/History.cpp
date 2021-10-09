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

#include "core/history/History.h"

#include "Application.h"
#include "events/SnapshotRestoreEvent.h"
#include "events/SnapshotCreateEvent.h"

const char* History::SNAPSHOT_CREATE_EVENT = "snapshot_create";
const char* History::SNAPSHOT_RESTORE_EVENT = "snapshot_restore";


History::History(Application* application): _application(application) {}

History::History(const History &reference): std::list<Snapshot*>(reference), _application(reference._application) {}

History::~History() {
    for (const auto& snapshot : *this) {
        delete snapshot;
    }
};

const Snapshot *History::makeSnapshot() {
    SnapshotCreateEvent event{new Snapshot{}};
    this->emit_event(History::SNAPSHOT_CREATE_EVENT, event);
    this->emplace_front(event.snapshot);
    return event.snapshot;
}

bool History::rollbackToSnapshot(History::const_iterator snapshot) {
    if (snapshot == this->cend()) {
        return false;
    }
    SnapshotRestoreEvent event{*snapshot};
    this->emit_event(History::SNAPSHOT_RESTORE_EVENT, event);
    this->erase(this->cbegin(), ++snapshot);
    this->_application->getMainWindow().getViewport()->update();
    return true;
}
