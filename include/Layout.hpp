// include/Layout.hpp
#pragma once
#include "RenderBuffer.hpp"

struct Rect { int x, y, w, h; };

struct Layout {
    // margenes y alturas fijas
    int marginLeft = 2;
    int marginRight = 2;
    int marginTopHUD = 2;
    int marginBottom = 1;

    Rect hud{0,0,0,0};
    Rect play{0,0,0,0};
    Rect footer{0,0,0,0};

    /* Se computan las áreas de HUD, playfield y footer según el tamaño total W x H
     * donde se determina que el HUD tiene altura 1 y está en la parte superior,
     * el footer tiene altura 1 y está en la parte inferior, y el playfield ocupa el 
     * resto de la ventana
    */
    void compute(int W, int H) {
        hud = {marginLeft, 0, W - marginLeft - marginRight, 1};
        footer = {marginLeft, H - marginBottom, W - marginLeft - marginRight, 1};
        int topPlay = hud.y + hud.h + 1;
        int hPlay = (H - marginBottom) - topPlay;
        if (hPlay < 1) hPlay = 1;
        play = {marginLeft, topPlay, W - marginLeft - marginRight, hPlay};
    }

    /* Se dibuja en el RenderBuffer rb asumiendo que compute(W,H) ya fue llamado con 
     * las dimensiones correctas, lo siguiente:
     *  - Marco del playfield con 1 carácter de grosor
     *  - Línea separadora entre HUD y playfield
     *  - Línea separadora entre playfield y footer
    */
    void drawFrames(RenderBuffer& rb) const {
        rb.rect(play.x - 1, play.y - 1, play.w + 2, play.h + 1);
        rb.hlineRB(hud.x, hud.y + hud.h, hud.w, '=');
        rb.hlineRB(footer.x, footer.y - 1, footer.w, '=');
    }


};