#include "habilidade.h"
#include "jogador.h" // Necessario aqui (e nao no .h) pois efeito() chama metodos do Jogador completo
#include <iostream>
using namespace std;

namespace rpg {

// ---------- Exceções ----------

HabilidadeNaoEncontradaError::HabilidadeNaoEncontradaError(const string& msg) : runtime_error(msg) {}
HabilidadePassivaError::HabilidadePassivaError(const string& msg) : runtime_error(msg) {}


// ---------- Habilidade (abstrata) ----------

Habilidade::Habilidade(string nome, bool ativa) : nome(nome), ativa(ativa) {}
Habilidade::~Habilidade() {}

string Habilidade::get_nome() { return nome; }
bool   Habilidade::eh_ativa() { return ativa; }


// ---------- Camadas intermediarias (abstratas) ----------

HabilidadeOfensiva::HabilidadeOfensiva(string nome, int dano_base, bool ativa)
    : Habilidade(nome, ativa), dano_base(dano_base) {}

HabilidadeDefensiva::HabilidadeDefensiva(string nome, int valor_defesa, bool ativa)
    : Habilidade(nome, ativa), valor_defesa(valor_defesa) {}

HabilidadeSuporte::HabilidadeSuporte(string nome, int valor_cura, bool ativa)
    : Habilidade(nome, ativa), valor_cura(valor_cura) {}


// ---------- Habilidades concretas ----------

GolpePoderoso::GolpePoderoso() : HabilidadeOfensiva("Golpe Poderoso", 30) {}

void GolpePoderoso::efeito(Jogador* usuario, Jogador* alvo) {
    if (!alvo) return;
    cout << usuario->get_nome() << " usa GOLPE PODEROSO em " << alvo->get_nome() << "!" << endl;
    alvo->receber_dano(dano_base); // A armadura do alvo ja reduz o dano automaticamente
}


BolaDeFogo::BolaDeFogo() : HabilidadeOfensiva("Bola de Fogo", 45) {}

void BolaDeFogo::efeito(Jogador* usuario, Jogador* alvo) {
    if (!alvo) return;
    // Polimorfismo: cada subclasse de Jogador decide como (ou se) cobra esse custo
    if (!usuario->consumir_recurso(20)) {
        cout << usuario->get_nome() << " nao tem recurso suficiente para Bola de Fogo!" << endl;
        return;
    }
    cout << usuario->get_nome() << " conjura BOLA DE FOGO em " << alvo->get_nome() << "!" << endl;
    alvo->receber_dano(dano_base);
}


PosturaDefensiva::PosturaDefensiva() : HabilidadeDefensiva("Postura Defensiva", 20) {}

void PosturaDefensiva::efeito(Jogador* usuario, Jogador* alvo) {
    cout << usuario->get_nome() << " assume POSTURA DEFENSIVA e recupera " << valor_defesa << " de HP!" << endl;
    usuario->curar(valor_defesa);
}


CuraMenor::CuraMenor() : HabilidadeSuporte("Cura Menor", 25) {}

void CuraMenor::efeito(Jogador* usuario, Jogador* alvo) {
    Jogador* beneficiado = alvo ? alvo : usuario; // Sem alvo definido, cura a si mesmo
    cout << usuario->get_nome() << " usa CURA MENOR em " << beneficiado->get_nome() << "!" << endl;
    beneficiado->curar(valor_cura);
}


Regeneracao::Regeneracao() : HabilidadeSuporte("Regeneracao", 5, false) {} // ativa = false -> passiva

void Regeneracao::efeito(Jogador* usuario, Jogador* alvo) {
    cout << "[PASSIVA] " << usuario->get_nome() << " regenera " << valor_cura << " de HP." << endl;
    usuario->curar(valor_cura);
}

} // namespace rpg
