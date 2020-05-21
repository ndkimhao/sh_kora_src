//
// Created by kh on 5/20/20.
//

#include "Model.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <glpk.h>

namespace kora {

void Model::load_from_json(str json) {
    using namespace rapidjson;

    Document doc;
    doc.ParseInsitu(json.data());

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
                mids.emplace_back(get_metabolites_id(m.name.GetString()), m.value.GetDouble());
            }
            double obj = 0;
            if (r.HasMember("objective_coefficient"))
                obj = r["objective_coefficient"].GetDouble();
            reactions_.emplace_back(id, sid, Reaction::Bounds{lb, ub}, move(mids), obj);
            map_reactions_.emplace(sid, id);
            id++;
        }
    }
}

int Model::get_reaction_id(const str &sid) const {
    const auto it = map_reactions_.find(sid);
    CHECK(it != map_reactions_.end());
    return it->second;
}

int Model::get_metabolites_id(const str &sid) const {
    const auto it = map_metabolites_.find(sid);
    CHECK(it != map_metabolites_.end());
    return it->second;
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
    for (const Reaction &r : reactions_) {
        const auto &mc = r.mcoeffs();
        ind.resize(sz(mc) + 1), val.resize(sz(mc) + 1);
        int i = 1;
        for (const auto &p : mc) {
            ind[i] = p.mid + 1, val[i] = p.coeff;
            i++;
        }
        glp_set_mat_col(lp, r.fwd_id(), sz(mc), ind.data(), val.data());
        for (double &v : val) v = -v;
        glp_set_mat_col(lp, r.rev_id(), sz(mc), ind.data(), val.data());
    }

    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev = GLP_MSG_OFF;
    int ecode = glp_simplex(lp, &parm);
    assert(ecode == 0);

    str ret;
    {
        using namespace rapidjson;
        int status = glp_get_status(lp);
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
        doc.AddMember("fluxes", fluxes, doc.GetAllocator());


        Value show_prices;
        show_prices.SetObject();
        for (const auto &m : metabolites_) {
            double v = glp_get_row_dual(lp, m.row_id());
            show_prices.AddMember(StringRef(m.sid().c_str()), v, doc.GetAllocator());
        }
        doc.AddMember("show_prices", show_prices, doc.GetAllocator());

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        doc.Accept(writer);

        ret = buffer.GetString();
    }

    glp_delete_prob(lp);
    return ret;
}

}