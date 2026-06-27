#ifndef RPG_HABILIDADE_H
#define RPG_HABILIDADE_H

#include <string>
#include <stdexcept>
using namespace std;

namespace rpg {

// Forward declaration: Habilidade so guarda PONTEIROS para Jogador nas assinaturas,
// quem precisa do Jogador completo (efeito() das classes concretas) inclui jogador.h no .cpp
class Jogador;


// =============================================================
// EXCEÇÕES DAS HABILIDADES
// =============================================================

class HabilidadeNaoEncontradaError : public runtime_error {
public:
    HabilidadeNaoEncontradaError(const string& msg);
};

class HabilidadePassivaError : public runtime_error {
public:
    HabilidadePassivaError(const string& msg);
};


// =============================================================
// HABILIDADES
// Polimorfismo e Abstração: skills ativas e passivas com efeitos variaveis
// =============================================================

/**
 * @class Habilidade
 * @brief Classe abstrata base de todas as skills do jogo (ativas e passivas).
 *
 * Habilidades ativas precisam ser usadas manualmente via Jogador::usar_habilidade().
 * Habilidades passivas (ativa=false) agem automaticamente a cada turno de batalha,
 * via Jogador::aplicar_habilidades_passivas(), e lançam HabilidadePassivaError se
 * alguém tentar usá-las manualmente.
 */
class Habilidade {
private:
    string nome;
    bool   ativa; // true = precisa ser usada manualmente; false = passiva (age sozinha a cada turno)

public:
    Habilidade(string nome, bool ativa);
    virtual ~Habilidade(); // Destrutor Virtual: evita memory leak ao deletar pelo ponteiro da base

    string get_nome();
    bool   eh_ativa();

    // Método Abstrato: cada habilidade concreta define seu proprio efeito sobre usuario/alvo
    virtual void efeito(Jogador* usuario, Jogador* alvo) = 0;
};


// Herança: HabilidadeOfensiva especializa Habilidade para skills que causam dano
class HabilidadeOfensiva : public Habilidade {
protected:
    int dano_base;
public:
    HabilidadeOfensiva(string nome, int dano_base, bool ativa = true);
};


// Herança: HabilidadeDefensiva especializa Habilidade para skills de proteção
class HabilidadeDefensiva : public Habilidade {
protected:
    int valor_defesa;
public:
    HabilidadeDefensiva(string nome, int valor_defesa, bool ativa = true);
};


// Herança: HabilidadeSuporte especializa Habilidade para skills de cura/auxilio
class HabilidadeSuporte : public Habilidade {
protected:
    int valor_cura;
public:
    HabilidadeSuporte(string nome, int valor_cura, bool ativa = true);
};


// =============================================================
// HABILIDADES CONCRETAS
// Os efeitos so podem ser implementados no .cpp, pois precisam do Jogador completo
// =============================================================

// Ofensiva ativa generica: qualquer personagem pode aprender e usar
class GolpePoderoso : public HabilidadeOfensiva {
public:
    GolpePoderoso();
    void efeito(Jogador* usuario, Jogador* alvo) override;
};

// Ofensiva ativa que consome um recurso especial via consumir_recurso() (ex: mana do Mago)
class BolaDeFogo : public HabilidadeOfensiva {
public:
    BolaDeFogo();
    void efeito(Jogador* usuario, Jogador* alvo) override;
};

// Defensiva ativa: recupera HP do usuario, simulando uma postura que absorve dano
class PosturaDefensiva : public HabilidadeDefensiva {
public:
    PosturaDefensiva();
    void efeito(Jogador* usuario, Jogador* alvo) override;
};

// Suporte ativa: cura o usuario ou um aliado indicado como alvo
class CuraMenor : public HabilidadeSuporte {
public:
    CuraMenor();
    void efeito(Jogador* usuario, Jogador* alvo) override;
};

// Suporte passiva: regenera HP automaticamente a cada turno, sem precisar ser ativada manualmente
class Regeneracao : public HabilidadeSuporte {
public:
    Regeneracao(); // ativa = false -> passiva
    void efeito(Jogador* usuario, Jogador* alvo) override;
};

} // namespace rpg

#endif // RPG_HABILIDADE_H
