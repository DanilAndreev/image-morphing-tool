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

#include "gui/Canvas.h"

#include <QPainter>
#include <QPen>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
    this->setFixedSize(1000, 1000);
}

Canvas::~Canvas() {

}

void Canvas::paintEvent(QPaintEvent *event) {
    QPen pen;
    pen.setColor(QColor{255, 0, 0});
    QPainter painter(this);
    painter.setPen(pen);
    painter.drawLine(1, 1, 100, 111);
    QWidget::paintEvent(event);
}
