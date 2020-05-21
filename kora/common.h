//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_COMMON_H
#define SH_KORA_COMMON_H

#include <utility>
#include <string>
#include <vector>
#include <cassert>

namespace kora {

using str = std::string;
template<typename T> using vec = std::vector<T>;

template<typename T>
int sz(const T &c) { return int(c.size()); }

}

extern void __assert_fail(const char *__assertion, const char *__file,
                          unsigned int __line, const char *__function)
noexcept __attribute__ ((__noreturn__));

#define CHECK(expr)                            \
     (static_cast <bool> (expr)                \
      ? void (0)                            \
      : __assert_fail (#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#endif //SH_KORA_COMMON_H
