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

#ifndef IMAGE_MORPHING_TOOL_SNAPSHOTCREATEDEVENT_H
#define IMAGE_MORPHING_TOOL_SNAPSHOTCREATEDEVENT_H

#include <event_emitter.h>
#include "core/history/Snapshot.h"

class SnapshotCreatedEvent: public events::event_base {
public:
    const Snapshot *const snapshot;
public:
    explicit SnapshotCreatedEvent(const Snapshot* snapshot) noexcept:  events::event_base(), snapshot(snapshot) {}
    SnapshotCreatedEvent(const SnapshotCreateEvent& reference) noexcept: events::event_base(reference), snapshot(reference.snapshot) {}
    ~SnapshotCreatedEvent() override = default;
};


#endif //IMAGE_MORPHING_TOOL_SNAPSHOTCREATEDEVENT_H
