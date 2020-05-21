//
// Created by kh on 5/21/20.
//

#include "utils.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <glpk.h>

using namespace rapidjson;

namespace kora {

str json_err(const char *msg) {
    Document doc;
    doc.SetObject();
    doc.AddMember("error", true, doc.GetAllocator());
    doc.AddMember("msg", StringRef(msg), doc.GetAllocator());
    return to_json(doc);
}

str json_err(const str &msg) {
    return json_err(msg.c_str());
}

str to_json(const rapidjson::Document &doc) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

void col_bnds(glp_prob *lp, int j, double lb, double ub) {
    CHECK(lb <= ub);
    int type = -1;
    if (lb == -DINF && ub == DINF) type = GLP_FR;
    else if (lb != -DINF && ub != DINF) type = (lb == ub) ? GLP_FX : GLP_DB;
    else if (lb != -DINF) type = GLP_LO;
    else if (ub != DINF) type = GLP_UP;
    else
        CHECK(0);

    glp_set_col_bnds(lp, j, type, lb, ub);
}

}