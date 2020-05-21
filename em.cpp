//
// Created by kh on 5/20/20.
//

#include <kora/Model.h>

#include <emscripten/bind.h>

#include <cstring>

using namespace kora;

str analyze_fba(str json) {
    Model m;
    str err = m.load_from_json(move(json));
    if (!err.empty()) return err;
    return m.fba();
}

EMSCRIPTEN_BINDINGS(sh_kora) {
    emscripten::function("analyze_fba", &analyze_fba);
}