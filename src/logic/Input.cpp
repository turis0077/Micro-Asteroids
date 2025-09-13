#include "../../include/Input.hpp"

#ifdef _WIN32
    #include <conio.h>
#else
    #include <ncurses.h>
    #include <unistd.h>
#endif

void Input::init() {
    #ifndef _WIN32
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
    #endif
}

void Input::shutdown() {
    #ifndef _WIN32
        endwin();
    #endif
}

static void applyKey(InputState& st, int ch) {
    switch (ch) {
        // Sistema
        case 'q': case 'Q': st.quit = true; break;
        case 'p': case 'P': st.pause = true; break;
        case 'n': case 'N': st.newround = true; break;
        // P1
        case 'w': case 'W': st.p1_thrust = true; break;
        case 'a': case 'A': st.p1_left   = true; break;
        case 'd': case 'D': st.p1_right  = true; break;
        case 's': case 'S': st.p1_brake  = true; break;
        case ' ':           st.p1_fire   = true; break;
        // P2
        case 'i': case 'I': st.p2_thrust = true; break;
        case 'j': case 'J': st.p2_left   = true; break;
        case 'l': case 'L': st.p2_right  = true; break;
        case 'k': case 'K': st.p2_fire   = true; break;
    }
}

InputState Input::poll() {
    InputState st{};
#ifdef _WIN32
    // Modo simple no bloqueante en Windows (conio)
    while (_kbhit()) {
        int ch = _getch();
        applyKey(st, ch);
    }
#else
    int ch;
    // Lee todas las teclas disponibles este frame
    while ((ch = getch()) != ERR) {
        applyKey(st, ch);
    }
#endif
    return st;
}

