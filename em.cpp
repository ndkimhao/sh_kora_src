//
// Created by kh on 5/20/20.
//

#include <kora/Model.h>

#include <emscripten/bind.h>

#include <cstring>

using namespace kora;

str analyze_fba(str json) {
    Model m;
    m.load_from_json(move(json));
    return m.fba();
}

EMSCRIPTEN_BINDINGS(sh_kora) {
    emscripten::function("analyze_fba", &analyze_fba);
}