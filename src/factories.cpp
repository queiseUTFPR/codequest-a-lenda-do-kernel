#include "factories.h"
#include <stdexcept>
using namespace std;

namespace rpg {

// ---------- ItemFactory ----------

Arma* ItemFactory::criar_arma(string tipo) {
    if (tipo == "espada") return new Espada();
    if (tipo == "cajado") return new Cajado();
    if (tipo == "adaga")  return new Adaga();
    throw invalid_argument("Arma '" + tipo + "' nao existe no catalogo.");
}

Armadura* ItemFactory::criar_armadura(string tipo) {
    if (tipo == "ferro") return new PeitorialFerro();
    if (tipo == "tela")  return new MalhaEncantada();
    if (tipo == "couro") return new CouroFurtivo();
    throw invalid_argument("Armadura '" + tipo + "' nao existe no catalogo.");
}

Pocao* ItemFactory::criar_pocao(string tipo) {
    if (tipo == "vida")  return new PocaoVida();
    if (tipo == "mana")  return new PocaoMana();
    if (tipo == "forca") return new PocaoForca();
    throw invalid_argument("Pocao '" + tipo + "' nao existe no catalogo.");
}

ItemEspecial* ItemFactory::criar_item_especial(string tipo) {
    if (tipo == "anel")    return new AnelFortuna();
    if (tipo == "reliquia") return new ReliquiaAntiga();
    throw invalid_argument("Item especial '" + tipo + "' nao existe no catalogo.");
}

Item* ItemFactory::criar_item_por_codigo(string codigo) {
    if (codigo == "-" || codigo.empty()) return nullptr;
    if (codigo.rfind("arma:", 0) == 0)     return criar_arma(codigo.substr(5));
    if (codigo.rfind("armadura:", 0) == 0) return criar_armadura(codigo.substr(9));
    if (codigo.rfind("pocao:", 0) == 0)    return criar_pocao(codigo.substr(6));
    if (codigo.rfind("especial:", 0) == 0) return criar_item_especial(codigo.substr(9));
    throw invalid_argument("Codigo de item invalido no save: " + codigo);
}

string ItemFactory::codigo_item(Item* item) {
    if (!item) return "-";
    string nome = item->get_nome();

    if (dynamic_cast<Arma*>(item)) {
        if (nome == "Espada de Aco")   return "arma:espada";
        if (nome == "Cajado Arcano")   return "arma:cajado";
        if (nome == "Adaga Sombria")   return "arma:adaga";
    }
    if (dynamic_cast<Armadura*>(item)) {
        if (nome == "Peitoral de Ferro") return "armadura:ferro";
        if (nome == "Malha Encantada")   return "armadura:tela";
        if (nome == "Couro Furtivo")     return "armadura:couro";
    }
    if (dynamic_cast<Pocao*>(item)) {
        if (nome == "Pocao de Vida")  return "pocao:vida";
        if (nome == "Pocao de Mana")  return "pocao:mana";
        if (nome == "Pocao de Forca") return "pocao:forca";
    }
    if (dynamic_cast<ItemEspecial*>(item)) {
        if (nome == "Anel da Fortuna") return "especial:anel";
        if (nome == "Reliquia Antiga") return "especial:reliquia";
    }

    throw invalid_argument("Nao foi possivel serializar o item: " + nome);
}

int ItemFactory::preco_item_por_codigo(string codigo) {
    if (codigo == "arma:espada")      return 70;
    if (codigo == "arma:cajado")      return 60;
    if (codigo == "arma:adaga")       return 45;
    if (codigo == "armadura:ferro")   return 80;
    if (codigo == "armadura:tela")    return 65;
    if (codigo == "armadura:couro")   return 55;
    if (codigo == "pocao:vida")       return 25;
    if (codigo == "pocao:mana")       return 20;
    if (codigo == "pocao:forca")      return 35;
    if (codigo == "especial:anel")    return 50;
    if (codigo == "especial:reliquia") return 100;
    throw invalid_argument("Nao ha preco cadastrado para o item: " + codigo);
}


// ---------- JogadorFactory ----------

Guerreiro* JogadorFactory::criar_guerreiro(string nome, string raca) {
    return new Guerreiro(nome, 30, raca); // Cria Guerreiro com atributos padrão da classe
}

Mago* JogadorFactory::criar_mago(string nome, string raca) {
    return new Mago(nome, 25, 30, raca); // Cria Mago com atributos padrão da classe
}

Monstro* JogadorFactory::criar_monstro(string nome, string raca) {
    return new Monstro(nome, 20, 0.4f, raca); // Cria Monstro com atributos padrão da classe
}

Arqueiro* JogadorFactory::criar_arqueiro(string nome, string raca) {
    return new Arqueiro(nome, 22, 8, raca); // Destreza e flechas iniciais
}

Clerigo* JogadorFactory::criar_clerigo(string nome, string raca) {
    return new Clerigo(nome, 20, 35, raca); // Sabedoria e fé iniciais
}


// ---------- HabilidadeFactory ----------

Habilidade* HabilidadeFactory::criar_ofensiva(string tipo) {
    if (tipo == "golpe") return new GolpePoderoso();
    if (tipo == "fogo")  return new BolaDeFogo();
    throw invalid_argument("Habilidade ofensiva '" + tipo + "' nao existe.");
}

Habilidade* HabilidadeFactory::criar_defensiva(string tipo) {
    if (tipo == "postura") return new PosturaDefensiva();
    throw invalid_argument("Habilidade defensiva '" + tipo + "' nao existe.");
}

Habilidade* HabilidadeFactory::criar_suporte(string tipo) {
    if (tipo == "cura")        return new CuraMenor();
    if (tipo == "regeneracao") return new Regeneracao();
    throw invalid_argument("Habilidade de suporte '" + tipo + "' nao existe.");
}

Habilidade* HabilidadeFactory::criar_por_codigo(string codigo) {
    if (codigo == "golpe")       return criar_ofensiva("golpe");
    if (codigo == "fogo")        return criar_ofensiva("fogo");
    if (codigo == "postura")     return criar_defensiva("postura");
    if (codigo == "cura")        return criar_suporte("cura");
    if (codigo == "regeneracao") return criar_suporte("regeneracao");
    throw invalid_argument("Codigo de habilidade invalido no save: " + codigo);
}

string HabilidadeFactory::codigo_habilidade(Habilidade* habilidade) {
    if (!habilidade) return "-";
    string nome = habilidade->get_nome();
    if (nome == "Golpe Poderoso")    return "golpe";
    if (nome == "Bola de Fogo")      return "fogo";
    if (nome == "Postura Defensiva") return "postura";
    if (nome == "Cura Menor")        return "cura";
    if (nome == "Regeneracao")       return "regeneracao";
    throw invalid_argument("Nao foi possivel serializar a habilidade: " + nome);
}

} // namespace rpg
