#include "include/UIControl.hpp"
#include "include/UICommon.hpp"

int main() {
    UICommon::initConsoleUTF8();
    int opcion = 0;
    do {
        UIControl::showMenu();
        opcion = UICommon::readInt("-> ", 1, 5);
        UIControl::processEntry(opcion);
    } while (opcion != 5);
    return 0;
}
