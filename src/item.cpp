#include "item.h"
#include <iostream>
using namespace std;

namespace rpg {

// ---------- Item ----------

Item::Item(string nome, float peso) : nome(nome), peso(peso) {}

Item::~Item() {}

string Item::get_nome() { return nome; }
float  Item::get_peso() { return peso; }


// ---------- Arma ----------

Arma::Arma(string nome, float peso, int dano) : Item(nome, peso), dano(dano) {}

int Arma::get_dano() { return dano; }

Arma Arma::operator+(Arma& outra) {
    return Arma(get_nome() + " + " + outra.get_nome(), get_peso() + outra.get_peso(), dano + outra.get_dano());
}

void Arma::usar() {
    cout << "[ARMA] " << get_nome() << " equipada! Dano base: " << dano << endl;
}


// ---------- Armadura ----------

Armadura::Armadura(string nome, float peso, int defesa) : Item(nome, peso), defesa(defesa) {}

int Armadura::get_defesa() { return defesa; }

void Armadura::usar() {
    cout << "[ARMADURA] " << get_nome() << " equipada! Defesa base: " << defesa << endl;
}


// ---------- Pocao ----------

Pocao::Pocao(string nome, float peso, string efeito, int valor)
    : Item(nome, peso), efeito(efeito), valor(valor) {}

string Pocao::get_efeito() { return efeito; }
int    Pocao::get_valor()  { return valor; }

void Pocao::usar() {
    cout << "[POCAO] " << get_nome() << " consumida! " << efeito << ": +" << valor << endl;
}


// ---------- ItemEspecial ----------

ItemEspecial::ItemEspecial(string nome, float peso, string efeito, int valor)
    : Item(nome, peso), efeito(efeito), valor(valor) {}

string ItemEspecial::get_efeito() { return efeito; }
int    ItemEspecial::get_valor()  { return valor; }

void ItemEspecial::usar() {
    cout << "[ITEM ESPECIAL] " << get_nome() << " ativado! " << efeito << ": +" << valor << endl;
}


// ---------- Produtos concretos ----------

Espada::Espada() : Arma("Espada de Aco", 5.0f, 40) {}
Cajado::Cajado() : Arma("Cajado Arcano", 3.0f, 30) {}
Adaga::Adaga()   : Arma("Adaga Sombria", 2.0f, 25) {}

PeitorialFerro::PeitorialFerro() : Armadura("Peitoral de Ferro", 15.0f, 30) {}
MalhaEncantada::MalhaEncantada() : Armadura("Malha Encantada",    8.0f, 20) {}
CouroFurtivo::CouroFurtivo()     : Armadura("Couro Furtivo",      6.0f, 25) {}

PocaoVida::PocaoVida()   : Pocao("Pocao de Vida",  0.5f, "HP restaurado",   50) {}
PocaoMana::PocaoMana()   : Pocao("Pocao de Mana",  0.5f, "Mana restaurada", 30) {}
PocaoForca::PocaoForca() : Pocao("Pocao de Forca", 0.5f, "Forca aumentada", 10) {}

AnelFortuna::AnelFortuna()     : ItemEspecial("Anel da Fortuna", 1.0f, "Ouro recebido", 40) {}
ReliquiaAntiga::ReliquiaAntiga() : ItemEspecial("Reliquia Antiga", 2.0f, "Ouro recebido", 80) {}

} // namespace rpg
