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

#include "core/Tool/Tool.h"
#include "core/Stroke/Stroke.h"
#include "gui/Viewport.h"
#include "events/VPaintEvent.h"
#include "events/VMouseEvent.h"


class CurveMorphingTool : public ITool {
public:
    std::mutex processLock;
    Stroke strokeFrom;
    Stroke strokeTo;
public:
    bool displayDirections;
protected:
    events::event_emitter::event_handler_t eventPaintCallback;
public:
    CurveMorphingTool() noexcept;
    virtual ~CurveMorphingTool() = default;
public:
    void handlePaintEvent(VPaintEvent& event);
public:
    void initialize(Application* application) override;
    void uninitialize(Application* application) noexcept override;
};


#endif //IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H
