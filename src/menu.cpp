#include "menu.h"
#include "gerenciador.h"
#include "factories.h"
#include "batalha.h"
#include "persistencia.h"
#include "observer.h"
#include <iostream>
#include <limits>
using namespace std;

namespace rpg {

// Observer: notifica eventos (level up, derrota) de qualquer personagem criado pelo menu
static NotificadorConsole g_notificador;

// Excecao de controle de fluxo (nao e um erro de negocio): sinaliza que a entrada
// padrao acabou (EOF), para que o menu encerre de forma limpa em vez de travar num loop.
class EntradaFinalizadaException {};

// ---------- Helpers de entrada (tratam entrada invalida sem travar o loop) ----------

static int ler_int(string prompt) {
    int valor;
    while (true) {
        cout << prompt;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa o resto da linha apos o numero
            return valor;
        }
        if (cin.eof()) throw EntradaFinalizadaException();
        cout << "Entrada invalida. Digite um numero." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

static int ler_int_intervalo(string prompt, int minimo, int maximo) {
    while (true) {
        int valor = ler_int(prompt);
        if (valor >= minimo && valor <= maximo) return valor;
        cout << "Valor deve estar entre " << minimo << " e " << maximo << "." << endl;
    }
}

// IMPORTANTE: nao chama cin.ignore() aqui. Como ler_int ja limpa sua propria quebra de
// linha, o buffer sempre chega limpo em ler_linha — mesmo quando duas chamadas de
// ler_linha acontecem em sequencia (ex: nome seguido de raca).
static string ler_linha(string prompt) {
    cout << prompt;
    string linha;
    if (!getline(cin, linha)) throw EntradaFinalizadaException();
    return linha;
}

static Jogador* selecionar_personagem(GerenciadorDeJogo& jogo, string contexto) {
    jogo.listar_personagens();
    if (jogo.get_personagens().empty()) return nullptr;
    string nome = ler_linha("\n" + contexto + " (nome): ");
    try {
        return jogo.buscar_por_nome(nome);
    } catch (const PersonagemNaoEncontradoError& e) {
        cout << "[ERRO]: " << e.what() << endl;
        return nullptr;
    }
}

// ---------- RF1: Criação de personagens com distribuição de pontos ----------

static void menu_criar_personagem(GerenciadorDeJogo& jogo) {
    cout << "\n--- Criar Personagem ---" << endl;
    cout << "1. Guerreiro\n2. Mago\n3. Monstro\n4. Arqueiro\n5. Clerigo" << endl;
    int classe = ler_int_intervalo("Classe: ", 1, 5);

    string nome = ler_linha("Nome: ");
    string raca = ler_linha("Raca (Humano, Elfo, Anao ou Orc): ");

    Jogador* novo = nullptr;
    if (classe == 1) {
        cout << "\nForca base: 20. Voce tem 20 pontos extras para distribuir." << endl;
        int pontos = ler_int_intervalo("Pontos extras em Forca (0-20): ", 0, 20);
        novo = jogo.criar_guerreiro(nome, raca, 20 + pontos);
    } else if (classe == 2) {
        cout << "\nInteligencia base: 10 | Mana base: 10. Voce tem 20 pontos extras no total." << endl;
        int pontos_int = ler_int_intervalo("Pontos extras em Inteligencia (0-20): ", 0, 20);
        int pontos_mana = ler_int_intervalo("Pontos extras em Mana (0-" + to_string(20 - pontos_int) + "): ", 0, 20 - pontos_int);
        novo = jogo.criar_mago(nome, raca, 10 + pontos_int, 10 + pontos_mana);
    } else if (classe == 3) {
        cout << "\nFerocidade base: 15. Voce tem 20 pontos extras para distribuir." << endl;
        int pontos = ler_int_intervalo("Pontos extras em Ferocidade (0-20): ", 0, 20);
        novo = jogo.criar_monstro(nome, raca, 15 + pontos, 0.3f);
    } else if (classe == 4) {
        cout << "\nDestreza base: 15 | Flechas base: 6. Voce tem 20 pontos extras para distribuir." << endl;
        int pontos = ler_int_intervalo("Pontos extras em Destreza (0-20): ", 0, 20);
        novo = jogo.criar_arqueiro(nome, raca, 15 + pontos, 6);
    } else {
        cout << "\nSabedoria base: 12 | Fe base: 20. Voce tem 20 pontos extras no total." << endl;
        int pontos_sab = ler_int_intervalo("Pontos extras em Sabedoria (0-20): ", 0, 20);
        int pontos_fe = ler_int_intervalo("Pontos extras em Fe (0-" + to_string(20 - pontos_sab) + "): ", 0, 20 - pontos_sab);
        novo = jogo.criar_clerigo(nome, raca, 12 + pontos_sab, 20 + pontos_fe);
    }

    novo->adicionar_observador(&g_notificador); // Observer: passa a receber notificacoes automaticas
    cout << "\n" << novo->get_nome() << " foi criado com sucesso!" << endl;
    novo->exibirStatus();
}

// ---------- RF2: Listagem e detalhamento ----------

static void menu_detalhar_personagem(GerenciadorDeJogo& jogo) {
    Jogador* p = selecionar_personagem(jogo, "Detalhar qual personagem?");
    if (!p) return;
    p->exibirStatus();
    p->listar_habilidades();
    p->get_inventario().listar();
}

// ---------- RF3/RF4: Inventário ----------


static Item* criar_item_menu_por_tipo(int tipo, const string& codigo) {
    if (tipo == 1) return ItemFactory::criar_arma(codigo);
    if (tipo == 2) return ItemFactory::criar_armadura(codigo);
    if (tipo == 3) return ItemFactory::criar_pocao(codigo);
    if (tipo == 4) return ItemFactory::criar_item_especial(codigo);
    throw invalid_argument("Tipo de item invalido.");
}

static string codigo_item_menu(int tipo, const string& codigo) {
    if (tipo == 1) return "arma:" + codigo;
    if (tipo == 2) return "armadura:" + codigo;
    if (tipo == 3) return "pocao:" + codigo;
    if (tipo == 4) return "especial:" + codigo;
    throw invalid_argument("Tipo de item invalido.");
}

static void menu_inventario(GerenciadorDeJogo& jogo) {
    Jogador* p = selecionar_personagem(jogo, "Gerenciar inventario de qual personagem?");
    if (!p) return;

    bool voltar = false;
    while (!voltar) {
        cout << "\n--- Inventario de " << p->get_nome() << " ---" << endl;
        cout << "Ouro disponivel: " << p->get_ouro() << endl;
        cout << "1. Ver inventario" << endl;
        cout << "2. Comprar item na loja" << endl;
        cout << "3. Vender/remover item da mochila" << endl;
        cout << "4. Equipar arma" << endl;
        cout << "5. Equipar armadura" << endl;
        cout << "6. Desequipar arma" << endl;
        cout << "7. Desequipar armadura" << endl;
        cout << "8. Usar pocao" << endl;
        cout << "9. Usar item especial" << endl;
        cout << "0. Voltar" << endl;
        int opcao = ler_int("Escolha: ");

        try {
            if (opcao == 1) {
                p->get_inventario().listar();
            } else if (opcao == 2) {
                cout << "\n--- Loja ---" << endl;
                cout << "Armas: espada (70 ouro), cajado (60 ouro), adaga (45 ouro)" << endl;
                cout << "Armaduras: ferro (80 ouro), tela (65 ouro), couro (55 ouro)" << endl;
                cout << "Pocoes: vida (25 ouro), mana (20 ouro), forca (35 ouro)" << endl;
                cout << "Itens especiais: anel (50 ouro), reliquia (100 ouro)" << endl;
                cout << "1. Arma  2. Armadura  3. Pocao  4. Item Especial" << endl;

                int tipo = ler_int_intervalo("Tipo: ", 1, 4);
                string codigo;
                if (tipo == 1) codigo = ler_linha("Qual arma? (espada, cajado, adaga): ");
                else if (tipo == 2) codigo = ler_linha("Qual armadura? (ferro, tela, couro): ");
                else if (tipo == 3) codigo = ler_linha("Qual pocao? (vida, mana, forca): ");
                else codigo = ler_linha("Qual item especial? (anel, reliquia): ");

                string codigo_save = codigo_item_menu(tipo, codigo);
                int preco = ItemFactory::preco_item_por_codigo(codigo_save);

                cout << "Preco: " << preco << " ouro." << endl;
                if (!p->gastar_ouro(preco)) {
                    cout << "Compra cancelada." << endl;
                    continue;
                }

                Item* item = nullptr;
                try {
                    item = criar_item_menu_por_tipo(tipo, codigo);
                    p->get_inventario().adicionar_item(item);
                    cout << "Compra realizada com sucesso." << endl;
                } catch (...) {
                    if (item) delete item;
                    p->ganhar_ouro(preco); // Estorna se falhar por peso/capacidade/catalogo
                    throw;
                }
            } else if (opcao == 3) {
                string nome_item = ler_linha("Nome exato do item: ");
                Item* removido = p->get_inventario().remover_item(nome_item);
                string codigo = ItemFactory::codigo_item(removido);
                int valor_venda = ItemFactory::preco_item_por_codigo(codigo) / 2;
                delete removido;
                p->ganhar_ouro(valor_venda);
                cout << "Item vendido por " << valor_venda << " ouro." << endl;
            } else if (opcao == 4) {
                p->get_inventario().equipar_arma(ler_linha("Nome exato da arma: "));
            } else if (opcao == 5) {
                p->get_inventario().equipar_armadura(ler_linha("Nome exato da armadura: "));
            } else if (opcao == 6) {
                p->get_inventario().desequipar_arma();
            } else if (opcao == 7) {
                p->get_inventario().desequipar_armadura();
            } else if (opcao == 8) {
                p->usar_pocao(ler_linha("Nome exato da pocao: "));
            } else if (opcao == 9) {
                p->get_inventario().usar_item_especial(ler_linha("Nome exato do item especial: "), p);
            } else if (opcao == 0) {
                voltar = true;
            } else {
                cout << "Opcao invalida." << endl;
            }
        } catch (const exception& e) {
            cout << "[ERRO]: " << e.what() << endl;
        }
    }
}

// ---------- Habilidades ----------

static void menu_habilidades(GerenciadorDeJogo& jogo) {
    Jogador* p = selecionar_personagem(jogo, "Gerenciar habilidades de qual personagem?");
    if (!p) return;

    bool voltar = false;
    while (!voltar) {
        cout << "\n--- Habilidades de " << p->get_nome() << " ---" << endl;
        cout << "1. Listar habilidades" << endl;
        cout << "2. Aprender habilidade" << endl;
        cout << "3. Usar habilidade" << endl;
        cout << "0. Voltar" << endl;
        int opcao = ler_int("Escolha: ");

        try {
            if (opcao == 1) {
                p->listar_habilidades();
            } else if (opcao == 2) {
                cout << "1. Ofensiva (golpe, fogo)\n2. Defensiva (postura)\n3. Suporte (cura, regeneracao)" << endl;
                int tipo = ler_int_intervalo("Categoria: ", 1, 3);
                string t = ler_linha("Tipo: ");
                Habilidade* h = nullptr;
                if (tipo == 1)      h = HabilidadeFactory::criar_ofensiva(t);
                else if (tipo == 2) h = HabilidadeFactory::criar_defensiva(t);
                else                h = HabilidadeFactory::criar_suporte(t);
                p->aprender_habilidade(h);
            } else if (opcao == 3) {
                string nome_skill = ler_linha("Nome da habilidade: ");
                Jogador* alvo = selecionar_personagem(jogo, "Usar em qual personagem? (pode ser o mesmo)");
                p->usar_habilidade(nome_skill, alvo);
            } else if (opcao == 0) {
                voltar = true;
            } else {
                cout << "Opcao invalida." << endl;
            }
        } catch (const exception& e) {
            cout << "[ERRO]: " << e.what() << endl;
        }
    }
}

// ---------- RF6: Ganhar XP com redistribuição de pontos ----------

static void menu_ganhar_xp(GerenciadorDeJogo& jogo) {
    Jogador* p = selecionar_personagem(jogo, "Quem vai ganhar XP?");
    if (!p) return;
    int xp = ler_int("Quantos pontos de XP? ");

    int nivel_antes = p->get_nivel();
    p->ganhar_xp(xp);

    if (p->get_nivel() > nivel_antes) {
        cout << "\n" << p->get_nome() << " subiu de nivel! Distribua 10 pontos extras no atributo principal." << endl;
        int pontos = ler_int_intervalo("Pontos a distribuir (0-10): ", 0, 10);
        p->redistribuir_pontos(pontos); // Polimorfismo: cada classe redistribui no seu proprio atributo
    }
}

// ---------- RF5: Batalha (bot vs bot, humano vs bot, humano vs humano) ----------

static void menu_batalha(GerenciadorDeJogo& jogo) {
    if (jogo.get_personagens().size() < 2) {
        cout << "Sao necessarios pelo menos 2 personagens para uma batalha." << endl;
        return;
    }

    cout << "\n--- Iniciar Batalha ---" << endl;
    Jogador* p1 = selecionar_personagem(jogo, "Primeiro personagem");
    if (!p1) return;
    Jogador* p2 = selecionar_personagem(jogo, "Segundo personagem");
    if (!p2) return;
    if (p1 == p2) { cout << "Escolha dois personagens diferentes." << endl; return; }

    cout << "\n1. Bot vs Bot" << endl;
    cout << "2. Humano vs Bot (voce controla o primeiro)" << endl;
    cout << "3. Humano vs Humano" << endl;
    int modo = ler_int_intervalo("Modo: ", 1, 3);

    vector<Jogador*> participantes = {p1, p2};
    Batalha batalha(participantes);

    if (modo == 2) {
        batalha.adicionar_jogador_humano(p1);
    } else if (modo == 3) {
        batalha.adicionar_jogador_humano(p1); // RF5: humano vs humano -> os dois sao marcados
        batalha.adicionar_jogador_humano(p2);
    }

    batalha.iniciar(20);
}

// ---------- RF7: Persistência ----------

static void menu_salvar(GerenciadorDeJogo& jogo) {
    string caminho = ler_linha("Nome do arquivo para salvar (ex: meu_save.txt): ");
    try {
        Persistencia::salvar(jogo.get_personagens(), caminho);
    } catch (const PersistenciaError& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }
}

static void menu_carregar(GerenciadorDeJogo& jogo) {
    string caminho = ler_linha("Nome do arquivo para carregar (ex: meu_save.txt): ");
    try {
        vector<Jogador*> carregados = Persistencia::carregar(caminho);
        for (Jogador* p : carregados) {
            p->adicionar_observador(&g_notificador);
            jogo.get_personagens().push_back(p); // Os personagens carregados passam a pertencer ao Gerenciador
        }
        cout << carregados.size() << " personagem(ns) adicionados ao jogo atual." << endl;
    } catch (const PersistenciaError& e) {
        cout << "[ERRO]: " << e.what() << endl;
    }
}

// ---------- Loop principal (RF8) ----------

void executar_menu_interativo() {
    GerenciadorDeJogo& jogo = GerenciadorDeJogo::instance();

    cout << "==================================" << endl;
    cout << "          CODEQUEST: A LENDA DO KERNEL" << endl;
    cout << "==================================" << endl;

    bool rodando = true;
    try {
        while (rodando) {
            cout << "\n===== MENU PRINCIPAL =====" << endl;
            cout << "1.  Criar personagem" << endl;
            cout << "2.  Listar personagens" << endl;
            cout << "3.  Detalhar personagem" << endl;
            cout << "4.  Gerenciar inventario" << endl;
            cout << "5.  Gerenciar habilidades" << endl;
            cout << "6.  Ganhar XP / level up" << endl;
            cout << "7.  Iniciar batalha" << endl;
            cout << "8.  Salvar jogo" << endl;
            cout << "9.  Carregar jogo" << endl;
            cout << "10. Remover personagem" << endl;
            cout << "0.  Sair" << endl;
            int opcao = ler_int("Escolha: ");

            try {
                switch (opcao) {
                    case 1:  menu_criar_personagem(jogo);    break;
                    case 2:  jogo.listar_personagens();       break;
                    case 3:  menu_detalhar_personagem(jogo);  break;
                    case 4:  menu_inventario(jogo);           break;
                    case 5:  menu_habilidades(jogo);          break;
                    case 6:  menu_ganhar_xp(jogo);            break;
                    case 7:  menu_batalha(jogo);               break;
                    case 8:  menu_salvar(jogo);                break;
                    case 9:  menu_carregar(jogo);               break;
                    case 10: {
                        string nome = ler_linha("Nome do personagem a remover: ");
                        jogo.remover_personagem(nome);
                        break;
                    }
                    case 0:  rodando = false; cout << "\nAte a proxima!" << endl; break;
                    default: cout << "Opcao invalida." << endl;
                }
            } catch (const EntradaFinalizadaException&) {
                throw; // Repassa para o catch externo encerrar o programa de forma limpa
            } catch (const exception& e) {
                cout << "[ERRO]: " << e.what() << endl;
            }
        }
    } catch (const EntradaFinalizadaException&) {
        // Entrada padrao acabou (ex: EOF de um arquivo redirecionado, ou Ctrl+D no terminal)
        cout << "\nEntrada finalizada. Encerrando o programa." << endl;
    }

    GerenciadorDeJogo::destruir_instancia(); // Libera todos os personagens criados nesta sessao
}

} // namespace rpg
