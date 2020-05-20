//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_MODEL_H
#define SH_KORA_MODEL_H

#include <kora/common.h>
#include <kora/Reaction.h>
#include <kora/Metabolite.h>

#include <tsl/robin_map.h>

namespace kora {

class Model {
public:
    void load_from_file(const str &path);

    void load_from_json(str json);

protected:
    [[nodiscard]] int get_reaction_id(const str &sid) const;

    [[nodiscard]] int get_metabolites_id(const str &sid) const;

private:
    vec<Reaction> reactions_;
    vec<Metabolite> metabolites_;
    tsl::robin_map<str, int> map_reactions_;
    tsl::robin_map<str, int> map_metabolites_;
};

}

#endif //SH_KORA_MODEL_H
