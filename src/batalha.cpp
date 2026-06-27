#include "batalha.h"
#include "inventario.h" // Para capturar ItemNaoEncontradoError no menu interativo
#include "habilidade.h" // Para capturar HabilidadeNaoEncontradaError/HabilidadePassivaError
#include <iostream>
using namespace std;

namespace rpg {

Batalha::Batalha(vector<Jogador*> personagens, Jogador* humano, EstrategiaDeAlvo* estrategia_alvo) {
    participantes  = personagens;
    turno_atual    = 0;
    if (humano) humanos.push_back(humano); // Compatibilidade: forma antiga de marcar 1 humano
    estrategia     = estrategia_alvo ? estrategia_alvo : new AlvoRoundRobin(); // Padrao: comportamento original
}

Batalha::~Batalha() {
    delete estrategia;
}

void Batalha::adicionar_jogador_humano(Jogador* j) {
    humanos.push_back(j); // RF5: permite marcar mais de um participante como humano (humano vs humano)
}

void Batalha::definir_estrategia(EstrategiaDeAlvo* nova) {
    delete estrategia;
    estrategia = nova;
}

void Batalha::registrar_log(string mensagem) { log.push_back(mensagem); }

void Batalha::exibir_log() {
    cout << "\n===== LOG DA BATALHA =====" << endl;
    for (string linha : log) cout << linha << endl;
}

void Batalha::exibir_estatisticas() {
    cout << "\n===== ESTATISTICAS DE DANO =====" << endl;
    for (auto& par : dano_causado) {
        cout << "  " << par.first << ": " << par.second << " de dano total" << endl;
    }
}

bool Batalha::batalha_terminou() {
    int vivos = 0;
    for (Jogador* p : participantes) if (p->get_hp_atual() > 0) vivos++;
    return vivos <= 1;
}

Jogador* Batalha::vencedor() {
    Jogador* vivo = nullptr;
    int      vivos = 0;
    for (Jogador* p : participantes) {
        if (p->get_hp_atual() > 0) { vivo = p; vivos++; }
    }
    return (vivos == 1) ? vivo : nullptr;
}

Jogador* Batalha::escolher_alvo(Jogador* atacante) {
    return estrategia->escolher(atacante, participantes); // Strategy: delega ao algoritmo configurado
}

void Batalha::agir_bot(Jogador* atacante) {
    Jogador* alvo = escolher_alvo(atacante);
    if (!alvo) return;

    int dano = atacante->atacar(); // Polimorfismo: cada classe ataca de um jeito diferente
    alvo->receber_dano(dano);
    dano_causado[atacante->get_nome()] += dano; // STL map: acumula o total de dano por nome
    registrar_log(atacante->get_nome() + " atacou " + alvo->get_nome() + " causando " + to_string(dano) + " de dano.");

    if (alvo->get_hp_atual() <= 0) {
        cout << alvo->get_nome() << " foi derrotado!" << endl;
        registrar_log(alvo->get_nome() + " foi derrotado.");
    }
}

void Batalha::agir_humano(Jogador* atacante) {
    Jogador* alvo = escolher_alvo(atacante);
    if (!alvo) return;

    cout << "\n--- Sua vez, " << atacante->get_nome() << "! ---" << endl;
    cout << "1. Atacar " << alvo->get_nome() << endl;
    cout << "2. Usar Pocao de Vida" << endl;
    cout << "3. Usar Habilidade" << endl;
    cout << "4. Ver Status" << endl;
    cout << "Escolha: ";
    int escolha;
    cin >> escolha;

    if (escolha == 4) {
        atacante->exibirStatus();
        atacante->listar_habilidades();
        agir_humano(atacante); // Pede a acao novamente apos ver o status
        return;
    }

    if (escolha == 3) {
        cout << "Nome da habilidade: ";
        cin.ignore();
        string nome_skill;
        getline(cin, nome_skill);
        try {
            atacante->usar_habilidade(nome_skill, alvo);
            registrar_log(atacante->get_nome() + " usou a habilidade '" + nome_skill + "'.");
            if (alvo->get_hp_atual() <= 0) {
                cout << alvo->get_nome() << " foi derrotado!" << endl;
                registrar_log(alvo->get_nome() + " foi derrotado.");
            }
            return; // Usar habilidade consome o turno
        } catch (const exception& e) {
            cout << "[ERRO]: " << e.what() << " Atacando no lugar." << endl;
        }
    }

    if (escolha == 2) {
        try {
            atacante->usar_pocao("Pocao de Vida");
            registrar_log(atacante->get_nome() + " usou uma Pocao de Vida.");
            return; // Usar pocao consome o turno, sem atacar
        } catch (const ItemNaoEncontradoError& e) {
            cout << "[ERRO]: " << e.what() << " Atacando no lugar." << endl;
        }
    }

    int dano = atacante->atacar();
    alvo->receber_dano(dano);
    dano_causado[atacante->get_nome()] += dano;
    registrar_log(atacante->get_nome() + " atacou " + alvo->get_nome() + " causando " + to_string(dano) + " de dano.");

    if (alvo->get_hp_atual() <= 0) {
        cout << alvo->get_nome() << " foi derrotado!" << endl;
        registrar_log(alvo->get_nome() + " foi derrotado.");
    }
}

void Batalha::executar_turno() {
    turno_atual++;
    cout << "\n----- TURNO " << turno_atual << " -----" << endl;
    for (Jogador* atacante : participantes) {
        if (batalha_terminou()) break;
        if (atacante->get_hp_atual() <= 0) continue; // Derrotados nao agem

        atacante->aplicar_habilidades_passivas(); // Habilidades passivas agem automaticamente

        bool eh_humano = false;
        for (Jogador* h : humanos) { if (h == atacante) { eh_humano = true; break; } }

        if (eh_humano) agir_humano(atacante);
        else           agir_bot(atacante);
    }
}

void Batalha::iniciar(int max_turnos) {
    cout << "\n===== BATALHA INICIADA =====" << endl;
    for (Jogador* p : participantes) p->exibirStatus();

    while (!batalha_terminou() && turno_atual < max_turnos) {
        executar_turno();
    }

    cout << "\n===== FIM DA BATALHA =====" << endl;
    Jogador* venc = vencedor();
    if (venc) {
        cout << venc->get_nome() << " venceu a batalha!" << endl;
        registrar_log(venc->get_nome() + " venceu a batalha!");
        venc->ganhar_xp(50);
        venc->ganhar_ouro(30);
    } else {
        cout << "A batalha terminou sem vencedor (limite de turnos atingido)." << endl;
    }
    exibir_log();
    exibir_estatisticas();
}

} // namespace rpg
