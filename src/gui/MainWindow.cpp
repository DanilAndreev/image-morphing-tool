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

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QBoxLayout>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent) noexcept: QMainWindow(parent) {
    this->viewport = new Viewport(this);
    this->toolBar = new ToolBar(this);

    this->setCentralWidget(new QWidget{});

    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, qApp, QApplication::quit);

    QAction *undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(tr("CTRL+Z"));
    connect(undoAction, &QAction::triggered, qApp, [](){
        qDebug() << "Undo";
    });

    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(tr("CTRL+SHIFT+Z"));


    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(quitAction);
    fileMenu->addAction(undoAction);
    fileMenu->addAction(redoAction);


    QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this->centralWidget());
    layout->addWidget(this->toolBar);
    layout->addWidget(this->viewport);
    this->centralWidget()->setLayout(layout);
}

Viewport *MainWindow::getViewport() const {
    return viewport;
}

ToolBar *MainWindow::getToolBar() const {
    return toolBar;
}
