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

#include "core/Tool/ToolViewportEvents.h"

ToolViewportEvents::ToolViewportEvents() {
    this->eventPaintCallback = [this](events::event_base &event) {
        this->paintEventHandler(dynamic_cast<VPaintEvent &>(event));
    };
    this->eventMouseMoveCallback = [this](events::event_base &event) {
        this->mouseMoveEventHandler(dynamic_cast<VMouseEvent &>(event));
    };
    this->eventMousePressCallback = [this](events::event_base &event) {
        this->mousePressEventHandler(dynamic_cast<VMouseEvent &>(event));
    };
    this->eventMouseReleaseCallback = [this](events::event_base &event) {
        this->mouseReleaseEventHandler(dynamic_cast<VMouseEvent &>(event));
    };

    this->eventKeyPressCallback = [this](events::event_base &event) {
        this->keyPressEventHandler(dynamic_cast<VKeyEvent &>(event));
    };
    this->eventKeyReleaseCallback= [this](events::event_base &event) {
        this->keyReleaseEventHandler(dynamic_cast<VKeyEvent &>(event));
    };

}

void ToolViewportEvents::initialize(Application *application) {
    Viewport* viewport = application->getMainWindow().getViewport();
    viewport->add_listener(Viewport::PAINT_EVENT, &this->eventPaintCallback);

    viewport->add_listener(Viewport::MOUSE_MOVE_EVENT, &this->eventMouseMoveCallback);
    viewport->add_listener(Viewport::MOUSE_PRESS_EVENT, &this->eventMousePressCallback);
    viewport->add_listener(Viewport::MOUSE_RELEASE_EVENT, &this->eventMouseReleaseCallback);

    viewport->add_listener(Viewport::KEY_PRESS_EVENT, &this->eventKeyPressCallback);
    viewport->add_listener(Viewport::KEY_RELEASE_EVENT, &this->eventKeyReleaseCallback);
}

void ToolViewportEvents::uninitialize(Application *application) noexcept {
    Viewport* viewport = application->getMainWindow().getViewport();
    viewport->remove_listener(Viewport::PAINT_EVENT, &this->eventPaintCallback);

    viewport->remove_listener(Viewport::MOUSE_MOVE_EVENT, &this->eventMouseMoveCallback);
    viewport->remove_listener(Viewport::MOUSE_PRESS_EVENT, &this->eventMousePressCallback);
    viewport->remove_listener(Viewport::MOUSE_RELEASE_EVENT, &this->eventMouseReleaseCallback);

    viewport->remove_listener(Viewport::KEY_PRESS_EVENT, &this->eventKeyPressCallback);
    viewport->remove_listener(Viewport::KEY_RELEASE_EVENT, &this->eventKeyReleaseCallback);
}
