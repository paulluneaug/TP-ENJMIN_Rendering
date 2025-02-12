#pragma once
#include "Engine/Vector3Int.h"
#include "Noise/INoiseGenerator.h"

class OreGenerator
{
public :
	/// <summary>
	/// X = 0
	/// Y = Max
	/// Z = 0
	/// </summary>
	Vector3Int OreGenerationRange;
	INoiseGenerator* PerlinNoiseGenerator;
	float PerlinTreshold;
	INoiseGenerator* VoronoiNoiseGenerator;
	float VoronoiTreshold;

public :
	bool ShouldGenerateOre(int worldX, int worldY, int worldZ);


};

