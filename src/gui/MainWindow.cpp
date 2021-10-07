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

#include "gui/MainWindow.h"

#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent) noexcept: QWidget(parent) {
    this->viewport = new Viewport(this);
    this->toolBar = new ToolBar(this);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
    layout->addWidget(this->toolBar);
    layout->addWidget(this->viewport);
    this->setLayout(layout);
}

Viewport *MainWindow::getViewport() const {
    return viewport;
}

ToolBar *MainWindow::getToolBar() const {
    return toolBar;
}
