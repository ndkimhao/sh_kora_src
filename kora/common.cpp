//
// Created by kh on 5/21/20.
//

#include <kora/common.h>

extern const char *__progname;

namespace kora {

void assert_fail(const char *assertion, const char *file, unsigned int line, const char *function) noexcept {
    fprintf(stderr, "%s%s%s:%u: %s%sAssertion `%s' failed.\n",
            __progname, __progname[0] ? ": " : "",
            file, line,
            function ? function : "", function ? ": " : "",
            assertion);
    abort();
}

}