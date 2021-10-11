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

#include "core/utils.h"

#include <fstream>

std::string textFromFile(const char* filepath) {
    //TODO: handle errors
    std::string result{};
    std::ifstream file{filepath};
    file.seekg(0, std::ios::end);
    result.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    result.assign((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    file.close();
    return result;
}
