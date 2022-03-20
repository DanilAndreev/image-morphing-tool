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
#include "core/Tool/ToolSnapshotEvents.h"
#include "core/Tool/ToolCanvasEvents.h"
#include "core/Stroke/Stroke.h"
#include "core/history/Memento.h"

#include "CMTVulkanBackend.h"
#include "tools/gui/ShadersEditor.h"
#include "CMTSettings.h"

class CMTGui;

struct CurveMorphingToolMemento : public Memento {
    Stroke strokeFrom;
    Stroke strokeTo;
};

class CurveMorphingTool : public ToolViewportEvents, public ToolSnapshotEvents, public ToolCanvasEvents {
    friend CMTGui;
public:
    std::mutex processLock;
    Stroke strokeFrom;
    Stroke strokeTo;
    CMTSettings settings;
    Application* _application;
    events::event_emitter::event_handler_t settingsUpdateEventHandler;
    events::event_emitter::event_handler_t shadersUpdateEventHandler;
protected:
    CMTGui* _gui;
public:
    std::vector<QPoint> currentStroke;
public:
    bool displayDirections = false;
    bool firstStroke = true;

protected:
    CMTVulkanBackend backend{};
public:
    CurveMorphingTool() noexcept;
    virtual ~CurveMorphingTool() = default;
public:
    void canvasPaintEventHandler(CPaintEvent& event) override;
    void mouseMoveEventHandler(VMouseEvent& event) override;
    void mousePressEventHandler(VMouseEvent& event) override;
    void mouseReleaseEventHandler(VMouseEvent& event) override;
    void snapshotCreateEventHandler(SnapshotCreateEvent &event) override;
    void snapshotRestoreEventHandler(SnapshotRestoreEvent &event) override;
    void keyPressEventHandler(VKeyEvent& event) override;
public:
    void initialize(Application* application) override;
    void uninitialize(Application* application) noexcept override;
protected:
    bool executeMorphing(Image* targetImage = nullptr) noexcept;
    void hotRepaint(bool queueRepaint = true) noexcept;
};


#endif //IMAGE_MORPHING_TOOL_CURVEMORPHINGTOOL_H
