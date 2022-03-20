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
#include <QButtonGroup>
#include <QLabel>
#include "tools/CurveMorphingTool.h"
#include "tools/gui/ShadersEditor.h"


ShadersEditor::ShadersEditor(ShaderManager* manager, QString name, QWidget *parent)
    : QWidget(parent), _manager(manager), _name(name) {
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);

    QLabel* shaderNameLabel = new QLabel{this->_name};

    this->_textEdit = new QTextEdit{};
    this->_defaultText = this->_manager->glsl.c_str();
    this->_textEdit->setText(this->_defaultText);

    this->_errorsTextEdit = new QTextEdit{};
    this->_errorsTextEdit->setReadOnly(true);

    auto* applyChangesButton = new QPushButton{};
    applyChangesButton->setText("Apply changes.");
    connect(applyChangesButton, SIGNAL(released()), this, SLOT(applyChangesSlot()));
    auto* resetDefaultButton = new QPushButton{};
    resetDefaultButton->setText("Reset dafault.");
    connect(resetDefaultButton, SIGNAL(released()), this, SLOT(resetDefaultSlot()));

    layout->addWidget(shaderNameLabel);
    layout->addWidget(this->_textEdit, 4);
    layout->addWidget(new QLabel{"Errors:"});
    layout->addWidget(this->_errorsTextEdit, 1);


    QLayout* buttonGroup = new QBoxLayout{QBoxLayout::Direction::LeftToRight};
    buttonGroup->addWidget(applyChangesButton);
    buttonGroup->addWidget(resetDefaultButton);
    layout->addItem(buttonGroup);

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

ShadersEditor::~ShadersEditor() {
    this->_manager->remove_listener(ShaderManager::SHADER_UPDATE_EVENT, &this->shaderUpdateEventHandler);
}

void ShadersEditor::applyChangesSlot() {
    this->_manager->validateAndApply(this->_textEdit->toPlainText().toStdString());
}

void ShadersEditor::resetDefaultSlot() {
    this->_manager->validateAndApply(this->_defaultText.toStdString());
}
