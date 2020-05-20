//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_REACTION_H
#define SH_KORA_REACTION_H

#include <kora/common.h>

#include <utility>

namespace kora {

class Reaction {
public:
    struct Bounds {
        double lower, upper;
    };

public:
    Reaction(str id, str name) : id_(move(id)), name_(move(name)), bounds_{-1000, 1000} {}

    Reaction(str id, str name, const Bounds &bounds) : id_(move(id)), name_(move(name)), bounds_(bounds) {}

    [[nodiscard]] const str &id() const { return id_; }

    [[nodiscard]] const str &name() const { return name_; }

    [[nodiscard]] const Bounds &bounds() const { return bounds_; }

private:
    str id_, name_;
    Bounds bounds_;
};

}

#endif //SH_KORA_REACTION_H
