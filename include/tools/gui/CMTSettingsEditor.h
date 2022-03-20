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

#ifndef IMAGE_MORPHING_TOOL_CMTSETTINGSEDITOR_H
#define IMAGE_MORPHING_TOOL_CMTSETTINGSEDITOR_H

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include <event_emitter.h>

class CMTSettings;

class CMTSettingsEditor : public QWidget {
    Q_OBJECT
public:
    static constexpr std::size_t FLOAT_SLIDER_PRECISION = 100;
protected:
    events::event_emitter::event_handler_t settingsUpdateEventHandler;
    CMTSettings* _settings;

protected:
    QSlider* magnitudeSlider;
    QCheckBox* hotEditCheckbox;
    QCheckBox* preserveBordersCheckbox;
public:
    CMTSettingsEditor(CMTSettings* settings, QWidget* parent = nullptr);
    ~CMTSettingsEditor() override;
private slots:
    void hotEditCheckboxCheckedSlot(bool value);
    void magnitudeSliderSlot(int value);
    void preserveBordersCheckboxCheckedSlot(bool value);
};


#endif//IMAGE_MORPHING_TOOL_CMTSETTINGSEDITOR_H
