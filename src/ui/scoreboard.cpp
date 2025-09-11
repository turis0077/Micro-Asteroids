#include "scoreboard.hpp"
#include "../logic/render.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

static void pressEnter() {
    cout << "\nPresiona ENTER para volver...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (cin.peek()=='\n') cin.get();
}

vector<ScoreEntry> loadScores(const std::string& path) {
    vector<ScoreEntry> v;
    ifstream in(path);
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name; int sc = 0;
        if (getline(ss, name, ',') && (ss >> sc)) v.push_back({name, sc});
    }
    sort(v.begin(), v.end(), [](auto& a, auto& b){ return a.score > b.score; });
    return v;
}

void showScoreboardUI(const string& path) {
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
    int N = min<int>(10, scores.size());
    for (int i=0;i<N;i++) {
        int y = 5 + i;
        string rank = to_string(i+1);
        rb.drawText(4, y, rank);

        // Estrella para el top 1
        string name = (i==0 ? "★ " + scores[i].name : "  " + scores[i].name);
        if ((int)name.size() > 28) name = name.substr(0,28);
        rb.drawText(8, y, name);

        string sc = to_string(scores[i].score);
        rb.drawText(40, y, sc);
    }

    rb.hline(1, 5+N, 58, '-');
    rb.drawText(6, 5+N+2, "Tip: agrega mas filas en data/scores.txt (nombre,score)");
    rb.drawText(6, 5+N+3, "ENTER para volver al menu.");

    rb.present();
    // consumir el newline pendiente si viene de cin >> opcion
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    pressEnter();
}
