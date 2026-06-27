cmake_minimum_required(VERSION 3.10)
project(CodeQuestKernel CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Biblioteca com todas as classes do jogo (tudo, exceto main.cpp), reutilizada pelo executavel e pelos testes
add_library(rpg_lib
    src/item.cpp
    src/inventario.cpp
    src/habilidade.cpp
    src/raca.cpp
    src/jogador.cpp
    src/personagens.cpp
    src/estrategia.cpp
    src/factories.cpp
    src/persistencia.cpp
    src/batalha.cpp
    src/gerenciador.cpp
    src/menu.cpp
)
target_include_directories(rpg_lib PUBLIC src)

# Executavel principal do jogo
add_executable(codequest src/main.cpp)
target_link_libraries(codequest PRIVATE rpg_lib)

# Testes unitarios com Google Test (opcional: so habilita se a biblioteca estiver instalada)
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    add_executable(rpg_tests tests/test_rpg.cpp)
    target_link_libraries(rpg_tests PRIVATE rpg_lib GTest::gtest pthread)
    add_test(NAME rpg_tests COMMAND rpg_tests)
else()
    message(STATUS "GTest nao encontrado - alvo 'rpg_tests' nao sera criado. Instale libgtest-dev para habilitar os testes.")
endif()
