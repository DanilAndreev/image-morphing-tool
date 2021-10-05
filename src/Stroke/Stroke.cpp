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
