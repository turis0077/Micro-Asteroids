// include/AsciiLegend.hpp
#pragma once
#include <string>

namespace ascii {
    inline constexpr char ShipUp    = '^';
    inline constexpr char ShipDown  = 'v';
    inline constexpr char ShipLeft  = '<';
    inline constexpr char ShipRight = '>';

    inline constexpr char BigAst   = '@';
    inline constexpr char MedAst   = '0';
    inline constexpr char SmallAst = 'o';

    inline constexpr char Bullet   = '.';
    inline constexpr char Boom     = '*';

    inline const std::string HUDTemplate = "P1 ❤❤❤ Score: 000 | P2 ❤❤❤ Score: 000";
}