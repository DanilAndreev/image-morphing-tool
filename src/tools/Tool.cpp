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

#include "tools/Tool.h"

void Tool::eventFilter(QEvent *event, Viewport *origin) noexcept {
    switch (event->type()) {
        case QEvent::Type::Paint:
            this->paintEvent(static_cast<QPaintEvent *>(event), origin);
            break;
        case QEvent::Type::KeyPress:
            this->keyPressEvent(static_cast<QKeyEvent *>(event), origin);
            break;
        case QEvent::Type::KeyRelease:
            this->keyReleaseEvent(static_cast<QKeyEvent *>(event), origin);
            break;
        case QEvent::Type::MouseMove:
            this->mouseMoveEvent(static_cast<QMouseEvent *>(event), origin);
            break;
        case QEvent::Type::MouseButtonPress:
            this->mouseButtonPressEvent(static_cast<QMouseEvent *>(event), origin);
            break;
        case QEvent::Type::MouseButtonRelease:
            this->mouseButtonReleaseEvent(static_cast<QMouseEvent *>(event), origin);
            break;
        default:
            break;
    }
}
