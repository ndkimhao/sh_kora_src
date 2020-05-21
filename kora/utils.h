//
// Created by kh on 5/21/20.
//

#ifndef SH_KORA_UTILS_H
#define SH_KORA_UTILS_H

#include <kora/common.h>

#include <memory>
#include <rapidjson/document.h>

struct glp_prob;

namespace kora {

str json_err(const char *msg);

str json_err(const str &msg);

str to_json(const rapidjson::Document &doc);

template<typename ... Args>
std::string format(const std::string &format, Args ... args) {
    int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    CHECK(size > 0);
    std::string ret(size, '\0');
    snprintf(ret.data(), size, format.c_str(), args ...);
    ret.resize(ret.size() - 1);
    return ret;
}

const double DINF = 1e300;

void col_bnds(glp_prob *lp, int j, double lb, double ub);

}


#endif //SH_KORA_UTILS_H
