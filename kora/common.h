//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_COMMON_H
#define SH_KORA_COMMON_H

#include <utility>
#include <string>
#include <vector>

namespace kora {

using str = std::string;
template<typename T> using vec = std::vector<T>;

}

#ifndef NDEBUG

#include <iostream>
#include <fmt/format.h>

#define DBG(f, ...) std::cerr << fmt::format(f "\n", ##__VA_ARGS__)
#else
#define DBG(...)
#endif

#endif //SH_KORA_COMMON_H
