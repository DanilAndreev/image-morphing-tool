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

#ifndef IMAGE_MORPHING_TOOL_CEVENT_H
#define IMAGE_MORPHING_TOOL_CEVENT_H

#include "event_emitter.h"
#include "../gui/Canvas.h"

class CEvent : public events::event_base {
protected:
    Canvas* _origin = nullptr;
    bool _repaintQueued = false;
public:
    explicit CEvent(Canvas* origin) noexcept: events::event_base(), _origin(origin) {}
    CEvent(const CEvent& reference) noexcept: events::event_base(reference), _origin(reference._origin) {}
    ~CEvent() override = default;
public:
    Canvas* origin() noexcept { return this->_origin; }
public:
    void queueRepaint() noexcept {this->_repaintQueued = true; }
    [[nodiscard]] const bool& repaintQueued() const noexcept {
        return this->_repaintQueued;
    }
};


#endif //IMAGE_MORPHING_TOOL_CEVENT_H
