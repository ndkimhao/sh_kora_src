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


#if defined BUILD_EM || !defined NDEBUG

#define CHECK(expr) assert(expr)

#else

void assert_fail(const char *assertion, const char *file, unsigned int line, const char *function)
noexcept __attribute__ ((__noreturn__));

#define CHECK(expr)                            \
     (static_cast <bool> (expr)                \
      ? void (0)                            \
      : ::kora::assert_fail(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#endif

}

#endif //SH_KORA_COMMON_H
