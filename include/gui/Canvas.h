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

#ifndef IMAGE_MORPHING_TOOL_CANVAS_H
#define IMAGE_MORPHING_TOOL_CANVAS_H

#include <QWidget>

class Canvas : public QWidget {
public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas() override;
public:
protected:
    void paintEvent(QPaintEvent *event) override;
};


#endif //IMAGE_MORPHING_TOOL_CANVAS_H
