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

#include <exceptions/NullPointerException.h>
#include <Stroke/StrokeManager.h>
#include "Viewport/Viewport.h"

#include "Stroke/StrokeDrawer.h"

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
    drawLines(painter);
    QWidget::paintEvent(event);
}

void Viewport::mouseMoveEvent(QMouseEvent *event) noexcept {
    if (event->buttons().testFlag(Qt::MouseButton::LeftButton)) {
        this->currentStroke.push_back(event->pos());
        this->repaint();
    }
    QWidget::mouseMoveEvent(event);
}

void Viewport::mousePressEvent(QMouseEvent *event) noexcept {
    if (event->button() == Qt::MouseButton::LeftButton) {
        this->currentStroke.clear();
    }
    QWidget::mousePressEvent(event);
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) noexcept {
    if (event->button() == Qt::LeftButton && !this->currentStroke.empty()) {
        Stroke stroke{this->currentStroke};
        StrokeManager manager{&stroke};
        manager.setTargetSize(20).rebuild();
        this->strokes.emplace_back(stroke);
        this->currentStroke.clear();
        this->repaint();
    }
    QWidget::mouseReleaseEvent(event);
}
