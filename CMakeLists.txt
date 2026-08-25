cmake_minimum_required(VERSION 3.16)
project(logic_prover LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(logic_core
    src/prop.cpp
    src/sequent.cpp
    src/proof_tree.cpp
    src/parser.cpp
    src/solver.cpp
    src/counterexample.cpp
)

target_include_directories(logic_core PUBLIC include)

add_executable(prover src/main.cpp)
target_link_libraries(prover PRIVATE logic_core)

add_executable(tests src/tests.cpp)
target_link_libraries(tests PRIVATE logic_core)
