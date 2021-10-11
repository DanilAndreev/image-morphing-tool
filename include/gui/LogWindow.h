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

#ifndef IMAGE_MORPHING_TOOL_LOGWINDOW_H
#define IMAGE_MORPHING_TOOL_LOGWINDOW_H

#include <QWidget>

class LogWindow : public QWidget {
public:
    LogWindow(QWidget* parent = nullptr);
    ~LogWindow() override;
};


#endif //IMAGE_MORPHING_TOOL_LOGWINDOW_H
