// src/ui/gameInstructions.cpp
#include "../include/UIControl.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/AsciiLegend.hpp"
#include "../../include/KeyBindings.hpp"
#include "../../include/Config.hpp"

#include <iostream>

namespace UIControl {
    using namespace std;

    void showGameInstructions() {
        UICommon::clearScreen();
        cout << "\n╔══════════════════════════════════════════════════════════════════════════╗\n"
        "║   Guía del Astronauta:                                                   ║\n"
        "║   ---------------------------------------------------------------------- ║\n"
        "║   Su objetivo es evitar colisionar con los asteroides y destruirlos.     ║\n"
        "║   Se encontrarán con 3 tipos de asteroides que reaccionarán de forma     ║\n"
        "║   distina cuando un proyectil impacte en estos:                          ║\n"
        "║                                                                          ║\n"
        "║             " << ascii::BigAst << " : Grandes -> se dividen en 2 medianos                      ║\n"
        "║             " << ascii::MedAst << " : mediandos -> se dividen en 2 medianos                    ║\n"
        "║             " << ascii::SmallAst << " : pequeños -> explotan (+10pts)                            ║\n"
        "║                                                                          ║\n"
        "║    El astronauta pierde cuando ya no tiene vidas (solo tendrán           ║\n"
        "║    3 cada uno).                                                          ║\n"
        "║                                                                          ║\n"
        "║   El juego original no tiene fin a menos que pierdas pero en esta        ║\n"
        "║   versión se puede ganar, la limitación depende de cuantos jugadores     ║\n"
        "║   jueguen:                                                               ║\n"
        "║                 Solitario (1 jugador): obteniendo 60pts                  ║\n"
        "║                   Duo (2 jugadores): obteniendo 100pts                   ║\n"
        "║                                                                          ║\n"
        "║   ¡Tengan cuidado al avanzar!                                            ║\n"
        "║   La nave conservará la magnitud y dirección de la velocidad en la que   ║\n"
        "║   muevan sus naves. Cuanto más tiempo mantengan precionadas las teclas   ║\n"
        "║   de movimiento más acelerará la nave hacia esa dirección. Unicamente    ║\n"
        "║   el jugador 1 puede desacelerar.                                        ║\n"
        "║                                                                          ║\n"
        "║   Nota: si se topan con un extremo de la ventana, la nave aparecerá del  ║\n"
        "║   lado opuesto.                                                          ║\n"
        "║                                                                          ║\n"
        "║                    Así se verán sus naves: " << ascii::ShipLeft << " " << ascii::ShipUp << " " << ascii::ShipRight << " " << ascii::ShipDown 
        << "                       ║\n"
        "║                 Y estas serán sus balas (proyectiles): " << ascii::Bullet << "                 ║\n"
        "╚══════════════════════════════════════════════════════════════════════════╝\n\n";

        UICommon::waitEnter();
    }
}