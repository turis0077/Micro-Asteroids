// src/ui/gameInstructions.cpp
#include <iostream>

using namespace std;

void showInstructions() {
    cout << "╔══════════════════════════════════════════════════════════════════════════╗\n"
"║   Guia del Astronauta:                                                   ║\n"
"║   ---------------------------------------------------------------------- ║\n"
"║   Su objetivo es evitar colisionar con los asteroides y destruirlos.     ║\n"
"║   Se encontrarán con 3 tipos de asteroides que reaccionarán de forma     ║\n"
"║   distina cuando un proyectil impacte en estos:                          ║\n"
"║                                                                          ║\n"
"║             @ : Grandes -> se dividen en 2 medianos                      ║\n"
"║             0 : mediandos -> se dividen en 2 medianos                    ║\n"
"║             o : pequeños -> explotan (+10pts)                            ║\n"
"║                                                                          ║\n"
"║    El astronauta pierde cuando ya no tiene vidas (solo tendrán           ║\n"
"║    3 cada uno).                                                          ║\n"
"║                                                                          ║\n"
"║   El juego original no tiene fin a menos que pierdas pero en esta        ║\n"
"║   versión se puede ganar, la limitación depende de cuantos jugadores     ║\n"
"║   jueguen:                                                               ║\n"
"║                    1 jugador: obteniendo 60pts                           ║\n"
"║                    2 jugadores: obteniendo 100pts                        ║\n"
"║                                                                          ║\n"
"║   ¡Tengan cuidado al avanzar!                                            ║\n"
"║   La nave conservará la magnitud y dirección de la velocidad en la que   ║\n"
"║   muevan sus naves. Cuanto más tiempo mantengan precionadas las teclas   ║\n"
"║   de movimiento más acelerará la nave hacia esa dirección a menos de     ║\n"
"║   que se muevan a la dirección opuesta.                                  ║\n"
"║                                                                          ║\n"
"║   Nota: si se topan con un extremo de la ventana, la nave aparecerá del  ║\n"
"║   lado opuesto.                                                          ║\n"
"║                                                                          ║\n"
"║                    Así se verán sus naves: < ^ > v                       ║\n"
"║                 Y estas serán sus balas (proyectiles): .                 ║\n"
"╚══════════════════════════════════════════════════════════════════════════╝\n"
}