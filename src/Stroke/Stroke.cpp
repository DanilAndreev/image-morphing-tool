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

#include "Stroke/Stroke.h"

#include <QVector2D>

Stroke::Stroke() noexcept: std::vector<QPoint>() {

}

Stroke::Stroke(const std::list<QPoint> &points) noexcept: std::vector<QPoint>() {
    this->clear();
    for (const QPoint &point : points) {
        this->push_back(point);
    }
}

Stroke::Stroke(const std::vector<QPoint> &points) noexcept: std::vector<QPoint>(points.begin(), points.end()) {}

qreal Stroke::length() {
    qreal length = 0.0;
    for (std::size_t i = 1; i < this->size(); i++) {
        length += Stroke::distance(this->at(i - 1), this->at(i));
    }
}

qreal Stroke::distance(const QPoint &a, const QPoint &b) noexcept {
    return (QVector2D{a} - QVector2D{b}).length();;
}
