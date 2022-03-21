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

#include "../gui/Viewport.h"
#include "event_emitter.h"
#include <QPoint>
#include <QScrollBar>

class VEvent : public events::event_base {
protected:
    Viewport *_origin = nullptr;
    QPoint _scroll{};
    bool _repaintQueued = false;

public:
    explicit VEvent(Viewport *origin) noexcept : events::event_base(),
                                                 _origin(origin),
                                                 _scroll(origin->horizontalScrollBar()->value(),
                                                         origin->verticalScrollBar()->value()) {}
    VEvent(const VEvent &reference) noexcept : events::event_base(reference),
                                               _origin(reference._origin),
                                               _scroll(reference._scroll) {}
    ~VEvent() override = default;

public:
    Viewport *origin() noexcept { return this->_origin; }
    const QPoint &scroll() const noexcept { return this->_scroll; }

public:
    void queueRepaint() noexcept { this->_repaintQueued = true; }
    [[nodiscard]] const bool &repaintQueued() const noexcept {
        return this->_repaintQueued;
    }
};


#endif//IMAGE_MORPHING_TOOL_VEVENT_H
