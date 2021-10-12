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

#ifndef IMAGE_MORPHING_TOOL_BOTTOMLINEWIDGET_H
#define IMAGE_MORPHING_TOOL_BOTTOMLINEWIDGET_H

#include <QWidget>

class Application;

class BottomLineWidget : public QWidget {
protected:
    Application* _application;
public:
    explicit BottomLineWidget(Application* application, QWidget* parent = nullptr);
    ~BottomLineWidget() override;
};


#endif //IMAGE_MORPHING_TOOL_BOTTOMLINEWIDGET_H
