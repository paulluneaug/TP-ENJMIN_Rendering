#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/Block.h"

class Cube {
	VertexBuffer<VertexLayout_PositionUV> vb;
	IndexBuffer ib;

public:
	Matrix model;
	BlockId id = BlockId::GRASS;

	Cube(Vector3 pos);

	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);

private:
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int id);
};