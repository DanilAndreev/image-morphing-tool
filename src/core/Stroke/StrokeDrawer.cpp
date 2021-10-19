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

#include "core/Stroke/StrokeDrawer.h"

StrokeDrawer::StrokeDrawer(const Stroke *target) noexcept: stroke(target) {}

StrokeDrawer::StrokeDrawer(const StrokeDrawer &reference) noexcept: stroke(reference.stroke) {}

void StrokeDrawer::bind(const Stroke *target) noexcept {
    this->stroke = target;
}

void StrokeDrawer::unbind() noexcept {
    this->stroke = nullptr;
}

void StrokeDrawer::draw(QPainter &painter) {
    if (!this->stroke)
        throw Exceptions::NullPointerException{
                "You have to use 'bind()' method before executing context based operations."};
    for (std::size_t i = 1; i < this->stroke->size(); i++) {
        StrokeDrawer::drawLine(painter, this->stroke->at(i - 1), this->stroke->at(i));
    }
}

void StrokeDrawer::drawLine(QPainter &painter, const QPoint &a, const QPoint &b) {
    painter.drawLine(a.x(), a.y(), b.x(), b.y());
}

void StrokeDrawer::drawPoints(QPainter &painter) {
    if (!this->stroke)
        throw Exceptions::NullPointerException{
        "You have to use 'bind()' method before executing context based operations."};
    for (const QPoint& point : *this->stroke) {
        painter.drawPoint(point);
    }
}
