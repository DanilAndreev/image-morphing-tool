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

#include "gui/Canvas.h"

#include <QPainter>
#include <QPen>

#include "Application.h"
#include "events/CPaintEvent.h"

const char* Canvas::PAINT_EVENT = "paint";

Canvas::Canvas(Application *application, QWidget *parent) : QWidget(parent), _application(application) {
    this->eventDocumentRedrawCallback = [this](events::event_base& event) {
        this->documentRedrawEventHandler(dynamic_cast<DocumentRedrawEvent&>(event));
    };
    application->document()->add_listener(Document::REDRAW_EVENT, &this->eventDocumentRedrawCallback);
    this->setFixedSize(1000, 1000);
}

Canvas::~Canvas() {
    this->_application->document()->remove_listener(Document::REDRAW_EVENT, &this->eventDocumentRedrawCallback);
}

void Canvas::paintEvent(QPaintEvent *event) {
    Image &image = this->_application->document()->image();
    if (this->size() != image.size())
        this->setFixedSize(image.size());
    QPainter painter(this);
    painter.drawImage(painter.window(), image);
    painter.end();

    CPaintEvent cEvent{*event, this};
    this->emit_event(Canvas::PAINT_EVENT, cEvent);
    if (cEvent.repaintQueued()) this->repaint();
    QWidget::paintEvent(event);
}

void Canvas::documentRedrawEventHandler(DocumentRedrawEvent& event) {
    this->repaint();
}
