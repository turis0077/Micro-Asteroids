// src/logic/Game.cpp
#include "../../include/Game.hpp"
#include "../../include/UICommon.hpp"
#include "../../include/AsciiLegend.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>

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

void Game::run() {
    is_running = true;
    
    // Iniciar hilos
    input_thread = std::thread(&Game::inputThreadFunc, this);
    render_thread = std::thread(&Game::renderThreadFunc, this);
    ship1_thread = std::thread(&Game::ship1ThreadFunc, this);
    
    if (cfg.players == 2) {
        ship2_thread = std::thread(&Game::ship2ThreadFunc, this);
    }

    // Bucle principal del juego
    while (is_running) {
        auto current_time = chrono::steady_clock::now();
        float delta_time = chrono::duration<float>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        {
            std::lock_guard<std::mutex> lock(game_mutex);
            if (!paused && !game_over) {
                update(delta_time);
                checkGameOver();
            }
        }

        if (game_over) {
            showGameOverScreen();
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(16)); // ~60 FPS
    }

    // Esperar a que terminen los hilos
    if (input_thread.joinable()) input_thread.join();
    if (render_thread.joinable()) render_thread.join();
    if (ship1_thread.joinable()) ship1_thread.join();
    if (ship2_thread.joinable()) ship2_thread.join();

    input.shutdown();
}

void Game::inputThreadFunc() {
    while (is_running) {
        std::lock_guard<std::mutex> lock(game_mutex);
        processInput();
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void Game::renderThreadFunc() {
    while (is_running) {
        std::lock_guard<std::mutex> lock(game_mutex);
        render();
        this_thread::sleep_for(chrono::milliseconds(16)); // ~60 FPS
    }
}

void Game::ship1ThreadFunc() {
    while (is_running && !game_over) {
        std::lock_guard<std::mutex> lock(game_mutex);
        if (!paused && ships.size() > 0) {
            auto& ship = ships[0];
            // Lógica específica del jugador 1
            if (last_input.p1_left) ship.angle -= deg2rad(180.f) * 0.016f;
            if (last_input.p1_right) ship.angle += deg2rad(180.f) * 0.016f;
            if (last_input.p1_thrust) {
                ship.vx += cos(ship.angle) * 40.f * 0.016f;
                ship.vy += sin(ship.angle) * 40.f * 0.016f;
            }
            if (last_input.p1_brake) {
                ship.vx *= 0.98f;
                ship.vy *= 0.98f;
            }
            if (last_input.p1_fire && p1_cd <= 0.f) {
                fireBullet(0);
                p1_cd = 0.3f;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(16));
    }
}

void Game::ship2ThreadFunc() {
    while (is_running && !game_over) {
        std::lock_guard<std::mutex> lock(game_mutex);
        if (!paused && ships.size() > 1) {
            auto& ship = ships[1];
            // Lógica específica del jugador 2
            if (last_input.p2_left) ship.angle -= deg2rad(180.f) * 0.016f;
            if (last_input.p2_right) ship.angle += deg2rad(180.f) * 0.016f;
            if (last_input.p2_thrust) {
                ship.vx += cos(ship.angle) * 40.f * 0.016f;
                ship.vy += sin(ship.angle) * 40.f * 0.016f;
            }
            if (last_input.p2_brake) {
                ship.vx *= 0.98f;
                ship.vy *= 0.98f;
            }
            if (last_input.p2_fire && p2_cd <= 0.f) {
                fireBullet(1);
                p2_cd = 0.3f;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(16));
    }
}

void Game::checkGameOver() {
    // Verificar si todos los asteroides han sido destruidos
    if (asteroids.empty()) {
        game_over = true;
        if (cfg.players == 1) {
            player1_won = true;
        } else {
            // En modo 2 jugadores, gana quien tenga más puntos
            player1_won = (hud.st.p1Score >= hud.st.p2Score);
            player2_won = (hud.st.p2Score > hud.st.p1Score);
        }
        return;
    }

    // Verificar si algún jugador se quedó sin vidas
    if (hud.st.p1Lives <= 0) {
        game_over = true;
        if (cfg.players == 2) {
            player2_won = true;
        }
        return;
    }

    if (cfg.players == 2 && hud.st.p2Lives <= 0) {
        game_over = true;
        player1_won = true;
        return;
    }
}

void Game::showGameOverScreen() {
    clearScreen();
    
    cout << "\n\n";
    cout << "  ╔══════════════════════════════════════╗\n";
    cout << "  ║            JUEGO TERMINADO           ║\n";
    cout << "  ╠══════════════════════════════════════╣\n";
    
    if (cfg.players == 1) {
        if (player1_won) {
            cout << "  ║          ¡FELICIDADES!               ║\n";
            cout << "  ║      Has destruido todos los         ║\n";
            cout << "  ║          asteroides!                 ║\n";
        } else {
            cout << "  ║          GAME OVER                   ║\n";
            cout << "  ║      Te quedaste sin vidas           ║\n";
        }
        cout << "  ║                                      ║\n";
        cout << "  ║  Puntuación Final: " << setw(15) << hud.st.p1Score << "  ║\n";
    } else {
        if (player1_won) {
            cout << "  ║        ¡JUGADOR 1 GANA!              ║\n";
        } else if (player2_won) {
            cout << "  ║        ¡JUGADOR 2 GANA!              ║\n";
        } else {
            cout << "  ║           ¡EMPATE!                   ║\n";
        }
        cout << "  ║                                      ║\n";
        cout << "  ║  Jugador 1: " << setw(20) << hud.st.p1Score << "  ║\n";
        cout << "  ║  Jugador 2: " << setw(20) << hud.st.p2Score << "  ║\n";
    }
    
    cout << "  ╠══════════════════════════════════════╣\n";
    cout << "  ║  Ingresa tu nombre para guardar      ║\n";
    cout << "  ║  tu puntuación:                      ║\n";
    cout << "  ╚══════════════════════════════════════╝\n";
    cout << "\n  Nombre: ";
    
    string playerName;
    getline(cin, playerName);
    
    if (playerName.empty()) {
        playerName = "Anónimo";
    }
    
    // Guardar puntuación del ganador o del jugador único
    int finalScore = (cfg.players == 1) ? hud.st.p1Score : 
                     (player1_won ? hud.st.p1Score : hud.st.p2Score);
    
    saveScore(playerName, finalScore);
    
    cout << "\n  ¡Puntuación guardada exitosamente!\n";
    cout << "  Presiona Enter para continuar...";
    cin.get();
}

void Game::saveScore(const std::string& playerName, int score) {
    std::ofstream file("scores.csv", std::ios::app);
    
    if (!file.is_open()) {
        // Si el archivo no existe, crear con encabezados
        std::ofstream newFile("scores.csv");
        newFile << "Nombre,Puntuacion,Fecha\n";
        newFile.close();
        file.open("scores.csv", std::ios::app);
    }
    
    // Obtener fecha actual
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    char dateStr[100];
    std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", &tm);
    
    file << playerName << "," << score << "," << dateStr << "\n";
    file.close();
}

void Game::processInput() {
    InputState st = input.poll();

    if (st.quit) { is_running = false; return; }
    if (st.pause) paused = !paused;
    if (st.newround) {
        hud.st.p1Score = hud.st.p2Score = 0;
        hud.st.p1Lives = cfg.livesPerPlayer;
        hud.st.p2Lives = (cfg.players==2 ? cfg.livesPerPlayer : 0);
        game_over = false;
        player1_won = false;
        player2_won = false;
        
        for (auto& s: ships) { 
            s.x = layout.play.x + layout.play.w/2.f; 
            s.y = layout.play.y + layout.play.h/2.f; 
            s.vx = s.vy = 0.f; 
            s.angle = -deg2rad(90.f); 
            s.sprite = ShipUp; 
        }
        bullets.clear(); 
        asteroids.clear(); 
        spawnInitialAsteroids();
    }
    last_input = st;
}

void Game::update(float delta_time) {
    p1_cd = max(0.f, p1_cd - delta_time);
    p2_cd = max(0.f, p2_cd - delta_time);

    // Los hilos de las naves manejan su propia lógica
    // updateShips(delta_time); // Comentado porque ahora lo manejan los hilos
    updateAsteroids(delta_time);
    updateBullets(delta_time);
    handleCollisions();
}

void Game::updateShips(float dt) {
    // Este método ahora es manejado por los hilos individuales de cada nave
    // Se mantiene por compatibilidad pero la lógica está en ship1ThreadFunc y ship2ThreadFunc
    
    const float MAXS = 60.f;
    
    for (auto& s : ships) {
        // Aplicar velocidad máxima
        float speed = sqrt(s.vx*s.vx + s.vy*s.vy);
        if (speed > MAXS) {
            s.vx = (s.vx / speed) * MAXS;
            s.vy = (s.vy / speed) * MAXS;
        }
        
        // Actualizar posición
        s.x += s.vx * dt;
        s.y += s.vy * dt;
        
        // Envolver en área de juego
        wrapInPlay(s);
        
        // Actualizar sprite según ángulo
        float deg = s.angle * 180.f / 3.1415926535f;
        while (deg < 0) deg += 360;
        while (deg >= 360) deg -= 360;
        
        if (deg >= 315 || deg < 45) s.sprite = ShipUp;
        else if (deg >= 45 && deg < 135) s.sprite = ShipRight;
        else if (deg >= 135 && deg < 225) s.sprite = ShipDown;
        else s.sprite = ShipLeft;
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
