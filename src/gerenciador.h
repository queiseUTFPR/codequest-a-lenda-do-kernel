#ifndef RPG_GERENCIADOR_H
#define RPG_GERENCIADOR_H

#include <string>
#include <vector>
#include <stdexcept>
#include "personagens.h"
using namespace std;

namespace rpg {

class PersonagemNaoEncontradoError : public runtime_error {
public:
    PersonagemNaoEncontradoError(const string& msg);
};


/**
 * @class GerenciadorDeJogo
 * @brief Padrão Singleton: ponto único de acesso à coleção de personagens do jogo.
 *
 * Centraliza a criação (RF1) e a listagem/busca (RF2) de todos os personagens,
 * garantindo que exista exatamente uma instância em toda a execução do programa.
 * É o "dono" dos personagens criados por ele: ao destruir a instância, libera a
 * memória de todos os personagens que ainda não foram removidos individualmente.
 */
class GerenciadorDeJogo {
private:
    static GerenciadorDeJogo* instancia; ///< Única instância da classe (Singleton)
    vector<Jogador*> personagens;

    GerenciadorDeJogo();                                   ///< Construtor privado: impede `new GerenciadorDeJogo()` externo
    GerenciadorDeJogo(const GerenciadorDeJogo&) = delete;   ///< Impede cópia da instância
    GerenciadorDeJogo& operator=(const GerenciadorDeJogo&) = delete;

public:
    ~GerenciadorDeJogo();

    /** @brief Acesso único e global à instância do Singleton, criando-a na primeira chamada. */
    static GerenciadorDeJogo& instance();

    /** @brief Destroi a instância atual, liberando todos os personagens. Permite recriar do zero. */
    static void destruir_instancia();

    // RF1: criação de personagens com atributos definidos por quem chama (distribuição de pontos)
    Guerreiro* criar_guerreiro(string nome, string raca, int forca);
    Mago*      criar_mago(string nome, string raca, int inteligencia, int mana);
    Monstro*    criar_monstro(string nome, string raca, int ferocidade, float chance_critico);
    Arqueiro*  criar_arqueiro(string nome, string raca, int destreza, int flechas);
    Clerigo*   criar_clerigo(string nome, string raca, int sabedoria, int fe);

    // RF2: listagem e busca de personagens já criados
    void      listar_personagens();
    Jogador*  buscar_por_nome(string nome); ///< Lança PersonagemNaoEncontradoError se não existir
    void      remover_personagem(string nome);

    vector<Jogador*>& get_personagens();
};

} // namespace rpg

#endif // RPG_GERENCIADOR_H
