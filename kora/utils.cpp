//
// Created by kh on 5/21/20.
//

#include "utils.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

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

}