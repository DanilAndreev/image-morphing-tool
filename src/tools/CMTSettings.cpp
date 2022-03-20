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

#include "tools/CMTSettings.h"

void CMTSettings::setHotEditEnabled(bool value) {
    this->_hotEditEnabled = value;
    SettingsUpdateEvent event{*this};
    this->emit_event(SETTINGS_UPDATE_EVENT, event);
}

void CMTSettings::setToolMagnitude(float value) {
    this->_toolMagnitude = value;
    SettingsUpdateEvent event{*this};
    this->emit_event(SETTINGS_UPDATE_EVENT, event);
}

void CMTSettings::setPreserveBorders(bool value) {
    this->_preserveBorders = value;
    SettingsUpdateEvent event{*this};
    this->emit_event(SETTINGS_UPDATE_EVENT, event);
}
