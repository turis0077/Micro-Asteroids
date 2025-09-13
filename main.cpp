#include "src/ui/menu.hpp"
#include "include/UICommon.hpp"

using namespace UICommon;

int main() {
    initConsoleUTF8();
    int opcion = 0;
    do {
        showMenu();
        opcion = readInt("-> ", 1, 5);
        processEntry(opcion);
    } while (opcion != 5);
    return 0;
}
