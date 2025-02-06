#include "pch.h"
#include "Vector2Int.h"

Vector2Int::Vector2Int() :
	X(0),
	Y(0)
{

}

Vector2Int::Vector2Int(int x, int y) :
	X(x),
	Y(y)
{
}

Vector2 Vector2Int::ToVector2()
{
	return Vector2{ float(X), float(Y)};
}

Vector2Int Vector2Int::operator+(const Vector2Int& v) const
{
	return Vector2Int(X + v.X, Y + v.Y);
}

bool Vector2Int::operator==(const Vector2Int& other) const
{
	return X == other.X && Y == other.Y;
}

Vector2Int Vector2Int::operator*(const int& scalar) const
{
	return Vector2Int(X * scalar, Y * scalar);
}