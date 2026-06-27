#include "gerenciador.h"
#include <iostream>
#include <algorithm>
using namespace std;

namespace rpg {

PersonagemNaoEncontradoError::PersonagemNaoEncontradoError(const string& msg) : runtime_error(msg) {}

// Inicializacao do ponteiro estatico do Singleton (obrigatoria fora da classe)
GerenciadorDeJogo* GerenciadorDeJogo::instancia = nullptr;

GerenciadorDeJogo::GerenciadorDeJogo() {}

GerenciadorDeJogo::~GerenciadorDeJogo() {
    for (Jogador* p : personagens) delete p;
}

GerenciadorDeJogo& GerenciadorDeJogo::instance() {
    if (!instancia) instancia = new GerenciadorDeJogo(); // Cria a unica instancia na primeira chamada
    return *instancia;
}

void GerenciadorDeJogo::destruir_instancia() {
    delete instancia;
    instancia = nullptr;
}

Guerreiro* GerenciadorDeJogo::criar_guerreiro(string nome, string raca, int forca) {
    Guerreiro* g = new Guerreiro(nome, forca, raca);
    personagens.push_back(g);
    return g;
}

Mago* GerenciadorDeJogo::criar_mago(string nome, string raca, int inteligencia, int mana) {
    Mago* m = new Mago(nome, inteligencia, mana, raca);
    personagens.push_back(m);
    return m;
}

Monstro* GerenciadorDeJogo::criar_monstro(string nome, string raca, int ferocidade, float chance_critico) {
    Monstro* l = new Monstro(nome, ferocidade, chance_critico, raca);
    personagens.push_back(l);
    return l;
}

Arqueiro* GerenciadorDeJogo::criar_arqueiro(string nome, string raca, int destreza, int flechas) {
    Arqueiro* a = new Arqueiro(nome, destreza, flechas, raca);
    personagens.push_back(a);
    return a;
}

Clerigo* GerenciadorDeJogo::criar_clerigo(string nome, string raca, int sabedoria, int fe) {
    Clerigo* c = new Clerigo(nome, sabedoria, fe, raca);
    personagens.push_back(c);
    return c;
}

void GerenciadorDeJogo::listar_personagens() {
    cout << "\n===== PERSONAGENS DO JOGO (" << personagens.size() << ") =====" << endl;
    if (personagens.empty()) {
        cout << "  Nenhum personagem criado ainda." << endl;
        return;
    }
    for (Jogador* p : personagens) {
        cout << "  - " << p->get_nome() << " | Nivel " << p->get_nivel()
             << " | HP " << p->get_hp_atual() << "/" << p->get_hp_max()
             << " | Raca: " << p->get_raca()
             << " | Ouro: " << p->get_ouro() << endl;
    }
}

Jogador* GerenciadorDeJogo::buscar_por_nome(string nome) {
    for (Jogador* p : personagens) {
        if (p->get_nome() == nome) return p;
    }
    throw PersonagemNaoEncontradoError("Personagem '" + nome + "' nao encontrado.");
}

void GerenciadorDeJogo::remover_personagem(string nome) {
    for (size_t i = 0; i < personagens.size(); i++) {
        if (personagens[i]->get_nome() == nome) {
            delete personagens[i];
            personagens.erase(personagens.begin() + i);
            cout << "Personagem '" << nome << "' removido do jogo." << endl;
            return;
        }
    }
    throw PersonagemNaoEncontradoError("Personagem '" + nome + "' nao encontrado.");
}

vector<Jogador*>& GerenciadorDeJogo::get_personagens() { return personagens; }

} // namespace rpg
