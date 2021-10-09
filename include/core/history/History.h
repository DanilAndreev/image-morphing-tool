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

#include <event_emitter.h>

#include "Snapshot.h"

class Application;

class History : protected std::list<Snapshot*>, public events::event_emitter {
public:
    using storage_t = std::list<Snapshot*>;
public:
    static const char* SNAPSHOT_CREATE_EVENT;
    static const char* SNAPSHOT_RESTORE_EVENT;
protected:
    /// maxLength - maximal history length. If equals 0 - length is unlimited.
    std::size_t maxLength = 0;
    Application* const _application;
public:
    History(Application* application);
    History(const History& reference);
    virtual ~History();
public:
    using storage_t::cbegin;
    using storage_t::cend;
public:
    const Snapshot* makeSnapshot();
    bool rollbackToSnapshot(History::const_iterator snapshot);
};


#endif //IMAGE_MORPHING_TOOL_HISTORY_H
