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

#include "gui/BottomLineWidget.h"

#include <QBoxLayout>
#include <QPushButton>
#include "gui/LastLogMessageWidget.h"

BottomLineWidget::BottomLineWidget(Application* application, QWidget *parent) : QWidget(parent), _application(application) {
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    LastLogMessageWidget* lastLogMessageWidget = new LastLogMessageWidget(application, this);
    QPushButton* button = new QPushButton{};
    button->setContentsMargins({1, 1, 1, 1});
    button->setText("terminal");

    layout->setAlignment(Qt::AlignRight);
    this->setMaximumHeight(40);
    layout->addWidget(lastLogMessageWidget);
    layout->addWidget(button);
    this->setLayout(layout);
}

BottomLineWidget::~BottomLineWidget() {}
