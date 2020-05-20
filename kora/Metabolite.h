//
// Created by kh on 5/20/20.
//

#ifndef SH_KORA_METABOLITE_H
#define SH_KORA_METABOLITE_H

#include <kora/common.h>

namespace kora {

class Metabolite {
public:
    Metabolite(str id, str name) : id_(std::move(id)), name_(std::move(name)) {}

    [[nodiscard]] const str &id() const { return id_; }

    [[nodiscard]] const str &name() const { return name_; }

private:
    str id_, name_;
};

}

#endif //SH_KORA_METABOLITE_H
