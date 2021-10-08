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

#ifndef IMAGE_MORPHING_TOOL_SNAPSHOT_H
#define IMAGE_MORPHING_TOOL_SNAPSHOT_H

#include <map>
#include <string>
#include <QDate>

#include "Memento.h"

class Snapshot : public std::map<std::string, Memento *> {
protected:
    QDate date;
public:
    Snapshot() noexcept;
    Snapshot(const Snapshot &) = delete;
    virtual ~Snapshot();
};


#endif //IMAGE_MORPHING_TOOL_SNAPSHOT_H
