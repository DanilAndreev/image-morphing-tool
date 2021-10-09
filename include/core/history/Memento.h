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

#ifndef IMAGE_MORPHING_TOOL_MEMENTO_H
#define IMAGE_MORPHING_TOOL_MEMENTO_H


class Memento {
public:
    Memento() = default;
    Memento(const Memento&) = default;
    virtual ~Memento() = default;
};


#endif //IMAGE_MORPHING_TOOL_MEMENTO_H
