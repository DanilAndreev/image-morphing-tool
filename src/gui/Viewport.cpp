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

#include "gui/Viewport.h"

#include <QVariant>
#include <QPaintEngine>

#include "core/Stroke/StrokeDrawer.h"
#include "core/Stroke/StrokeManager.h"

#include "events/VPaintEvent.h"
#include "events/VMouseEvent.h"

const char *Viewport::PAINT_EVENT = "paint";

const char *Viewport::MOUSE_EVENT = "mouse";
const char *Viewport::MOUSE_MOVE_EVENT = "mouse_move";
const char *Viewport::MOUSE_PRESS_EVENT = "mouse_press";
const char *Viewport::MOUSE_RELEASE_EVENT = "mouse_release";
const char *Viewport::MOUSE_LEAVE_EVENT = "mouse_leave";

const char *Viewport::KEY_EVENT = "key";
const char *Viewport::KEY_PRESS_EVENT = "key_press";
const char *Viewport::KEY_RELEASE_EVENT = "key_release";


Viewport::Viewport(QWidget *parent) : QScrollArea(parent) {
    this->setProperty("qssClass", "Viewport");
    this->_canvas = new Canvas{parent};
    this->setWidget(this->_canvas);
}

void Viewport::paintEvent(QPaintEvent *event) noexcept {
//    QPainter painter(this);
//    QPaintEngine* e = this->paintEngine();
//    painter.eraseRect(painter.window());
//    painter.end();
//    VPaintEvent vEvent{*event, this};
//    this->emit_event(Viewport::PAINT_EVENT, vEvent);
//    QAbstractScrollArea::paintEvent(event);
//    if (vEvent.repaintQueued()) this->repaint();
}

void Viewport::mouseMoveEvent(QMouseEvent *event) noexcept {
    VMouseEvent vEvent{*event, this};
    this->emit_event(Viewport::MOUSE_MOVE_EVENT, vEvent);
    QAbstractScrollArea::mouseMoveEvent(event);
    if (vEvent.repaintQueued())
        this->repaint();
}

void Viewport::mousePressEvent(QMouseEvent *event) noexcept {
    VMouseEvent vEvent{*event, this};
    this->emit_event(Viewport::MOUSE_PRESS_EVENT, vEvent);
    QAbstractScrollArea::mousePressEvent(event);
    if (vEvent.repaintQueued()) this->repaint();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) noexcept {
    VMouseEvent vEvent{*event, this};
    this->emit_event(Viewport::MOUSE_RELEASE_EVENT, vEvent);
    QAbstractScrollArea::mouseReleaseEvent(event);
    if (vEvent.repaintQueued()) this->repaint();
}

Canvas *Viewport::canvas() noexcept {
    return this->_canvas;
}
