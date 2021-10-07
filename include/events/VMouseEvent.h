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

#ifndef IMAGE_MORPHING_TOOL_VMOUSEEVENT_H
#define IMAGE_MORPHING_TOOL_VMOUSEEVENT_H

#include <QMouseEvent>

#include "VEvent.h"

class VMouseEvent: public VEvent, public QMouseEvent {
public:
    VMouseEvent(const QMouseEvent& qevent, Viewport* origin) noexcept: VEvent(origin), QMouseEvent(qevent) {}
    VMouseEvent(const VMouseEvent& reference): VEvent(reference), QMouseEvent(reference) {}
    ~VMouseEvent() override = default;
public:
    [[nodiscard]] VMouseEvent * copy() const noexcept override { return new VMouseEvent(*this); }
};


#endif //IMAGE_MORPHING_TOOL_VMOUSEEVENT_H
