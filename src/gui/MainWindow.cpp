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
#include <QFileDialog>
#include <QMessageBox>

#include "Application.h"
#include "core/utils.h"
#include "core/exceptions/FileIOError.h"

MainWindow::MainWindow(Application *application, QWidget *parent) noexcept: QMainWindow(parent),
                                                                            application(application) {
    this->viewport = new Viewport(application, this);
    this->toolBar = new ToolBar(this);
    this->logWindow = new LogWindow(application);
    BottomLineWidget *bottomLineWidget = new BottomLineWidget{application, this};

    this->setCentralWidget(new QWidget{});

    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, qApp, QApplication::quit);

    QAction *undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(tr("CTRL+Z"));
    connect(undoAction, &QAction::triggered, qApp, [application]() {
        if (application->history().undo()) {
            application->log("Undo", LogEvent::LOG_LEVEL::INFO);
        } else {
            application->log("Nothing to undo.", LogEvent::LOG_LEVEL::WARNING);
        }
    });

    QAction *loadImageAction = new QAction("&Load Image", this);
    connect(loadImageAction, &QAction::triggered, qApp, [application, parent]() {
        QString filename = QFileDialog::getOpenFileName(parent,
                                                        tr("Open image"),
                                                        "/images",
                                                        tr("PNG (*.png)"));
        if (filename.isNull()) return;
        try {
            QImage img{filename};
            application->document()->loadImage(img);
            application->history().reset();
            application->log(QString{"Successfully loaded image: "} + filename, LogEvent::LOG_LEVEL::INFO);
        } catch (Exceptions::FileIOError& error) {
            application->log(QString{"Failed to load image: "} + filename, LogEvent::LOG_LEVEL::ERROR);
            QMessageBox msgBox;
            msgBox.setText("Failed to load image.");
            msgBox.exec();
        }
    });

    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(tr("CTRL+SHIFT+Z"));
    connect(redoAction, &QAction::triggered, qApp, [application]() {
        if (application->history().redo()) {
            application->log("Redo", LogEvent::LOG_LEVEL::INFO);
        } else {
            application->log("Nothing to redo.", LogEvent::LOG_LEVEL::WARNING);
        };
    });


    QMenu *fileMenu = this->menuBar()->addMenu("&File");
    fileMenu->setProperty("qssClass", "MenuList MainWindowFileMenu");
    fileMenu->addAction(loadImageAction);
    fileMenu->addAction(undoAction);
    fileMenu->addAction(redoAction);
    fileMenu->addAction(quitAction);


    QWidget *rightWidget = new QWidget(this->centralWidget());
    QBoxLayout *horizontalLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this->centralWidget());
    horizontalLayout->addWidget(this->toolBar);
    horizontalLayout->addWidget(rightWidget);
    horizontalLayout->setContentsMargins({0, 0, 0, 0});
    horizontalLayout->setSpacing(0);
    this->centralWidget()->setLayout(horizontalLayout);

    QBoxLayout *verticalLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    verticalLayout->addWidget(this->viewport);
    verticalLayout->addWidget(bottomLineWidget);
    verticalLayout->setContentsMargins({0, 0, 0, 0});
    rightWidget->setLayout(verticalLayout);

    this->centralWidget()->setProperty("qssClass", "MainWindowCentral");
    this->menuWidget()->setProperty("qssClass", "MainWindowMenu");
    this->setProperty("qssClass", "MainWindow");

    try {
        std::string stylesheet = textFromFile("style/style.qss"); //TODO: handle errors
        this->setStyleSheet(stylesheet.c_str());
    } catch (std::exception &error) {
        qDebug() << error.what() << Qt::endl;
    }
}

Viewport *MainWindow::getViewport() const {
    return this->viewport;
}

ToolBar *MainWindow::getToolBar() const {
    return this->toolBar;
}

LogWindow *MainWindow::getLogWindow() const {
    return this->logWindow;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qApp->quit();
}
