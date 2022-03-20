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

class CurveMorphingTool;

class CMTSettingsEditor : public QWidget {
protected:
    CurveMorphingTool* _tool;
public:
    CMTSettingsEditor(CurveMorphingTool* tool, QWidget* parent = nullptr);
};


#endif//IMAGE_MORPHING_TOOL_CMTSETTINGSEDITOR_H
