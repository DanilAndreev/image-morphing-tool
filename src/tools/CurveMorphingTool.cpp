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

#include "tools/CurveMorphingTool.h"

#include <QPainter>

#include "core/Stroke/StrokeManager.h"
#include "core/Stroke/StrokeDrawer.h"

CurveMorphingTool::CurveMorphingTool() noexcept: displayDirections(false) {
    this->eventPaintCallback = [this](events::event_base &event) {
        this->handlePaintEvent(dynamic_cast<VPaintEvent &>(event));
    };
    this->eventMouseMoveCallback = [this](events::event_base &event) {
        this->handleMouseMoveEvent(dynamic_cast<VMouseEvent &>(event));
    };
    this->eventMousePressCallback = [this](events::event_base &event) {
        this->handleMousePressEvent(dynamic_cast<VMouseEvent &>(event));
    };
    this->eventMouseReleaseCallback = [this](events::event_base &event) {
        this->handleMouseReleaseEvent(dynamic_cast<VMouseEvent &>(event));
    };
}

void CurveMorphingTool::initialize(Application *application) {
    application->getMainWindow().getViewport()->add_listener(Viewport::PAINT_EVENT, &this->eventPaintCallback);
    application->getMainWindow().getViewport()->add_listener(Viewport::MOUSE_MOVE_EVENT, &this->eventMouseMoveCallback);
    application->getMainWindow().getViewport()->add_listener(Viewport::MOUSE_PRESS_EVENT, &this->eventMousePressCallback);
    application->getMainWindow().getViewport()->add_listener(Viewport::MOUSE_RELEASE_EVENT, &this->eventMouseReleaseCallback);
}

void CurveMorphingTool::uninitialize(Application *application) noexcept {
    application->getMainWindow().getViewport()->remove_listener(Viewport::PAINT_EVENT, &this->eventPaintCallback);
}

void CurveMorphingTool::handlePaintEvent(VPaintEvent &event) {
    QPainter painter(event.origin());
    StrokeDrawer drawer{};
    if (!this->strokeFrom.empty()) {
        drawer.bind(&this->strokeFrom);
        drawer.draw(painter);
        drawer.drawPoints(painter);
    }
    if (!this->strokeTo.empty()) {
        drawer.bind(&this->strokeTo);
        drawer.draw(painter);
        drawer.drawPoints(painter);
    }
    if (!this->currentStroke.empty()) {
        Stroke current = Stroke{this->currentStroke};
        drawer.bind(&current);
        drawer.draw(painter);
    }
    drawer.unbind();

    if (this->displayDirections) {
        for (std::size_t i = 0; i < this->strokeFrom.size(); i++) {
            painter.drawLine(this->strokeFrom.at(i), this->strokeTo.at(i));
        }
    }
}

void CurveMorphingTool::handleMouseMoveEvent(VMouseEvent &event) {
    if (event.buttons().testFlag(Qt::MouseButton::LeftButton)) {
        this->currentStroke.push_back(event.pos());
        event.queueRepaint();
    }
}

void CurveMorphingTool::handleMousePressEvent(VMouseEvent &event) {
    if (event.button() == Qt::MouseButton::LeftButton) {
        this->currentStroke.clear();
    }
}

void CurveMorphingTool::handleMouseReleaseEvent(VMouseEvent &event) {
    if (event.button() == Qt::LeftButton && !this->currentStroke.empty()) {
        Stroke stroke{this->currentStroke};
        StrokeManager manager{&stroke};
        manager.setTargetSize(20).rebuild();
        if (this->firstStroke) {
            this->strokeFrom = stroke;
        } else {
            this->strokeTo = stroke;
        }
        this->currentStroke.clear();
        event.queueRepaint();
    }
}
