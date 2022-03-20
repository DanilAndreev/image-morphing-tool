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

//
// Created by onepl on 3/20/2022.
//

#include "tools/gui/CMTGui.h"
#include "tools/CurveMorphingTool.h"

CMTGui::CMTGui(CurveMorphingTool *tool, QWidget *parent): QPushButton(parent), _tool(tool) {
    this->setText("aaa");
    this->_shadersEditor = new ShadersEditor{tool->backend.shaders.vertexShader, "Vertex shader"};
    this->_shadersEditor->resize(800, 600);

    this->_menu = new QMenu{"CMT", this};
    QAction* action1 = new QAction{"aaaa1"};
    QAction* action2 = new QAction{"aaaa2"};
    this->_menu->addAction(action1);
    this->_menu->addAction(action2);
    this->_menu->setDefaultAction(action1);
    this->_shadersEditor->show();

    connect(this, SIGNAL(rightClicked()), this, SLOT(showMenuSlot()));
}

void CMTGui::showMenuSlot() {
    auto pos = this->pos() + this->window()->pos();
    pos.setX(pos.x() + this->size().width());
    pos.setX(pos.y() + this->size().height());
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
