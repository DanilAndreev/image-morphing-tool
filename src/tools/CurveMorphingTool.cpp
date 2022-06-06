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

#include "tools/CurveMorphingTool.h"

#include <QApplication>
#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <iostream>
#include <QMessageBox>
#include <QMenu>

#include "core/Stroke/StrokeDrawer.h"
#include "core/Stroke/StrokeManager.h"
#include "tools/gui/CMTGui.h"
#include "tools/ShaderManager.h"


CurveMorphingTool::CurveMorphingTool() noexcept : ToolViewportEvents(), displayDirections(false) {}

void CurveMorphingTool::initialize(Application *application) {
    ToolViewportEvents::initialize(application);
    ToolSnapshotEvents::initialize(application);
    ToolCanvasEvents::initialize(application);
    this->_application = application;
    VkResult status = this->backend.initialize();
    if (status != VK_SUCCESS) {
        QString errorText{};
        switch (status) {
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                errorText = "Vulkan driver is not compatible with your device.";
                break;
            default:
                errorText = "Unexpected error occurred while initializing Vulkan backend.";
        }
        this->_application->log(errorText, LogEvent::LOG_LEVEL::ERROR);
        QMessageBox errorMessageBox{};
        errorMessageBox.setText(errorText);
        return;
    }


    this->backend.setSettings(&this->settings);

    this->_gui = new CMTGui{this, &application->getMainWindow()};


    ToolBar *toolbar = this->_application->getMainWindow().getToolBar();
    toolbar->layout()->addWidget(this->_gui);

    this->settingsUpdateEventHandler = [this](events::event_base& bEvent){
        auto& event = dynamic_cast<CMTSettings::SettingsUpdateEvent&>(bEvent);
        if (event.settings.hotEditEnabled()) {
            this->hotRepaint();
        }
    };
    this->shadersUpdateEventHandler = [this](events::event_base& bEvent){
        auto& event = dynamic_cast<ShaderManager::ShaderUpdateEvent&>(bEvent);
        if (event.valid && this->settings.hotEditEnabled()) {
            this->hotRepaint();
        }
    };
    this->settings.add_listener(CMTSettings::SETTINGS_UPDATE_EVENT, &this->settingsUpdateEventHandler);
    this->backend.shaders.fragmentShader->add_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shadersUpdateEventHandler);
    this->backend.shaders.vertexShader->add_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shadersUpdateEventHandler);
    this->isInitialized = true;
}

void CurveMorphingTool::uninitialize(Application *application) noexcept {
    if (this->isInitialized) {
        this->backend.shaders.fragmentShader->remove_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shadersUpdateEventHandler);
        this->backend.shaders.vertexShader->remove_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shadersUpdateEventHandler);
        this->settings.remove_listener(CMTSettings::SETTINGS_UPDATE_EVENT, &this->settingsUpdateEventHandler);
        this->backend.release();
        ToolViewportEvents::uninitialize(application);
        ToolSnapshotEvents::uninitialize(application);
        ToolCanvasEvents::uninitialize(application);
        this->isInitialized = false;
    }
}

void CurveMorphingTool::canvasPaintEventHandler(CPaintEvent &event) {
    QPen redPen = QPen{};
    redPen.setColor({255, 0, 0});
    redPen.setWidth(3);

    QPen blackPen = QPen{};
    blackPen.setColor({0, 0, 0});
    blackPen.setWidth(3);

    QPen yellowPen = QPen{};
    blackPen.setColor({150, 150, 0});
    blackPen.setWidth(3);

    QPen bluePen = QPen{};
    bluePen.setColor({0, 0, 255});
    bluePen.setWidth(3);

    QPainter painter(event.origin());
    StrokeDrawer drawer{};

    if (!this->strokeFrom.empty()) {
        drawer.bind(&this->strokeFrom);
        painter.setPen(blackPen);
        drawer.draw(painter);
        painter.setPen(redPen);
        drawer.drawPoints(painter);
    }
    if (!this->strokeTo.empty()) {
        drawer.bind(&this->strokeTo);
        painter.setPen(blackPen);
        drawer.draw(painter);
        painter.setPen(yellowPen);
        drawer.drawPoints(painter);
    }
    if (!this->currentStroke.empty()) {
        Stroke current = Stroke{this->currentStroke};
        drawer.bind(&current);
        painter.setPen(bluePen);
        drawer.draw(painter);
    }
    drawer.unbind();

    if (this->displayDirections) {
        for (std::size_t i = 0; i < this->strokeFrom.size(); i++) {
            painter.drawLine(this->strokeFrom.at(i), this->strokeTo.at(i));
        }
    }
}

void CurveMorphingTool::mouseMoveEventHandler(VMouseEvent &event) {
    if (event.buttons().testFlag(Qt::MouseButton::LeftButton)) {
        this->currentStroke.push_back(event.pos() + event.scroll());
        event.queueRepaint();
    }
}

void CurveMorphingTool::mousePressEventHandler(VMouseEvent &event) {
    if (event.button() == Qt::MouseButton::LeftButton) {
        this->currentStroke.clear();
    }
}

void CurveMorphingTool::mouseReleaseEventHandler(VMouseEvent &event) {
    if (event.button() == Qt::LeftButton && !this->currentStroke.empty()) {
        this->_application->history().makeSnapshot();
        Stroke stroke{this->currentStroke};
        StrokeManager manager{&stroke};
        manager.setTargetSize(100).rebuild();
        if (this->firstStroke) {
            this->strokeFrom = stroke;
        } else {
            this->strokeTo = stroke;
        }
        this->currentStroke.clear();
        if (this->settings.hotEditEnabled())
            this->hotRepaint(false);
        event.queueRepaint();
    }
}

void CurveMorphingTool::snapshotCreateEventHandler(SnapshotCreateEvent &event) {
    auto memento = new CurveMorphingToolMemento{};
    memento->strokeFrom = this->strokeFrom;
    memento->strokeTo = this->strokeTo;
    event.snapshot->insert(std::make_pair("curve-morphing-tool", memento));
}

void CurveMorphingTool::snapshotRestoreEventHandler(SnapshotRestoreEvent &event) {
    auto memento = dynamic_cast<CurveMorphingToolMemento *>(event.snapshot->at("curve-morphing-tool"));
    this->strokeFrom = memento->strokeFrom;
    this->strokeTo = memento->strokeTo;
}

void CurveMorphingTool::keyPressEventHandler(VKeyEvent &event) {
    switch (event.key()) {
        case Qt::Key::Key_Space:
            this->firstStroke = !this->firstStroke;
            break;
        case Qt::Key::Key_Enter:
        case Qt::Key::Key_Return:
            this->_application->history().makeSnapshot();
            if (this->executeMorphing())
                event.queueRepaint();
    }
}

bool CurveMorphingTool::executeMorphing(Image* targetImage) noexcept {
    bool retVal;
    this->processLock.lock();
    if (!targetImage) targetImage = &this->_application->document()->image();

    if (this->backend.shaders.vertexShader->isValid() && this->backend.shaders.fragmentShader->isValid()) {
        this->backend.execute(*targetImage, this->strokeFrom, this->strokeTo);
        retVal = true;
    } else {
        QString message = "One or more shaders are incorrect:\n";
        if (!this->backend.shaders.vertexShader->isValid()) {
            message += "Vertex shader:\n";
            message += QString(this->backend.shaders.vertexShader->getError().c_str());
        }
        if (!this->backend.shaders.fragmentShader->isValid()) {
            message += "Fragment shader:\n";
            message += QString(this->backend.shaders.fragmentShader->getError().c_str());
        }
        this->_application->log(message, LogEvent::LOG_LEVEL::ERROR);
        QMessageBox messageBox{};
        messageBox.setText("One or more shaders are incorrect. See console for details.");
        messageBox.exec();
        retVal = false;
    }
    this->processLock.unlock();
    return retVal;
}
void CurveMorphingTool::hotRepaint(bool queueRepaint) noexcept {
    const Snapshot* snapshot = this->_application->history().last();
    if (!snapshot) return;
    auto* memento = dynamic_cast<DocumentMemento*>(snapshot->at("document"));
    if (!memento) return;
    Image picture = Image{memento->image};
    if (this->executeMorphing(&picture)) {
        this->_application->document()->image() = picture;
        if (queueRepaint)
            this->_application->document()->redraw();
    }
}
