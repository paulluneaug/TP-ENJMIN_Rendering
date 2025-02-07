#include "pch.h"

#include "World.h"

World::World() {
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				Chunk* c = new Chunk(this, Vector3(x, y, z) * CHUNK_SIZE);
				chunks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_HEIGHT] = c;
			}
		}
	}
}

void World::Generate(DeviceResources* deviceRes) {
	/*memset(data, EMPTY, sizeof(data));

	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int z = 0; z < WORLD_SIZE; z++) {
			for (int y = 0; y < 3; y++) {
				auto block = GetCubes(x, y, z);
				if (!block) continue;
				*block = DIRT;
			}
			auto block = GetCubes(x, 3, z);
			if (!block) continue;
			*block = GRASS;
		}
	}

	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_SIZE; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				auto block = GetCubes(x, y, z);
				if (!block) continue;
				if (EMPTY == *block) continue;

				auto& cube = cubes.emplace_back(Vector3(x, y, z));
				cube.id = *block;
				cube.Generate(deviceRes);
			}
		}
	}*/

	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++)
		chunks[idx]->Generate(deviceRes);

	constantBufferModel.Create(deviceRes);
}

void World::Draw(DeviceResources* deviceRes) {
	constantBufferModel.ApplyToVS(deviceRes, 0);

	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
		constantBufferModel.data.model = chunks[idx]->model.Transpose();
		constantBufferModel.UpdateBuffer(deviceRes);
		chunks[idx]->Draw(deviceRes);
	}
}

BlockId* World::GetCubes(int gx, int gy, int gz) {
	/*uint32_t index = gx + gy * WORLD_SIZE + gz * WORLD_SIZE * WORLD_SIZE;
	if (index < 0 || index > WORLD_SIZE * WORLD_SIZE * WORLD_SIZE) return nullptr;
	return &data[index];*/
	return nullptr;
}