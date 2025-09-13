// include/KeyBindings.hpp
#pragma once
#include <string>

struct KeyBindingsP1 {
    static inline const std::string thrust = "W";
    static inline const std::string left   = "A";
    static inline const std::string right  = "D";
    static inline const std::string brake  = "S";
    static inline const std::string fire   = "Space";
};
struct KeyBindingsP2 {
    static inline const std::string thrust = "I";
    static inline const std::string left   = "J";
    static inline const std::string right  = "L";
    static inline const std::string fire   = "K (o RightShift)";
};
struct KeyBindingsSys {
    static inline const std::string pause  = "P";
    static inline const std::string newrd  = "N";
    static inline const std::string quit   = "Q";
};