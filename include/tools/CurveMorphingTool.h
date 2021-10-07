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
#include <vector>

#include <QPaintEvent>
#include <QPainter>

#include "core/Tool/ToolViewportEvents.h"
#include "core/Stroke/Stroke.h"


class CurveMorphingTool : public ToolViewportEvents {
public:
    std::mutex processLock;
    Stroke strokeFrom;
    Stroke strokeTo;
public:
    std::vector<QPoint> currentStroke;
public:
    bool displayDirections = false;
    bool firstStroke = true;
public:
    CurveMorphingTool() noexcept;
    virtual ~CurveMorphingTool() = default;
public:
    void paintEventHandler(VPaintEvent& event) override;
    void mouseMoveEventHandler(VMouseEvent& event) override;
    void mousePressEventHandler(VMouseEvent& event) override;
    void mouseReleaseEventHandler(VMouseEvent& event) override;
public:
    void initialize(Application* application) override;
    void uninitialize(Application* application) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H
