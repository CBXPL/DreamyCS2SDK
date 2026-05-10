#pragma once
#include <cstdint>
#include <string>

struct Vector3 {
    float x, y, z;
};

namespace ESP {
    void Render();
    bool WorldToScreen(const Vector3& world, Vector3& screen);
    void Draw3DBox(const Vector3& origin, float w, float h, uint32_t color);
}
