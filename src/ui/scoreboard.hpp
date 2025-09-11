#pragma once
#include <string>
#include <vector>

struct ScoreEntry {
    std::string name;
    int score;
};

std::vector<ScoreEntry> loadScores(const std::string& path);
void showScoreboardUI(const std::string& path);
