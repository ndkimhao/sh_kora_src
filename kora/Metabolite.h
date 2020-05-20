//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_METABOLITE_H
#define SH_KORA_METABOLITE_H

#include <kora/common.h>

#include <utility>

namespace kora {

class Metabolite {
public:
    Metabolite(int id, str sid) : id_(id), sid_(std::move(sid)) {}

    [[nodiscard]] int id() const { return id_; }

    [[nodiscard]] const str &sid() const { return sid_; }

private:
    int id_;
    str sid_;
};

}

#endif //SH_KORA_METABOLITE_H
