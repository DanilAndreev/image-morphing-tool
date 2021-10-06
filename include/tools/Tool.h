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

#ifndef IMAGE_MORPHING_TOOL_TOOL_H
#define IMAGE_MORPHING_TOOL_TOOL_H

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QEvent>

#include "ITool.h"

class Tool : public ITool {
public:
    void paintEvent(QPaintEvent) noexcept override {};
    void mouseMoveEvent(QMouseEvent* event) noexcept override {};
    void mouseButtonPressEvent(QMouseEvent* event) noexcept override {};
    void mouseButtonReleaseEvent(QMouseEvent* event) noexcept override {};
    void keyPressEvent(QKeyEvent* event) noexcept override {};
    void keyReleaseEvent(QKeyEvent* event) noexcept override {};
public:
    void eventFilter(QEvent* event) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_TOOL_H
