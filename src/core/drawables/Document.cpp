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
#include "core/exceptions/FileIOError.h"


const char *Document::REDRAW_EVENT = "redraw";
const char *Document::LOAD_EVENT = "load";
const char *Document::UNLOAD_EVENT = "close";

Document::Document() {
    QImage img{R"(C:\Projects\image-morphing-tool\cmake-build-debug-mingw-qt6\images\tiger.png)"};
    this->_image = new Image(img.convertToFormat(QImage::Format::Format_RGBA8888));
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

Image *Document::swapImage(Image *nImage) noexcept {
    Image *oldImage = this->_image;
    this->_image = nImage;
    return oldImage;
}

void Document::loadImage(const QImage& image) {
    this->loadImage(new Image(image.convertToFormat(QImage::Format::Format_RGBA8888)));
}

void Document::loadImage(Image *image) {
    if (image->isNull())
        throw Exceptions::FileIOError("Failed to open image.");
    delete this->_image;
    this->_image = image;
}
