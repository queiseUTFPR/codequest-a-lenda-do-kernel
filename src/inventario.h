#ifndef RPG_INVENTARIO_H
#define RPG_INVENTARIO_H

#include <string>
#include <vector>
#include <stdexcept>
#include "item.h"
using namespace std;

namespace rpg {

// Forward declaration: necessária pois usar_pocao() chama métodos de Jogador (implementado em jogador.cpp)
class Jogador;


// =============================================================
// EXCEÇÕES DO INVENTÁRIO
// Tratamento de Exceções: cada classe mapeia uma violação de regra do inventário
// =============================================================

class InventarioCheioError : public runtime_error {
public:
    InventarioCheioError(const string& msg);
};

class PesoExcedidoError : public runtime_error {
public:
    PesoExcedidoError(const string& msg);
};

class ItemNaoEncontradoError : public runtime_error {
public:
    ItemNaoEncontradoError(const string& msg);
};


/**
 * @class Inventario
 * @brief Composição: pertence a um único Jogador e gerencia seus itens.
 *
 * Controla capacidade (número de slots) e peso máximo da mochila, além dos
 * slots de equipamento (arma/armadura), que fornecem bônus automáticos de
 * ataque/defesa ao personagem (RF4).
 */
class Inventario {
private:
    vector<Item*> mochila;          // Itens carregados mas nao equipados
    Arma*         arma_equipada;
    Armadura*     armadura_equipada;
    int           capacidade_max;   // Numero maximo de slots na mochila
    float         peso_max;         // Peso maximo suportado (kg)

public:
    Inventario(int capacidade = 10, float peso_maximo = 50.0f);
    ~Inventario(); // Libera todos os itens da mochila e os equipados

    float peso_atual();

    // RF3: Adiciona/remove itens da mochila, validando capacidade e peso
    void  adicionar_item(Item* item);
    Item* remover_item(string nome);

    // RF4: Equipar/desequipar arma e armadura, com atualizacao automatica de atributos
    void equipar_arma(string nome);
    void desequipar_arma();
    void equipar_armadura(string nome);
    void desequipar_armadura();

    int bonus_ataque(); // Bonus de ataque da arma equipada
    int bonus_defesa(); // Bonus de defesa da armadura equipada

    // RF3: Consome uma pocao da mochila e aplica o efeito no dono (implementado em inventario.cpp,
    // que inclui jogador.h, pois precisa chamar dono->curar())
    void usar_pocao(string nome, Jogador* dono);
    void usar_item_especial(string nome, Jogador* dono);

    // RF7: serializa e restaura mochila, arma equipada e armadura equipada
    string serializar();
    void restaurar(string dados);

    void listar();
};

} // namespace rpg

#endif // RPG_INVENTARIO_H
