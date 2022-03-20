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

#include <QBoxLayout>
#include <QCloseEvent>
#include <QMainWindow>

#include "BottomLineWidget.h"
#include "ToolBar.h"
#include "Viewport.h"
#include "gui/LogWindow.h"

class Application;

class MainWindow : public QMainWindow {
protected:
    Viewport *viewport;
    ToolBar *toolBar;
    Application *application;
    LogWindow *logWindow;

protected:
    QBoxLayout *_topOuterDock;
    QBoxLayout *_bottomOuterDock;
    QBoxLayout *_leftOuterDock;
    QBoxLayout *_rightOuterDock;
    QBoxLayout *_topInnerDock;
    QBoxLayout *_bottomInnerDock;

public:
    explicit MainWindow(Application *application, QWidget *parent = nullptr) noexcept;
    ~MainWindow() noexcept override = default;

private:
    void buildMenu(QWidget *parent);

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    [[nodiscard]] Viewport *getViewport() const;
    [[nodiscard]] ToolBar *getToolBar() const;
    [[nodiscard]] LogWindow *getLogWindow() const;

public:
    [[nodiscard]] QBoxLayout &topOuterDock() const noexcept { return *this->_topOuterDock; }
    [[nodiscard]] QBoxLayout &bottomOuterDock() const noexcept { return *this->_bottomOuterDock; }
    [[nodiscard]] QBoxLayout &leftOuterDock() const noexcept { return *this->_leftOuterDock; }
    [[nodiscard]] QBoxLayout &rightOuterDock() const noexcept { return *this->_rightOuterDock; }
    [[nodiscard]] QBoxLayout &topInnerDock() const noexcept { return *this->_topInnerDock; }
    [[nodiscard]] QBoxLayout &bottomInnerDock() const noexcept { return *this->_bottomInnerDock; }
};


#endif//IMAGE_MORPHING_TOOL_MAINWINDOW_H
