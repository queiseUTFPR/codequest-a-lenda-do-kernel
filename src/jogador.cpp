#include "jogador.h"
#include "factories.h" // Necessario para serializar/restaurar habilidades pela HabilidadeFactory
#include <algorithm> // STL: usado em remover_observador (std::remove)
using namespace std;

namespace rpg {

Jogador::Jogador(string nome_jogador, string raca_jogador) {
    nome     = nome_jogador;
    raca     = RacaFactory::criar(raca_jogador);
    nivel    = 1;
    hp_max   = 100;
    raca->aplicar_bonus_base(hp_max);
    hp_atual = hp_max;
    xp       = 0;
    ouro     = 100; // Valor inicial para permitir compras no início do jogo
}

// Destrutor para liberar recursos e avisar encerramento (Virtual: evita memory leak com polimorfismo)
Jogador::~Jogador() {
    cout << "Objeto de " << nome << " destruido. Recursos liberados." << endl;
    for (Habilidade* h : habilidades) delete h; // Libera as habilidades aprendidas
    delete raca;
}

string Jogador::get_nome()     { return nome; }
string Jogador::get_raca()     { return raca ? raca->get_nome() : "Sem raca"; }
string Jogador::get_descricao_raca() { return raca ? raca->get_descricao() : ""; }
int    Jogador::get_nivel()    { return nivel; }
int    Jogador::get_hp_max()   { return hp_max; }
int    Jogador::get_hp_atual() { return hp_atual; }
int    Jogador::get_xp()       { return xp; }
int    Jogador::get_ouro()     { return ouro; }

void Jogador::restaurar_estado(int nivel_salvo, int hp_max_salvo, int hp_atual_salvo, int xp_salvo) {
    nivel    = nivel_salvo;
    hp_max   = hp_max_salvo;
    hp_atual = hp_atual_salvo;
    xp       = xp_salvo;
}

void Jogador::restaurar_ouro(int ouro_salvo) {
    ouro = ouro_salvo;
    if (ouro < 0) ouro = 0;
}

// Sobrecarga de operador: formato base comum a todos os personagens; subclasses acrescentam
// seus atributos especificos (usado tanto para exibir no console quanto para salvar em arquivo)
string Jogador::serializar() {
    return nome + ";" + get_raca() + ";" + to_string(nivel) + ";" + to_string(hp_max) + ";" +
           to_string(hp_atual) + ";" + to_string(xp);
}

ostream& operator<<(ostream& os, Jogador& j) {
    os << j.serializar();
    return os;
}

bool Jogador::operator==(Jogador& outro) { return nome == outro.nome; }
bool Jogador::operator<(Jogador& outro)  { return hp_atual < outro.hp_atual; }

Inventario& Jogador::get_inventario()  { return inventario; }
int  Jogador::get_bonus_ataque()       { return inventario.bonus_ataque(); }
int  Jogador::get_bonus_defesa()       { return inventario.bonus_defesa(); }
void Jogador::usar_pocao(string nome)  { inventario.usar_pocao(nome, this); }

// Virtual: por padrao, qualquer personagem pode usar uma habilidade sem custo extra
bool Jogador::consumir_recurso(int quantidade) { return true; }

void Jogador::restaurar_recurso(int quantidade) {
    cout << nome << " nao possui recurso especial para restaurar." << endl;
}

void Jogador::aprender_habilidade(Habilidade* h) {
    habilidades.push_back(h);
    cout << nome << " aprendeu a habilidade '" << h->get_nome() << "'!" << endl;
}

string Jogador::serializar_habilidades() {
    if (habilidades.empty()) return "-";
    string saida;
    for (size_t i = 0; i < habilidades.size(); i++) {
        if (i > 0) saida += ",";
        saida += HabilidadeFactory::codigo_habilidade(habilidades[i]);
    }
    return saida;
}

void Jogador::restaurar_habilidades(string dados) {
    for (Habilidade* h : habilidades) delete h;
    habilidades.clear();

    if (dados.empty() || dados == "-") return;

    string atual;
    for (char c : dados) {
        if (c == ',') {
            if (!atual.empty()) habilidades.push_back(HabilidadeFactory::criar_por_codigo(atual));
            atual.clear();
        } else {
            atual += c;
        }
    }
    if (!atual.empty()) habilidades.push_back(HabilidadeFactory::criar_por_codigo(atual));
}

void Jogador::listar_habilidades() {
    cout << "\n--- Habilidades de " << nome << " ---" << endl;
    if (habilidades.empty()) { cout << "  Nenhuma habilidade aprendida." << endl; return; }
    for (Habilidade* h : habilidades) {
        cout << "  - " << h->get_nome() << " (" << (h->eh_ativa() ? "Ativa" : "Passiva") << ")" << endl;
    }
}

void Jogador::usar_habilidade(string nome_habilidade, Jogador* alvo) {
    for (Habilidade* h : habilidades) {
        if (h->get_nome() == nome_habilidade) {
            if (!h->eh_ativa()) {
                throw HabilidadePassivaError("'" + nome_habilidade + "' e uma habilidade passiva e age automaticamente.");
            }
            h->efeito(this, alvo); // Polimorfismo: cada habilidade concreta aplica seu proprio efeito
            return;
        }
    }
    throw HabilidadeNaoEncontradaError("Habilidade '" + nome_habilidade + "' nao foi aprendida por " + nome + ".");
}

void Jogador::aplicar_habilidades_passivas() {
    for (Habilidade* h : habilidades) {
        if (!h->eh_ativa()) h->efeito(this, nullptr); // Passivas agem sobre o proprio usuario
    }
}

void Jogador::adicionar_observador(ObservadorDeEventos* obs) { observadores.push_back(obs); }

void Jogador::remover_observador(ObservadorDeEventos* obs) {
    observadores.erase(remove(observadores.begin(), observadores.end(), obs), observadores.end());
}

void Jogador::notificar_observadores(string evento) {
    for (ObservadorDeEventos* obs : observadores) obs->notificar(evento);
}

// SETTER: Valida o valor antes de alterar o HP (evita vida negativa ou acima do máximo)
void Jogador::set_hp_atual(int valor) {
    if (valor > hp_max)  hp_atual = hp_max;
    else if (valor < 0)  hp_atual = 0;
    else                 hp_atual = valor;
}

void Jogador::subir_nivel() {
    nivel++;
    xp = 0;
    hp_max  += 50;
    hp_atual = hp_max;
    cout << "!!! LVL UP !!! " << nome << " agora e nivel " << nivel << endl;
    notificar_observadores(nome + " subiu para o nivel " + to_string(nivel) + "!"); // Observer: avisa interessados
}

void Jogador::ganhar_xp(int pontos) {
    if (pontos > 0) {
        xp += pontos;
        cout << nome << " ganhou " << pontos << " de XP!" << endl;
        if (xp >= 100) subir_nivel(); // Se atingir ou passar de 100 XP, o jogador evolui
    }
}

void Jogador::exibirStatus() {
    cout << "\n**** Status de " << nome << " ****" << endl;
    cout << "Raca: " << get_raca() << " (" << get_descricao_raca() << ")" << endl;
    cout << "Nivel: " << nivel << " | XP: " << xp << "/100" << endl;
    cout << "Ouro: " << ouro << endl;
    cout << "HP: " << hp_atual << "/" << hp_max << endl;
    cout << "Bonus Ataque (arma): " << get_bonus_ataque() << " | Bonus Defesa (armadura): " << get_bonus_defesa() << endl;
}

void Jogador::receber_dano(int quantidade) {
    if (quantidade > 0) {
        // RF4: Atualizacao automatica de atributos — a armadura equipada reduz o dano recebido
        int dano_final = quantidade - get_bonus_defesa();
        if (dano_final < 0) dano_final = 0;
        bool estava_vivo = hp_atual > 0;
        set_hp_atual(hp_atual - dano_final);
        cout << nome << " recebeu " << dano_final << " de dano!" << endl;
        if (estava_vivo && hp_atual <= 0) {
            notificar_observadores(nome + " foi derrotado!"); // Observer: avisa sem que o chamador precise saber
        }
    }
}

void Jogador::curar(int quantidade) {
    if (quantidade > 0) {
        if ((hp_atual + quantidade) > hp_max) {
            cout << "Aviso: O valor de cura (" << quantidade << ") excede o HP maximo. Acao cancelada!" << endl;
        } else {
            hp_atual += quantidade;
            cout << nome << " foi curado em " << quantidade << " pontos!" << endl;
        }
    } else {
        cout << "Quantidade de cura invalida para " << nome << endl;
    }
}

void Jogador::ganhar_ouro(int quantidade) {
    if (quantidade <= 0) return;
    ouro += quantidade;
    cout << nome << " recebeu " << quantidade << " moedas de ouro. (Ouro atual: " << ouro << ")" << endl;
}

bool Jogador::gastar_ouro(int quantidade) {
    if (quantidade <= 0) return true;
    if (ouro < quantidade) {
        cout << nome << " nao possui ouro suficiente. Necessario: " << quantidade << " | Atual: " << ouro << endl;
        return false;
    }
    ouro -= quantidade;
    cout << nome << " gastou " << quantidade << " moedas de ouro. (Ouro atual: " << ouro << ")" << endl;
    return true;
}

// Virtual: por padrao nao ha atributo proprio para redistribuir; subclasses sobrescrevem
void Jogador::redistribuir_pontos(int pontos) {
    cout << nome << " nao tem um atributo redistribuivel definido." << endl;
}

} // namespace rpg
