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
    
}

}