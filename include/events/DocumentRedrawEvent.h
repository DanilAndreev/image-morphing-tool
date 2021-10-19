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

//
// Created by danil on 19.10.21.
//

#ifndef IMAGE_MORPHING_TOOL_DOCUMNETREDRAWEVENT_H
#define IMAGE_MORPHING_TOOL_DOCUMNETREDRAWEVENT_H

#include <event_emitter.h>

class Document;

class DocumentRedrawEvent : public events::event_base {
    const Document* _document;
public:
    explicit DocumentRedrawEvent(const Document* document) noexcept:  events::event_base(), _document(document) {}
    DocumentRedrawEvent(const DocumentRedrawEvent& reference) noexcept: events::event_base(reference), _document(reference._document) {}
    ~DocumentRedrawEvent() override = default;
public:
    inline const Document* document() const noexcept { return this->_document; }
};


#endif //IMAGE_MORPHING_TOOL_DOCUMNETREDRAWEVENT_H
