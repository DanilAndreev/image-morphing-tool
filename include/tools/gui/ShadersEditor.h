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

class ShadersEditor : public QWidget {
    Q_OBJECT
protected:
    QTextEdit *_textEdit;

private:
    using applyChangesCallback_t = std::function<void(QString)>;
    applyChangesCallback_t applyChangesCallback;

public:
    explicit ShadersEditor(QWidget *parent = nullptr);

public:
    void setApplyChangesCallback(applyChangesCallback_t callback) { this->applyChangesCallback = std::move(callback); }
private slots:
    void applyChangesSlot();
};


#endif//IMAGE_MORPHING_TOOL_SHADERSEDITOR_H
