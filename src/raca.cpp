#include "raca.h"
#include <algorithm>
#include <cctype>
using namespace std;

namespace rpg {

Raca::~Raca() {}

string Humano::get_nome() const { return "Humano"; }
string Humano::get_descricao() const { return "equilibrado, sem penalidades ou bonus extremos"; }
void Humano::aplicar_bonus_base(int& hp_max) const { (void)hp_max; }
Raca* Humano::clone() const { return new Humano(*this); }

string Elfo::get_nome() const { return "Elfo"; }
string Elfo::get_descricao() const { return "agil e espiritual, ideal para magos/arqueiros"; }
void Elfo::aplicar_bonus_base(int& hp_max) const { (void)hp_max; }
Raca* Elfo::clone() const { return new Elfo(*this); }

string Anao::get_nome() const { return "Anao"; }
string Anao::get_descricao() const { return "resistente, recebe +25 de HP maximo inicial"; }
void Anao::aplicar_bonus_base(int& hp_max) const { hp_max += 25; }
Raca* Anao::clone() const { return new Anao(*this); }

string Orc::get_nome() const { return "Orc"; }
string Orc::get_descricao() const { return "brutal e resistente, recebe +15 de HP maximo inicial"; }
void Orc::aplicar_bonus_base(int& hp_max) const { hp_max += 15; }
Raca* Orc::clone() const { return new Orc(*this); }

string RacaFactory::normalizar(string texto) {
    string r;
    for (char c : texto) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (!isspace(uc)) r += static_cast<char>(tolower(uc));
    }
    return r;
}

Raca* RacaFactory::criar(string tipo) {
    string t = normalizar(tipo);
    if (t.empty() || t == "humano") return new Humano();
    if (t == "elfo") return new Elfo();
    if (t == "anao" || t == "anão") return new Anao();
    if (t == "orc") return new Orc();
    throw invalid_argument("Raca '" + tipo + "' nao existe. Use Humano, Elfo, Anao ou Orc.");
}

} // namespace rpg
