//
// Created by kh on 5/20/20.
//

#include <kora/Model.h>

#include <emscripten/em_macros.h>
#include <cstring>

using namespace kora;

extern "C" {

EMSCRIPTEN_KEEPALIVE char *analyze_fba(const char *json) {
    Model m;
    m.load_from_json(json);

    str sret = m.fba();
    char *ret = (char *) malloc(sz(sret) + 1);
    memcpy(ret, sret.c_str(), sz(sret) + 1);
    return ret;
}

}