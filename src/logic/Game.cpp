// src/logic/Game.cpp
#include "../../include/Game.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/AsciiLegend.hpp"
#include "../../include/Concurrency.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>

using namespace ascii;
using namespace UICommon;
using namespace std;

static inline float deg2rad(float d){ return d * 3.1415926535f / 180.0f; }

namespace {
    inline std::string sym(char c) { return string(1, c); }
    inline std::string sym(const char* s) { return string(s); }
    inline std::string sym(const string& s) { return s; }
}

Game::Game(int width, int height, const GameConfig& cfgIn)
    : width(width), height(height), buffer(width, height), cfg(cfgIn), last_frame_time(chrono::steady_clock::now()) {
    
    layout.compute(width, height);

    float cx = layout.play.x + layout.play.w * 0.5f;
    float cy = layout.play.y + layout.play.h * 0.5f;
    

    // Crear naves y establecer vidas
    ships.emplace_back(cx-5.f, cy, 0.f, 0.f, sym(ShipUp), EntityKind::Ship1, 1.0f, -deg2rad(90.f));
    if (cfg.players == 2)
        ships.emplace_back(cx+5.f, cy, 0.f, 0.f, sym(ShipUp), EntityKind::Ship2, 1.0f, -deg2rad(90.f));

    hud.st.p1Lives = cfg.livesPerPlayer;
    hud.st.p2Lives = (cfg.players==2 ? cfg.livesPerPlayer : 0);

    input.init();
    initConsoleUTF8();

    // Crear asteroides iniciales
    spawnInitialAsteroids();
}

void Game::startThreads() {
    if (!use_threads) return;

    gs.game = this;
    const int N = 4;

    initConcurrency(gs, N + 1);
    gs.running = true;

    worker_threads.clear();
    worker_threads.reserve(N);

    pthread_t thread;

    pthread_create(&thread, nullptr, inputThread, &gs);
    worker_threads.push_back(thread);
    pthread_create(&thread, nullptr, asteroidThread, &gs);
    worker_threads.push_back(thread);
    pthread_create(&thread, nullptr, collisionThread, &gs);
    worker_threads.push_back(thread);
    pthread_create(&thread, nullptr, bulletManagerThread, &gs);
    worker_threads.push_back(thread);
}

void Game::stopThreads() {
    if (!use_threads) return;
    if (!gs.running.load()) return;

    gs.running = false;
    Concurrency::syncFrame(gs);

    for (pthread_t th : worker_threads) pthread_join(th, nullptr);
    worker_threads.clear();
    Concurrency::destroy(gs);
}

void Game::run() {
    is_running = true;
    if (use_threads) startThreads();
    while (is_running) {
        auto current_time = chrono::steady_clock::now();
        float delta_time = chrono::duration<float>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        gs.dt = delta_time;
        gs.paused.store(paused, std::memory_order_release);

        processInput();
        if (!paused) update(delta_time);
        render();

        if (use_threads) Concurrency::syncFrame(gs); // sincronización por frame
        this_thread::sleep_for(chrono::milliseconds(5));
    }
    if (use_threads) stopThreads();
    input.shutdown();
}

void Game::processInput() {
    InputState st = input.poll();

    if (st.quit) { is_running = false; return; }
    if (st.pause) paused = !paused;
    if (st.newround) {
        hud.st.p1Score = hud.st.p2Score = 0;
        for (auto& s: ships) { s.x = layout.play.x + layout.play.w/2.f; s.y = layout.play.y + layout.play.h/2.f; s.vx = s.vy = 0.f; s.angle = -deg2rad(90.f); s.sprite = ShipUp; }
        bullets.clear(); asteroids.clear(); spawnInitialAsteroids();
    }
    last_input = st;
}

void Game::update(float delta_time) {
    p1_cd = max(0.f, p1_cd - delta_time);
    p2_cd = max(0.f, p2_cd - delta_time);

    updateShips(delta_time);
    updateAsteroids(delta_time);
    updateBullets(delta_time);
    handleCollisions();
}

void Game::updateShips(float dt) {
    // Constantes de control
    const float ROT = deg2rad(180.f);
    const float THRUST = 40.f;
    const float BRAKE = 0.80f;
    const float MAXS = 60.f;

    for (size_t i=0;i<ships.size();++i) {
        auto& s = ships[i];
        // lectura según jugador
        bool L=false,R=false,T=false,B=false,F=false;
        if (i==0) {
            L = last_input.p1_left; R = last_input.p1_right; T = last_input.p1_thrust; B = last_input.p1_brake; F = last_input.p1_fire;
            if (F && p1_cd<=0.f) { fireBullet(0); p1_cd = 0.25f; }
        } else {
            L = last_input.p2_left; R = last_input.p2_right; T = last_input.p2_thrust; B = false; F = last_input.p2_fire;
            if (F && p2_cd<=0.f) { fireBullet(1); p2_cd = 0.25f; }
        }

        // rotación
        if (L) s.angle -= ROT * dt;
        if (R) s.angle += ROT * dt;

        // thrust (acelera hacia el frente)
        if (T) {
            s.vx += std::cos(s.angle) * THRUST * dt;
            s.vy += std::sin(s.angle) * THRUST * dt;
        }

        // brake (sólo P1 según tu guía)
        if (B) { s.vx *= BRAKE; s.vy *= BRAKE; }

        // clamp de velocidad
        float sp2 = s.vx*s.vx + s.vy*s.vy;
        if (sp2 > MAXS*MAXS) {
            float k = MAXS / std::sqrt(sp2);
            s.vx *= k; s.vy *= k;
        }

        // integrar y envolver
        s.update(dt);
        wrapInPlay(s);

        // actualizar sprite según ángulo (4 direcciones)
        float a = std::fmod(s.angle, 2.f*3.1415926535f);
        if (a < 0) a += 2.f*3.1415926535f;
        // sectores de 90°
        if (a > deg2rad(45) && a <= deg2rad(135)) s.sprite = ShipDown;
        else if (a > deg2rad(135) && a <= deg2rad(225)) s.sprite = ShipLeft;
        else if (a > deg2rad(225) && a <= deg2rad(315)) s.sprite = ShipUp;
        else s.sprite = ShipRight;
    }
}

void Game::updateAsteroids(float dt) {
    for (auto& a: asteroids) { a.update(dt); wrapInPlay(a); }
}

void Game::updateBullets(float dt) {
    for (auto& b: bullets) { b.update(dt); wrapInPlay(b); }
    // eliminar expiradas
    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const GameEntity& b){ return b.ttl <= 0.f; }), bullets.end());
}

void Game::handleCollisions() {
    // balas vs asteroides (círculo simple)
    for (size_t bi=0; bi<bullets.size();) {
        bool hit = false;
        for (size_t ai=0; ai<asteroids.size(); ++ai) {
            auto& b = bullets[bi];
            auto& a = asteroids[ai];
            float dx = a.x - b.x, dy = a.y - b.y;
            float r = a.radius + b.radius;
            if (dx*dx + dy*dy <= r*r) {
                hit = true;
                if (b.kind==EntityKind::Bullet1) hud.st.p1Score += cfg.scorePerSmall;
                else hud.st.p2Score += cfg.scorePerSmall;
                splitAsteroid(ai);
                break;
            }
        }
        if (hit) bullets.erase(bullets.begin()+bi);
        else ++bi;
    }
}

void Game::splitAsteroid(size_t idx) {
    // reemplaza asteroide impactado
    auto a = asteroids[idx];
    asteroids.erase(asteroids.begin()+idx);

    if (a.kind == EntityKind::AstBig) {
        spawnAsteroid(a.x, a.y, EntityKind::AstMed, 18.f);
        spawnAsteroid(a.x, a.y, EntityKind::AstMed, 18.f);
    } else if (a.kind == EntityKind::AstMed) {
        spawnAsteroid(a.x, a.y, EntityKind::AstSmall, 24.f);
        spawnAsteroid(a.x, a.y, EntityKind::AstSmall, 24.f);
    } else {
        // small: se borra
    }
}

void Game::spawnInitialAsteroids() {
    uniform_real_distribution<float> px(layout.play.x+2, layout.play.x + layout.play.w-3);
    uniform_real_distribution<float> py(layout.play.y+2, layout.play.y + layout.play.h-3);

    for (int i=0;i<cfg.largeAsteroids;i++) {
        spawnAsteroid(px(rng), py(rng), EntityKind::AstBig, 12.f);
    }
}

void Game::spawnAsteroid(float x,float y, EntityKind k, float speed) {
    // velocidad aleatoria
    uniform_real_distribution<float> ang(0.f, 2.f*3.1415926535f);
    float a = ang(rng);
    float vx = cos(a)*speed, vy = sin(a)*speed;

    // sprite y radio por tamaño
    string spr = sym(SmallAst);
    float r = 1.f;
    if (k==EntityKind::AstBig)   { spr = sym(BigAst);   r = 2.0f; }
    if (k==EntityKind::AstMed)   { spr = sym(MedAst);   r = 1.5f; }
    if (k==EntityKind::AstSmall) { spr = sym(SmallAst); r = 1.0f; }

    asteroids.emplace_back(x, y, vx, vy, spr, k, r);
}

void Game::fireBullet(int playerIdx) {
    if (playerIdx < 0 || playerIdx >= (int)ships.size()) return;
    auto& s = ships[playerIdx];
    float spd = 80.f;
    float bx = s.x + cos(s.angle)*1.5f;
    float by = s.y + sin(s.angle)*1.5f;
    float bvx = s.vx + cos(s.angle)*spd;
    float bvy = s.vy + sin(s.angle)*spd;

    EntityKind bk = (playerIdx==0) ? EntityKind::Bullet1 : EntityKind::Bullet2;
    bullets.emplace_back(bx, by, bvx, bvy, sym(Bullet), bk, 0.4f, 0.f, 1.2f);
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

    for (const auto& a : asteroids) a.draw(buffer);
    for (const auto& s : ships) s.draw(buffer);
    for (const auto& b : bullets) b.draw(buffer);
    hud.draw(layout, cfg, buffer);

    if (paused) {
        string p = "[PAUSED]";
        int px = layout.play.x + (layout.play.w - (int)p.size())/2;
        int py = layout.play.y + layout.play.h/2;
        buffer.drawText(px, py, p);
    }

    buffer.present();
}
