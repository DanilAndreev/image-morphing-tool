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

#include "../Stroke/Stroke.h"

class CurveMorphingTool {
public:
    std::mutex processLock;
    Stroke strokeFrom;
    Stroke strokeTo;
public:

public:
    CurveMorphingTool() noexcept;
    CurveMorphingTool(const CurveMorphingTool& reference) noexcept;
    virtual ~CurveMorphingTool() = default;
public:
    void paintEvent(QPaintEvent *event) noexcept;
public:
    void init();
};


#endif //IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H
