#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "personagens.h"
#include "factories.h"
#include "batalha.h"
#include "persistencia.h"
#include "estrategia.h"
#include "observer.h"
#include "menu.h"

using namespace std;
using namespace rpg;

/**
 * @brief Roteiro de demonstração automática de todos os módulos do sistema.
 * Não depende de entrada do usuário — útil para verificação rápida (ex: ./codequest --demo).
 * O menu interativo real (RF8) está em executar_menu_interativo(), em menu.cpp.
 */
static void executar_demonstracao() {
    srand(static_cast<unsigned>(time(0))); // Inicializa a semente para números aleatórios

    // JogadorFactory: criando personagens com raça definida pela fábrica
    cout << "===== CRIANDO PERSONAGENS (JogadorFactory) =====" << endl;
    Guerreiro* guerreiro = JogadorFactory::criar_guerreiro("Kain",      "Humano");
    Mago*      mago      = JogadorFactory::criar_mago("Dumbledore",     "Humano");
    Monstro*    monstro    = JogadorFactory::criar_monstro("Gorath",        "Orc");
    Arqueiro*  arqueiro  = JogadorFactory::criar_arqueiro("Legolas",     "Elfo");
    Clerigo*   clerigo   = JogadorFactory::criar_clerigo("Anduin",       "Humano");

    vector<Jogador*> grupo = {guerreiro, mago, monstro, arqueiro, clerigo};

    // Observer: registra um notificador para receber eventos automaticos de level up e derrota
    NotificadorConsole notificador;
    guerreiro->adicionar_observador(&notificador);
    mago->adicionar_observador(&notificador);
    monstro->adicionar_observador(&notificador);
    arqueiro->adicionar_observador(&notificador);
    clerigo->adicionar_observador(&notificador);

    cout << "\n===== STATUS INICIAL =====" << endl;
    for (Jogador* personagem : grupo) personagem->exibirStatus();

    // ===== INVENTARIO: montando a mochila do Guerreiro via ItemFactory =====
    cout << "===== MONTANDO INVENTARIO DO GUERREIRO =====" << endl;
    guerreiro->get_inventario().adicionar_item(ItemFactory::criar_arma("espada"));
    guerreiro->get_inventario().adicionar_item(ItemFactory::criar_armadura("ferro"));
    guerreiro->get_inventario().adicionar_item(ItemFactory::criar_pocao("vida"));
    guerreiro->get_inventario().adicionar_item(ItemFactory::criar_item_especial("anel"));
    guerreiro->get_inventario().listar();

    cout << "\n===== EQUIPANDO O GUERREIRO =====" << endl;
    guerreiro->get_inventario().equipar_arma("Espada de Aco");
    guerreiro->get_inventario().equipar_armadura("Peitoral de Ferro");
    guerreiro->get_inventario().listar();

    cout << "\nAtaque do Guerreiro COM equipamento:" << endl;
    guerreiro->atacar();

    cout << "\n===== DESEQUIPANDO A ARMA =====" << endl;
    guerreiro->get_inventario().desequipar_arma();
    cout << "Ataque do Guerreiro SEM arma:" << endl;
    guerreiro->atacar();
    guerreiro->get_inventario().equipar_arma("Espada de Aco");

    cout << "\n===== USANDO POCAO DO INVENTARIO =====" << endl;
    guerreiro->receber_dano(40);
    guerreiro->usar_pocao("Pocao de Vida");
    guerreiro->get_inventario().usar_item_especial("Anel da Fortuna", guerreiro);
    guerreiro->exibirStatus();

    cout << "===== TESTANDO EXCECOES DO INVENTARIO =====" << endl;
    cout << "\n--- Item nao encontrado na mochila ---" << endl;
    try {
        guerreiro->get_inventario().equipar_arma("Machado Lendario");
    } catch (const ItemNaoEncontradoError& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }

    cout << "\n--- Capacidade maxima da mochila ---" << endl;
    for (int i = 0; i < 15; i++) {
        Pocao* nova_pocao = ItemFactory::criar_pocao("mana");
        try {
            guerreiro->get_inventario().adicionar_item(nova_pocao);
        } catch (const InventarioCheioError& e) {
            cout << "[ERRO]: " << e.what() << endl;
            delete nova_pocao;
            break;
        }
    }

    cout << "\n===== TESTANDO OUTROS ITENS (ItemFactory) =====" << endl;
    Arma*     a2 = ItemFactory::criar_arma("cajado");    a2->usar(); delete a2;
    Armadura* r2 = ItemFactory::criar_armadura("tela");  r2->usar(); delete r2;
    ItemEspecial* e1 = ItemFactory::criar_item_especial("reliquia"); e1->usar(); delete e1;
    cout << "Preco da Reliquia Antiga na loja: " << ItemFactory::preco_item_por_codigo("especial:reliquia") << " ouro" << endl;
    Arma*     a3 = ItemFactory::criar_arma("adaga");     a3->usar(); delete a3;
    Armadura* r3 = ItemFactory::criar_armadura("couro"); r3->usar(); delete r3;

    cout << "\n--- Usando pocoes ---" << endl;
    Pocao* p2 = ItemFactory::criar_pocao("mana");  p2->usar(); delete p2;
    Pocao* p3 = ItemFactory::criar_pocao("forca"); p3->usar(); delete p3;

    cout << "\n--- Tipo invalido ---" << endl;
    try {
        Arma* inv = ItemFactory::criar_arma("lanca");
        delete inv;
    } catch (const invalid_argument& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }

    // ===== HABILIDADES =====
    cout << "\n===== APRENDENDO HABILIDADES =====" << endl;
    guerreiro->aprender_habilidade(HabilidadeFactory::criar_ofensiva("golpe"));
    mago->aprender_habilidade(HabilidadeFactory::criar_ofensiva("fogo"));
    mago->aprender_habilidade(HabilidadeFactory::criar_suporte("regeneracao"));
    monstro->aprender_habilidade(HabilidadeFactory::criar_defensiva("postura"));
    clerigo->aprender_habilidade(HabilidadeFactory::criar_suporte("cura"));

    guerreiro->listar_habilidades();
    mago->listar_habilidades();
    monstro->listar_habilidades();
    clerigo->listar_habilidades();

    cout << "\n===== USANDO HABILIDADES ATIVAS =====" << endl;
    guerreiro->usar_habilidade("Golpe Poderoso", monstro);
    mago->usar_habilidade("Bola de Fogo", monstro);
    monstro->usar_habilidade("Postura Defensiva");

    cout << "\n--- Tentando usar uma habilidade passiva manualmente ---" << endl;
    try {
        mago->usar_habilidade("Regeneracao");
    } catch (const HabilidadePassivaError& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }

    cout << "\n--- Aplicando habilidades passivas (chamada automatica do sistema) ---" << endl;
    mago->aplicar_habilidades_passivas();

    cout << "\n--- Tentando usar uma habilidade nunca aprendida ---" << endl;
    try {
        guerreiro->usar_habilidade("Bola de Fogo", monstro);
    } catch (const HabilidadeNaoEncontradaError& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }

    cout << "\n===== RODADA DE COMBATE =====" << endl;
    for (Jogador* personagem : grupo) personagem->atacar();

    cout << "\n--- Mago esgotando a mana ---" << endl;
    mago->atacar();
    mago->atacar();
    mago->atacar();

    cout << "\n" << endl;
    guerreiro->receber_dano(30);
    mago->receber_dano(25);
    monstro->receber_dano(20);
    cout << "\n" << endl;

    guerreiro->ganhar_xp(120);

    cout << "\n===== STATUS FINAL =====" << endl;
    for (Jogador* personagem : grupo) personagem->exibirStatus();

    // ===== BATALHA POR TURNOS =====
    cout << "\n\n===== INICIANDO BATALHA POR TURNOS =====" << endl;
    Batalha batalha(grupo); // jogador_humano = nullptr -> bot vs bot
    batalha.iniciar(15);

    // Para batalha humano vs bot:
    // Batalha batalha_interativa(grupo, guerreiro);
    // batalha_interativa.iniciar();

    // ===== PADRAO STRATEGY =====
    cout << "\n\n===== DEMONSTRACAO: PADRAO STRATEGY =====" << endl;
    Guerreiro* arthas = JogadorFactory::criar_guerreiro("Arthas", "Humano");
    Guerreiro* thrall  = JogadorFactory::criar_guerreiro("Thrall", "Orc");
    Guerreiro* jaina   = JogadorFactory::criar_guerreiro("Jaina",  "Humano");
    jaina->receber_dano(60);
    vector<Jogador*> trio = {arthas, thrall, jaina};

    Batalha demo_padrao(trio);
    cout << "Estrategia padrao (Round-Robin): " << arthas->get_nome() << " ataca -> "
         << demo_padrao.escolher_alvo(arthas)->get_nome() << endl;

    Batalha demo_trocada(trio);
    demo_trocada.definir_estrategia(new AlvoMaisFraco());
    cout << "Estrategia trocada (Mais Fraco):  " << arthas->get_nome() << " ataca -> "
         << demo_trocada.escolher_alvo(arthas)->get_nome() << endl;

    delete arthas;
    delete thrall;
    delete jaina;

    // ===== SOBRECARGA DE OPERADORES =====
    cout << "\n\n===== DEMONSTRACAO: SOBRECARGA DE OPERADORES =====" << endl;
    cout << "operator<< (formato de save) - Kain : " << *guerreiro << endl;
    cout << "operator<< (formato de save) - Gorath: " << *monstro << endl;

    cout << "\nKain == Gorath? " << ((*guerreiro == *monstro) ? "Sim" : "Nao") << endl;
    cout << "Gorath < Kain (por HP)? " << ((*monstro < *guerreiro) ? "Sim" : "Nao") << endl;

    Arma* espada_base = ItemFactory::criar_arma("espada");
    Arma* adaga_base  = ItemFactory::criar_arma("adaga");
    Arma  combinada   = *espada_base + *adaga_base;
    cout << "\nArma combinada: " << combinada.get_nome() << " | Dano: " << combinada.get_dano() << endl;
    delete espada_base;
    delete adaga_base;

    // ===== PERSISTÊNCIA =====
    cout << "\n\n===== DEMONSTRACAO: PERSISTENCIA =====" << endl;
    string caminho_save = "save_rpg.txt";
    Persistencia::salvar(grupo, caminho_save);

    vector<Jogador*> grupo_carregado = Persistencia::carregar(caminho_save);
    cout << "\n--- Personagens carregados do arquivo ---" << endl;
    for (Jogador* p : grupo_carregado) p->exibirStatus();

    cout << "\n--- Tentando carregar um arquivo que nao existe ---" << endl;
    try {
        Persistencia::carregar("arquivo_inexistente.txt");
    } catch (const PersistenciaError& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }

    for (Jogador* p : grupo_carregado) delete p;

    // Libera a memória dos personagens criados pela fábrica
    delete guerreiro;
    delete mago;
    delete monstro;
    delete arqueiro;
    delete clerigo;
}


/**
 * @brief Ponto de entrada do programa.
 * Sem argumentos: abre o menu interativo (RF8), cobrindo todas as operações do sistema.
 * Com "--demo": executa o roteiro de demonstração automática de todos os módulos.
 */
int main(int argc, char** argv) {
    if (argc > 1 && string(argv[1]) == "--demo") {
        executar_demonstracao();
    } else {
        executar_menu_interativo();
    }
    return 0;
}
