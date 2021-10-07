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

#ifndef IMAGE_MORPHING_TOOL_VKEYEVENT_H
#define IMAGE_MORPHING_TOOL_VKEYEVENT_H

#include <QKeyEvent>

#include "VEvent.h"

class VKeyEvent: public VEvent, public QKeyEvent {
public:
    VKeyEvent(const QKeyEvent& qevent, Viewport* origin) noexcept: VEvent(origin), QKeyEvent(qevent) {}
    VKeyEvent(const VKeyEvent& reference): VEvent(reference), QKeyEvent(reference) {}
    ~VKeyEvent() override = default;
};


#endif //IMAGE_MORPHING_TOOL_VKEYEVENT_H
