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
    Reaction(int id, str sid, const Bounds &bounds) : id_(id), sid_(std::move(sid)), bounds_(bounds) {}

    [[nodiscard]] int id() const { return id_; }

    [[nodiscard]] const str &sid() const { return sid_; }

    [[nodiscard]] const Bounds &bounds() const { return bounds_; }

private:
    int id_;
    str sid_;
    Bounds bounds_;
};

}

#endif //SH_KORA_REACTION_H
