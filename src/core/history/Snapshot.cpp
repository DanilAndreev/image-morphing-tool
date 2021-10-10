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

#include "core/history/Snapshot.h"

Snapshot::Snapshot() noexcept: std::map<std::string, Memento *>(), date(QDate::currentDate()) {}

Snapshot::~Snapshot() {
    for (const auto &item : *this) {
        delete item.second;
    }
}
