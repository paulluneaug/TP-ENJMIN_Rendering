#pragma once
#include "Engine/VertexLayout.h"
#include "Buffers.h"
#include <Minicraft/Block.h>
#include "Engine/Vector3Int.h"

using VertexLayout = VertexLayout_PositionUV;

class Chunk
{
public:
	static constexpr int CHUNK_SIZE = 16;

	enum NeighbouringChunkIndex : int
	{
		X_POS = 0,
		X_NEG = 1,
		Y_POS = 2,
		Y_NEG = 3,
		Z_POS = 4,
		Z_NEG = 5,
	};

public:
	Matrix ModelMatrix;

private :
	VertexBuffer<VertexLayout> m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	BlockId m_blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

	Chunk* m_neighbouringChunks[6];

public:

	Chunk(Vector3 position);

	void Generate(DeviceResources* deviceRes);

	void Draw(DeviceResources* deviceRes);

	void SetNeighbouringChunk(Chunk* chunk, NeighbouringChunkIndex chunkRelativePosition);

	inline static Vector4 ToVec4(Vector3 v3);

private:
	void FillChunk();
	void GenerateMesh();

	void PushCube(const Vector3& position, const BlockData& blockData, byte facesToDraw);
	void PushFace(const Vector3& position, const Vector3& up, const Vector3& right, bool draw, int uvID);

	inline Vector3Int IndexToCoordinates(int index);
	inline int CoordinatesToIndex(const Vector3Int& coordinates);
	inline int CoordinatesToIndex(int x, int y, int z);

	byte GetDisplayFaceFlags(int x, int y, int z);
	bool ShouldDisplayFace(int x, int y, int z, int ox, int oy, int oz);

	const BlockId& GetBlockAtPosition(int x, int y, int z);
};

