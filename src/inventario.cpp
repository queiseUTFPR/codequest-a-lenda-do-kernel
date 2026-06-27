#include "inventario.h"
#include "jogador.h" // Necessario aqui (e nao no .h) pois usar_pocao() chama dono->curar()
#include "factories.h"
#include <iostream>
using namespace std;

namespace rpg {

// ---------- Exceções ----------

InventarioCheioError::InventarioCheioError(const string& msg) : runtime_error(msg) {}
PesoExcedidoError::PesoExcedidoError(const string& msg) : runtime_error(msg) {}
ItemNaoEncontradoError::ItemNaoEncontradoError(const string& msg) : runtime_error(msg) {}


// ---------- Inventario ----------

Inventario::Inventario(int capacidade, float peso_maximo) {
    capacidade_max    = capacidade;
    peso_max          = peso_maximo;
    arma_equipada     = nullptr;
    armadura_equipada = nullptr;
}

Inventario::~Inventario() {
    for (Item* item : mochila) delete item;
    delete arma_equipada;
    delete armadura_equipada;
}

float Inventario::peso_atual() {
    float total = 0.0f;
    for (Item* item : mochila) total += item->get_peso();
    if (arma_equipada)     total += arma_equipada->get_peso();
    if (armadura_equipada) total += armadura_equipada->get_peso();
    return total;
}

void Inventario::adicionar_item(Item* item) {
    if ((int)mochila.size() >= capacidade_max) {
        throw InventarioCheioError("Mochila cheia! Capacidade maxima: " + to_string(capacidade_max));
    }
    if (peso_atual() + item->get_peso() > peso_max) {
        throw PesoExcedidoError("Peso maximo excedido! Limite: " + to_string(peso_max) + "kg");
    }
    mochila.push_back(item);
    cout << item->get_nome() << " adicionado a mochila." << endl;
}

Item* Inventario::remover_item(string nome) {
    for (size_t i = 0; i < mochila.size(); i++) {
        if (mochila[i]->get_nome() == nome) {
            Item* item = mochila[i];
            mochila.erase(mochila.begin() + i);
            return item;
        }
    }
    throw ItemNaoEncontradoError("Item '" + nome + "' nao encontrado na mochila.");
}

void Inventario::equipar_arma(string nome) {
    Item* encontrado = remover_item(nome);
    Arma* arma = dynamic_cast<Arma*>(encontrado);
    if (!arma) {
        mochila.push_back(encontrado); // Devolve o item, pois nao e uma Arma
        throw ItemNaoEncontradoError("'" + nome + "' nao e uma arma.");
    }
    if (arma_equipada) mochila.push_back(arma_equipada); // Desequipa a anterior automaticamente
    arma_equipada = arma;
    cout << nome << " equipada como arma!" << endl;
}

void Inventario::desequipar_arma() {
    if (arma_equipada) {
        cout << arma_equipada->get_nome() << " desequipada." << endl;
        mochila.push_back(arma_equipada);
        arma_equipada = nullptr;
    }
}

void Inventario::equipar_armadura(string nome) {
    Item* encontrado = remover_item(nome);
    Armadura* armadura = dynamic_cast<Armadura*>(encontrado);
    if (!armadura) {
        mochila.push_back(encontrado);
        throw ItemNaoEncontradoError("'" + nome + "' nao e uma armadura.");
    }
    if (armadura_equipada) mochila.push_back(armadura_equipada);
    armadura_equipada = armadura;
    cout << nome << " equipada como armadura!" << endl;
}

void Inventario::desequipar_armadura() {
    if (armadura_equipada) {
        cout << armadura_equipada->get_nome() << " desequipada." << endl;
        mochila.push_back(armadura_equipada);
        armadura_equipada = nullptr;
    }
}

int Inventario::bonus_ataque() { return arma_equipada     ? arma_equipada->get_dano()     : 0; }
int Inventario::bonus_defesa() { return armadura_equipada ? armadura_equipada->get_defesa() : 0; }

void Inventario::usar_pocao(string nome, Jogador* dono) {
    Item* encontrado = remover_item(nome);
    Pocao* pocao = dynamic_cast<Pocao*>(encontrado);
    if (!pocao) {
        mochila.push_back(encontrado); // Devolve o item, pois nao e uma Pocao
        throw ItemNaoEncontradoError("'" + nome + "' nao e uma pocao.");
    }
    pocao->usar(); // Imprime o efeito da pocao
    if (pocao->get_efeito() == "HP restaurado") {
        dono->curar(pocao->get_valor()); // Aplica o efeito automaticamente no dono
    } else if (pocao->get_efeito() == "Mana restaurada") {
        dono->restaurar_recurso(pocao->get_valor());
    } else if (pocao->get_efeito() == "Forca aumentada") {
        dono->redistribuir_pontos(pocao->get_valor());
    }
    delete pocao; // Pocao e consumivel: e destruida apos o uso
}


void Inventario::usar_item_especial(string nome, Jogador* dono) {
    Item* encontrado = remover_item(nome);
    ItemEspecial* especial = dynamic_cast<ItemEspecial*>(encontrado);
    if (!especial) {
        mochila.push_back(encontrado); // Devolve o item, pois nao e um ItemEspecial
        throw ItemNaoEncontradoError("'" + nome + "' nao e um item especial.");
    }

    especial->usar();
    if (especial->get_efeito() == "Ouro recebido") {
        dono->ganhar_ouro(especial->get_valor());
    }
    delete especial; // ItemEspecial de uso unico
}

static vector<string> dividir_string(string texto, char separador) {
    vector<string> partes;
    string atual;
    for (char c : texto) {
        if (c == separador) {
            partes.push_back(atual);
            atual.clear();
        } else {
            atual += c;
        }
    }
    partes.push_back(atual);
    return partes;
}

string Inventario::serializar() {
    string saida = "arma=" + ItemFactory::codigo_item(arma_equipada);
    saida += "|armadura=" + ItemFactory::codigo_item(armadura_equipada);
    saida += "|mochila=";

    if (mochila.empty()) {
        saida += "-";
    } else {
        for (size_t i = 0; i < mochila.size(); i++) {
            if (i > 0) saida += ",";
            saida += ItemFactory::codigo_item(mochila[i]);
        }
    }
    return saida;
}

void Inventario::restaurar(string dados) {
    for (Item* item : mochila) delete item;
    mochila.clear();
    delete arma_equipada;
    delete armadura_equipada;
    arma_equipada = nullptr;
    armadura_equipada = nullptr;

    if (dados.empty() || dados == "-") return;

    string codigo_arma = "-";
    string codigo_armadura = "-";
    string codigos_mochila = "-";

    for (string parte : dividir_string(dados, '|')) {
        size_t pos = parte.find('=');
        if (pos == string::npos) continue;
        string chave = parte.substr(0, pos);
        string valor = parte.substr(pos + 1);
        if (chave == "arma") codigo_arma = valor;
        else if (chave == "armadura") codigo_armadura = valor;
        else if (chave == "mochila") codigos_mochila = valor;
    }

    if (codigo_arma != "-") {
        Item* item = ItemFactory::criar_item_por_codigo(codigo_arma);
        arma_equipada = dynamic_cast<Arma*>(item);
        if (!arma_equipada) { delete item; throw ItemNaoEncontradoError("Item salvo como arma nao e uma arma: " + codigo_arma); }
    }

    if (codigo_armadura != "-") {
        Item* item = ItemFactory::criar_item_por_codigo(codigo_armadura);
        armadura_equipada = dynamic_cast<Armadura*>(item);
        if (!armadura_equipada) { delete item; throw ItemNaoEncontradoError("Item salvo como armadura nao e uma armadura: " + codigo_armadura); }
    }

    if (codigos_mochila != "-" && !codigos_mochila.empty()) {
        for (string codigo : dividir_string(codigos_mochila, ',')) {
            if (!codigo.empty() && codigo != "-") adicionar_item(ItemFactory::criar_item_por_codigo(codigo));
        }
    }
}

void Inventario::listar() {
    cout << "\n--- Inventario (" << mochila.size() << "/" << capacidade_max << " slots | "
         << peso_atual() << "/" << peso_max << "kg) ---" << endl;
    cout << "Arma equipada: "     << (arma_equipada     ? arma_equipada->get_nome()     : "Nenhuma") << endl;
    cout << "Armadura equipada: " << (armadura_equipada ? armadura_equipada->get_nome() : "Nenhuma") << endl;
    for (Item* item : mochila) cout << "  - " << item->get_nome() << endl;
}

} // namespace rpg
