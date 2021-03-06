// Copyright (c) 2022-2022.
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

#include "tools/gui/CMTGui.h"
#include "tools/CurveMorphingTool.h"

CMTGui::CMTGui(CurveMorphingTool *tool, QWidget *parent): QPushButton(parent), _tool(tool) {
    QIcon icon{"icons/CMTIcon.png"};
    this->setIcon(icon);
//    this->_cmtSettings = new CMTSettingsEditor(&tool->settings, tool->_application->getMainWindow().getViewport());
    this->_cmtSettings = new CMTSettingsEditor(&tool->settings, &tool->_application->getMainWindow());
    tool->_application->getMainWindow().bottomInnerDock().addWidget(this->_cmtSettings);
    this->_cmtSettings->hide();

    this->_vertexShaderEditor = new ShadersEditor{tool->backend.shaders.vertexShader, "Vertex shader"};
    this->_fragmentShaderEditor = new ShadersEditor{tool->backend.shaders.fragmentShader, "Fragment shader"};
    this->_vertexShaderEditor->resize(800, 600);
    this->_fragmentShaderEditor->resize(800, 600);

    this->_menu = new QMenu{"CMT", this};
    QAction* morphingSettingsEditAction = new QAction{"Morphing settings"};
    QAction* vertexShaderEditAction = new QAction{"Vertex shader editor"};
    QAction* fragmentShaderEditAction = new QAction{"Fragment shader editor"};
    this->_menu->addAction(morphingSettingsEditAction);
    this->_menu->addAction(vertexShaderEditAction);
    this->_menu->addAction(fragmentShaderEditAction);

    this->setProperty("qssClass", "ToolBarIcon");

    connect(this, SIGNAL(rightClicked()), this, SLOT(showMenuSlot()));
    connect(morphingSettingsEditAction, SIGNAL(triggered()), this, SLOT(morphingSettingsEditActionSlot()));
    connect(vertexShaderEditAction, SIGNAL(triggered()), this, SLOT(vertexShaderEditActionSlot()));
    connect(fragmentShaderEditAction, SIGNAL(triggered()), this, SLOT(fragmentShaderEditActionSlot()));
}

void CMTGui::showMenuSlot() {
    auto pos = this->pos() + this->window()->pos();
    pos.setX(pos.x() + this->size().width());
    pos.setY(pos.y() + this->size().height());
    this->_menu->move(pos);
    this->_menu->exec();
}
void CMTGui::mousePressEvent(QMouseEvent *e) {
    switch (e->button()) {
        case Qt::RightButton:
            emit rightClicked();
            break;
        default:
            QAbstractButton::mousePressEvent(e);
    }
}

void CMTGui::morphingSettingsEditActionSlot() {
    if (this->_cmtSettings->isHidden()) {
        this->_cmtSettings->show();
        this->_cmtSettings->activateWindow();
    } else {
        this->_cmtSettings->hide();
    }
}

void CMTGui::vertexShaderEditActionSlot() {
    this->_vertexShaderEditor->show();
    this->_vertexShaderEditor->activateWindow();
}

void CMTGui::fragmentShaderEditActionSlot() {
    this->_fragmentShaderEditor->show();
    this->_fragmentShaderEditor->activateWindow();
}
