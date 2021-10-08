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

#ifndef IMAGE_MORPHING_TOOL_VPAINTEVENT_H
#define IMAGE_MORPHING_TOOL_VPAINTEVENT_H


#include <QPaintEvent>
#include <QPainter>

#include "VEvent.h"

class VPaintEvent: public VEvent, public QPaintEvent {
public:
    VPaintEvent(const QPaintEvent& qevent, Viewport* origin) noexcept: VEvent(origin), QPaintEvent(qevent) {}
    VPaintEvent(const VPaintEvent& reference) noexcept: VEvent(reference), QPaintEvent(reference) {}
    ~VPaintEvent() override = default;
};


#endif //IMAGE_MORPHING_TOOL_VPAINTEVENT_H
