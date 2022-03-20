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

#ifndef IMAGE_MORPHING_TOOL_CMTGUI_H
#define IMAGE_MORPHING_TOOL_CMTGUI_H

#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include "tools/gui/ShadersEditor.h"

class CurveMorphingTool;

class CMTGui: public QPushButton {
    Q_OBJECT
protected:
    CurveMorphingTool* _tool;
protected:
    ShadersEditor * _vertexShaderEditor = nullptr;
    ShadersEditor * _fragmentShaderEditor = nullptr;
    QMenu* _menu;
public:
    CMTGui(CurveMorphingTool* tool, QWidget* parent);
signals:
    void rightClicked();
private slots:
    void showMenuSlot();
    void morphingSettingsEditActionSlot();
    void vertexShaderEditActionSlot();
    void fragmentShaderEditActionSlot();
    void mousePressEvent(QMouseEvent *e) override;
};


#endif//IMAGE_MORPHING_TOOL_CMTGUI_H
