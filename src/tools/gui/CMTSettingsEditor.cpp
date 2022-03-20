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
#include <QLabel>
#include "tools/gui/CMTSettingsEditor.h"
#include "tools/CMTSettings.h"

static QBoxLayout* makeLine(const QString& text, QWidget* widget) {
    QBoxLayout* layout = new QBoxLayout{QBoxLayout::Direction::LeftToRight};
    QLabel* label = new QLabel(text);
    layout->addWidget(label);
    layout->addWidget(widget);
    return layout;
}

CMTSettingsEditor::CMTSettingsEditor(CMTSettings *settings, QWidget *parent) : QWidget(parent), _settings(settings) {
    this->hotEditCheckbox = new QCheckBox{this};
    this->hotEditCheckbox->setCheckState(settings->hotEditEnabled() ? Qt::Checked : Qt::Unchecked);
    this->preserveBordersCheckbox = new QCheckBox{this};
    this->preserveBordersCheckbox->setCheckState(settings->preserveBorders() ? Qt::Checked : Qt::Unchecked);
    this->wireframeCheckbox = new QCheckBox{this};
    this->wireframeCheckbox->setCheckState(settings->wireframe() ? Qt::Checked : Qt::Unchecked);

    this->magnitudeSlider = new QSlider{this};
    this->magnitudeSlider->setMinimum(0);
    this->magnitudeSlider->setMaximum(FLOAT_SLIDER_PRECISION);
    this->magnitudeSlider->setValue(static_cast<int>(settings->toolMagnitude() * FLOAT_SLIDER_PRECISION));
    magnitudeSlider->setOrientation(Qt::Orientation::Horizontal);
    magnitudeSlider->setTickPosition(QSlider::TicksBelow);


    QBoxLayout* layout = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    layout->addLayout(makeLine("Hot edit", this->hotEditCheckbox));
    layout->addLayout(makeLine("Tool magnitude", this->magnitudeSlider));
    layout->addLayout(makeLine("Preserve borders", this->preserveBordersCheckbox));
    layout->addLayout(makeLine("Wireframe", this->wireframeCheckbox));
    this->setLayout(layout);

    this->settingsUpdateEventHandler = [this](events::event_base& bEvent){
        auto& event = dynamic_cast<CMTSettings::SettingsUpdateEvent&>(bEvent);
        this->hotEditCheckbox->setCheckState(event.settings.hotEditEnabled() ? Qt::Checked : Qt::Unchecked);
        this->magnitudeSlider->setValue(static_cast<int>(event.settings.toolMagnitude() * FLOAT_SLIDER_PRECISION));
        this->preserveBordersCheckbox->setCheckState(event.settings.preserveBorders() ? Qt::Checked : Qt::Unchecked);
        this->wireframeCheckbox->setCheckState(event.settings.wireframe() ? Qt::Checked : Qt::Unchecked);
    };
    this->_settings->add_listener(CMTSettings::SETTINGS_UPDATE_EVENT, &this->settingsUpdateEventHandler);

    connect(this->hotEditCheckbox, SIGNAL(clicked(bool)), this, SLOT(hotEditCheckboxCheckedSlot(bool)));
    connect(this->magnitudeSlider, SIGNAL(valueChanged(int)), this, SLOT(magnitudeSliderSlot(int)));
    connect(this->preserveBordersCheckbox, SIGNAL(clicked(bool)), this, SLOT(preserveBordersCheckboxCheckedSlot(bool)));
    connect(this->wireframeCheckbox, SIGNAL(clicked(bool)), this, SLOT(wireframeCheckboxCheckedSlot(bool)));
}

CMTSettingsEditor::~CMTSettingsEditor() {
    this->_settings->remove_listener(CMTSettings::SETTINGS_UPDATE_EVENT, &this->settingsUpdateEventHandler);
}

void CMTSettingsEditor::hotEditCheckboxCheckedSlot(bool value) {
    this->_settings->setHotEditEnabled(value);
}

void CMTSettingsEditor::magnitudeSliderSlot(int value) {
    this->_settings->setToolMagnitude(static_cast<float>(value) / FLOAT_SLIDER_PRECISION);
}

void CMTSettingsEditor::preserveBordersCheckboxCheckedSlot(bool value) {
    this->_settings->setPreserveBorders(value);
}

void CMTSettingsEditor::wireframeCheckboxCheckedSlot(bool value) {
    this->_settings->setWireframe(value);
}
