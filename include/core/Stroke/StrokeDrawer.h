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

#ifndef IMAGE_MORPHING_TOOL_STROKEDRAWER_H
#define IMAGE_MORPHING_TOOL_STROKEDRAWER_H

#include <QPainter>

#include "Stroke.h"
#include "../exceptions/NullPointerException.h"

class StrokeDrawer {
protected:
    const Stroke *stroke;
public:
    explicit StrokeDrawer(const Stroke *target = nullptr) noexcept;
    StrokeDrawer(const StrokeDrawer &reference) noexcept;
    virtual ~StrokeDrawer() = default;
public:
    void bind(const Stroke *target) noexcept;
    void unbind() noexcept;
public:
    /**
     * @throw Exceptions::NullPointerException
     */
    void draw(QPainter& painter);
    void drawPoints(QPainter& painter);
private:
    inline static void drawLine(QPainter& painter, const QPoint& a, const QPoint& b);
};


#endif //IMAGE_MORPHING_TOOL_STROKEDRAWER_H
