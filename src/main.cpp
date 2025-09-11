#include "../ui/menu.hpp"
#include "../include/UICommon.hpp"

#include <iostream>

using namespace std;
using namespace UICommon;

int main() {
    int opcion = 0;
    do {
        clearScreen();
        showMenu();
        cin >> opcion;
    } while (opcion != 5);
    return 0;
}
