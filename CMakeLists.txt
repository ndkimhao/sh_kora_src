cmake_minimum_required(VERSION 3.16)
project(sh_kora)

set(CMAKE_CXX_STANDARD 17)

add_executable(sh_kora main.cpp kora/Model.cpp kora/Model.h kora/common.h kora/Reaction.cpp kora/Reaction.h kora/Metabolite.cpp kora/Metabolite.h)

target_include_directories(sh_kora PUBLIC
        .
        third_party/fmt/include
        third_party/rapidjson/include
        third_party/robin-map/include)

target_sources(sh_kora PUBLIC
        third_party/fmt/src/format.cc third_party/fmt/src/os.cc)