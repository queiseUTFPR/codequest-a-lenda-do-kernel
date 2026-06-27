#include "persistencia.h"
#include <fstream>
#include <iostream>
using namespace std;

namespace rpg {

PersistenciaError::PersistenciaError(const string& msg) : runtime_error(msg) {}

void Persistencia::salvar(vector<Jogador*>& personagens, string caminho) {
    ofstream arquivo(caminho);
    if (!arquivo.is_open()) {
        throw PersistenciaError("Nao foi possivel abrir o arquivo para salvar: " + caminho);
    }
    for (Jogador* p : personagens) {
        // RF7 completo: salva o estado base + atributos da subclasse + inventario + habilidades.
        arquivo << *p
                << ";OURO;" << p->get_ouro()
                << ";INV;" << p->get_inventario().serializar()
                << ";HAB;" << p->serializar_habilidades()
                << endl;
    }
    arquivo.close();
    cout << "Jogo salvo em '" << caminho << "' (" << personagens.size() << " personagens)." << endl;
}

vector<Jogador*> Persistencia::carregar(string caminho) {
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        throw PersistenciaError("Arquivo de save nao encontrado: " + caminho);
    }

    vector<Jogador*> personagens;
    string linha;
    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;
        personagens.push_back(reconstruir(linha));
    }
    arquivo.close();
    cout << "Jogo carregado de '" << caminho << "' (" << personagens.size() << " personagens)." << endl;
    return personagens;
}

vector<string> Persistencia::dividir(string linha, char separador) {
    vector<string> campos;
    string atual;
    for (char c : linha) {
        if (c == separador) { campos.push_back(atual); atual = ""; }
        else atual += c;
    }
    campos.push_back(atual);
    return campos;
}

static bool eh_marcador_extra(const vector<string>& c, size_t pos) {
    if (pos >= c.size()) return true;
    return c[pos] == "INV" || c[pos] == "HAB" || c[pos] == "OURO";
}

Jogador* Persistencia::reconstruir(string linha) {
    vector<string> c = dividir(linha, ';');
    if (c.size() < 7) {
        throw PersistenciaError("Linha de save invalida: " + linha);
    }

    string tipo     = c[0];
    string nome     = c[1];
    string raca     = c[2];
    int    nivel    = stoi(c[3]);
    int    hp_max   = stoi(c[4]);
    int    hp_atual = stoi(c[5]);
    int    xp       = stoi(c[6]);

    size_t pos = 7;
    Jogador* p = nullptr;

    if (tipo == "Guerreiro") {
        if (c.size() <= pos) throw PersistenciaError("Save de Guerreiro incompleto: " + linha);
        int forca = stoi(c[pos++]);
        p = new Guerreiro(nome, forca, raca);
    } else if (tipo == "Mago") {
        if (c.size() <= pos) throw PersistenciaError("Save de Mago incompleto: " + linha);
        int inteligencia = stoi(c[pos++]);
        int mana_atual = 0;
        int mana_max = 0;

        // Compatibilidade: saves antigos tinham apenas mana_max; saves novos têm mana_atual e mana_max.
        if (pos + 1 < c.size() && !eh_marcador_extra(c, pos + 1)) {
            mana_atual = stoi(c[pos++]);
            mana_max   = stoi(c[pos++]);
        } else {
            mana_max   = stoi(c[pos++]);
            mana_atual = mana_max;
        }
        Mago* m = new Mago(nome, inteligencia, mana_max, raca);
        m->restaurar_mana(mana_atual, mana_max);
        p = m;
    } else if (tipo == "Monstro" || tipo == "Ladrao") {
        // "Ladrao" mantido apenas para compatibilidade com arquivos salvos antes da troca para Monstro.
        if (c.size() <= pos + 1) throw PersistenciaError("Save de Monstro incompleto: " + linha);
        int   ferocidade      = stoi(c[pos++]);
        float chance_critico = stof(c[pos++]);
        p = new Monstro(nome, ferocidade, chance_critico, raca);
    } else if (tipo == "Arqueiro") {
        if (c.size() <= pos + 1) throw PersistenciaError("Save de Arqueiro incompleto: " + linha);
        int destreza = stoi(c[pos++]);
        int flechas  = stoi(c[pos++]);
        p = new Arqueiro(nome, destreza, flechas, raca);
    } else if (tipo == "Clerigo") {
        if (c.size() <= pos + 2) throw PersistenciaError("Save de Clerigo incompleto: " + linha);
        int sabedoria = stoi(c[pos++]);
        int fe_atual  = stoi(c[pos++]);
        int fe_max    = stoi(c[pos++]);
        Clerigo* cl = new Clerigo(nome, sabedoria, fe_max, raca);
        cl->restaurar_fe(fe_atual, fe_max);
        p = cl;
    } else {
        throw PersistenciaError("Tipo de personagem desconhecido no save: " + tipo);
    }

    p->restaurar_estado(nivel, hp_max, hp_atual, xp); // Corrige nivel/HP/XP para o valor salvo

    while (pos < c.size()) {
        string marcador = c[pos++];
        if (pos >= c.size()) break;
        string valor = c[pos++];
        if (marcador == "OURO") {
            p->restaurar_ouro(stoi(valor));
        } else if (marcador == "INV") {
            p->get_inventario().restaurar(valor);
        } else if (marcador == "HAB") {
            p->restaurar_habilidades(valor);
        }
    }

    return p;
}

} // namespace rpg
