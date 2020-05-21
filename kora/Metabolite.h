//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_METABOLITE_H
#define SH_KORA_METABOLITE_H

#include <kora/common.h>

#include <utility>

struct glp_prob;

namespace kora {

class Metabolite {
public:
    Metabolite(int id, str sid) : id_(id), sid_(std::move(sid)) {}

    [[nodiscard]] int id() const { return id_; }

    [[nodiscard]] int row_id() const { return id_ + 1; }

    [[nodiscard]] const str &sid() const { return sid_; }

    void setup_row(glp_prob *lp) const;

private:
    int id_;
    str sid_;
};

}

#endif //SH_KORA_METABOLITE_H
