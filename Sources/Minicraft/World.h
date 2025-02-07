#pragma once

#include "Minicraft/Block.h"
#include "Minicraft/Cube.h"

#define WORLD_SIZE 3
#define WORLD_HEIGHT 1

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

	BlockId* GetCubes(int gx, int gy, int gz);
};