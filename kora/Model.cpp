//
// Created by kh on 5/20/20.
//

#include "Model.h"

#include <fstream>

#include <rapidjson/document.h>

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
        for (const auto &m : ar) {
            str sid = m["id"].GetString();
            double lb = m["lower_bound"].GetDouble();
            double ub = m["upper_bound"].GetDouble();
            reactions_.emplace_back(id, sid, Reaction::Bounds{lb, ub});
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

}