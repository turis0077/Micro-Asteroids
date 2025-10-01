// ui/controlsGuide.cpp
#include "../../include/UIControl.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/KeyBindings.hpp"

namespace UIControl {
     void showControlsGuide() {
          using namespace std;
          UICommon::clearScreen();

          cout << "\nControles del juego: \n";
          cout << "Jugador 1:\n | " 
               << KeyBindingsP1::thrust << ": acelerar\n | "
               << KeyBindingsP1::left << ": girar a la izquierda\n | "
               << KeyBindingsP1::right << ": girar a la derecha\n | "
               << KeyBindingsP1::brake << ": frenar\n | "
               << KeyBindingsP1::fire << ": disparar\n\n";
          cout << "Jugador 2:\n | "
               << KeyBindingsP2::thrust << ": acelerar\n | "
               << KeyBindingsP2::left << ": girar a la izquierda\n | "
               << KeyBindingsP2::right << ": girar a la derecha\n | "
               << KeyBindingsP2::fire << ": disparar\n\n";
          cout << "Sistema:\n | "
               << KeyBindingsSys::pause << ": pausar\n | "
               << KeyBindingsSys::newrd << ": nueva partida\n | "
               << KeyBindingsSys::quit << ": salir\n ·\n";

          UICommon::waitEnter();
     }
}

