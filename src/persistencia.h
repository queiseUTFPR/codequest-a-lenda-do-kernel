#ifndef RPG_PERSISTENCIA_H
#define RPG_PERSISTENCIA_H

#include <vector>
#include <string>
#include <stdexcept>
#include "personagens.h"
using namespace std;

namespace rpg {

class PersistenciaError : public runtime_error {
public:
    PersistenciaError(const string& msg);
};


// =============================================================
// PERSISTÊNCIA
// Salva e carrega o estado do jogo em arquivo de texto, reaproveitando
// o operador << (via serializar()) e os construtores diretos das subclasses
// =============================================================

/**
 * @class Persistencia
 * @brief RF7: salva e carrega o estado dos personagens em arquivo de texto.
 *
 * Reaproveita o operador<< (via Jogador::serializar()) para gravação, e reconstrói
 * cada personagem pelo construtor concreto correto a partir do prefixo de tipo salvo.
 */
class Persistencia {
public:
    // RF7: Salva o estado de todos os personagens em um arquivo de texto
    static void salvar(vector<Jogador*>& personagens, string caminho);

    // RF7: Carrega o estado dos personagens a partir de um arquivo de texto
    static vector<Jogador*> carregar(string caminho);

private:
    static vector<string> dividir(string linha, char separador);
    static Jogador* reconstruir(string linha);
};

} // namespace rpg

#endif // RPG_PERSISTENCIA_H
