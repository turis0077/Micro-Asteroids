// src/logic/Game.cpp
#include "../../include/Game.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/AsciiLegend.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace ascii;
using namespace UICommon;
using namespace std;

Game::Game(int width, int height, const GameConfig& cfgIn)
    : width(width), height(height), buffer(width, height), cfg(cfgIn), last_frame_time(chrono::steady_clock::now()) {
    
    layout.compute(width, height);

    float cx = layout.play.x + layout.play.w / 2.0f;
    float cy = layout.play.y + layout.play.h / 2.0f;
    entities.emplace_back(cx, cy, 5.0f, 0.0f, ShipRight);
    
    entities.emplace_back(width / 2.0f, height / 2.0f, 5.0f, 0.0f, ShipRight);

    hud.st.p1Lives = cfg.livesPerPlayer;
    hud.st.p2Lives = (cfg.players==2 ? cfg.livesPerPlayer : 0);

    input.init();
    initConsoleUTF8();
}

void Game::run() {
    is_running = true;
    while (is_running) {
        auto current_time = chrono::steady_clock::now();
        float delta_time = chrono::duration<float>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        processInput();
        if (!paused) update(delta_time);
        render();

        this_thread::sleep_for(chrono::milliseconds(5));
    }
    input.shutdown();
}

void Game::processInput() {
    InputState st = input.poll();

    if (st.quit) { is_running = false; return; }
    if (st.pause) paused = !paused;
    if (st.newround) {
        hud.st.p1Score = hud.st.p2Score = 0;
        for (auto& e: entities) { e.x = layout.play.x + layout.play.w/2.0f; e.y = layout.play.y + layout.play.h/2.0f; e.vx = 5.0f; e.vy = 0.0f; }
    }

    // Controles mínimos de P1 (solo orientativo para probar entrada)
    if (!entities.empty()) {
        auto& p1 = entities[0];
        if (st.p1_left)  p1.vx = -30.0f;
        if (st.p1_right) p1.vx =  30.0f;
        if (st.p1_brake) p1.vx *= 0.8f;
        if (st.p1_thrust) p1.vy = -10.0f;
        if (st.p1_fire)   hud.st.p1Score += cfg.scorePerSmall;
    }

    // P2 prueba (si existe jugador 2: sumar puntos al presionar K)
    if (cfg.players==2 && st.p2_fire) {
        hud.st.p2Score += cfg.scorePerSmall;
    }
}

void Game::update(float delta_time) {
    for (auto& entity : entities) {
        entity.update(delta_time);
        wrapInPlay(entity);
    }
}

void Game::wrapInPlay(GameEntity& e) {
    float minx = layout.play.x;
    float maxx = layout.play.x + layout.play.w - 1;
    float miny = layout.play.y;
    float maxy = layout.play.y + layout.play.h - 1;

    if (e.x < minx) e.x = maxx;
    if (e.x > maxx) e.x = minx;
    if (e.y < miny) e.y = maxy;
    if (e.y > maxy) e.y = miny;
}

void Game::render() {
    clearScreen();
    buffer.clear(' ');

    layout.compute(width, height);
    layout.drawFrames(buffer);

    for (const auto& e : entities) e.draw(buffer);

    hud.draw(layout, cfg, buffer);

    if (paused) {
        string p = "[PAUSED]";
        int px = layout.play.x + (layout.play.w - (int)p.size())/2;
        int py = layout.play.y + layout.play.h/2;
        buffer.drawText(px, py, p);
    }

    buffer.present();
}
