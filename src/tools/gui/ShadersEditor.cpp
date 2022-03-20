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


ShadersEditor::ShadersEditor(QWidget *parent) : QWidget(parent) {
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);

    this->_textEdit = new QTextEdit();
    auto* applyChangesButton = new QPushButton{};
    applyChangesButton->setText("Apply changes.");
    connect(applyChangesButton, SIGNAL(released()), this, SLOT(applyChangesSlot()));

    layout->addWidget(_textEdit);
    layout->addWidget(applyChangesButton);
    this->setLayout(layout);
    this->resize(800, 600);
}

void ShadersEditor::applyChangesSlot() {
    this->applyChangesCallback(this->_textEdit->toPlainText());
}
