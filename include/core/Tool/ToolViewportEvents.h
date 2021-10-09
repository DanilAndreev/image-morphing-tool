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

#ifndef IMAGE_MORPHING_TOOL_TOOLVIEWPORTEVENTS_H
#define IMAGE_MORPHING_TOOL_TOOLVIEWPORTEVENTS_H

#include "ITool.h"
#include "Application.h"
#include "event_emitter.h"
#include "../../events/VMouseEvent.h"
#include "../../events/VPaintEvent.h"
#include "../../events/VKeyEvent.h"

class ToolViewportEvents : public virtual ITool {
private:
    events::event_emitter::event_handler_t eventPaintCallback;

    events::event_emitter::event_handler_t eventMousePressCallback;
    events::event_emitter::event_handler_t eventMouseReleaseCallback;
    events::event_emitter::event_handler_t eventMouseMoveCallback;

    events::event_emitter::event_handler_t eventKeyPressCallback;
    events::event_emitter::event_handler_t eventKeyReleaseCallback;
public:
    ToolViewportEvents();
    ToolViewportEvents(const ToolViewportEvents& ref) : ToolViewportEvents() {}
    ~ToolViewportEvents() = default;
protected:
    virtual void paintEventHandler(VPaintEvent& event) {};

    virtual void mouseMoveEventHandler(VMouseEvent& event) {};
    virtual void mousePressEventHandler(VMouseEvent& event) {};
    virtual void mouseReleaseEventHandler(VMouseEvent& event) {};

    virtual void keyPressEventHandler(VKeyEvent& event) {};
    virtual void keyReleaseEventHandler(VKeyEvent& event) {};
public:
    void initialize(Application *application) override;
    void uninitialize(Application *application) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_TOOLVIEWPORTEVENTS_H
