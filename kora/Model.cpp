//
// Created by kh on 5/20/20.
//

#include "Model.h"

#include <fstream>

#include <rapidjson/document.h>
#include <glpk.h>

namespace kora {

void Model::load_from_file(const str &path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path, std::ios_base::binary);

    file.seekg(0, std::ios::end);
    std::size_t sz = file.tellg();
    file.seekg(0, std::ios::beg);

    str ret;
    ret.resize(sz);
    file.read(ret.data(), sz);
    load_from_json(move(ret));
}

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

    int ecode = glp_simplex(lp, nullptr);
    DBG("ecode = {}", ecode);
    assert(ecode == 0);

    glp_delete_prob(lp);
    return "ret";
}

}