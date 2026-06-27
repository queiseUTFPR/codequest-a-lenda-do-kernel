#ifndef RPG_OBSERVER_H
#define RPG_OBSERVER_H

#include <string>
#include <iostream>
using namespace std;

namespace rpg {

// =============================================================
// PADRÃO OBSERVER
// Permite que outras partes do sistema sejam avisadas de eventos do Jogador
// (level up, derrota) sem que o Jogador precise saber quem esta observando.
// Header-only: as classes sao pequenas o suficiente para nao precisar de .cpp
// =============================================================

/**
 * @class ObservadorDeEventos
 * @brief Padrão Observer: contrato para quem deseja ser notificado de eventos de um Jogador.
 */
class ObservadorDeEventos {
public:
    virtual ~ObservadorDeEventos() {}

    // Método Abstrato: cada observador concreto decide o que fazer com o evento recebido
    virtual void notificar(string evento) = 0;
};


/** @brief Observador concreto que imprime os eventos recebidos no console. */
class NotificadorConsole : public ObservadorDeEventos {
public:
    void notificar(string evento) override {
        cout << "[NOTIFICACAO] " << evento << endl;
    }
};

} // namespace rpg

#endif // RPG_OBSERVER_H
