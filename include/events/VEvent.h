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

#ifndef IMAGE_MORPHING_TOOL_VEVENT_H
#define IMAGE_MORPHING_TOOL_VEVENT_H

#include "event_emitter.h"
#include "../gui/Viewport.h"

class VEvent : public events::event_base {
protected:
    mutable Viewport* _origin;
public:
    explicit VEvent(Viewport* origin) noexcept: events::event_base(), _origin(origin) {}
    VEvent(const VEvent& reference) noexcept: events::event_base(reference), _origin(reference._origin) {}
    ~VEvent() override = default;
public:
    Viewport* origin() noexcept { return this->_origin; }
    [[nodiscard]] VEvent * copy() const noexcept override { return new VEvent(*this); }
};


#endif //IMAGE_MORPHING_TOOL_VEVENT_H
