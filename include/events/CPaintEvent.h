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

#ifndef IMAGE_MORPHING_TOOL_CPAINTEVENT_H
#define IMAGE_MORPHING_TOOL_CPAINTEVENT_H


#include <QPaintEvent>
#include <QPainter>

#include "CEvent.h"

class CPaintEvent: public CEvent, public QPaintEvent {
public:
    CPaintEvent(const QPaintEvent& qevent, Canvas* origin) noexcept: CEvent(origin), QPaintEvent(qevent) {}
    CPaintEvent(const CPaintEvent& reference) noexcept: CEvent(reference), QPaintEvent(reference) {}
    ~CPaintEvent() override = default;
};


#endif //IMAGE_MORPHING_TOOL_CPAINTEVENT_H
