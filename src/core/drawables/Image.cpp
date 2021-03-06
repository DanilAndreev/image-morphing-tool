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

#include "core/drawables/Image.h"

Image::Image() noexcept: QImage(QSize{}, QImage::Format::Format_RGBA8888), events::event_emitter() {}

Image::Image(const QSize &size) : QImage(size, QImage::Format::Format_RGBA8888), events::event_emitter() {}

Image::Image(int width, int height) : QImage(width, height, QImage::Format::Format_RGBA8888), events::event_emitter() {}

Image::Image(const QString &fileName, const char *format) : QImage(fileName, format), events::event_emitter() {}

Image::Image(const Image &reference): QImage(reference), events::event_emitter() {}

Image::~Image() {}
Image::Image(const QImage &reference) : QImage(reference.convertToFormat(QImage::Format::Format_RGBA8888)) {}
