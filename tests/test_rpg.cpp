// Testes unitarios com Google Test, cobrindo os principais conceitos de POO
// e regras de negocio do sistema: encapsulamento, heranca, polimorfismo,
// classes abstratas, excecoes, padroes de projeto e persistencia.

#include <gtest/gtest.h>
#include "personagens.h"
#include "factories.h"
#include "inventario.h"
#include "habilidade.h"
#include "estrategia.h"
#include "batalha.h"
#include "persistencia.h"
#include "observer.h"
#include "gerenciador.h"
#include <cstdio> // remove() para limpar arquivos de teste

using namespace rpg;

// =============================================================
// ENCAPSULAMENTO
// =============================================================

TEST(Encapsulamento, HpNaoPodePassarDoMaximo) {
    Guerreiro g("Teste", 30);
    g.curar(9999); // Tenta curar muito acima do maximo
    EXPECT_EQ(g.get_hp_atual(), 100); // SETTER deve travar no hp_max, sem ultrapassar
}

TEST(Encapsulamento, HpNaoFicaNegativo) {
    Guerreiro g("Teste", 30);
    g.receber_dano(99999); // Dano absurdamente alto
    EXPECT_EQ(g.get_hp_atual(), 0); // Nunca deve ficar negativo
}

TEST(Encapsulamento, GanharXpAcumulaCorretamente) {
    Guerreiro g("Teste", 30);
    g.ganhar_xp(50);
    EXPECT_EQ(g.get_xp(), 50);
    EXPECT_EQ(g.get_nivel(), 1); // Ainda nao deve subir de nivel
}


// =============================================================
// HERANÇA E POLIMORFISMO
// =============================================================

TEST(Polimorfismo, CadaClasseTemDanoDiferente) {
    Guerreiro guerreiro("G", 30);
    Mago      mago("M", 25, 30);
    Monstro    monstro("L", 20, 0.0f); // 0% de critico para resultado deterministico

    EXPECT_EQ(guerreiro.atacar(), 45); // 30 * 1.5
    EXPECT_EQ(mago.atacar(), 50);      // 25 * 2.0 (com mana suficiente)
    EXPECT_EQ(monstro.atacar(), 20);    // Sem critico, dano = ferocidade
}

TEST(Polimorfismo, SubirNivelAumentaHpMaximo) {
    Guerreiro g("Teste", 30);
    g.ganhar_xp(100); // Exatamente o suficiente para subir de nivel
    EXPECT_EQ(g.get_nivel(), 2);
    EXPECT_EQ(g.get_hp_max(), 150); // 100 + 50
    EXPECT_EQ(g.get_hp_atual(), 150); // HP restaurado ao subir de nivel
}

TEST(Polimorfismo, ColecaoHeterogeneaChamaMetodoCorreto) {
    // Lista do tipo base armazenando subclasses diferentes (Coleção Heterogênea)
    Guerreiro g("G", 30);
    Mago      m("M", 25, 30);
    vector<Jogador*> grupo = {&g, &m};

    int total = 0;
    for (Jogador* p : grupo) total += p->atacar(); // Polimorfismo dinamico
    EXPECT_GT(total, 0);
}


// =============================================================
// CLASSE ABSTRATA
// =============================================================

TEST(ClasseAbstrata, MagoSemManaCausaApenasDanoBase) {
    Mago m("M", 25, 5); // So 5 de mana, insuficiente para o custo de 10
    int dano = m.atacar();
    EXPECT_EQ(dano, 5); // Dano base minimo quando sem mana
}


// =============================================================
// EXCEÇÕES CUSTOMIZADAS
// =============================================================

TEST(Excecoes, ItemFactoryLancaExcecaoParaTipoInvalido) {
    EXPECT_THROW(ItemFactory::criar_arma("inexistente"), invalid_argument);
}

TEST(Excecoes, HabilidadeFactoryLancaExcecaoParaTipoInvalido) {
    EXPECT_THROW(HabilidadeFactory::criar_ofensiva("inexistente"), invalid_argument);
    EXPECT_THROW(HabilidadeFactory::criar_defensiva("inexistente"), invalid_argument);
    EXPECT_THROW(HabilidadeFactory::criar_suporte("inexistente"), invalid_argument);
}

TEST(Excecoes, InventarioLancaExcecaoQuandoItemNaoExiste) {
    Guerreiro g("G", 30);
    EXPECT_THROW(g.get_inventario().equipar_arma("Nao Existe"), ItemNaoEncontradoError);
}

TEST(Excecoes, InventarioLancaExcecaoQuandoMochilaEstaCheia) {
    Guerreiro g("G", 30);
    // Capacidade padrao e 10; adiciona 10 pocoes leves (nao excede o peso)
    for (int i = 0; i < 10; i++) {
        g.get_inventario().adicionar_item(ItemFactory::criar_pocao("mana"));
    }
    EXPECT_THROW(g.get_inventario().adicionar_item(ItemFactory::criar_pocao("mana")), InventarioCheioError);
}

TEST(Excecoes, HabilidadeNaoAprendidaLancaExcecao) {
    Guerreiro g("G", 30);
    EXPECT_THROW(g.usar_habilidade("Bola de Fogo"), HabilidadeNaoEncontradaError);
}

TEST(Excecoes, HabilidadePassivaNaoPodeSerUsadaManualmente) {
    Mago m("M", 25, 30);
    m.aprender_habilidade(HabilidadeFactory::criar_suporte("regeneracao"));
    EXPECT_THROW(m.usar_habilidade("Regeneracao"), HabilidadePassivaError);
}

TEST(Excecoes, PersistenciaLancaExcecaoParaArquivoInexistente) {
    EXPECT_THROW(Persistencia::carregar("arquivo_que_nao_existe_12345.txt"), PersistenciaError);
}


// =============================================================
// INVENTÁRIO (composição, RF3, RF4)
// =============================================================

TEST(Inventario, EquiparArmaDaBonusDeAtaque) {
    Guerreiro g("G", 30);
    g.get_inventario().adicionar_item(ItemFactory::criar_arma("espada")); // dano 40
    g.get_inventario().equipar_arma("Espada de Aco");
    EXPECT_EQ(g.get_bonus_ataque(), 40);
}

TEST(Inventario, DesequiparRemoveOBonus) {
    Guerreiro g("G", 30);
    g.get_inventario().adicionar_item(ItemFactory::criar_arma("espada"));
    g.get_inventario().equipar_arma("Espada de Aco");
    g.get_inventario().desequipar_arma();
    EXPECT_EQ(g.get_bonus_ataque(), 0);
}

TEST(Inventario, ArmaduraReduzDanoRecebido) {
    Guerreiro g("G", 30);
    g.get_inventario().adicionar_item(ItemFactory::criar_armadura("ferro")); // defesa 30
    g.get_inventario().equipar_armadura("Peitoral de Ferro");
    g.receber_dano(50);
    EXPECT_EQ(g.get_hp_atual(), 80); // 100 - (50 - 30)
}

TEST(Inventario, CuraQueExcedeOMaximoECancelada) {
    Guerreiro g("G", 30);
    g.receber_dano(40); // HP: 60/100
    g.get_inventario().adicionar_item(ItemFactory::criar_pocao("vida")); // cura 50
    g.usar_pocao("Pocao de Vida"); // 60 + 50 = 110 > 100 -> curar() cancela a acao
    EXPECT_EQ(g.get_hp_atual(), 60); // Permanece 60: a cura e cancelada, nao truncada
}

TEST(Inventario, UsarPocaoDeVidaCuraOPersonagem) {
    Guerreiro g("G", 30);
    g.receber_dano(70); // HP: 30/100
    g.get_inventario().adicionar_item(ItemFactory::criar_pocao("vida")); // cura 50
    g.usar_pocao("Pocao de Vida"); // 30 + 50 = 80, dentro do maximo
    EXPECT_EQ(g.get_hp_atual(), 80);
}

TEST(Inventario, PesoExcedidoLancaExcecao) {
    Guerreiro g("G", 30);
    // Peso maximo padrao e 50kg; cada Peitoral de Ferro pesa 15kg -> 4 unidades excedem o limite
    EXPECT_THROW({
        for (int i = 0; i < 10; i++) {
            g.get_inventario().adicionar_item(ItemFactory::criar_armadura("ferro"));
        }
    }, PesoExcedidoError);
}

TEST(Inventario, DesequiparArmaduraRemoveOBonus) {
    Guerreiro g("G", 30);
    g.get_inventario().adicionar_item(ItemFactory::criar_armadura("ferro"));
    g.get_inventario().equipar_armadura("Peitoral de Ferro");
    EXPECT_EQ(g.get_bonus_defesa(), 30);
    g.get_inventario().desequipar_armadura();
    EXPECT_EQ(g.get_bonus_defesa(), 0);
}

TEST(Inventario, EquiparItemQueNaoEArmaLancaExcecao) {
    Guerreiro g("G", 30);
    g.get_inventario().adicionar_item(ItemFactory::criar_pocao("vida"));
    EXPECT_THROW(g.get_inventario().equipar_arma("Pocao de Vida"), ItemNaoEncontradoError);
}


// =============================================================
// PADRÃO STRATEGY
// =============================================================

TEST(Strategy, RoundRobinEscolhePrimeiroDaLista) {
    Guerreiro a("A", 30), b("B", 30), c("C", 30);
    vector<Jogador*> grupo = {&a, &b, &c};
    Batalha batalha(grupo); // Estrategia padrao: AlvoRoundRobin
    EXPECT_EQ(batalha.escolher_alvo(&a)->get_nome(), "B");
}

TEST(Strategy, AlvoMaisFracoEscolheMenorHp) {
    Guerreiro a("A", 30), b("B", 30), c("C", 30);
    c.receber_dano(90); // C fica com 10 de HP, o mais fraco
    vector<Jogador*> grupo = {&a, &b, &c};
    Batalha batalha(grupo);
    batalha.definir_estrategia(new AlvoMaisFraco()); // Troca o algoritmo em runtime
    EXPECT_EQ(batalha.escolher_alvo(&a)->get_nome(), "C");
}

TEST(Strategy, AlvoMaisForteEscolheMaiorHp) {
    Guerreiro a("A", 30), b("B", 30), c("C", 30);
    b.receber_dano(90); // B fica fraco; C continua o mais forte (mesmo HP que A, mas vem depois na lista)
    vector<Jogador*> grupo = {&a, &b, &c};
    Batalha batalha(grupo);
    batalha.definir_estrategia(new AlvoMaisForte());
    EXPECT_EQ(batalha.escolher_alvo(&a)->get_nome(), "C");
}


// =============================================================
// PADRÃO OBSERVER
// =============================================================

class ContadorDeEventos : public ObservadorDeEventos {
public:
    int total = 0;
    void notificar(string evento) override { total++; }
};

TEST(Observer, NotificaAoSubirDeNivel) {
    Guerreiro g("G", 30);
    ContadorDeEventos contador;
    g.adicionar_observador(&contador);
    g.ganhar_xp(100); // Sobe de nivel
    EXPECT_EQ(contador.total, 1);
}

TEST(Observer, NotificaAoSerDerrotado) {
    Guerreiro g("G", 30);
    ContadorDeEventos contador;
    g.adicionar_observador(&contador);
    g.receber_dano(200); // Derrota o personagem
    EXPECT_EQ(contador.total, 1);
}


// =============================================================
// FACTORY (JogadorFactory)
// =============================================================

TEST(Factory, JogadorFactoryCriaComAtributosPadrao) {
    Guerreiro* g = JogadorFactory::criar_guerreiro("G", "Humano");
    Mago*      m = JogadorFactory::criar_mago("M", "Humano");
    Monstro*    l = JogadorFactory::criar_monstro("L", "Humano");
    EXPECT_EQ(g->get_forca(), 30);
    EXPECT_EQ(m->get_inteligencia(), 25);
    EXPECT_EQ(l->get_ferocidade(), 20);
    delete g; delete m; delete l;
}


// =============================================================
// PADRÃO SINGLETON (GerenciadorDeJogo)
// =============================================================

TEST(Singleton, InstanceRetornaSempreAMesmaReferencia) {
    GerenciadorDeJogo::destruir_instancia(); // Garante estado limpo antes do teste
    GerenciadorDeJogo& a = GerenciadorDeJogo::instance();
    GerenciadorDeJogo& b = GerenciadorDeJogo::instance();
    EXPECT_EQ(&a, &b); // Mesma instancia (Singleton)
    GerenciadorDeJogo::destruir_instancia();
}

TEST(Singleton, CriarPersonagemAdicionaNaLista) {
    GerenciadorDeJogo::destruir_instancia();
    GerenciadorDeJogo& jogo = GerenciadorDeJogo::instance();
    jogo.criar_guerreiro("Kain", "Humano", 30);
    jogo.criar_mago("Dumbledore", "Humano", 25, 30);
    EXPECT_EQ(jogo.get_personagens().size(), 2u);
    GerenciadorDeJogo::destruir_instancia();
}

TEST(Singleton, BuscarPorNomeEncontraOuLancaExcecao) {
    GerenciadorDeJogo::destruir_instancia();
    GerenciadorDeJogo& jogo = GerenciadorDeJogo::instance();
    jogo.criar_monstro("Gorath", "Orc", 20, 0.4f);

    Jogador* encontrado = jogo.buscar_por_nome("Gorath");
    EXPECT_EQ(encontrado->get_nome(), "Gorath");
    EXPECT_THROW(jogo.buscar_por_nome("NaoExiste"), PersonagemNaoEncontradoError);
    GerenciadorDeJogo::destruir_instancia();
}

TEST(Singleton, RemoverPersonagemFuncionaELancaExcecaoSeNaoExistir) {
    GerenciadorDeJogo::destruir_instancia();
    GerenciadorDeJogo& jogo = GerenciadorDeJogo::instance();
    jogo.criar_guerreiro("Kain", "Humano", 30);

    jogo.remover_personagem("Kain");
    EXPECT_EQ(jogo.get_personagens().size(), 0u);
    EXPECT_THROW(jogo.remover_personagem("Kain"), PersonagemNaoEncontradoError);
    GerenciadorDeJogo::destruir_instancia();
}

TEST(Singleton, DestruirInstanciaPermiteRecomecarDoZero) {
    GerenciadorDeJogo::destruir_instancia();
    GerenciadorDeJogo::instance().criar_guerreiro("Temp", "Humano", 30);
    GerenciadorDeJogo::destruir_instancia(); // Libera o personagem e zera a instancia

    GerenciadorDeJogo& nova = GerenciadorDeJogo::instance();
    EXPECT_EQ(nova.get_personagens().size(), 0u); // Instancia nova, lista vazia
    GerenciadorDeJogo::destruir_instancia();
}


// =============================================================
// REDISTRIBUIÇÃO DE PONTOS (RF6) E OBSERVER (remover_observador)
// =============================================================

TEST(RedistribuicaoDePontos, GuerreiroRedistribuiNaForca) {
    Guerreiro g("G", 30);
    g.redistribuir_pontos(5);
    EXPECT_EQ(g.atacar(), static_cast<int>(35 * 1.5)); // Forca 30+5=35
}

TEST(RedistribuicaoDePontos, MagoRedistribuiNaInteligencia) {
    Mago m("M", 25, 30);
    m.redistribuir_pontos(5);
    EXPECT_EQ(m.get_inteligencia(), 30);
}

TEST(Observer, RemoverObservadorParaDeReceberNotificacoes) {
    Guerreiro g("G", 30);
    ContadorDeEventos contador;
    g.adicionar_observador(&contador);
    g.remover_observador(&contador);
    g.receber_dano(200); // Derrota o personagem, mas o observador ja foi removido
    EXPECT_EQ(contador.total, 0);
}

TEST(Operadores, IgualdadeComparaPorNome) {
    Guerreiro a("Kain", 30), b("Kain", 30), c("Gorath", 30);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(Operadores, MenorQueComparaPorHp) {
    Guerreiro a("A", 30), b("B", 30);
    b.receber_dano(50);
    EXPECT_TRUE(b < a);
}

TEST(Operadores, SomaDeArmasCombinaDanoEPeso) {
    Arma* espada = ItemFactory::criar_arma("espada"); // dano 40, peso 5
    Arma* adaga  = ItemFactory::criar_arma("adaga");   // dano 25, peso 2
    Arma combinada = *espada + *adaga;
    EXPECT_EQ(combinada.get_dano(), 65);
    delete espada;
    delete adaga;
}


// =============================================================
// PERSISTÊNCIA
// =============================================================

TEST(Persistencia, SalvarECarregarPreservaOEstado) {
    string caminho = "teste_save_temp.txt";
    Guerreiro g("Kain", 30);
    g.ganhar_xp(100); // Sobe para nivel 2
    g.receber_dano(20);

    vector<Jogador*> grupo = {&g};
    Persistencia::salvar(grupo, caminho);

    vector<Jogador*> carregado = Persistencia::carregar(caminho);
    ASSERT_EQ(carregado.size(), 1u);
    EXPECT_EQ(carregado[0]->get_nome(), "Kain");
    EXPECT_EQ(carregado[0]->get_nivel(), 2);
    EXPECT_EQ(carregado[0]->get_hp_atual(), 130); // 150 - 20

    for (Jogador* p : carregado) delete p;
    remove(caminho.c_str()); // Limpa o arquivo de teste
}

TEST(Racas, AnaoRecebeBonusDeHpInicial) {
    Guerreiro g("Gimli", 30, "Anao");
    EXPECT_EQ(g.get_raca(), "Anao");
    EXPECT_EQ(g.get_hp_max(), 125);
    EXPECT_EQ(g.get_hp_atual(), 125);
}

TEST(Persistencia, SalvarECarregarPreservaInventarioEHabilidades) {
    string caminho = "teste_save_completo_temp.txt";
    Guerreiro g("Kain", 30, "Humano");
    g.get_inventario().adicionar_item(ItemFactory::criar_arma("espada"));
    g.get_inventario().equipar_arma("Espada de Aco");
    g.get_inventario().adicionar_item(ItemFactory::criar_pocao("vida"));
    g.aprender_habilidade(HabilidadeFactory::criar_ofensiva("golpe"));

    vector<Jogador*> grupo = {&g};
    Persistencia::salvar(grupo, caminho);

    vector<Jogador*> carregado = Persistencia::carregar(caminho);
    ASSERT_EQ(carregado.size(), 1u);
    EXPECT_EQ(carregado[0]->get_bonus_ataque(), 40);

    Guerreiro alvo("Alvo", 30);
    EXPECT_NO_THROW(carregado[0]->usar_habilidade("Golpe Poderoso", &alvo));

    carregado[0]->receber_dano(50);
    EXPECT_NO_THROW(carregado[0]->usar_pocao("Pocao de Vida"));
    EXPECT_EQ(carregado[0]->get_hp_atual(), carregado[0]->get_hp_max());

    for (Jogador* p : carregado) delete p;
    remove(caminho.c_str());
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
