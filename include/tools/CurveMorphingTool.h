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

#ifndef IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H
#define IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H

#include <mutex>

#include <QPaintEvent>
#include <QPainter>

#include "Tool.h"
#include "../Stroke/Stroke.h"
#include <gui/Viewport.h>

class CurveMorphingTool : public Tool {
public:
    std::mutex processLock;
    Stroke strokeFrom;
    Stroke strokeTo;
public:
    bool displayDirections;
public:
    CurveMorphingTool() noexcept;
    virtual ~CurveMorphingTool() = default;
public:
    void paintEvent(QPaintEvent *event, Viewport* origin) noexcept override;
public:
    void init() override;
};


#endif //IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H
