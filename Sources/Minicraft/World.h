#pragma once

#include "Minicraft/Block.h"
#include "Minicraft/Chunk.h"

#define WORLD_SIZE 5
#define WORLD_HEIGHT 3

class Chunk;
class World {
	Chunk* chunks[WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE];

	struct ModelData {
		Matrix model;
	};
	ConstantBuffer<ModelData> constantBufferModel;
public:
	World();
	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);

	Chunk* GetChunk(int cx, int cy, int cz);
	BlockId* GetCubes(int gx, int gy, int gz);

	friend class Chunk;
};