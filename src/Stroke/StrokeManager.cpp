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

#include "Stroke/StrokeManager.h"
#include <QVector2D>

#include "exceptions/NullPointerException.h"

StrokeManager::StrokeManager(Stroke *target) noexcept: stroke(target), targetSize(target ? target->size() : 0) {}

StrokeManager::StrokeManager(const StrokeManager &reference) noexcept: stroke(reference.stroke),
                                                                       targetSize(reference.targetSize) {}

void StrokeManager::bind(Stroke *target) noexcept {
    this->stroke = target;
}

void StrokeManager::unbind() noexcept {
    this->stroke = nullptr;
}

const StrokeManager &StrokeManager::rebuild() const {
    if (!this->stroke)
        throw Exceptions::NullPointerException{
                "You have to use 'bind()' method before executing context based operations."};
    if (this->stroke->empty()) return *this;
    std::vector<QPoint> newPoints;
    qreal step = this->stroke->length() / static_cast<qreal>(this->targetSize);
    qreal remainder = 0.0;
    newPoints.emplace_back(QPoint{static_cast<int>(this->stroke->at(0).x()),
                                  static_cast<int>(this->stroke->at(1).y())});
    for (std::size_t i = 1; i < this->stroke->size(); i++) {
        QVector2D left = QVector2D{this->stroke->at(i - 1)};
        QVector2D right = QVector2D{this->stroke->at(i)};
        QVector2D direction = (right - left).normalized();
        qreal length = (right - left).length();
        qreal shift = -remainder;
        std::size_t topBorder = static_cast<std::size_t>((length + remainder) / step);
        for (std::size_t j = 0; j < topBorder; j++) {
            shift += step;
            QVector2D point = left + direction * static_cast<float>(shift);
            newPoints.emplace_back(QPoint{static_cast<int>(point.x()), static_cast<int>(point.y())});
        }
        remainder = length - shift;
    }
    this->stroke->clear();
    this->stroke->insert(this->stroke->begin(), newPoints.begin(), newPoints.end());
    return *this;
}

StrokeManager &StrokeManager::setTargetSize(const std::size_t &newSize) noexcept {
    if (!newSize) this->targetSize = 1; //TODO: throw an exception.
    this->targetSize = newSize - 1;
    return *this;
}
