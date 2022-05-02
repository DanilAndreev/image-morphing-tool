// Copyright (c) 2021-2021.
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

#include "gui/ToolBar.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QVariant>

ToolBar::ToolBar(QWidget *parent): QWidget(parent) {
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins({1, 1, 1, 1});
    this->setLayout(layout);

    this->setMaximumWidth(64);
    this->setProperty("qssClass", "ToolBar");
}
