#include "pch.h"
#include "NoiseUtils.h"

float NoiseUtils::Smoothstep(float w)
{
    w = std::clamp(w, 0.0f, 1.0f);
    return w * w * (3.0f - 2.0f * w);
}

float NoiseUtils::Smootherstep(float w)
{
    w = std::clamp(w, 0.0f, 1.0f);
    return ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w);
}

float NoiseUtils::Interpolate(float a0, float a1, float w, bool smootherStep)
{
    float smoothW = 0;
    if (smootherStep)
    {
        smoothW = Smootherstep(w);
    }
    else
    {
        smoothW = Smoothstep(w);
    }

    return a0 + (a1 - a0) * smoothW;
}

uint32_t NoiseUtils::Hash(uint32_t s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

float NoiseUtils::RandomFloat(uint32_t& r_seed)
{
    r_seed = Hash(r_seed);
    float random = float(r_seed) / 4294967295.0; // 2^32-1 
    return random * 2 - 1; // [-1;1]
}

Vector3 NoiseUtils::RandomFloat3InsideUnitSphere(uint32_t& r_seed)
{
    Vector3 attempt;
    do
    {
        attempt = Vector3{ RandomFloat(r_seed), RandomFloat(r_seed), RandomFloat(r_seed) };
    } while (attempt.x * attempt.x + attempt.y * attempt.y + attempt.z * attempt.z > 1);
    return attempt;
}

Vector3 NoiseUtils::RandomGradient3D(int ix, int iy, int iz, int gradientOffset)
{
    // No precomputed gradients mean this works for any number of grid coordinates
    constexpr uint32_t w = 8 * 4;
    constexpr uint32_t s = w / 2;
    uint32_t a = ix, b = iy, c = iz;
    a *= 3284157443;
    b ^= a << s | a >> w - s;
    b *= 1911520717 - abs(gradientOffset);
    c ^= b << s | b >> w - s;
    c *= 1529716214;
    a ^= c << s | c >> w - s;
    a *= 2048419325;

    return RandomFloat3InsideUnitSphere(a);
}

Vector2 NoiseUtils::RandomGradient2D(int ix, int iy, int gradientOffset)
{
    // No precomputed gradients mean this works for any number of grid coordinates
    constexpr uint32_t w = 8 * 4;
    constexpr uint32_t s = w / 2;
    uint32_t a = ix, b = iy;
    a *= 3284157443;
    b ^= a << s | a >> w - s;
    b *= 1911520717 - abs(gradientOffset);
    a ^= b << s | b >> w - s;
    a *= 2048419325;

    return Vector2(cos(a), sin(a));
}

float NoiseUtils::DotGridGradient3D(int ix, int iy, int iz, float x, float y, float z, int gradientOffset)
{
    Vector3 randomVec = RandomGradient3D(ix, iy, iz, gradientOffset);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;
    float dz = z - (float)iz;

    // Compute the dot-product
    return (dx * randomVec.x + dy * randomVec.y + dz * randomVec.z);
}

float NoiseUtils::DotGridGradient2D(int ix, int iy, float x, float y, int gradientOffset)
{
    Vector2 randomVec = RandomGradient2D(ix, iy, gradientOffset);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx * randomVec.x + dy * randomVec.y);
}
