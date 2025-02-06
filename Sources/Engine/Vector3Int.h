#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Vector3Int 
{
public:
	int X, Y, Z;

public:
	Vector3Int();
	Vector3Int(int x, int y, int z);

	Vector3 ToVector3();

	Vector3Int operator+ (const Vector3Int& v) const;
	bool operator== (const Vector3Int& other) const;
	Vector3Int operator* (const int& scalar) const;
};

struct Vector3IntHasher
{
	size_t operator() (const Vector3Int& v) const
	{
		return std::hash<int>()(v.X) ^ std::hash<int>()(v.Y) ^ std::hash<int>()(v.Z);
	}
};