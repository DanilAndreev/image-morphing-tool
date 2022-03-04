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

#include "core/Tool/ToolCanvasEvents.h"

ToolCanvasEvents::ToolCanvasEvents() {
    this->eventCanvasPaintCallback = [this](events::event_base &event) {
        this->canvasPaintEventHandler(dynamic_cast<CPaintEvent &>(event));
    };
}

void ToolCanvasEvents::initialize(Application *application) {
    Canvas* canvas = application->getMainWindow().getViewport()->canvas();
    canvas->add_listener(Canvas::PAINT_EVENT, &this->eventCanvasPaintCallback);
}

void ToolCanvasEvents::uninitialize(Application *application) noexcept {
    Canvas* canvas = application->getMainWindow().getViewport()->canvas();
    canvas->remove_listener(Canvas::PAINT_EVENT, &this->eventCanvasPaintCallback);
}
