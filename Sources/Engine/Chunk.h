#pragma once
#include "Engine/VertexLayout.h"
#include "Buffers.h"

using VertexLayout = VertexLayout_PositionUV;

class Chunk
{
private :
	VertexBuffer<VertexLayout> m_vertexBuffer;
	IndexBuffer m_indexBuffer;

public:
	Matrix ModelMatrix;

	Chunk();

	void Generate(DeviceResources* deviceRes);

	void Draw(DeviceResources* deviceRes);

	inline static Vector4 ToVec4(Vector3 v3);

private:

	void PushFace(Vector3 position, Vector3 up, Vector3 right);
};

