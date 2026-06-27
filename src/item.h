#ifndef RPG_ITEM_H
#define RPG_ITEM_H

#include <string>
using namespace std;

namespace rpg {

// =============================================================
// ITENS
// Factory: Item serve como Product base para ItemFactory
// =============================================================

/**
 * @class Item
 * @brief Classe abstrata base de todos os itens do jogo.
 *
 * Define o contrato comum (nome, peso, uso) que Arma, Armadura, Pocao e ItemEspecial especializam.
 * Não pode ser instanciada diretamente — serve apenas como Product base para a ItemFactory.
 */
class Item {
private:
    string nome;
    float  peso; // Peso em kg, usado pelo Inventario para controle de capacidade

public:
    Item(string nome, float peso);

    // Destrutor Virtual: evita memory leak ao deletar pelo ponteiro da base
    virtual ~Item();

    string get_nome();
    float  get_peso();

    // Método Abstrato: cada item define como é usado
    virtual void usar() = 0;
};


/** @brief Herança: especializa Item com um atributo de dano, usado em ataques e habilidades. */
class Arma : public Item {
private:
    int dano;

public:
    Arma(string nome, float peso, int dano);

    int get_dano();

    // Sobrecarga de operador: combina duas armas numa nova, somando dano e peso (mini-crafting)
    Arma operator+(Arma& outra);

    // Polimorfismo: Arma exibe dano ao ser usada
    void usar() override;
};


/** @brief Herança: especializa Item com um atributo de defesa, reduz dano recebido. */
class Armadura : public Item {
private:
    int defesa;

public:
    Armadura(string nome, float peso, int defesa);

    int get_defesa();

    // Polimorfismo: Armadura exibe defesa ao ser usada
    void usar() override;
};


/** @brief Herança: especializa Item com efeito e valor; consumível (destruída ao ser usada). */
class Pocao : public Item {
private:
    string efeito;
    int    valor;

public:
    Pocao(string nome, float peso, string efeito, int valor);

    // Getters necessários para o Inventario aplicar o efeito automaticamente no dono
    string get_efeito();
    int    get_valor();

    // Polimorfismo: Pocao exibe efeito ao ser consumida
    void usar() override;
};


// Produtos concretos de Arma
class Espada : public Arma { public: Espada(); };
class Cajado : public Arma { public: Cajado(); };
class Adaga  : public Arma { public: Adaga();  };

// Produtos concretos de Armadura
class PeitorialFerro : public Armadura { public: PeitorialFerro(); };
class MalhaEncantada : public Armadura { public: MalhaEncantada(); };
class CouroFurtivo   : public Armadura { public: CouroFurtivo();   };

// Produtos concretos de Pocao
class PocaoVida  : public Pocao { public: PocaoVida();  };
class PocaoMana  : public Pocao { public: PocaoMana();  };
class PocaoForca : public Pocao { public: PocaoForca(); };


/** @brief Herança: especializa Item com um efeito especial de uso único. */
class ItemEspecial : public Item {
private:
    string efeito;
    int    valor;

public:
    ItemEspecial(string nome, float peso, string efeito, int valor);

    string get_efeito();
    int    get_valor();

    // Polimorfismo: item especial exibe seu efeito ao ser ativado
    void usar() override;
};

// Produtos concretos de ItemEspecial
class AnelFortuna   : public ItemEspecial { public: AnelFortuna();   };
class ReliquiaAntiga : public ItemEspecial { public: ReliquiaAntiga(); };


} // namespace rpg

#endif // RPG_ITEM_H
