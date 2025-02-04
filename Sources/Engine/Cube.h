#pragma once
#include "Engine/VertexLayout.h"

class Cube
{
public:
	static void PushCube(std::vector<VertexLayout_Position>& r_vertices, std::vector<uint32_t>& r_indices, Vector3 position);
private:
	static void PushFace(std::vector<VertexLayout_Position>& r_vertices, std::vector<uint32_t>& r_indices, Vector3 position, Vector3 up, Vector3 right);
};

