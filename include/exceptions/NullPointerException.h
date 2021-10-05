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

#ifndef IMAGE_MORPHING_TOOL_NULLPOINTEREXCEPTION_H
#define IMAGE_MORPHING_TOOL_NULLPOINTEREXCEPTION_H

#include "Exception.h"

namespace Exceptions {
    class NullPointerException : public Exception {
    public:
        explicit NullPointerException(const QByteArray &message = "") noexcept: Exception(message) {}
    };
}

#endif //IMAGE_MORPHING_TOOL_NULLPOINTEREXCEPTION_H
