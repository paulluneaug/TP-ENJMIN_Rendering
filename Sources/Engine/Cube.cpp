#include "pch.h"
#include "Cube.h"

void Cube::PushCube(std::vector<VertexLayout_Position>& r_vertices, std::vector<uint32_t>& r_indices, Vector3 position)
{
}

void Cube::PushFace(std::vector<VertexLayout_Position>& r_vertices, std::vector<uint32_t>& r_indices, Vector3 position, Vector3 up, Vector3 right)
{
	uint32_t startIndice = r_vertices.size();

	Vector4 positionV4 = Vector4(position);
	Vector4 upV4 = Vector4(up);
	Vector4 rightV4 = Vector4(right);

	r_vertices.push_back({ positionV4 });
	r_vertices.push_back({ positionV4 + rightV4 });
	r_vertices.push_back({ positionV4 + upV4 });
	r_vertices.push_back({ positionV4 + upV4 + rightV4 });

	r_indices.push_back(startIndice);
	r_indices.push_back(startIndice + 2);
	r_indices.push_back(startIndice + 1);
	r_indices.push_back(startIndice + 1);
	r_indices.push_back(startIndice + 2);
	r_indices.push_back(startIndice + 3);
}
