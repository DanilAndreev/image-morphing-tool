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

#include <exception>
#include <iostream>

#include <QApplication>
#include <QDebug>

#include "Viewport/Viewport.h"

int main(int argc, char *argv[]) {
    try {
        QApplication a(argc, argv);
        Viewport viewport{};
        viewport.resize(800, 600);
        viewport.show();
        return QApplication::exec();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;
        exit(-1);
    } catch (...) {
        std::cerr << "Fatal error: Unexpected exception." << std::endl;
        exit(-1);
    }
}
