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

#ifndef IMAGE_MORPHING_TOOL_STROKE_H
#define IMAGE_MORPHING_TOOL_STROKE_H

#include <vector>
#include <QPoint>
#include <list>

class Stroke : public std::vector<QPoint> {
public:
    explicit Stroke(const std::list<QPoint>& points) noexcept;
    explicit Stroke(const std::vector<QPoint>& points) noexcept;
    virtual ~Stroke() = default;
public:

};


#endif //IMAGE_MORPHING_TOOL_STROKE_H
