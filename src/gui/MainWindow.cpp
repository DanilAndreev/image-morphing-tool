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
#include <QScrollArea>

#include "Application.h"
#include "core/utils.h"
#include "core/exceptions/FileIOError.h"

MainWindow::MainWindow(Application *application, QWidget *parent) noexcept: QMainWindow(parent),
                                                                            application(application) {
    this->buildMenu(parent);

    this->setCentralWidget(new QWidget{});

    this->viewport = new Viewport(application, this);
    this->toolBar = new ToolBar(this);
    this->logWindow = new LogWindow(application);
    BottomLineWidget *bottomLineWidget = new BottomLineWidget{application, this};

    this->_topOuterDock = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    this->_bottomOuterDock = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    this->_leftOuterDock = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    this->_rightOuterDock = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    this->_topInnerDock = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    this->_bottomInnerDock = new QBoxLayout{QBoxLayout::Direction::TopToBottom};

    this->_topOuterDock->setSpacing(0);
    this->_bottomOuterDock->setSpacing(0);
    this->_leftOuterDock->setSpacing(0);
    this->_rightOuterDock->setSpacing(0);
    this->_topInnerDock->setSpacing(0);
    this->_bottomInnerDock->setSpacing(0);

    QBoxLayout* outerVerticalLayout = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    outerVerticalLayout->addLayout(this->_topOuterDock);
    QBoxLayout* outerHorizontalLayout = new QBoxLayout{QBoxLayout::Direction::LeftToRight};
    outerVerticalLayout->addLayout(outerHorizontalLayout);
    outerVerticalLayout->addLayout(this->_bottomOuterDock);

    outerHorizontalLayout->addLayout(this->_leftOuterDock);
    QBoxLayout *innerVerticalLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    outerHorizontalLayout->addLayout(innerVerticalLayout);
    outerHorizontalLayout->addLayout(this->_rightOuterDock);

    innerVerticalLayout->addLayout(this->_topInnerDock);
    QBoxLayout* toolbarAndViewportLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    innerVerticalLayout->addLayout(toolbarAndViewportLayout);
    innerVerticalLayout->addLayout(this->_bottomInnerDock);
    innerVerticalLayout->addWidget(bottomLineWidget);
    this->centralWidget()->setLayout(outerVerticalLayout);

    toolbarAndViewportLayout->setContentsMargins({0, 0, 0, 0});
    outerVerticalLayout->setContentsMargins({0, 0, 0, 0});
    outerHorizontalLayout->setContentsMargins({0, 0, 0, 0});
    innerVerticalLayout->setContentsMargins({0, 0, 0, 0});

    toolbarAndViewportLayout->addWidget(this->toolBar);
    toolbarAndViewportLayout->addWidget(this->viewport);
    toolbarAndViewportLayout->setContentsMargins({0, 0, 0, 0});
    toolbarAndViewportLayout->setSpacing(0);

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

void MainWindow::buildMenu(QWidget* parent) {
    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, qApp, QApplication::quit);

    QAction *undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(tr("CTRL+Z"));
    connect(undoAction, &QAction::triggered, qApp, [this]() {
        if (this->application->history().undo()) {
            this->application->log("Undo", LogEvent::LOG_LEVEL::INFO);
        } else {
            this->application->log("Nothing to undo.", LogEvent::LOG_LEVEL::WARNING);
        }
    });

    QAction *loadImageAction = new QAction("&Load Image", this);
    connect(loadImageAction, &QAction::triggered, qApp, [this, parent]() {
        QString filename = QFileDialog::getOpenFileName(parent,
                                                        tr("Open image"),
                                                        "/images",
                                                        tr("PNG (*.png)"));
        if (filename.isNull()) return;
        try {
            QImage img{filename};
            this->application->document()->loadImage(img);
            this->application->history().reset();
            this->application->log(QString{"Successfully loaded image: "} + filename, LogEvent::LOG_LEVEL::INFO);
        } catch (Exceptions::FileIOError& error) {
            this->application->log(QString{"Failed to load image: "} + filename, LogEvent::LOG_LEVEL::ERROR);
            QMessageBox msgBox;
            msgBox.setText("Failed to load image.");
            msgBox.exec();
        }
    });

    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(tr("CTRL+SHIFT+Z"));
    connect(redoAction, &QAction::triggered, qApp, [this]() {
        if (this->application->history().redo()) {
            this->application->log("Redo", LogEvent::LOG_LEVEL::INFO);
        } else {
            this->application->log("Nothing to redo.", LogEvent::LOG_LEVEL::WARNING);
        };
    });


    QMenu *fileMenu = this->menuBar()->addMenu("&File");
    fileMenu->setProperty("qssClass", "MenuList MainWindowFileMenu");
    fileMenu->addAction(loadImageAction);
    fileMenu->addAction(undoAction);
    fileMenu->addAction(redoAction);
    fileMenu->addAction(quitAction);
}
