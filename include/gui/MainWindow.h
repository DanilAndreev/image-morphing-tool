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

#ifndef IMAGE_MORPHING_TOOL_MAINWINDOW_H
#define IMAGE_MORPHING_TOOL_MAINWINDOW_H

#include <QWidget>

#include "Viewport.h"
#include "ToolBar.h"

class MainWindow : public QWidget {
protected:
    Viewport* viewport;
    ToolBar* toolBar;
public:
    MainWindow(QWidget* parent = nullptr) noexcept;
    ~MainWindow() noexcept = default;
};


#endif //IMAGE_MORPHING_TOOL_MAINWINDOW_H
