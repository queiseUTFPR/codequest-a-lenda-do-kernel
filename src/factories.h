#ifndef RPG_FACTORIES_H
#define RPG_FACTORIES_H

#include "item.h"
#include "personagens.h"
#include "habilidade.h"
using namespace std;

namespace rpg {

/** @brief Padrão Factory: centraliza a criação de Arma/Armadura/Pocao/ItemEspecial a partir de uma string de tipo. */
class ItemFactory {
public:
    static Arma*     criar_arma(string tipo);
    static Armadura* criar_armadura(string tipo);
    static Pocao*    criar_pocao(string tipo);
    static ItemEspecial* criar_item_especial(string tipo);
    static Item*     criar_item_por_codigo(string codigo);
    static string    codigo_item(Item* item);
    static int       preco_item_por_codigo(string codigo);
};


/** @brief Padrão Factory: cria personagens com atributos padrão fixos por classe. */
class JogadorFactory {
public:
    static Guerreiro* criar_guerreiro(string nome, string raca);
    static Mago*       criar_mago(string nome, string raca);
    static Monstro*      criar_monstro(string nome, string raca);
    static Arqueiro*    criar_arqueiro(string nome, string raca);
    static Clerigo*     criar_clerigo(string nome, string raca);
};


/** @brief Padrão Factory: centraliza a criação de habilidades por categoria. */
class HabilidadeFactory {
public:
    static Habilidade* criar_ofensiva(string tipo);
    static Habilidade* criar_defensiva(string tipo);
    static Habilidade* criar_suporte(string tipo);
    static Habilidade* criar_por_codigo(string codigo);
    static string      codigo_habilidade(Habilidade* habilidade);
};

} // namespace rpg

#endif // RPG_FACTORIES_H
