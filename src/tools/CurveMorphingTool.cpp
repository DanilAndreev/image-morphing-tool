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

#include "core/Stroke/StrokeDrawer.h"
#include "core/Stroke/StrokeManager.h"


CurveMorphingTool::CurveMorphingTool() noexcept : ToolViewportEvents(), displayDirections(false) {}

void CurveMorphingTool::initialize(Application *application) {
    ToolViewportEvents::initialize(application);
    ToolSnapshotEvents::initialize(application);
    ToolCanvasEvents::initialize(application);
    this->_application = application;
    this->backend.initialize();//TODO: get status;

    this->_shadersEditor = new ShadersEditor{};
    this->_shadersEditor->setApplyChangesCallback([this](QString shaderText){
        QMessageBox message{};
        message.setText(shaderText);
        message.exec();
    });
    this->_shadersEditor->resize(800, 600);

    ToolBar *toolbar = this->_application->getMainWindow().getToolBar();
    auto *button1 = new QPushButton{};
    button1->setText("mode");
//    button1->connect(button1, SIGNAL(released()), toolbar, []() {
//                         if (this->_shadersEditor->isVisible()) {
//                             this->_shadersEditor->close();
//                         } else {
//                             this->_shadersEditor->show();
//                         }
//                     });

    toolbar->layout()->addWidget(button1);
    this->_shadersEditor->show();
}

void CurveMorphingTool::uninitialize(Application *application) noexcept {
    this->backend.release();
    ToolViewportEvents::uninitialize(application);
    ToolSnapshotEvents::uninitialize(application);
    ToolCanvasEvents::uninitialize(application);
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
        this->currentStroke.push_back(event.pos());
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
            this->backend.execute(this->_application->document()->image(),
                                  this->strokeFrom, this->strokeTo);
            event.queueRepaint();
            break;
    }
}
