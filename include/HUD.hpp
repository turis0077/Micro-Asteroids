// include/HUD.hpp
#pragma once
#include <string>
#include <algorithm>
#include <string>
#include <cstdio>

#include "RenderBuffer.hpp"
#include "Config.hpp"
#include "AsciiLegend.hpp"
#include "Layout.hpp"


// Estado base del HUD
struct HUDState {
    int p1Lives = 3;
    int p2Lives = 3;
    int p1Score = 0;
    int p2Score = 0;
};

// Funcion para formatear corazones
inline std::string makeHearts(int n) {
    n = std::clamp(n, 0, 9);
    std::string s;
    for (int i = 0; i < n; ++i) s += u8"❤";
    return s;
}

// Funcion para formatear puntajes con ceros a la izquierda
inline std::string zero3(int v) {
    v = std::clamp(v, 0, 999);
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%03d", v);
    return std::string(buf);
}

/* Estructura que maneja el HUD y footer del juego, dibujando el estado actual (vidas, 
 * puntajes, etc) en el RenderBuffer, de tal forma que:
 *  - dibuja un marco alrededor del área de juego
 *  - dibuja la línea de HUD con vidas y puntajes, ajustando su ancho
 *  - establece lo que se muestra en el HUD y el footer
*/
struct HUD {
    HUDState st;

    void draw(const Layout& ly, const GameConfig& cfg, RenderBuffer& rb) {
        std::string line;
        line += "P1 " + makeHearts(st.p1Lives) + " Score: " + zero3(st.p1Score);
        if (cfg.players == 2) {
            line += "  |  ";
            line += "P2 " + makeHearts(st.p2Lives) + " Score: " + zero3(st.p2Score);
        }
        if ((int)line.size() > ly.hud.w) line.resize(ly.hud.w);
        rb.drawText(ly.hud.x, ly.hud.y, line);

        std::string ft = "W/A/D/S + Space | I/J/L + K | P=Pause  N=New  Q=Quit";
        if ((int)ft.size() > ly.footer.w) ft.resize(ly.footer.w);
        rb.drawText(ly.footer.x, ly.footer.y, ft);
    }
};
