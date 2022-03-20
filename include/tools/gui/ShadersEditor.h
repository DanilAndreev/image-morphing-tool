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

#ifndef IMAGE_MORPHING_TOOL_SHADERSEDITOR_H
#define IMAGE_MORPHING_TOOL_SHADERSEDITOR_H

#include <QTextEdit>
#include <QWidget>
#include <functional>
#include <utility>

#include "tools/ShaderManager.h"

class ShadersEditor : public QWidget {
    Q_OBJECT
protected:
    events::event_emitter::event_handler_t shaderUpdateEventHandler;
    QTextEdit *_textEdit;
    QTextEdit *_errorsTextEdit;
    ShaderManager* _manager;

protected:
    QString _defaultText;
    QString _name;
public:
    explicit ShadersEditor(ShaderManager* manager, QString name, QWidget *parent = nullptr);
    ~ShadersEditor() override;
private slots:
    void applyChangesSlot();
    void resetDefaultSlot();
};


#endif//IMAGE_MORPHING_TOOL_SHADERSEDITOR_H
