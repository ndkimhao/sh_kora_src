#include <kora/Model.h>

using namespace kora;

int main(int argc, char *argv[]) {
    Model m;
    m.load_from_file(argv[1]);
    return 0;
}