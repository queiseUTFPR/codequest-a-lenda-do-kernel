# CodeQuest: A Lenda do Kernel

Sistema de gerenciamento de jogo de RPG em C++17, desenvolvido para a disciplina de
Programação Orientada a Objetos (UTFPR — Engenharia de Computação).

## Visão Geral

O sistema simula um jogo de RPG clássico em linha de comando: criação de personagens,
gerenciamento de inventário, sistema de habilidades, batalhas por turnos e persistência
do estado do jogo em arquivo.

Todo o código está organizado dentro do `namespace rpg`, separado em módulos (`.h`/`.cpp`)
por responsabilidade.

## Estrutura do Projeto

```
rpg_manager_cpp/
├── CMakeLists.txt        # Configuração de build (biblioteca + executável + testes)
├── Doxyfile              # Configuração do Doxygen (gera docs/doxygen/html)
├── README.md
├── docs/
│   ├── diagrama_classes.dot    # Diagrama de classes UML (fonte Graphviz)
│   ├── diagrama_classes.png
│   ├── diagrama_classes.svg
│   ├── doxygen/html/           # Documentação gerada via `doxygen Doxyfile`
│   └── coverage/                # Relatório de cobertura gerado via lcov/genhtml
├── src/
│   ├── item.h / item.cpp                 # Item, Arma, Armadura, Pocao, ItemEspecial + produtos concretos
│   ├── inventario.h / inventario.cpp     # Inventario (composição) + exceções
│   ├── habilidade.h / habilidade.cpp      # Habilidade (abstrata) + skills concretas + exceções
│   ├── observer.h                         # Padrão Observer (ObservadorDeEventos, NotificadorConsole)
│   ├── raca.h / raca.cpp                 # Raca abstrata + Humano, Elfo, Anao e Orc
│   ├── jogador.h / jogador.cpp           # Jogador (classe abstrata base)
│   ├── personagens.h / personagens.cpp   # Guerreiro, Mago, Monstro, Arqueiro e Clerigo
│   ├── estrategia.h / estrategia.cpp     # Padrão Strategy (escolha de alvo em batalha)
│   ├── factories.h / factories.cpp       # ItemFactory, JogadorFactory, HabilidadeFactory
│   ├── gerenciador.h / gerenciador.cpp   # Padrão Singleton: GerenciadorDeJogo (RF1/RF2)
│   ├── persistencia.h / persistencia.cpp # Salvar/carregar o estado do jogo em arquivo
│   ├── batalha.h / batalha.cpp           # Sistema de combate por turnos
│   ├── menu.h / menu.cpp                  # RF8: menu interativo em linha de comando
│   └── main.cpp                          # Ponto de entrada (menu interativo ou --demo)
└── tests/
    └── test_rpg.cpp        # Testes unitários (Google Test)
```

## Como Usar o Programa

```bash
./build/codequest          # Abre o menu interativo (RF8) — cobre todas as operações
./build/codequest --demo   # Roteiro de demonstração automática, sem precisar digitar nada
```

O menu interativo cobre: criação de personagens com distribuição de pontos (RF1),
listagem/detalhamento (RF2), inventário completo — comprar com ouro/vender/equipar/desequipar/usar
itens (RF3/RF4), batalha por turnos em três modos — bot vs bot, humano vs bot, humano vs
humano (RF5), ganho de XP com redistribuição de pontos no level up (RF6), e salvar/carregar
o jogo em arquivo (RF7).

## Conceitos de POO Aplicados

| Conceito | Onde aparece |
|---|---|
| Encapsulamento | Atributos privados em todas as classes, com getters e setters validados |
| Herança | `Guerreiro`/`Mago`/`Monstro`/`Arqueiro`/`Clerigo` ← `Jogador`; `Humano`/`Elfo`/`Anao`/`Orc` ← `Raca`; `Arma`/`Armadura`/`Pocao`/`ItemEspecial` ← `Item` |
| Polimorfismo | `atacar()`, `exibirStatus()`, `serializar()`, `efeito()`, `escolher()` |
| Classes Abstratas | `Jogador`, `Item`, `Habilidade`, `EstrategiaDeAlvo`, `ObservadorDeEventos` |
| Composição | `Jogador` possui `Inventario` e `Raca`; `Inventario` possui `vector<Item*>` |
| Tratamento de Exceções | Exceções customizadas para cada regra de negócio violada |
| Sobrecarga de Operadores | `operator<<`, `operator==`, `operator<` (`Jogador`); `operator+` (`Arma`) |

## Padrões de Projeto

1. **Factory** — `ItemFactory`, `JogadorFactory`, `HabilidadeFactory` centralizam a criação de objetos.
2. **Strategy** — `EstrategiaDeAlvo` permite trocar o algoritmo de escolha de alvo da `Batalha`
   em tempo de execução, sem alterar a classe `Batalha`.
3. **Observer** — `Jogador` notifica `ObservadorDeEventos` registrados quando sobe de nível
   ou é derrotado, sem precisar saber quem está observando.
4. **Singleton** — `GerenciadorDeJogo` garante uma única instância responsável por toda a
   coleção de personagens criados, com acesso global via `GerenciadorDeJogo::instance()`.

## Como Compilar

Requisitos: CMake ≥ 3.10, compilador C++17, e (opcional) Google Test para os testes unitários.

```bash
mkdir build && cd build
cmake ..
make
```

Isso gera dois executáveis:
- `codequest` — o programa de demonstração (`main.cpp`)
- `rpg_tests` — os testes unitários (só é gerado se o Google Test estiver instalado)

## Como Executar

```bash
./build/codequest
```

O programa demonstra, em sequência: criação de personagens, raças concretas, moeda de ouro, inventário
(comprar/vender/equipar/desequipar/usar itens), habilidades (ativas e passivas), uma batalha por turnos
completa, o padrão Strategy, sobrecarga de operadores, e persistência completa
(salvar/carregar o estado em `save_rpg.txt`).

## Documentação Gerada (Doxygen)

Todas as classes possuem comentários no formato Doxygen (`/** @brief ... */`). Para gerar a
documentação HTML navegável:

```bash
doxygen Doxyfile
# Abra docs/doxygen/html/index.html no navegador
```

## Cobertura de Testes

Medida com `gcov`/`lcov`. **Somente com a suíte de testes unitários automatizados** (sem contar
execução manual do menu ou do `--demo`):

```bash
mkdir build_coverage && cd build_coverage
cmake .. -DCMAKE_CXX_FLAGS="--coverage -O0" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
make rpg_tests && ./rpg_tests
lcov --capture --directory . --output-file coverage.info --rc branch_coverage=0
lcov --remove coverage.info '*/tests/*' '/usr/*' --output-file coverage_unit.info --rc branch_coverage=0
lcov --summary coverage_unit.info --rc branch_coverage=0
```

Resultado: **62.4% de linhas cobertas** (396/635) e **70.8% de funções cobertas** (119/168),
acima do mínimo de 60% exigido. O relatório HTML está em `docs/coverage/index.html`.

## Como Rodar os Testes

```bash
cd build
ctest --output-on-failure
# ou diretamente:
./rpg_tests
```

Os testes cobrem: encapsulamento, herança/polimorfismo, classes abstratas, exceções
customizadas, inventário, os 4 padrões de projeto (Factory, Strategy, Observer, Singleton),
sobrecarga de operadores, redistribuição de pontos no level up e persistência (round-trip
de salvar/carregar).

## Ajustes de Compatibilidade com a Especificação

- A persistência agora salva e carrega o estado principal do personagem, os atributos específicos
  da subclasse, o inventário completo (mochila, arma equipada e armadura equipada) e as
  habilidades aprendidas e a quantidade de ouro de cada personagem.
- Foram adicionadas as classes concretas `Arqueiro` e `Clerigo`, além das raças concretas
  `Humano`, `Elfo`, `Anao` e `Orc`, modeladas por herança a partir de `Raca`.
- O save mantém compatibilidade básica com o formato antigo dos personagens já existentes.
- Foi adicionado `ItemEspecial` (`Anel da Fortuna` e `Reliquia Antiga`) para cobrir a hierarquia completa de itens.
- Foi adicionada a moeda `ouro`: cada personagem inicia com 100 moedas, compra itens pela loja, vende itens por metade do preço e recebe ouro como recompensa de batalha.

## Limitações Conhecidas

- A `Batalha` usa alvo único por estratégia (sem suporte a múltiplos alvos simultâneos).
- A interface é por linha de comando (CLI); a GUI permanece como extensão opcional/bônus da especificação.
