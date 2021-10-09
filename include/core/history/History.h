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

#ifndef IMAGE_MORPHING_TOOL_HISTORY_H
#define IMAGE_MORPHING_TOOL_HISTORY_H

#include <list>
#include <mutex>

#include <event_emitter.h>

#include "Snapshot.h"

class Application;

class History : protected std::list<Snapshot*>, public events::event_emitter {
public:
    using storage_t = std::list<Snapshot*>;
public:
    static const char* SNAPSHOT_CREATE_EVENT;
    static const char* SNAPSHOT_CREATED_EVENT;
    static const char* SNAPSHOT_RESTORE_EVENT;
private:
    std::size_t _maxLength = 0;
    std::mutex storageLock;
protected:
    /// maxLength - maximal history length. If equals 0 - length is unlimited.
    Application* const _application;
    /// _position - current active snapshot. If 0 - then current state is forward than first snapshot.
    std::size_t _position;
public:
    History(Application* application);
    History(const History& reference);
    virtual ~History();
public:
    using storage_t::cbegin;
    using storage_t::cend;
    using storage_t::size;
public:
    void setMaxLength(const std::size_t& length) noexcept;
    [[nodiscard]] std::size_t maxLength() const noexcept;
public:
    const Snapshot* makeSnapshot();
    bool moveToSnapshot(std::size_t position);
    bool undo();
    bool redo();
private:
    static inline History::const_iterator moveIterator(History::const_iterator iter, std::size_t shift, bool forward = true) noexcept;
};


#endif //IMAGE_MORPHING_TOOL_HISTORY_H
