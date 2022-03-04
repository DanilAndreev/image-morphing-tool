// Copyright (c) 2021.
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

#ifndef IMAGE_MORPHING_TOOL_CANVAS_H
#define IMAGE_MORPHING_TOOL_CANVAS_H

#include <QWidget>
#include <events/DocumentRedrawEvent.h>
#include <event_emitter.h>

class Application;

class Canvas : public QWidget, public events::event_emitter {
public:
    static const char* PAINT_EVENT;
private:
    events::event_emitter::event_handler_t eventDocumentRedrawCallback;
protected:
    Application* _application;
public:
    explicit Canvas(Application* application, QWidget* parent = nullptr);
    ~Canvas() override;
protected:
    void paintEvent(QPaintEvent *event) override;
    void documentRedrawEventHandler(DocumentRedrawEvent& event);
};


#endif //IMAGE_MORPHING_TOOL_CANVAS_H
