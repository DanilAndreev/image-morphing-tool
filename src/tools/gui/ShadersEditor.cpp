// Copyright (c) 2022.
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

#include <iostream>
#include <QBoxLayout>
#include <QPushButton>
#include "tools/CurveMorphingTool.h"
#include "tools/gui/ShadersEditor.h"


ShadersEditor::ShadersEditor(ShaderManager* manager, QWidget *parent) : QWidget(parent), _manager(manager) {
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);

    this->_textEdit = new QTextEdit{};
    this->_textEdit->setText(this->_manager->glsl.c_str());

    this->_errorsTextEdit = new QTextEdit{};
    this->_errorsTextEdit->setReadOnly(true);

    auto* applyChangesButton = new QPushButton{};
    applyChangesButton->setText("Apply changes.");
    connect(applyChangesButton, SIGNAL(released()), this, SLOT(applyChangesSlot()));

    layout->addWidget(this->_textEdit, 4);
    layout->addWidget(this->_errorsTextEdit, 1);
    layout->addWidget(applyChangesButton);
    this->setLayout(layout);
    this->resize(800, 600);

    this->shaderUpdateEventHandler = [this](events::event_base& event){
        auto& shaderEvent = dynamic_cast<ShaderManager::ShaderUpdateEvent&>(event);
        this->_textEdit->setText(shaderEvent.glsl.c_str());
        if (shaderEvent.valid) {
            this->_errorsTextEdit->setTextColor(QColor{0, 200, 0});
            this->_errorsTextEdit->setText("Shader is valid.");
        } else {
            this->_errorsTextEdit->setTextColor(QColor{200, 0, 0});
            this->_errorsTextEdit->setText(shaderEvent.error.c_str());
        }
    };
    this->_manager->add_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shaderUpdateEventHandler);
}

void ShadersEditor::applyChangesSlot() {
    this->_manager->validateAndApply(this->_textEdit->toPlainText().toStdString());
}

ShadersEditor::~ShadersEditor() {
    this->_manager->remove_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shaderUpdateEventHandler);
}
