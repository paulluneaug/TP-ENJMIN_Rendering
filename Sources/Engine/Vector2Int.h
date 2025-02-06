#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Vector2Int
{
public:
	int X, Y;

public:
	Vector2Int();
	Vector2Int(int x, int y);

	Vector2 ToVector2();

	Vector2Int operator+ (const Vector2Int& v) const;
	bool operator== (const Vector2Int& other) const;
	Vector2Int operator* (const int& scalar) const;
};

