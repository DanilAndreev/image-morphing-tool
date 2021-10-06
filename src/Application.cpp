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

#include "Application.h"

#include <QApplication>


Application::Application(int argc, char *argv[]): registeredTools{} {
    this->qApplication = new QApplication{argc, argv};
    this->mainWindow = new MainWindow{};
    this->mainWindow->resize(800, 600);
}

Application &Application::showGUI() noexcept {
    this->mainWindow->show();
    return *this;
}

int Application::exec() {
    return this->qApplication->exec();
}

Application::~Application() {
    delete this->qApplication;
    delete this->mainWindow;
}

Application &Application::registerTool(ITool *tool) noexcept {
    this->registeredTools.insert(tool);
    return *this;
}
