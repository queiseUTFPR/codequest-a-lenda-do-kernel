#ifndef RPG_JOGADOR_H
#define RPG_JOGADOR_H

#include <string>
#include <vector>
#include <iostream>
#include "inventario.h"  // Composição: Jogador tem um Inventario POR VALOR -> precisa do tipo completo
#include "habilidade.h"  // Composição: Jogador guarda vector<Habilidade*> e chama h->get_nome()/eh_ativa()
#include "observer.h"    // Observer: Jogador guarda vector<ObservadorDeEventos*> e notifica eventos
#include "raca.h"        // Composicao: Jogador possui uma Raca concreta
using namespace std;

namespace rpg {

/**
 * @class Jogador
 * @brief Classe abstrata base de todos os personagens jogáveis.
 *
 * Concentra os atributos comuns (nome, raça, nível, HP, XP) e a composição com
 * Inventario, lista de Habilidade aprendidas e lista de ObservadorDeEventos.
 * Cada subclasse concreta (Guerreiro, Mago, Monstro) implementa atacar() e pode
 * sobrescrever exibirStatus(), serializar(), consumir_recurso() e redistribuir_pontos().
 */
class Jogador {
private:
    string nome;
    Raca*  raca;    // Composicao: raça concreta do personagem
    int    nivel;
    int    hp_max;
    int    hp_atual;
    int    xp;      // Atributo de experiência
    int    ouro;    // Moeda do jogo, usada para comprar itens na loja

    Inventario                   inventario;   // Composição: o Inventario pertence ao Jogador e morre com ele
    vector<Habilidade*>          habilidades;  // Composição: skills aprendidas pelo personagem
    vector<ObservadorDeEventos*> observadores; // Observer: quem quer ser avisado dos eventos (nao pertence ao Jogador)

    void subir_nivel(); // Privado: incrementa nivel e restaura HP

public:
    Jogador(string nome_jogador, string raca_jogador = "Humano");

    // Destrutor Virtual: evita memory leak com polimorfismo; libera as habilidades aprendidas
    virtual ~Jogador();

    string get_nome();
    string get_raca();
    string get_descricao_raca();
    int    get_nivel();
    int    get_hp_max();
    int    get_hp_atual();
    int    get_xp();
    int    get_ouro();

    // Usado apenas pela Persistencia, para restaurar o estado exato salvo em arquivo
    void restaurar_estado(int nivel_salvo, int hp_max_salvo, int hp_atual_salvo, int xp_salvo);
    void restaurar_ouro(int ouro_salvo);

    // Sobrecarga de operador: serializa o personagem num formato compacto (usado por cout e por arquivo)
    virtual string serializar();
    friend ostream& operator<<(ostream& os, Jogador& j);

    // Sobrecarga de operadores: comparação por nome e por HP atual
    bool operator==(Jogador& outro);
    bool operator<(Jogador& outro);

    // Composição: acesso ao inventario do personagem para equipar/usar itens de fora da classe
    Inventario& get_inventario();
    int  get_bonus_ataque(); // RF4: vem automaticamente da arma equipada
    int  get_bonus_defesa(); // RF4: vem automaticamente da armadura equipada
    void usar_pocao(string nome_pocao);

    // Virtual: subclasses com recurso especial (como a mana do Mago) podem sobrescrever para validar custo
    virtual bool consumir_recurso(int quantidade);
    virtual void restaurar_recurso(int quantidade);

    void aprender_habilidade(Habilidade* h);
    string serializar_habilidades();
    void restaurar_habilidades(string dados);
    void listar_habilidades();
    void usar_habilidade(string nome_habilidade, Jogador* alvo = nullptr);
    void aplicar_habilidades_passivas();

    // Observer: registra/remove/notifica observadores deste personagem
    void adicionar_observador(ObservadorDeEventos* obs);
    void remover_observador(ObservadorDeEventos* obs);
    void notificar_observadores(string evento);

    void set_hp_atual(int valor); // SETTER: valida o valor antes de alterar o HP
    void ganhar_xp(int pontos);
    virtual void exibirStatus();
    void receber_dano(int quantidade);
    void curar(int quantidade);
    void ganhar_ouro(int quantidade);
    bool gastar_ouro(int quantidade);

    /**
     * @brief RF6: aplica pontos de redistribuição ao atributo principal da subclasse, ao subir de nível.
     * Implementação padrão (Jogador) não faz nada; cada subclasse sobrescreve para
     * direcionar os pontos ao seu atributo específico (força, inteligência, ferocidade).
     */
    virtual void redistribuir_pontos(int pontos);

    // Método Abstrato: obriga cada subclasse a definir seu próprio comportamento de ataque
    virtual int atacar() = 0;
};

} // namespace rpg

#endif // RPG_JOGADOR_H
