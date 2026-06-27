#ifndef RPG_MENU_H
#define RPG_MENU_H

namespace rpg {

/**
 * @brief RF8: ponto de entrada do menu interativo em linha de comando.
 * Cobre todas as operações funcionais do sistema: criação de personagens (RF1),
 * listagem (RF2), inventário (RF3/RF4), batalha (RF5), level up com redistribuição
 * de pontos (RF6) e persistência (RF7).
 */
void executar_menu_interativo();

} // namespace rpg

#endif // RPG_MENU_H
