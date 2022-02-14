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

#include <cassert>

#include "core/drawables/Document.h"
#include "events/DocumentRedrawEvent.h"

const char* Document::REDRAW_EVENT = "redraw";

Document::Document() {
    this->_image = new Image{R"(D:\Projects\image-morphing-tool\cmake-build-debug-qt\images\tiger.png)"};
    assert(!this->_image->isNull());
}

Document::Document(const Document &reference) {
    this->_image = new Image{*reference._image};
}

Document::~Document() {
    delete this->_image;
}

void Document::redraw() noexcept {
    DocumentRedrawEvent event{this};
    this->emit_event(Document::REDRAW_EVENT, event);
}

Image &Document::image() noexcept {
    return *this->_image;
}
