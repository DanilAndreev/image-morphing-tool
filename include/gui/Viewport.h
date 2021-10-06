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

#ifndef IMAGE_MORPHING_TOOL_VIEWPORT_H
#define IMAGE_MORPHING_TOOL_VIEWPORT_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <vector>
#include <list>

#include "core/Stroke/Stroke.h"
#include "event_emitter.h"

class Viewport : public QWidget, public events::event_emitter {
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
