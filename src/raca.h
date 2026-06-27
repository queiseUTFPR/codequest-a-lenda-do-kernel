#ifndef RPG_RACA_H
#define RPG_RACA_H

#include <string>
#include <stdexcept>
using namespace std;

namespace rpg {

/**
 * @class Raca
 * @brief Classe abstrata base para as raças do RPG.
 *
 * A especificação do projeto sugere raças concretas (Humano, Elfo, Anão e Orc).
 * Elas foram modeladas como uma composição dentro de Jogador: cada personagem
 * possui uma Raca, que pode aplicar pequenos bônus iniciais e fornecer uma
 * descrição própria.
 */
class Raca {
public:
    virtual ~Raca();
    virtual string get_nome() const = 0;
    virtual string get_descricao() const = 0;
    virtual void aplicar_bonus_base(int& hp_max) const = 0;
    virtual Raca* clone() const = 0;
};

class Humano : public Raca {
public:
    string get_nome() const override;
    string get_descricao() const override;
    void aplicar_bonus_base(int& hp_max) const override;
    Raca* clone() const override;
};

class Elfo : public Raca {
public:
    string get_nome() const override;
    string get_descricao() const override;
    void aplicar_bonus_base(int& hp_max) const override;
    Raca* clone() const override;
};

class Anao : public Raca {
public:
    string get_nome() const override;
    string get_descricao() const override;
    void aplicar_bonus_base(int& hp_max) const override;
    Raca* clone() const override;
};

class Orc : public Raca {
public:
    string get_nome() const override;
    string get_descricao() const override;
    void aplicar_bonus_base(int& hp_max) const override;
    Raca* clone() const override;
};

/** @brief Factory para criar raças concretas a partir do texto digitado no menu/save. */
class RacaFactory {
public:
    static Raca* criar(string tipo);
    static string normalizar(string texto);
};

} // namespace rpg

#endif // RPG_RACA_H
