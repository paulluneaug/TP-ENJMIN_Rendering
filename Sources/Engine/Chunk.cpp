#include "pch.h"
#include "Chunk.h"
#include "Utils.h"

Chunk::Chunk(Vector3Int position):
	m_chunkPostion(position)
{
	ModelMatrix = Matrix::CreateTranslation((position * CHUNK_SIZE).ToVector3());

	for (int i = 0; i < 6; ++i)
	{
		m_neighbouringChunks[i] = nullptr;
	}

	m_bounds = BoundingBox
	{
		{
			float(position.X * CHUNK_SIZE + CHUNK_SIZE / 2 - 0.5f),
			float(position.Y * CHUNK_SIZE + CHUNK_SIZE / 2 - 0.5f),
			float(position.Z * CHUNK_SIZE + CHUNK_SIZE / 2 - 0.5f)
		},
		{
			float(CHUNK_SIZE / 2),
			float(CHUNK_SIZE / 2),
			float(CHUNK_SIZE / 2)
		} 
	};
}

void Chunk::Generate(DeviceResources* deviceRes, GenerationSettings& generationSettings)
{
	GenerateBlocksValues(generationSettings);
	GenerateMesh(deviceRes);
}

void Chunk::PushCube(const Vector3& position, const BlockData& blockData, byte facesToGenerate)
{
	PushFace(position + Vector3{ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right, (facesToGenerate & (1 << 0)) != 0, blockData.texIdSide);
	PushFace(position + Vector3{ 0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward, (facesToGenerate & (1 << 1)) != 0, blockData.texIdSide);
	PushFace(position + Vector3{ 0.5f, -0.5f, -0.5f }, Vector3::Up, -Vector3::Right, (facesToGenerate & (1 << 2)) != 0, blockData.texIdSide);
	PushFace(position + Vector3{ -0.5f, -0.5f,-0.5f }, Vector3::Up, -Vector3::Forward, (facesToGenerate & (1 << 3)) != 0, blockData.texIdSide);

	PushFace(position + Vector3{ -0.5f, 0.5f, 0.5f }, Vector3::Forward, Vector3::Right, (facesToGenerate & (1 << 4)) != 0, blockData.texIdTop); // Top	
	PushFace(position + Vector3{ 0.5f, -0.5f, 0.5f }, Vector3::Forward, -Vector3::Right, (facesToGenerate & (1 << 5)) != 0, blockData.texIdBottom); // Bottom
}

void Chunk::PushFace(const Vector3& position, const Vector3& up, const Vector3& right, bool draw, int uvID)
{
	if (!draw)
	{
		return;
	}

	int blocksPerRow = 16;
	float blockSize = 1.0f / blocksPerRow;
	float xOffset = (float(uvID % blocksPerRow)) / blocksPerRow;
	float yOffset = (float(uvID / blocksPerRow)) / blocksPerRow;

	uint32_t a = m_vertexBuffer.PushVertex({ Utils::ToVec4(position),				{xOffset, yOffset + blockSize} });
	uint32_t b = m_vertexBuffer.PushVertex({ Utils::ToVec4(position + right),		{xOffset + blockSize, yOffset + blockSize} });
	uint32_t c = m_vertexBuffer.PushVertex({ Utils::ToVec4(position + up),			{xOffset, yOffset} });
	uint32_t d = m_vertexBuffer.PushVertex({ Utils::ToVec4(position + up + right),	{xOffset + blockSize, yOffset} });

	m_indexBuffer.PushTriangle(a, c, b);
	m_indexBuffer.PushTriangle(b, c, d);
}

void Chunk::Draw(DeviceResources* deviceRes)
{
	if (m_indexBuffer.Size() == 0) 
	{
		return;
	}

	m_vertexBuffer.Apply(deviceRes, 0);
	m_indexBuffer.Apply(deviceRes);

	deviceRes->GetD3DDeviceContext()->DrawIndexed(m_indexBuffer.Size(), 0, 0);
}

void Chunk::SetNeighbouringChunk(Chunk* chunk, NeighbouringChunkIndex chunkRelativePosition)
{
	m_neighbouringChunks[chunkRelativePosition] = chunk;
}

void Chunk::GenerateBlocksValues(GenerationSettings& generationSettings)
{
	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		int worldX = m_chunkPostion.X * CHUNK_SIZE + x;
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			int worldZ = m_chunkPostion.Z * CHUNK_SIZE + z;

			float stoneNoiseValue = generationSettings.StoneNoiseGenerator->Sample2DNoiseAtPosition(worldX, worldZ);
			float dirtNoiseValue = generationSettings.DirtNoiseGenerator->Sample2DNoiseAtPosition(worldX, worldZ);

			int stoneEndHeight = Utils::Lerp(generationSettings.StoneWidthRange, stoneNoiseValue);
			int dirtEndHeight = stoneEndHeight + Utils::Lerp(generationSettings.DirtWidthRange, dirtNoiseValue);

			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				int worldY = m_chunkPostion.Y * CHUNK_SIZE + y;

				float caveNoiseValue = generationSettings.CaveNoiseGenerator->Sample3DNoiseAtPosition(worldX, worldY, worldZ);
				bool inCave = caveNoiseValue > generationSettings.CaveThreshold;


				BlockId block;

				if (worldY == 0)
				{
					block = BEDROCK;
				}
				else if (worldY < stoneEndHeight)
				{
					block = inCave ? EMPTY : GetStoneBlock(generationSettings, worldX, worldY, worldZ);
				}
				else if (worldY < dirtEndHeight && !inCave)
				{
					block = inCave ? EMPTY : DIRT;
				}
				else if (worldY < generationSettings.WaterHeight)
				{
					block = WATER;
				}
				else if (worldY < dirtEndHeight + 1)
				{
					block = inCave ? EMPTY : GetGrassBlock(generationSettings, worldX, worldY, worldZ);
				}
				else
				{
					block = EMPTY;
				}

				m_blocks[CoordinatesToIndex(x, y, z)] = block;
			}
		}
	}
}

void Chunk::GenerateMesh(DeviceResources* deviceRes)
{
	for (int y = 0; y < CHUNK_SIZE; ++y)
	{
		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				BlockId block = m_blocks[CoordinatesToIndex(x, y, z)];
				if (block == EMPTY)
				{
					continue;
				}
				PushCube({ float(x), float(y), float(z) }, BlockData::Get(block), GetDisplayFaceFlags(x, y, z));
			}
		}
	}

	m_vertexBuffer.Create(deviceRes);
	m_indexBuffer.Create(deviceRes);
}

const BoundingBox& Chunk::GetBounds()
{
	return m_bounds;
}

inline Vector3Int Chunk::IndexToCoordinates(int index)
{
	int z = index / (CHUNK_SIZE * CHUNK_SIZE);
	int rst = index - (z * CHUNK_SIZE * CHUNK_SIZE);
	int y = rst / CHUNK_SIZE;
	return Vector3Int(rst - y * CHUNK_SIZE, y, z);
}

inline int Chunk::CoordinatesToIndex(const Vector3Int& coordinates)
{
	return CoordinatesToIndex(coordinates.X, coordinates.Y, coordinates.Z);
}

inline int Chunk::CoordinatesToIndex(int x, int y, int z)
{
	return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}

byte Chunk::GetDisplayFaceFlags(int x, int y, int z)
{
	byte result = 0;

	result |= ShouldDisplayFace(x, y, z, 0, 0, 1) ? 1 << 0 : 0;
	result |= ShouldDisplayFace(x, y, z, 1, 0, 0) ? 1 << 1 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, 0, -1) ? 1 << 2 : 0;
	result |= ShouldDisplayFace(x, y, z, -1, 0, 0) ? 1 << 3 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, 1, 0) ? 1 << 4 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, -1, 0) ? 1 << 5 : 0;

	return result;
}

bool Chunk::ShouldDisplayFace(int x, int y, int z, int offsetx, int offsetY, int offsetZ)
{
	int newX = x + offsetx;
	int newY = y + offsetY;
	int newZ = z + offsetZ;


	BlockId currentBlock = GetBlockAtPosition(x, y, z);
	BlockId otherBlock = GetBlockAtPosition(newX, newY, newZ);

	if (otherBlock == currentBlock)
	{
		return false;
	}


	if (otherBlock == EMPTY || BlockData::Get(otherBlock).transparent)
	{
		return true;
	}
	return false;
}

const BlockId& Chunk::GetBlockAtPosition(int x, int y, int z)
{
	if (x < 0)
	{
		Chunk* otherChunk = m_neighbouringChunks[NeighbouringChunkIndex::X_NEG];
		return otherChunk != nullptr ? otherChunk->GetBlockAtPosition(x + CHUNK_SIZE, y, z) : EMPTY;
	}
	if (CHUNK_SIZE <= x)
	{
		Chunk* otherChunk = m_neighbouringChunks[NeighbouringChunkIndex::X_POS];
		return otherChunk != nullptr ? otherChunk->GetBlockAtPosition(x - CHUNK_SIZE, y, z) : EMPTY;
	}

	if (y < 0)
	{
		Chunk* otherChunk = m_neighbouringChunks[NeighbouringChunkIndex::Y_NEG];
		return otherChunk != nullptr ? otherChunk->GetBlockAtPosition(x, y + CHUNK_SIZE, z) : EMPTY;
	}
	if (CHUNK_SIZE <= y)
	{
		Chunk* otherChunk = m_neighbouringChunks[NeighbouringChunkIndex::Y_POS];
		return otherChunk != nullptr ? otherChunk->GetBlockAtPosition(x, y - CHUNK_SIZE, z) : EMPTY;
	}

	if (z < 0)
	{
		Chunk* otherChunk = m_neighbouringChunks[NeighbouringChunkIndex::Z_NEG];
		return otherChunk != nullptr ? otherChunk->GetBlockAtPosition(x, y, z + CHUNK_SIZE) : EMPTY;
	}
	if (CHUNK_SIZE <= z)
	{
		Chunk* otherChunk = m_neighbouringChunks[NeighbouringChunkIndex::Z_POS];
		return otherChunk != nullptr ? otherChunk->GetBlockAtPosition(x, y, z - CHUNK_SIZE) : EMPTY;
	}

	return m_blocks[CoordinatesToIndex(x, y, z)];
}

BlockId Chunk::GetStoneBlock(GenerationSettings& generationSettings, int worldX, int worldY, int worldZ)
{
	if (generationSettings.CoalGenerator.ShouldGenerateOre(worldX, worldY, worldZ))
	{
		return COAL;
	}
	if (generationSettings.IronGenerator.ShouldGenerateOre(worldX, worldY, worldZ))
	{
		return IRON_ORE;
	}
	if (generationSettings.GoldGenerator.ShouldGenerateOre(worldX, worldY, worldZ))
	{
		return GOLD_ORE;
	}
	if (generationSettings.DiamondGenerator.ShouldGenerateOre(worldX, worldY, worldZ))
	{
		return DIAMOND_ORE;
	}
	if (generationSettings.RedstoneGenerator.ShouldGenerateOre(worldX, worldY, worldZ))
	{
		return REDSTONE_ORE;
	}
	float undergroundDirtNoiseValue = generationSettings.UndergroundDirtNoiseGenerator->Sample3DNoiseAtPosition(worldX, worldY, worldZ);
	if (undergroundDirtNoiseValue > generationSettings.UndergroundDirtThreshold)
	{
		return DIRT;
	}

	float undergroundGravelNoiseValue = generationSettings.UndergroundGravelNoiseGenerator->Sample3DNoiseAtPosition(worldX, worldY, worldZ);
	if (undergroundGravelNoiseValue > generationSettings.UndergroundGravelThreshold)
	{
		return GRAVEL;
	}
	return STONE;
}

BlockId Chunk::GetGrassBlock(GenerationSettings& generationSettings, int worldX, int worldY, int worldZ)
{
	int snowHeight = std::clamp(worldY, generationSettings.SnowHeightRange.X, generationSettings.SnowHeightRange.Y);
	float snowWeight = Utils::InverseLerp(snowHeight, generationSettings.SnowHeightRange);

	bool isSnowy = generationSettings.SnowNoiseGenerator->Sample2DNoiseAtPosition(worldX, worldZ) * snowWeight > generationSettings.SnowThreshold;
	return isSnowy ? SNOWY_GRASS : GRASS;
}
