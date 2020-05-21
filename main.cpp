#include <iostream>
#include <fstream>

#include <kora/Model.h>

using namespace kora;

str load_file(const str &path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path, std::ios_base::binary);

    file.seekg(0, std::ios::end);
    std::size_t sz = file.tellg();
    file.seekg(0, std::ios::beg);

    str ret;
    ret.resize(sz);
    file.read(ret.data(), sz);
    return ret;
}

int main(int argc, char *argv[]) {
    Model m;
    m.load_from_json(load_file(argv[1]));
    str res = m.fba();
    std::cout << res;
    if (argc >= 3) {
        std::ofstream ofs(argv[2]);
        ofs << res;
    }
    return 0;
}
