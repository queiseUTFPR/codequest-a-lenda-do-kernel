#ifndef RPG_BATALHA_H
#define RPG_BATALHA_H

#include <vector>
#include <list>
#include <map>
#include <string>
#include "jogador.h"
#include "estrategia.h"
using namespace std;

namespace rpg {

// =============================================================
// BATALHA
// Gerencia o loop de combate por turnos entre dois ou mais personagens
// =============================================================

/**
 * @class Batalha
 * @brief RF5: gerencia o loop de combate por turnos entre dois ou mais personagens.
 *
 * Delega a escolha de alvo para uma EstrategiaDeAlvo (Strategy) e suporta participantes
 * controlados por bot ou por menu interativo (humano vs bot, humano vs humano).
 */
class Batalha {
private:
    vector<Jogador*>  participantes;
    list<string>      log;            // STL list: historico sequencial de eventos da batalha
    map<string, int>  dano_causado;   // STL map: total de dano causado por cada participante (por nome)
    int               turno_atual;
    vector<Jogador*>  humanos;     // RF5: participantes controlados via menu (suporta humano vs humano)
    EstrategiaDeAlvo* estrategia;  // Strategy: algoritmo de escolha de alvo, intercambiavel em runtime

public:
    Batalha(vector<Jogador*> personagens, Jogador* humano = nullptr, EstrategiaDeAlvo* estrategia_alvo = nullptr);
    ~Batalha(); // Libera a estrategia, que pertence a esta Batalha

    // RF5: marca mais um participante como controlado por menu (permite humano vs humano)
    void adicionar_jogador_humano(Jogador* j);

    // Strategy: troca o algoritmo de escolha de alvo em tempo de execucao, sem alterar esta classe
    void definir_estrategia(EstrategiaDeAlvo* nova);

    void registrar_log(string mensagem);
    void exibir_log();
    void exibir_estatisticas(); // Total de dano causado por cada participante

    bool batalha_terminou(); // Termina quando restar 1 ou 0 participantes vivos
    Jogador* vencedor();     // Unico sobrevivente, ou nullptr se empatou

    Jogador* escolher_alvo(Jogador* atacante); // Strategy: delega para o algoritmo configurado

    void agir_bot(Jogador* atacante);    // Acao automatica: sempre ataca o alvo escolhido
    void agir_humano(Jogador* atacante); // Menu interativo (humano vs bot)

    void executar_turno();              // RF5: cada participante vivo age uma vez
    void iniciar(int max_turnos = 20);  // RF5: loop principal da batalha por turnos
};

} // namespace rpg

#endif // RPG_BATALHA_H
