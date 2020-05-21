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

template<typename T>
int sz(const T &c) { return int(c.size()); }

}

#ifndef NDEBUG

#  include <iostream>
#  include <fmt/format.h>

#  define DBG(f, ...) std::cerr << fmt::format(f "\n", ##__VA_ARGS__)

#else

#  define DBG(...)

#endif

#include <cassert>

#define CHECK(...) assert(__VA_ARGS__)

#endif //SH_KORA_COMMON_H
