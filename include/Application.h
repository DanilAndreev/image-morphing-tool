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

#ifndef IMAGE_MORPHING_TOOL_APPLICATION_H
#define IMAGE_MORPHING_TOOL_APPLICATION_H

#include <QApplication>

#include "gui/MainWindow.h"

class Application {
protected:
    QApplication* qApplication;
    MainWindow* mainWindow;
public:
    Application(int argc, char *argv[]);
    Application(const Application&) = delete;
    virtual ~Application();
public:
    Application& showGUI() noexcept;
    int exec();
};


#endif //IMAGE_MORPHING_TOOL_APPLICATION_H
