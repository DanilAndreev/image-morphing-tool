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

#include <QApplication>
#include <QDebug>
#include "Viewport/Viewport.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Viewport viewport{};
    viewport.resize(800, 600);
    viewport.show();
    return QApplication::exec();
}
