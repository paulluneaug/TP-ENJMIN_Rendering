
#include "pch.h"
#include "Vector3Int.h"


Vector3Int::Vector3Int():
	X(0),
	Y(0),
	Z(0)
{

}

Vector3Int::Vector3Int(int x, int y, int z):
	X(x),
	Y(y),
	Z(z)
{
}

Vector3 Vector3Int::ToVector3()
{
	return Vector3{ float(X), float(Y), float(Z) };
}

Vector3Int Vector3Int::operator+(const Vector3Int& v) const
{
	return Vector3Int(X + v.X, Y + v.Y, Z + v.Z);
}

bool Vector3Int::operator==(const Vector3Int& other) const
{
	return X == other.X && Y == other.Y && Z == other.Z;
}

Vector3Int Vector3Int::operator*(const int& scalar) const
{
	return Vector3Int(X * scalar, Y * scalar, Z * scalar);
}
