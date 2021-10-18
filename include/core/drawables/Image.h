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

#ifndef IMAGE_MORPHING_TOOL_IMAGE_H
#define IMAGE_MORPHING_TOOL_IMAGE_H

#include <QImage>
#include <event_emitter.h>

class Image : public QImage, public events::event_emitter {
public:
    Image() noexcept;
    Image(const QSize &size, Format format);
    Image(int width, int height, Format format);
    explicit Image(const QString &fileName, const char *format = nullptr);
    Image(const Image &reference);
    ~Image() override;
};


#endif //IMAGE_MORPHING_TOOL_IMAGE_H
