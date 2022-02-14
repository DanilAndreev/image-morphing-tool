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

#ifndef IMAGE_MORPHING_TOOL_DOCUMENT_H
#define IMAGE_MORPHING_TOOL_DOCUMENT_H

#include <event_emitter.h>

#include "Image.h"

class Document : public events::event_emitter {
public:
    static const char* LOAD_EVENT;
    static const char* UNLOAD_EVENT;
    static const char* REDRAW_EVENT;

protected:
    Image* _image = nullptr;
public:
    Document();
    Document(const Document& reference);
    ~Document();
public:
    void redraw() noexcept;
public:
    [[nodiscard]] Image& image() noexcept;
};


#endif //IMAGE_MORPHING_TOOL_DOCUMENT_H
