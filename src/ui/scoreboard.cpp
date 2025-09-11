#include "scoreboard.hpp"
#include "../logic/render.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

static void pressEnter() {
    std::cout << "\nPresiona ENTER para volver...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (std::cin.peek()=='\n') std::cin.get();
}

std::vector<ScoreEntry> loadScores(const std::string& path) {
    std::vector<ScoreEntry> v;
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name; int sc = 0;
        if (std::getline(ss, name, ',') && (ss >> sc)) v.push_back({name, sc});
    }
    std::sort(v.begin(), v.end(), [](auto& a, auto& b){ return a.score > b.score; });
    return v;
}

void showScoreboardUI(const std::string& path) {
    auto scores = loadScores(path); // ordenados desc
    RenderBuffer rb(60, 18);
    rb.clear(' ');

    rb.rect(0,0,60,18);
    rb.drawText(20, 1, "PUNTAJES DESTACADOS");

    // cabecera
    rb.hline(1,3,58,'-');
    rb.drawText(4, 2, "#");
    rb.drawText(8, 2, "Jugador");
    rb.drawText(40,2, "Puntaje");
    rb.hline(1,4,58,'-');

    // top N (max 10)
    int N = std::min<int>(10, scores.size());
    for (int i=0;i<N;i++) {
        int y = 5 + i;
        std::string rank = std::to_string(i+1);
        rb.drawText(4, y, rank);

        // Estrella para el top 1
        std::string name = (i==0 ? "★ " + scores[i].name : "  " + scores[i].name);
        if ((int)name.size() > 28) name = name.substr(0,28);
        rb.drawText(8, y, name);

        std::string sc = std::to_string(scores[i].score);
        rb.drawText(40, y, sc);
    }

    rb.hline(1, 5+N, 58, '-');
    rb.drawText(6, 5+N+2, "Tip: agrega mas filas en data/scores.txt (nombre,score)");
    rb.drawText(6, 5+N+3, "ENTER para volver al menu.");

    rb.present();
    // consumir el newline pendiente si viene de cin >> opcion
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    pressEnter();
}
