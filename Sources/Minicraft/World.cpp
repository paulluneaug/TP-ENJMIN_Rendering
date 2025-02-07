#include "pch.h"
#include "World.h"

const Vector3Int& World::WORLD_SIZE = Vector3Int{ 8, 8, 8 };

World::World()
{
}

World::~World()
{
	for (std::pair<Vector3Int, Chunk*> pair : m_chunks)
	{
		delete pair.second;
	}
	m_chunks.clear();
}

void World::Generate(DeviceResources* deviceRes)
{
	m_constantBufferModel.Create(deviceRes);
	GenerateChunksValues();
	GenerateChunksMeshes(deviceRes);
}

void World::Draw(DeviceResources* deviceRes)
{
	for (std::pair<Vector3Int, Chunk*> pair : m_chunks)
	{
		Chunk* chunk = pair.second;

		m_constantBufferModel.Data.Model = chunk->ModelMatrix.Transpose();
		m_constantBufferModel.UpdateBuffer(deviceRes);
		m_constantBufferModel.ApplyToVS(deviceRes, 0);

		chunk->Draw(deviceRes);
	}
}

Chunk* World::CreateChunkAtPosition(const Vector3Int& position)
{
	if (m_chunks.count(position) == 1) 
	{
		return m_chunks.at(position);
	}

	Chunk* newChunk = new Chunk{ position };
	SetNeighbouringChunks(position, newChunk);

	m_chunks.insert_or_assign(position, newChunk);

	return newChunk;
}

void World::GenerateChunksValues()
{
	for (int x = 0; x < WORLD_SIZE.X; ++x)
	{
		for (int y = 0; y < WORLD_SIZE.Y; ++y)
		{
			for (int z = 0; z < WORLD_SIZE.Z; ++z)
			{
				Chunk* newChunk = CreateChunkAtPosition({ x, y, z });
				newChunk->GenerateBlocksValues(m_generationSettings);
			}
		}
	}
}

void World::GenerateChunksMeshes(DeviceResources* deviceRes)
{
	for (std::pair<Vector3Int, Chunk*> pair : m_chunks) 
	{
		pair.second->GenerateMesh(deviceRes);
	}
}

void World::SetNeighbouringChunks(const Vector3Int& position, Chunk* newChunk)
{
	Vector3Int xPos = position + Vector3Int(1, 0, 0);
	if (m_chunks.count(xPos) == 1)
	{
		Chunk* other = m_chunks.at(xPos);
		newChunk->SetNeighbouringChunk(other, Chunk::NeighbouringChunkIndex::X_POS);
		other->SetNeighbouringChunk(newChunk, Chunk::NeighbouringChunkIndex::X_NEG);
	}
	Vector3Int xNeg = position + Vector3Int(-1, 0, 0);
	if (m_chunks.count(xNeg) == 1)
	{
		Chunk* other = m_chunks.at(xNeg);
		newChunk->SetNeighbouringChunk(other, Chunk::NeighbouringChunkIndex::X_NEG);
		other->SetNeighbouringChunk(newChunk, Chunk::NeighbouringChunkIndex::X_POS);
	}

	Vector3Int yPos = position + Vector3Int(0, 1, 0);
	if (m_chunks.count(yPos) == 1)
	{
		Chunk* other = m_chunks.at(yPos);
		newChunk->SetNeighbouringChunk(other, Chunk::NeighbouringChunkIndex::Y_POS);
		other->SetNeighbouringChunk(newChunk, Chunk::NeighbouringChunkIndex::Y_NEG);
	}
	Vector3Int yNeg = position + Vector3Int(0, -1, 0);
	if (m_chunks.count(yNeg) == 1)
	{
		Chunk* other = m_chunks.at(yNeg);
		newChunk->SetNeighbouringChunk(other, Chunk::NeighbouringChunkIndex::Y_NEG);
		other->SetNeighbouringChunk(newChunk, Chunk::NeighbouringChunkIndex::Y_POS);
	}

	Vector3Int zPos = position + Vector3Int(0, 0, 1);
	if (m_chunks.count(zPos) == 1)
	{
		Chunk* other = m_chunks.at(zPos);
		newChunk->SetNeighbouringChunk(other, Chunk::NeighbouringChunkIndex::Z_POS);
		other->SetNeighbouringChunk(newChunk, Chunk::NeighbouringChunkIndex::Z_NEG);
	}
	Vector3Int zNeg = position + Vector3Int(0, 0, -1);
	if (m_chunks.count(zNeg) == 1)
	{
		Chunk* other = m_chunks.at(zNeg);
		newChunk->SetNeighbouringChunk(other, Chunk::NeighbouringChunkIndex::Z_NEG);
		other->SetNeighbouringChunk(newChunk, Chunk::NeighbouringChunkIndex::Z_POS);
	}
}