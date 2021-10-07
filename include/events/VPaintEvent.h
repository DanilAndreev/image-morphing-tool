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
protected:
    mutable QPainter* _painter;
public:
    VPaintEvent(const QPaintEvent& qevent, Viewport* origin, QPainter* painter) noexcept:
            VEvent(origin), QPaintEvent(qevent), _painter(painter) {}
    VPaintEvent(const VPaintEvent& reference): VEvent(reference), QPaintEvent(reference) {}
    ~VPaintEvent() override = default;
public:
    [[nodiscard]] VPaintEvent * copy() const noexcept override { return new VPaintEvent(*this); }
public:
    QPainter* painter() const noexcept { return this->_painter; }
};


#endif //IMAGE_MORPHING_TOOL_VPAINTEVENT_H
