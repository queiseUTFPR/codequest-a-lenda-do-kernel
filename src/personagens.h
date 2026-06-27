#ifndef RPG_PERSONAGENS_H
#define RPG_PERSONAGENS_H

#include "jogador.h"
using namespace std;

namespace rpg {

/** @brief Herança: especializa Jogador com foco em força (ataque físico direto). */
class Guerreiro : public Jogador {
private:
    int forca;

public:
    Guerreiro(string nome_jogador, int forca_inicial, string raca = "Humano");
    ~Guerreiro();

    int get_forca();

    // Polimorfismo: dano fixo calculado como Forca × 1.5, somado ao bonus da arma equipada
    int atacar() override;
    void exibirStatus() override;

    // RF6: pontos de redistribuição vão para a força
    void redistribuir_pontos(int pontos) override;

    // Polimorfismo: acrescenta a forca ao formato base de serializacao (usado pela Persistencia)
    string serializar() override;
};


/** @brief Herança: especializa Jogador com inteligência e mana (ataque mágico com custo de recurso). */
class Mago : public Jogador {
private:
    int inteligencia;
    int mana;
    int mana_max;

public:
    Mago(string nome_jogador, int intel_inicial, int mana_inicial, string raca = "Humano");
    ~Mago();

    int get_inteligencia();
    int get_mana();
    int get_mana_max();
    void restaurar_mana(int mana_atual, int mana_maximo);

    // Polimorfismo: sobrescreve o custo generico de habilidades para consumir mana de fato
    bool consumir_recurso(int quantidade) override;
    void restaurar_recurso(int quantidade) override;

    // Polimorfismo: consome 10 de mana para causar Inteligencia × 2.0; dano mínimo se sem mana
    int atacar() override;
    void exibirStatus() override;
    string serializar() override;

    // RF6: pontos de redistribuição vão para a inteligencia
    void redistribuir_pontos(int pontos) override;
};


/** @brief Herança: especializa Jogador com ferocidade e chance de crítico (ataque probabilístico). */
class Monstro : public Jogador {
private:
    int   ferocidade;
    float chance_critico; // Valor entre 0.0 e 1.0 (ex: 0.4 = 40%)

public:
    Monstro(string nome_jogador, int ferocidade_inicial, float chance, string raca = "Humano");
    ~Monstro();

    int   get_ferocidade();
    float get_chance_critico();

    // Polimorfismo: dano crítico baseado em probabilidade, multiplicado por 3.0 se bem-sucedido
    int atacar() override;
    void exibirStatus() override;
    string serializar() override;

    // RF6: pontos de redistribuição vão para a ferocidade
    void redistribuir_pontos(int pontos) override;
};


/** @brief Herança: especializa Jogador com destreza e flechas, cobrindo a classe Arqueiro da especificação. */
class Arqueiro : public Jogador {
private:
    int destreza;
    int flechas;

public:
    Arqueiro(string nome_jogador, int destreza_inicial, int flechas_iniciais, string raca = "Humano");
    ~Arqueiro();

    int get_destreza();
    int get_flechas();

    int atacar() override;
    void exibirStatus() override;
    string serializar() override;
    void redistribuir_pontos(int pontos) override;
};


/** @brief Herança: especializa Jogador com sabedoria e fé, cobrindo a classe Clérigo da especificação. */
class Clerigo : public Jogador {
private:
    int sabedoria;
    int fe;
    int fe_max;

public:
    Clerigo(string nome_jogador, int sabedoria_inicial, int fe_inicial, string raca = "Humano");
    ~Clerigo();

    int get_sabedoria();
    int get_fe();
    int get_fe_max();
    void restaurar_fe(int fe_atual, int fe_maximo);

    bool consumir_recurso(int quantidade) override;
    void restaurar_recurso(int quantidade) override;

    int atacar() override;
    void exibirStatus() override;
    string serializar() override;
    void redistribuir_pontos(int pontos) override;
};

} // namespace rpg

#endif // RPG_PERSONAGENS_H
