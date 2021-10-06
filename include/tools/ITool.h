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


class ITool {
public:
    ITool(const ITool&) = delete;
public:
    virtual void init() = 0;
public:
    virtual void paintEvent(QPaintEvent) noexcept = 0;
    virtual void mouseMoveEvent(QMouseEvent* event) noexcept = 0;
    virtual void mouseButtonPressEvent(QMouseEvent* event) noexcept = 0;
    virtual void mouseButtonReleaseEvent(QMouseEvent* event) noexcept = 0;
    virtual void keyPressEvent(QKeyEvent* event) noexcept = 0;
    virtual void keyReleaseEvent(QKeyEvent* event) noexcept = 0;
public:
    virtual void eventFilter(QEvent* event) noexcept = 0;
};


#endif //IMAGE_MORPHING_TOOL_ITOOL_H
