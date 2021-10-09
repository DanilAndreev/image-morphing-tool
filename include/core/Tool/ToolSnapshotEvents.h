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

#ifndef IMAGE_MORPHING_TOOL_TOOLSNAPSHOTEVENT_H
#define IMAGE_MORPHING_TOOL_TOOLSNAPSHOTEVENT_H

#include "ITool.h"
#include "event_emitter.h"

#include "events/SnapshotCreateEvent.h"
#include "events/SnapshotRestoreEvent.h"

class ToolSnapshotEvents : public virtual ITool {
    events::event_emitter::event_handler_t eventSnapshotRestoreCallback;
    events::event_emitter::event_handler_t eventSnapshotCreateCallback;
public:
    ToolSnapshotEvents();
    ToolSnapshotEvents(const ToolSnapshotEvents& reference) : ToolSnapshotEvents() {}
    ~ToolSnapshotEvents() = default;
protected:
    virtual void snapshotCreateEventHandler(SnapshotCreateEvent& event) {};
    virtual void snapshotRestoreEventHandler(SnapshotRestoreEvent& event) {};
public:
    virtual void initialize(Application *application) override;
    virtual void uninitialize(Application *application) noexcept override;
};

#endif //IMAGE_MORPHING_TOOL_TOOLSNAPSHOTEVENT_H
