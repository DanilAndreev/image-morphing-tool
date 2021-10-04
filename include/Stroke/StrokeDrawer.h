#ifndef IMAGE_MORPHING_TOOL_STROKEDRAWER_H
#define IMAGE_MORPHING_TOOL_STROKEDRAWER_H

#include <QPainter>

#include "Stroke.h"

class StrokeDrawer {
protected:
    const Stroke *stroke;
    QPen pen;
public:
    explicit StrokeDrawer(const Stroke *target = nullptr) noexcept;
    StrokeDrawer(const StrokeDrawer &reference) noexcept;
    virtual ~StrokeDrawer() = default;
public:
    void bind(const Stroke *target) noexcept;
    void unbind() noexcept;
public:
    void draw(QPainter& painter);
private:
    inline static void drawLine(QPainter& painter, const QPoint& a, const QPoint& b);
};


#endif //IMAGE_MORPHING_TOOL_STROKEDRAWER_H
