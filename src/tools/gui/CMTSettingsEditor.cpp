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
#include "tools/CMTSettings.h"

CMTSettingsEditor::CMTSettingsEditor(CMTSettings *settings, QWidget *parent) : QWidget(parent), _settings(settings) {
    this->hotEditCheckbox = new QCheckBox{this};
    this->hotEditCheckbox->setCheckState(settings->hotEditEnabled() ? Qt::Checked : Qt::Unchecked);
    this->preserveBordersCheckbox = new QCheckBox{this};
    this->preserveBordersCheckbox->setCheckState(settings->preserveBorders() ? Qt::Checked : Qt::Unchecked);

    this->magnitudeSlider = new QSlider{this};
    magnitudeSlider->setOrientation(Qt::Orientation::Horizontal);
    magnitudeSlider->setTickPosition(QSlider::TicksBelow);


    QBoxLayout* layout = new QBoxLayout{QBoxLayout::Direction::TopToBottom};
    layout->addWidget(hotEditCheckbox);
    layout->addWidget(magnitudeSlider);
    this->setLayout(layout);

    this->settingsUpdateEventHandler = [this](events::event_base& bEvent){
        auto& event = dynamic_cast<CMTSettings::SettingsUpdateEvent&>(bEvent);
        this->hotEditCheckbox->setCheckState(event.settings.hotEditEnabled() ? Qt::Checked : Qt::Unchecked);
        this->magnitudeSlider->setValue(2); //TODO: convert event.settings.toolMagnitude()
        this->preserveBordersCheckbox->setCheckState(event.settings.preserveBorders() ? Qt::Checked : Qt::Unchecked);
    };
    this->_settings->add_listener(CMTSettings::SETTINGS_UPDATE_EVENT, &this->settingsUpdateEventHandler);
}

CMTSettingsEditor::~CMTSettingsEditor() {
    this->_settings->remove_listener(CMTSettings::SETTINGS_UPDATE_EVENT, &this->settingsUpdateEventHandler);
}
