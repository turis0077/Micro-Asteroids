// include/Input.hpp
#pragma once
#include <cstdint>

struct InputState {
    bool quit=false, pause=false, newround=false;
    bool p1_thrust=false, p1_left=false, p1_right=false, p1_brake=false, p1_fire=false;
    bool p2_thrust=false, p2_left=false, p2_right=false, p2_fire=false;
};

class Input {
public:
    void init();
    void shutdown();
    InputState poll();
};
