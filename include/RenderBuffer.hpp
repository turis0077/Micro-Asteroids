// include/RenderBuffer.hpp
#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#ifndef _WIN32
  #include <curses.h>
    #ifdef hline
    #undef hline
  #endif
    #ifdef vline
    #undef vline
  #endif
#endif

class RenderBuffer {
    int w, h;
    std::vector<std::string> buf;
public:
    RenderBuffer(int width, int height)
      : w(width), h(height), buf(height, std::string(width, ' ')) {}

    void clear(char fill = ' ') {
        for (auto& row : buf) std::fill(row.begin(), row.end(), fill);
    }
    int width()  const { return w; }
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

    void hlineRB(int x, int y, int len, char c='-') { for (int i = 0; i < len; ++i) set(x+i, y, c); }
    void vlineRB(int x, int y, int len, char c='|') { for (int i = 0; i < len; ++i) set(x, y+i, c); }
    void rect(int x, int y, int ww, int hh) {
        if (ww <= 0 || hh <= 0) return;
        hlineRB(x, y, ww, '-');
        hlineRB(x, y+hh-1, ww, '-');
        vlineRB(x, y, hh, '|');
        vlineRB(x+ww-1, y, hh, '|');
        set(x, y, '+'); set(x+ww-1, y, '+');
        set(x, y+hh-1, '+'); set(x+ww-1, y+hh-1, '+');
    }

    void present() const {
    #ifndef _WIN32
        int rows = std::min(h, LINES);
        int cols = std::min(w, COLS);
        for (int y = 0; y < rows; ++y) {
            mvaddnstr(y, 0, buf[y].c_str(), cols);
            if (cols < COLS) clrtoeol(); // limpia resto de la línea en terminal
        }
        for (int y = rows; y < LINES; ++y) { move(y, 0); clrtoeol(); }
        refresh();
    #else
        std::cout << "\x1b[H";
        for (const auto& row : buf) std::cout << row << "\n";
        std::cout.flush();
    #endif
    }
};