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

ToolBar::ToolBar(QWidget *parent): QWidget(parent) {
    QPushButton* button1 = new QPushButton();
    button1->setText("push me");

    QPushButton* button2 = new QPushButton();
    button2->setText("push me");

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, this);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->setContentsMargins({1, 1, 1, 1});
    this->setLayout(layout);

    this->setMaximumWidth(32);
    this->setStyleSheet("border-right: 1px");
}
