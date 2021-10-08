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

#ifndef IMAGE_MORPHING_TOOL_SNAPSHOTCREATEEVENT_H
#define IMAGE_MORPHING_TOOL_SNAPSHOTCREATEEVENT_H

#include <event_emitter.h>
#include "core/history/Snapshot.h"

class SnapshotCreateEvent: public events::event_base {
public:
    Snapshot *const snapshot;
public:
    explicit SnapshotCreateEvent(Snapshot* snapshot) noexcept:  events::event_base(), snapshot(snapshot) {}
    SnapshotCreateEvent(const SnapshotCreateEvent& reference) noexcept: events::event_base(reference), snapshot(reference.snapshot) {}
    ~SnapshotCreateEvent() override = default;
};


#endif //IMAGE_MORPHING_TOOL_SNAPSHOTCREATEEVENT_H
