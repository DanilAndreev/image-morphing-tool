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

#ifndef IMAGE_MORPHING_TOOL_TOOLCANVASEVENTS_H
#define IMAGE_MORPHING_TOOL_TOOLCANVASEVENTS_H

#include "ITool.h"
#include "Application.h"
#include "event_emitter.h"
#include "../../events/CPaintEvent.h"

class ToolCanvasEvents : public virtual ITool {
private:
    events::event_emitter::event_handler_t eventCanvasPaintCallback;
public:
    ToolCanvasEvents();
    ToolCanvasEvents(const ToolCanvasEvents& ref) : ToolCanvasEvents() {}
    ~ToolCanvasEvents() = default;
protected:
    virtual void canvasPaintEventHandler(CPaintEvent& event) {};
public:
    void initialize(Application *application) override;
    void uninitialize(Application *application) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_TOOLCANVASEVENTS_H
