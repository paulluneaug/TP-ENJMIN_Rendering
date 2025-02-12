#pragma once

#include "Vector2Int.h"


class Utils
{
public:
	static Vector4 ToVec4(Vector3 v3);
	static float Lerp(float a, float b, float t);
	static int Lerp(int a, int b, float t);
	static int Lerp(Vector2Int range, float t);

	static float InverseLerp(int value, Vector2Int range);
	static float InverseLerp(int value, int min, int max);
	static float InverseLerp(float value, float min, float max);
};

