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

#ifndef IMAGE_MORPHING_TOOL_EXCEPTION_H
#define IMAGE_MORPHING_TOOL_EXCEPTION_H

#include <QException>
#include <utility>

namespace Exceptions {
    class Exception : QException {
    protected:
        QByteArray message;
    public:
        explicit Exception(QByteArray message = "") noexcept: message(std::move(message)) {}

    public:
        [[nodiscard]] const char *what() const noexcept override { return reinterpret_cast<const char *>(this->message.data()); }

        [[nodiscard]] QException *clone() const noexcept override { return new Exception(*this); }
    };
}


#endif //IMAGE_MORPHING_TOOL_EXCEPTION_H
