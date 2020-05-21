//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_REACTION_H
#define SH_KORA_REACTION_H

#include <kora/common.h>

#include <utility>

struct glp_prob;

namespace kora {

class Reaction {
public:
    struct Bounds {
        double lower, upper;
    };

    struct MCoeff {
        int mid;
        double coeff;

        MCoeff(int mid, double coeff) : mid(mid), coeff(coeff) {}
    };

public:
    Reaction(int id, str sid, const Bounds &bounds, vec<MCoeff> mcoeffs, double obj_coeff) :
            id_(id), sid_(std::move(sid)), bounds_(bounds),
            mcoeffs_(std::move(mcoeffs)), obj_coeff_(obj_coeff) {}

    [[nodiscard]] int id() const { return id_; }

    [[nodiscard]] int fwd_id() const { return id_ * 2 + 1; }

    [[nodiscard]] int rev_id() const { return id_ * 2 + 2; }

    [[nodiscard]] const str &sid() const { return sid_; }

    [[nodiscard]] str sid_rev() const { return sid_ + "_rev"; }

    [[nodiscard]] const Bounds &bounds() const { return bounds_; }

    [[nodiscard]] const vec<MCoeff> &mcoeffs() const { return mcoeffs_; }

    [[nodiscard]] double obj_coeff() const { return obj_coeff_; }

    void setup_column(glp_prob *lp) const;

    void fill_column_coeffs(glp_prob *lp, vec<int> &ind, vec<double> &val) const;

private:
    int id_;
    str sid_;
    Bounds bounds_;
    vec<MCoeff> mcoeffs_;
    double obj_coeff_;
};

}

#endif //SH_KORA_REACTION_H
