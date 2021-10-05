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

Stroke::Stroke(const std::list<QPoint> &points) noexcept {
    this->clear();
    for (const QPoint& point : points) {
        this->push_back(point);
    }
}

Stroke::Stroke(const std::vector<QPoint> &points) noexcept {
    this->clear();
    this->insert(this->begin(), points.begin(), points.end());
}
