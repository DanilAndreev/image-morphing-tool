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

#include "Stroke/StrokeManager.h"

#include <exceptions/NullPointerException.h>

void StrokeManager::bind(Stroke *target) noexcept {
    this->stroke = target;
}

void StrokeManager::unbind() noexcept {
    this->stroke = nullptr;
}

const StrokeManager &StrokeManager::rebuild() const {
    if (!this->stroke)
        throw Exceptions::NullPointerException{
                "You have to use 'bind()' method before executing context based operations."};

    return *this;
}
