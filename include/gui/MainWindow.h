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

#include <QMainWindow>

#include "Viewport.h"
#include "ToolBar.h"

class Application;

class MainWindow : public QMainWindow {
protected:
    Viewport* viewport;
    ToolBar* toolBar;
    Application* application;
public:
    explicit MainWindow(Application* application, QWidget* parent = nullptr) noexcept;
    ~MainWindow() noexcept override = default;
public:
    [[nodiscard]] Viewport *getViewport() const;
    [[nodiscard]] ToolBar *getToolBar() const;
};


#endif //IMAGE_MORPHING_TOOL_MAINWINDOW_H
