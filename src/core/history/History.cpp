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
#include "events/SnapshotCreatedEvent.h"

const char *History::SNAPSHOT_CREATE_EVENT = "snapshot_create";
const char *History::SNAPSHOT_CREATED_EVENT = "snapshot_created";
const char *History::SNAPSHOT_RESTORE_EVENT = "snapshot_restore";


History::History(Application *application) : _application(application), _position(0), storageLock() {}

History::History(const History &ref) :
        std::list<Snapshot *>(ref),
        _application(ref._application),
        _position(ref._position),
        storageLock() {}

History::~History() {
    this->eraseAndFree(this->cbegin(), this->cend());
};

const Snapshot *History::makeSnapshot() {
    this->storageLock.lock();
    if (this->_maxLength && this->size() >= this->_maxLength) {
        this->erase(--this->cend(), this->cend());
    }
    SnapshotCreateEvent createEvent{new Snapshot{}};
    this->emit_event(History::SNAPSHOT_CREATE_EVENT, createEvent);
    this->eraseAndFree(this->cbegin(), History::moveIterator(this->cbegin(), this->_position));
    this->emplace_front(createEvent.snapshot);
    this->_position = 0;
    SnapshotCreatedEvent createdEvent{createEvent.snapshot};
    this->emit_event(History::SNAPSHOT_CREATED_EVENT, createEvent);
    this->storageLock.unlock();
    return createEvent.snapshot;
}

bool History::moveToSnapshot(std::size_t position) {
    if (position == 0) return false;
    if (position > this->size()) return false;
    if (!this->_position) this->makeSnapshot();
    this->storageLock.lock();
    this->_position = position;
    SnapshotRestoreEvent event{*History::moveIterator(this->cbegin(), --position)};
    this->emit_event(History::SNAPSHOT_RESTORE_EVENT, event);
    this->_application->getMainWindow().getViewport()->update();
    this->storageLock.unlock();
    return true;
}

bool History::undo() {
    std::size_t increment = 1;
    if (!this->_position) ++increment;
    return this->moveToSnapshot(this->_position + increment);
}

bool History::redo() {
    return this->moveToSnapshot(this->_position - 1);
}

History::const_iterator History::moveIterator(History::const_iterator iter, std::size_t shift) noexcept {
    for (std::size_t i = 0; i < shift; i++) {
        ++iter;
    }
    return iter;
}

void History::setMaxLength(const std::size_t &length) noexcept {
    this->storageLock.lock();
    this->_maxLength = length ? length + 1 : 0;
    this->storageLock.unlock();
}

std::size_t History::maxLength() const noexcept {
    return this->_maxLength ? this->_maxLength - 1 : 0;
}

void History::eraseAndFree(const History::const_iterator &begin, const History::const_iterator &end) noexcept {
    for (auto it = begin; it != end; ++it) {
        try {
            delete *it;
        } catch (std::exception &e) {
            qDebug() << "An error occurred while deleting history snapshot item: " << e.what() << Qt::endl;
            //TODO: handle;
        }
    }
    this->erase(begin, end);
}

void History::reset() {
    this->storageLock.lock();
    this->_position = 0;
    this->clear();
    this->storageLock.unlock();
}
const Snapshot* History::last() noexcept {
    if (this->empty()) return nullptr;
    auto iter = this->crbegin();
    return *iter;
}
