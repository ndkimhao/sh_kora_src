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

private:
    str id_, name_;
    tsl::robin_map<str, Reaction> reactions_;
    tsl::robin_map<str, Metabolite> metabolites_;
};

}

#endif //SH_KORA_MODEL_H
