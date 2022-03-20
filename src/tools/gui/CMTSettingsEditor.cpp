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

#include <QPushButton>
#include <QBoxLayout>
#include <QCheckBox>
#include <QSlider>
#include "tools/gui/CMTSettingsEditor.h"
#include "tools/CurveMorphingTool.h"

CMTSettingsEditor::CMTSettingsEditor(CurveMorphingTool *tool, QWidget *parent) : QWidget(parent), _tool(tool) {
    QCheckBox* hotEditCheckbox = new QCheckBox{this};

    QSlider* magnitudeSlider = new QSlider{this};
    magnitudeSlider->setOrientation(Qt::Orientation::Horizontal);
    magnitudeSlider->setTickPosition(QSlider::TicksBelow);


    QBoxLayout* layout = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    layout->addWidget(hotEditCheckbox);
    layout->addWidget(magnitudeSlider);
    this->setLayout(layout);
}
