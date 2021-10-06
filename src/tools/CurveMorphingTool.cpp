// Copyright (c) 2021-2021.
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

#include "tools/CurveMorphingTool.h"

#include "Stroke/StrokeDrawer.h"

CurveMorphingTool::CurveMorphingTool() noexcept: displayDirections(false) {

}

CurveMorphingTool::CurveMorphingTool(const CurveMorphingTool &reference) noexcept {

}

void CurveMorphingTool::paintEvent(QPaintEvent *event, QPainter& painter) noexcept {
    StrokeDrawer drawer{};
    drawer.bind(&this->strokeFrom);
    drawer.drawPoints(painter);
    drawer.bind(&this->strokeTo);
    drawer.drawPoints(painter);
    drawer.unbind();

    if (this->displayDirections) {
        for (std::size_t i = 0; i < this->strokeFrom.size(); i++) {
            painter.drawLine(this->strokeFrom.at(i), this->strokeTo.at(i));
        }
    }
}
