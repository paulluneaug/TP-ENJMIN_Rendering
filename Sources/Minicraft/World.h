#pragma once
#include <unordered_map>
#include <Engine/Vector3Int.h>
#include <Engine/Chunk.h>
#include "Camera.h"

class World
{
	struct ModelData
	{
		Matrix Model;
	};

public:
	static const Vector3Int& WORLD_SIZE;

private :
	std::unordered_map<Vector3Int, Chunk*, Vector3IntHasher> m_chunks;
	ConstantBuffer<ModelData> m_constantBufferModel;

	GenerationSettings m_generationSettings;

public:
	World();
	~World();
	void Generate(DeviceResources* deviceRes);
	void Draw(Camera& camera, DeviceResources* deviceRes);

private:
	Chunk* CreateChunkAtPosition(const Vector3Int& position);
	void GenerateChunksValues();
	void GenerateChunksMeshes(DeviceResources* deviceRes);

	void SetNeighbouringChunks(const Vector3Int& position, Chunk* newChunk);
};

