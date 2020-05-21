//
// Created by kh on 5/20/20.
//

#include "Reaction.h"

#include <glpk.h>

namespace kora {

const double DINF = 1e300;

void col_bnds(glp_prob *lp, int j, double lb, double ub) {
    assert(lb <= ub);
    int type = -1;
    if (lb == -DINF && ub == DINF) type = GLP_FR;
    else if (lb != -DINF && ub != DINF) type = (lb == ub) ? GLP_FX : GLP_DB;
    else if (lb != -DINF) type = GLP_LO;
    else if (ub != DINF) type = GLP_UP;
    else
        assert(0);

    glp_set_col_bnds(lp, j, type, lb, ub);
}

void Reaction::setup_column(glp_prob *lp) const {
    glp_set_col_name(lp, fwd_id(), sid().c_str());
    glp_set_col_name(lp, rev_id(), sid_rev().c_str());
    assert(bounds_.lower <= bounds_.upper);
    if (bounds_.lower > 0) {
        col_bnds(lp, fwd_id(), bounds_.lower, bounds_.upper);
        col_bnds(lp, rev_id(), 0, 0);
    } else if (bounds_.upper < 0) {
        col_bnds(lp, fwd_id(), 0, 0);
        col_bnds(lp, rev_id(), -bounds_.upper, -bounds_.lower);
    } else {
        col_bnds(lp, fwd_id(), 0, bounds_.upper);
        col_bnds(lp, rev_id(), 0, -bounds_.lower);
    }
    if (obj_coeff_ != 0) {
        glp_set_obj_coef(lp, fwd_id(), obj_coeff_);
        glp_set_obj_coef(lp, rev_id(), -obj_coeff_);
    }
}

void Reaction::fill_column_coeffs(glp_prob *lp, vec<int> &ind, vec<double> &val) const {
    const auto &mc = mcoeffs();
    ind.resize(sz(mc) + 1), val.resize(sz(mc) + 1);
    int i = 1;
    for (const auto &p : mc) {
        ind[i] = p.mid + 1, val[i] = p.coeff;
        i++;
    }
    glp_set_mat_col(lp, fwd_id(), sz(mc), ind.data(), val.data());
    for (double &v : val) v = -v;
    glp_set_mat_col(lp, rev_id(), sz(mc), ind.data(), val.data());
}

}