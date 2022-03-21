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

#ifndef IMAGE_MORPHING_TOOL_CMTSETTINGS_H
#define IMAGE_MORPHING_TOOL_CMTSETTINGS_H

#include <event_emitter.h>

class CMTSettings : public events::event_emitter {
public:
    static constexpr const char *SETTINGS_UPDATE_EVENT = "settings-update";

public:
    class SettingsUpdateEvent : public events::event_base {
    public:
        const CMTSettings &settings;

    public:
        explicit SettingsUpdateEvent(const CMTSettings &settings) : events::event_base(), settings(settings) {}
    };

private:
    bool _hotEditEnabled = false;
    float _toolMagnitude = 0.1f;
    bool _preserveBorders = false;
    bool _wireframe = false;
public:
    void setHotEditEnabled(bool value);
    void setToolMagnitude(float value);
    void setPreserveBorders(bool value);
    void setWireframe(bool value);

public:
    [[nodiscard]] bool hotEditEnabled() const { return this->_hotEditEnabled; }
    [[nodiscard]] float toolMagnitude() const { return this->_toolMagnitude; }
    [[nodiscard]] bool preserveBorders() const { return this->_preserveBorders; }
    [[nodiscard]] bool wireframe() const { return this->_wireframe; }
};

#endif//IMAGE_MORPHING_TOOL_CMTSETTINGS_H
