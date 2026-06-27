#ifndef RPG_ESTRATEGIA_H
#define RPG_ESTRATEGIA_H

#include <vector>
using namespace std;

namespace rpg {

// Forward declaration: os metodos so usam Jogador* / vector<Jogador*>&, nao precisam do tipo completo aqui
class Jogador;


// =============================================================
// PADRÃO STRATEGY
// Permite trocar o algoritmo de escolha de alvo sem alterar a classe Batalha
// =============================================================

/**
 * @class EstrategiaDeAlvo
 * @brief Padrão Strategy: contrato para algoritmos de escolha de alvo em batalha.
 * Permite à Batalha trocar o critério de seleção em tempo de execução (definir_estrategia()).
 */
class EstrategiaDeAlvo {
public:
    virtual ~EstrategiaDeAlvo();

    // Método Abstrato: cada estrategia concreta decide de um jeito diferente quem e o alvo
    virtual Jogador* escolher(Jogador* atacante, vector<Jogador*>& participantes) = 0;
};


/** @brief Estratégia concreta: ataca o primeiro participante vivo da lista. */
class AlvoRoundRobin : public EstrategiaDeAlvo {
public:
    Jogador* escolher(Jogador* atacante, vector<Jogador*>& participantes) override;
};


/** @brief Estratégia concreta: ataca sempre quem tem menos HP. */
class AlvoMaisFraco : public EstrategiaDeAlvo {
public:
    Jogador* escolher(Jogador* atacante, vector<Jogador*>& participantes) override;
};


/** @brief Estratégia concreta: ataca sempre quem tem mais HP (maior ameaça). */
class AlvoMaisForte : public EstrategiaDeAlvo {
public:
    Jogador* escolher(Jogador* atacante, vector<Jogador*>& participantes) override;
};

} // namespace rpg

#endif // RPG_ESTRATEGIA_H
