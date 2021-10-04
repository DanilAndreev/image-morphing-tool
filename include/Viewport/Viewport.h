#ifndef IMAGE_MORPHING_TOOL_VIEWPORT_H
#define IMAGE_MORPHING_TOOL_VIEWPORT_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <vector>
#include <list>

#include "Stroke/Stroke.h"

class Viewport : public QWidget {
protected:
    std::list<Stroke> strokes;
    std::list<QPoint> currentStroke;
public:
    explicit Viewport(QWidget *parent = nullptr);
    Viewport(const Viewport &reference) = delete;
    ~Viewport() override = default;
protected:
    void drawLines(QPainter &painter);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};


#endif //IMAGE_MORPHING_TOOL_VIEWPORT_H
