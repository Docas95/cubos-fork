#pragma once

#include <cubos/engine/prelude.hpp>

struct Player
{
    CUBOS_REFLECT;

    float speed{1.0F};          // Speed of the player
    float laneWidth{1.0F};      // Width of the lane
    int lane{0};                // Current lane
    int targetLane{0};          // Target lane
    int shield{0};              // Player has shield powerup
    int jetpack{0};             // Player has jetpack powerup
    float flightTiming{0.0f};   // Jetpack flight timing
    float flightPeriod{5.0f};   // Jetpack flight duration
};

void playerPlugin(cubos::engine::Cubos& cubos);
