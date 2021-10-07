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


Viewport::Viewport(QWidget *parent) : QWidget(parent) {}

void Viewport::drawLines(QPainter &painter) {
    QPen pen(Qt::black, 2, Qt::SolidLine);
    StrokeDrawer drawer{};
    for (const Stroke &stroke : this->strokes) {
        drawer.bind(&stroke);
        drawer.draw(painter);
        drawer.drawPoints(painter);
    }
    if (!this->currentStroke.empty()) {
        Stroke current = Stroke{this->currentStroke};
        drawer.bind(&current);
        drawer.draw(painter);
    }
    drawer.unbind();
}

void Viewport::paintEvent(QPaintEvent *event) noexcept {
    QPainter painter(this);
    painter.eraseRect(painter.window());
    painter.end();
    VPaintEvent vEvent{*event, this};
    this->emit_event(Viewport::PAINT_EVENT, vEvent);
    QWidget::paintEvent(event);
    if (vEvent.repaintQueued()) this->repaint();
}

void Viewport::mouseMoveEvent(QMouseEvent *event) noexcept {
    VMouseEvent vEvent{*event, this};
    this->emit_event(Viewport::MOUSE_MOVE_EVENT, vEvent);
    QWidget::mouseMoveEvent(event);
    if (vEvent.repaintQueued())
        this->repaint();
}

void Viewport::mousePressEvent(QMouseEvent *event) noexcept {
    VMouseEvent vEvent{*event, this};
    this->emit_event(Viewport::MOUSE_PRESS_EVENT, vEvent);
    QWidget::mousePressEvent(event);
    if (vEvent.repaintQueued()) this->repaint();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) noexcept {
    VMouseEvent vEvent{*event, this};
    this->emit_event(Viewport::MOUSE_RELEASE_EVENT, vEvent);
    QWidget::mouseReleaseEvent(event);
    if (vEvent.repaintQueued()) this->repaint();
}
