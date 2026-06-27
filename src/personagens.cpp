#include "personagens.h"
#include <cstdlib> // Aleatoriedade: rand() para a chance de critico do Monstro
using namespace std;

namespace rpg {

// ---------- Guerreiro ----------

Guerreiro::Guerreiro(string nome_jogador, int forca_inicial, string raca)
    : Jogador(nome_jogador, raca) {
    forca = forca_inicial;
}

Guerreiro::~Guerreiro() {
    cout << "Guerreiro " << get_nome() << " removido." << endl;
}

int Guerreiro::get_forca() { return forca; }

int Guerreiro::atacar() {
    int dano = static_cast<int>(forca * 1.5) + get_bonus_ataque();
    cout << get_nome() << " ataca com sua espada! Dano: " << dano << endl;
    return dano;
}

void Guerreiro::exibirStatus() {
    Jogador::exibirStatus(); // Exibe os atributos herdados da classe base
    cout << "Classe: Guerreiro | Forca: " << forca << endl;
    cout << "-----------------------------------\n" << endl;
}

string Guerreiro::serializar() {
    return "Guerreiro;" + Jogador::serializar() + ";" + to_string(forca);
}

void Guerreiro::redistribuir_pontos(int pontos) {
    forca += pontos;
    cout << get_nome() << " distribuiu " << pontos << " pontos em Forca! (Forca agora: " << forca << ")" << endl;
}


// ---------- Mago ----------

Mago::Mago(string nome_jogador, int intel_inicial, int mana_inicial, string raca)
    : Jogador(nome_jogador, raca) {
    inteligencia = intel_inicial;
    mana         = mana_inicial;
    mana_max     = mana_inicial;
}

Mago::~Mago() {
    cout << "Mago " << get_nome() << " removido." << endl;
}

int Mago::get_inteligencia() { return inteligencia; }
int Mago::get_mana()         { return mana; }
int Mago::get_mana_max()     { return mana_max; }

void Mago::restaurar_mana(int mana_atual, int mana_maximo) {
    mana_max = mana_maximo;
    mana = mana_atual;
    if (mana < 0) mana = 0;
    if (mana > mana_max) mana = mana_max;
}

bool Mago::consumir_recurso(int quantidade) {
    if (mana >= quantidade) {
        mana -= quantidade;
        cout << "(" << get_nome() << " consumiu " << quantidade << " de mana: " << mana << "/" << mana_max << ")" << endl;
        return true;
    }
    return false;
}

void Mago::restaurar_recurso(int quantidade) {
    if (quantidade <= 0) return;
    mana += quantidade;
    if (mana > mana_max) mana = mana_max;
    cout << get_nome() << " restaurou " << quantidade << " de mana. (Mana: " << mana << "/" << mana_max << ")" << endl;
}

int Mago::atacar() {
    const int CUSTO_MANA = 10;
    int dano;
    if (mana >= CUSTO_MANA) {
        mana -= CUSTO_MANA; // Consome mana ao lançar o feitiço
        dano = static_cast<int>(inteligencia * 2.0) + get_bonus_ataque();
        cout << get_nome() << " lanca um feitico! (Mana: " << mana << "/" << mana_max << ") Dano: " << dano << endl;
    } else {
        dano = 5 + get_bonus_ataque(); // Dano base mínimo quando não há mana suficiente
        cout << get_nome() << " esta sem mana! Ataque fraco. Dano: " << dano << endl;
    }
    return dano;
}

void Mago::exibirStatus() {
    Jogador::exibirStatus();
    cout << "Classe: Mago | Inteligencia: " << inteligencia << " | Mana: " << mana << "/" << mana_max << endl;
    cout << "-----------------------------------\n" << endl;
}

string Mago::serializar() {
    return "Mago;" + Jogador::serializar() + ";" + to_string(inteligencia) + ";" + to_string(mana) + ";" + to_string(mana_max);
}

void Mago::redistribuir_pontos(int pontos) {
    inteligencia += pontos;
    cout << get_nome() << " distribuiu " << pontos << " pontos em Inteligencia! (Inteligencia agora: " << inteligencia << ")" << endl;
}


// ---------- Monstro ----------

Monstro::Monstro(string nome_jogador, int ferocidade_inicial, float chance, string raca)
    : Jogador(nome_jogador, raca) {
    ferocidade      = ferocidade_inicial;
    chance_critico = chance;
}

Monstro::~Monstro() {
    cout << "Monstro " << get_nome() << " removido." << endl;
}

int   Monstro::get_ferocidade()      { return ferocidade; }
float Monstro::get_chance_critico() { return chance_critico; }

int Monstro::atacar() {
    int   dano_base = ferocidade;
    float sorte     = static_cast<float>(rand()) / RAND_MAX; // Número aleatório entre 0.0 e 1.0
    int   dano;
    if (sorte < chance_critico) { // Verifica se o ataque é crítico
        dano = static_cast<int>(dano_base * 3.0);
        cout << get_nome() << " aplica um GOLPE CRITICO! Dano: " << dano + get_bonus_ataque() << endl;
    } else {
        dano = dano_base; // Dano normal se não for crítico
        cout << get_nome() << " ataca com ferocidade. Dano: " << dano + get_bonus_ataque() << endl;
    }
    return dano + get_bonus_ataque();
}

void Monstro::exibirStatus() {
    Jogador::exibirStatus();
    cout << "Classe: Monstro | Ferocidade: " << ferocidade << " | Chance Critico: " << static_cast<int>(chance_critico * 100) << "%" << endl;
    cout << "-----------------------------------\n" << endl;
}

string Monstro::serializar() {
    return "Monstro;" + Jogador::serializar() + ";" + to_string(ferocidade) + ";" + to_string(chance_critico);
}

void Monstro::redistribuir_pontos(int pontos) {
    ferocidade += pontos;
    cout << get_nome() << " distribuiu " << pontos << " pontos em Ferocidade! (Ferocidade agora: " << ferocidade << ")" << endl;
}


// ---------- Arqueiro ----------

Arqueiro::Arqueiro(string nome_jogador, int destreza_inicial, int flechas_iniciais, string raca)
    : Jogador(nome_jogador, raca) {
    destreza = destreza_inicial;
    flechas  = flechas_iniciais;
}

Arqueiro::~Arqueiro() {
    cout << "Arqueiro " << get_nome() << " removido." << endl;
}

int Arqueiro::get_destreza() { return destreza; }
int Arqueiro::get_flechas()  { return flechas; }

int Arqueiro::atacar() {
    int dano;
    if (flechas > 0) {
        flechas--;
        dano = static_cast<int>(destreza * 1.4) + get_bonus_ataque();
        cout << get_nome() << " dispara uma flecha! (Flechas: " << flechas << ") Dano: " << dano << endl;
    } else {
        dano = static_cast<int>(destreza * 0.7) + get_bonus_ataque();
        cout << get_nome() << " esta sem flechas e usa ataque corpo a corpo. Dano: " << dano << endl;
    }
    return dano;
}

void Arqueiro::exibirStatus() {
    Jogador::exibirStatus();
    cout << "Classe: Arqueiro | Destreza: " << destreza << " | Flechas: " << flechas << endl;
    cout << "-----------------------------------\n" << endl;
}

string Arqueiro::serializar() {
    return "Arqueiro;" + Jogador::serializar() + ";" + to_string(destreza) + ";" + to_string(flechas);
}

void Arqueiro::redistribuir_pontos(int pontos) {
    destreza += pontos;
    cout << get_nome() << " distribuiu " << pontos << " pontos em Destreza! (Destreza agora: " << destreza << ")" << endl;
}


// ---------- Clerigo ----------

Clerigo::Clerigo(string nome_jogador, int sabedoria_inicial, int fe_inicial, string raca)
    : Jogador(nome_jogador, raca) {
    sabedoria = sabedoria_inicial;
    fe        = fe_inicial;
    fe_max    = fe_inicial;
}

Clerigo::~Clerigo() {
    cout << "Clerigo " << get_nome() << " removido." << endl;
}

int Clerigo::get_sabedoria() { return sabedoria; }
int Clerigo::get_fe()        { return fe; }
int Clerigo::get_fe_max()    { return fe_max; }

void Clerigo::restaurar_fe(int fe_atual, int fe_maximo) {
    fe_max = fe_maximo;
    fe = fe_atual;
    if (fe < 0) fe = 0;
    if (fe > fe_max) fe = fe_max;
}

bool Clerigo::consumir_recurso(int quantidade) {
    if (fe >= quantidade) {
        fe -= quantidade;
        cout << "(" << get_nome() << " consumiu " << quantidade << " de fe: " << fe << "/" << fe_max << ")" << endl;
        return true;
    }
    return false;
}

void Clerigo::restaurar_recurso(int quantidade) {
    if (quantidade <= 0) return;
    fe += quantidade;
    if (fe > fe_max) fe = fe_max;
    cout << get_nome() << " restaurou " << quantidade << " de fe. (Fe: " << fe << "/" << fe_max << ")" << endl;
}

int Clerigo::atacar() {
    int dano = static_cast<int>(sabedoria * 1.2) + (fe / 5) + get_bonus_ataque();
    if (fe >= 5) fe -= 5;
    cout << get_nome() << " invoca luz sagrada! (Fe: " << fe << "/" << fe_max << ") Dano: " << dano << endl;
    return dano;
}

void Clerigo::exibirStatus() {
    Jogador::exibirStatus();
    cout << "Classe: Clerigo | Sabedoria: " << sabedoria << " | Fe: " << fe << "/" << fe_max << endl;
    cout << "-----------------------------------\n" << endl;
}

string Clerigo::serializar() {
    return "Clerigo;" + Jogador::serializar() + ";" + to_string(sabedoria) + ";" + to_string(fe) + ";" + to_string(fe_max);
}

void Clerigo::redistribuir_pontos(int pontos) {
    sabedoria += pontos;
    cout << get_nome() << " distribuiu " << pontos << " pontos em Sabedoria! (Sabedoria agora: " << sabedoria << ")" << endl;
}

} // namespace rpg
