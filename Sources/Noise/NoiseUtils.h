#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class NoiseUtils
{
public:
    static float Smoothstep(float w);
    static float Smootherstep(float w);
    static float Interpolate(float a0, float a1, float w, bool smootherStep);

    // Hash function from H. Schechter & R. Bridson, goo.gl/RXiKaH
    static uint32_t Hash(uint32_t s);

    static float RandomFloat(uint32_t& r_seed);
    static float RandomFloat01(uint32_t& r_seed);

    static Vector3 RandomFloat3InsideUnitSphere(uint32_t& r_seed);

    static uint32_t HashCoordinates2D(int ix, int iy, int offset);
    static uint32_t HashCoordinates3D(int ix, int iy, int iz, int offset);

    static Vector2 RandomGradient2D(int ix, int iy, int gradientOffset);
    static Vector3 RandomGradient3D(int ix, int iy, int iz, int gradientOffset);

    static float DotGridGradient2D(int ix, int iy, float x, float y, int gradientOffset);
    static float DotGridGradient3D(int ix, int iy, int iz, float x, float y, float z, int gradientOffset);
};

