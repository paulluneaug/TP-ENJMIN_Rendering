#include "pch.h"
#include "Utils.h"

Vector4 Utils::ToVec4(Vector3 v3)
{
	return Vector4(v3.x, v3.y, v3.z, 1.0f);
}

float Utils::Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

int Utils::Lerp(int a, int b, float t)
{
	return a + (b - a) * t;
}

int Utils::Lerp(Vector2Int range, float t)
{
	return Lerp(range.X, range.Y, t);
}

float Utils::InverseLerp(int value, Vector2Int range)
{
    return InverseLerp(value, range.X, range.Y);
}

float Utils::InverseLerp(int value, int min, int max)
{
	return float(value - min) / float(max - min);
}

float Utils::InverseLerp(float value, float min, float max)
{
    return (value - min) / (max - min);
}
