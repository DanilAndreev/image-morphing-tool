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

#include <QScrollArea>
#include <QPaintEvent>
#include <QPainter>
#include <vector>
#include <list>

#include "Canvas.h"
#include "core/Stroke/Stroke.h"
#include "event_emitter.h"

class Application;

class Viewport : public QScrollArea, public events::event_emitter {
public:
    static const char* PAINT_EVENT;

    static const char* MOUSE_EVENT;
    static const char* MOUSE_MOVE_EVENT;
    static const char* MOUSE_PRESS_EVENT;
    static const char* MOUSE_RELEASE_EVENT;
    static const char* MOUSE_LEAVE_EVENT;

    static const char* KEY_EVENT;
    static const char* KEY_PRESS_EVENT;
    static const char* KEY_RELEASE_EVENT;
protected:
    Canvas* _canvas;
    Application* _application;
public:
    explicit Viewport(Application* application, QWidget *parent = nullptr);
    Viewport(const Viewport &reference) = delete;
    ~Viewport() override = default;
public:
    Canvas* canvas() noexcept;
protected:
    void paintEvent(QPaintEvent *event) noexcept override;
    void mouseMoveEvent(QMouseEvent *event) noexcept override;
    void mousePressEvent(QMouseEvent *event) noexcept override;
    void mouseReleaseEvent(QMouseEvent *event) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_VIEWPORT_H
