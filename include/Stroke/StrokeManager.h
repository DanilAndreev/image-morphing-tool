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

#ifndef IMAGE_MORPHING_TOOL_STROKEMANAGER_H
#define IMAGE_MORPHING_TOOL_STROKEMANAGER_H

#include "Stroke.h"

class StrokeManager {
protected:
    mutable Stroke *stroke = nullptr;
    std::size_t targetSize = 0;
public:
    void bind(Stroke* target) noexcept;
    void unbind() noexcept;
public:
    const StrokeManager& rebuild() const;
};


#endif //IMAGE_MORPHING_TOOL_STROKEMANAGER_H
