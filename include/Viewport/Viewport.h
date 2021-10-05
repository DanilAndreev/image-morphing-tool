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
private:
    bool isDrawing = false;
public:
    explicit Viewport(QWidget *parent = nullptr);
    Viewport(const Viewport &reference) = delete;
    ~Viewport() override = default;
protected:
    void drawLines(QPainter &painter);
protected:
    void paintEvent(QPaintEvent *event) noexcept override;
    void mouseMoveEvent(QMouseEvent *event) noexcept override;
    void mousePressEvent(QMouseEvent *event) noexcept override;
    void mouseReleaseEvent(QMouseEvent *event) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_VIEWPORT_H
