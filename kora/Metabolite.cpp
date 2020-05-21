//
// Created by kh on 5/20/20.
//

#include "Metabolite.h"

#include <glpk.h>

namespace kora {

void Metabolite::setup_row(glp_prob *lp) const {
    glp_set_row_name(lp, row_id(), sid().c_str());
    glp_set_row_bnds(lp, row_id(), GLP_FX, 0.0, 0.0);
}

}