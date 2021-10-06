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

#ifndef IMAGE_MORPHING_TOOL_APPLICATION_H
#define IMAGE_MORPHING_TOOL_APPLICATION_H

#include <QWidget>

#include "Viewport.h"
#include "ToolBar.h"

class Application : public QWidget {
protected:
    Viewport* viewport;
    ToolBar* toolBar;
public:
    Application(QWidget* parent = nullptr) noexcept;
    ~Application() noexcept = default;
};


#endif //IMAGE_MORPHING_TOOL_APPLICATION_H
