#pragma once
#include <vector>
#include <string>
#include <iostream>

class RenderBuffer {
    int w, h;
    std::vector<std::string> buf;
public:
    RenderBuffer(int width, int height): w(width), h(height), buf(height, std::string(width, ' ')) {}

    void clear(char fill = ' ') {
        for (auto &row : buf) std::fill(row.begin(), row.end(), fill);
    }
    int width() const { return w; }
    int height() const { return h; }

    void set(int x, int y, char c) {
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        buf[y][x] = c;
    }
    void drawText(int x, int y, const std::string& s) {
        if (y < 0 || y >= h) return;
        for (int i = 0; i < (int)s.size(); ++i) {
            int xx = x + i;
            if (xx < 0 || xx >= w) break;
            buf[y][xx] = s[i];
        }
    }
    void hline(int x, int y, int len, char c='-') {
        for (int i=0;i<len;i++) set(x+i, y, c);
    }
    void vline(int x, int y, int len, char c='|') {
        for (int i=0;i<len;i++) set(x, y+i, c);
    }
    void rect(int x, int y, int ww, int hh) {
        hline(x, y, ww, '-');
        hline(x, y+hh-1, ww, '-');
        vline(x, y, hh, '|');
        vline(x+ww-1, y, hh, '|');
        set(x, y, '+'); set(x+ww-1, y, '+');
        set(x, y+hh-1, '+'); set(x+ww-1, y+hh-1, '+');
    }
    void present() const {
        // limpieza simple de consola (Windows/Linux)
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        for (const auto& row : buf) std::cout << row << "\n";
    }
};
