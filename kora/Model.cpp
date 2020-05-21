//
// Created by kh on 5/20/20.
//

#include "Model.h"

#include <rapidjson/document.h>
#include <glpk.h>

#include <kora/utils.h>

namespace kora {

str Model::load_from_json(str json) {
    using namespace rapidjson;

    Document doc;
    doc.ParseInsitu(json.data());
    if (!doc.IsObject())
        return json_err("Model is not an object / Parse error");

    {
        const auto &am = doc["metabolites"].GetArray();
        metabolites_.reserve(am.Size());
        int id = 0;
        for (const auto &m : am) {
            str sid = m["id"].GetString();
            metabolites_.emplace_back(id, sid);
            map_metabolites_.emplace(sid, id);
            id++;
        }
    }
    {
        const auto &ar = doc["reactions"].GetArray();
        reactions_.reserve(ar.Size());
        int id = 0;
        for (const auto &r : ar) {
            str sid = r["id"].GetString();
            double lb = r["lower_bound"].GetDouble();
            double ub = r["upper_bound"].GetDouble();
            const auto &am = r["metabolites"].GetObject();
            vec<Reaction::MCoeff> mids;
            mids.reserve(am.MemberCount());
            for (const auto &m : am) {
                int mid = get_metabolites_id(m.name.GetString());
                if (mid == -1)
                    return json_err(format("Metabolite %s not found for reaction %s",
                                           m.name.GetString(), sid.c_str()));
                mids.emplace_back(mid, m.value.GetDouble());
            }
            double obj = 0;
            if (r.HasMember("objective_coefficient"))
                obj = r["objective_coefficient"].GetDouble();
            reactions_.emplace_back(id, sid, Reaction::Bounds{lb, ub}, move(mids), obj);
            map_reactions_.emplace(sid, id);
            id++;
        }
    }

    return "";
}

int Model::get_reaction_id(const str &sid) const {
    const auto it = map_reactions_.find(sid);
    return it != map_reactions_.end() ? it->second : -1;
}

int Model::get_metabolites_id(const str &sid) const {
    const auto it = map_metabolites_.find(sid);
    return it != map_metabolites_.end() ? it->second : -1;
}

static const char *glp_stt_to_str(int s) {
    switch (s) {
        case GLP_OPT:
            return "optimal";
        case GLP_FEAS:
            return "feasible";
        case GLP_INFEAS:
            return "infeasible";
        case GLP_NOFEAS:
            return "no feasible solution";
        case GLP_UNBND:
            return "unbounded solution";
        case GLP_UNDEF:
            return "undefined";
        default:
            assert(0);
    }
    return "";
}

str Model::fba() const {
    glp_prob *lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MAX);

    glp_add_rows(lp, sz(metabolites_));
    for (const Metabolite &m : metabolites_) m.setup_row(lp);

    glp_add_cols(lp, sz(reactions_) * 2);
    for (const Reaction &r : reactions_) r.setup_column(lp);

    vec<int> ind;
    vec<double> val;
    for (const Reaction &r : reactions_) r.fill_column_coeffs(lp, ind, val);

    glp_smcp parm;
    glp_init_smcp(&parm);
    //parm.msg_lev = GLP_MSG_ERR;
    parm.presolve = GLP_ON;
    int ecode = glp_simplex(lp, &parm);
    CHECK(ecode == 0 || ecode == GLP_ENOPFS || ecode == GLP_ENODFS);
    int status = glp_get_status(lp);
    if (ecode == GLP_ENOPFS || ecode == GLP_ENODFS || status == GLP_UNDEF) {
        fprintf(stderr, "LP solver returns GLP_UNDEF, turn off presolve and retry\n");
        parm.presolve = GLP_OFF;
        glp_adv_basis(lp, 0);
        ecode = glp_simplex(lp, &parm);
        CHECK(ecode == 0);
        status = glp_get_status(lp);
    }

    str ret;
    {
        using namespace rapidjson;
        double objective_value = glp_get_obj_val(lp);

        Document doc;
        doc.SetObject();
        doc.AddMember("status", StringRef(glp_stt_to_str(status)), doc.GetAllocator());
        doc.AddMember("objective_value", objective_value, doc.GetAllocator());

        Value fluxes;
        fluxes.SetObject();
        for (const auto &r : reactions_) {
            double fwd = glp_get_col_prim(lp, r.fwd_id()), rev = glp_get_col_prim(lp, r.rev_id());
            fluxes.AddMember(StringRef(r.sid().c_str()), fwd - rev, doc.GetAllocator());
        }
        doc.AddMember("fluxes", std::move(fluxes), doc.GetAllocator());


        Value show_prices;
        show_prices.SetObject();
        for (const auto &m : metabolites_) {
            double v = glp_get_row_dual(lp, m.row_id());
            show_prices.AddMember(StringRef(m.sid().c_str()), v, doc.GetAllocator());
        }
        doc.AddMember("show_prices", std::move(show_prices), doc.GetAllocator());

        ret = to_json(doc);
    }

    glp_delete_prob(lp);
    return ret;
}

str Model::fva(FvaOpts opts) const {
    CHECK(0 <= opts.fraction_of_optimum && opts.fraction_of_optimum <= 1);
    glp_prob *lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MAX);

    glp_add_rows(lp, sz(metabolites_));
    for (const Metabolite &m : metabolites_) m.setup_row(lp);

    glp_add_cols(lp, sz(reactions_) * 2);
    for (const Reaction &r : reactions_) r.setup_column(lp);

    vec<int> ind;
    vec<double> val;
    for (const Reaction &r : reactions_) r.fill_column_coeffs(lp, ind, val);

    glp_smcp parm;
    glp_init_smcp(&parm);
    //parm.msg_lev = GLP_MSG_ERR;
    parm.presolve = GLP_ON;
    int ecode = glp_simplex(lp, &parm);
    CHECK(ecode == 0 || ecode == GLP_ENOPFS || ecode == GLP_ENODFS);
    int status = glp_get_status(lp);
    if (ecode == GLP_ENOPFS || ecode == GLP_ENODFS || status != GLP_OPT) {
        return json_err(format("There is no optimal solution for the chosen objective (ecode=%d, status=%d)",
                               ecode, status));
    }
    double objective_value = glp_get_obj_val(lp);

    int obj_col = glp_add_cols(lp, 1);
    glp_set_col_name(lp, obj_col, "fva_old_objective");
    col_bnds(lp, obj_col, opts.fraction_of_optimum * objective_value, objective_value);

    int obj_row = glp_add_rows(lp, 1);
    glp_set_row_name(lp, obj_row, "fva_old_objective_constraint");
    glp_set_row_bnds(lp, obj_row, GLP_FX, 0, 0);
    {
        ind.resize(1), val.resize(1);
        for (const Reaction &r : reactions_) {
            double c = r.obj_coeff();
            if (c != 0.0) {
                ind.emplace_back(r.fwd_id()), val.emplace_back(c);
                ind.emplace_back(r.rev_id()), val.emplace_back(-c);
            }
            if (r.obj_coeff() != 0.0) {
                glp_set_obj_coef(lp, r.fwd_id(), 0);
                glp_set_obj_coef(lp, r.rev_id(), 0);
            }
        }
        ind.emplace_back(obj_col), val.emplace_back(-1.0);
        CHECK(ind.size() == val.size());
        glp_set_mat_row(lp, obj_row, sz(ind) - 1, ind.data(), val.data());
    }

    tsl::robin_map<str, double> res_min, res_max;
    res_min.reserve(opts.reactions.size()), res_max.reserve(opts.reactions.size());
    for (int _dir = 0; _dir < 2; ++_dir) {
        int dir = (_dir == 0) ? GLP_MIN : GLP_MAX;
        glp_set_obj_dir(lp, dir);
        bool first_run = true;

        auto &res = (dir == GLP_MIN) ? res_min : res_max;
        for (const Reaction &r : reactions_) {
            glp_set_obj_coef(lp, r.fwd_id(), 1.0);
            glp_set_obj_coef(lp, r.rev_id(), -1.0);

            parm.msg_lev = GLP_MSG_ERR;
            if (first_run) {
                parm.presolve = GLP_ON;
                glp_adv_basis(lp, 0);
                first_run = false;
            } else {
                parm.presolve = GLP_OFF;
            }
            ecode = glp_simplex(lp, &parm);
            CHECK(ecode == 0 || ecode == GLP_ENOPFS || ecode == GLP_ENODFS);
            status = glp_get_status(lp);
            if (ecode == GLP_ENOPFS || ecode == GLP_ENODFS || status == GLP_UNDEF) {
                fprintf(stderr, "FVA: LP solver returns GLP_UNDEF, retry\n");
                glp_adv_basis(lp, 0);
                parm.presolve = GLP_ON;
                ecode = glp_simplex(lp, &parm);
                CHECK(ecode == 0);
                status = glp_get_status(lp);
            }

            CHECK(status == GLP_OPT);
            double objval = glp_get_obj_val(lp);
            res[r.sid()] = objval;

            glp_set_obj_coef(lp, r.fwd_id(), 0.0);
            glp_set_obj_coef(lp, r.rev_id(), 0.0);
        }
    }

    {
        using namespace rapidjson;
        Document doc;
        doc.SetObject();

        Value amin;
        amin.SetObject();
        amin.MemberReserve(res_min.size(), doc.GetAllocator());
        for (const auto &p : res_min)
            amin.AddMember(StringRef(p.first.c_str()), p.second, doc.GetAllocator());
        doc.AddMember("minimum", std::move(amin), doc.GetAllocator());

        Value amax;
        amax.SetObject();
        amax.MemberReserve(res_max.size(), doc.GetAllocator());
        for (const auto &p : res_max)
            amax.AddMember(StringRef(p.first.c_str()), p.second, doc.GetAllocator());
        doc.AddMember("maximum", std::move(amax), doc.GetAllocator());

        return to_json(doc);
    }
}

}