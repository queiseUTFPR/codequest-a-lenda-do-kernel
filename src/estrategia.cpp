#include "estrategia.h"
#include "jogador.h" // Necessario aqui pois os corpos chamam p->get_hp_atual()
using namespace std;

namespace rpg {

EstrategiaDeAlvo::~EstrategiaDeAlvo() {}

Jogador* AlvoRoundRobin::escolher(Jogador* atacante, vector<Jogador*>& participantes) {
    for (Jogador* p : participantes) {
        if (p != atacante && p->get_hp_atual() > 0) return p;
    }
    return nullptr;
}

Jogador* AlvoMaisFraco::escolher(Jogador* atacante, vector<Jogador*>& participantes) {
    Jogador* mais_fraco = nullptr;
    for (Jogador* p : participantes) {
        if (p == atacante || p->get_hp_atual() <= 0) continue;
        if (!mais_fraco || p->get_hp_atual() < mais_fraco->get_hp_atual()) mais_fraco = p;
    }
    return mais_fraco;
}

Jogador* AlvoMaisForte::escolher(Jogador* atacante, vector<Jogador*>& participantes) {
    Jogador* mais_forte = nullptr;
    for (Jogador* p : participantes) {
        if (p == atacante || p->get_hp_atual() <= 0) continue;
        if (!mais_forte || p->get_hp_atual() > mais_forte->get_hp_atual()) mais_forte = p;
    }
    return mais_forte;
}

} // namespace rpg
