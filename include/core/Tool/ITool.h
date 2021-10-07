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

#ifndef IMAGE_MORPHING_TOOL_ITOOL_H
#define IMAGE_MORPHING_TOOL_ITOOL_H

class Viewport;
class Application;

class ITool {
public:
    ITool() = default;
    ITool(const ITool&) = delete;
    ~ITool() = default;
public:
    virtual void initialize(Application* application) = 0;
    virtual void uninitialize(Application* application) noexcept = 0;
public:
//    virtual void paintEvent(QPaintEvent* event, Viewport* origin) noexcept = 0;
//    virtual void mouseMoveEvent(QMouseEvent* event, Viewport* origin) noexcept = 0;
//    virtual void mouseButtonPressEvent(QMouseEvent* event, Viewport* origin) noexcept = 0;
//    virtual void mouseButtonReleaseEvent(QMouseEvent* event, Viewport* origin) noexcept = 0;
//    virtual void keyPressEvent(QKeyEvent* event, Viewport* origin) noexcept = 0;
//    virtual void keyReleaseEvent(QKeyEvent* event, Viewport* origin) noexcept = 0;
public:
//    virtual void eventFilter(QEvent* event, Viewport* origin) noexcept = 0;
};


#endif //IMAGE_MORPHING_TOOL_ITOOL_H
