#pragma once
#include "Noise/INoiseGenerator.h"
#include "Engine/Vector2Int.h"
#include "OreGenerator.h"


class GenerationSettings
{
public:
	INoiseGenerator* StoneNoiseGenerator;
	Vector2Int StoneWidthRange;

	INoiseGenerator* DirtNoiseGenerator;
	Vector2Int DirtWidthRange;

	INoiseGenerator* CaveNoiseGenerator;
	float CaveThreshold;

	INoiseGenerator* SnowNoiseGenerator;
	float SnowThreshold;
	/// <summary>
	/// Below x chance = 0
	/// Above y chance = 1
	/// </summary>
	Vector2Int SnowHeightRange;

	INoiseGenerator* UndergroundGravelNoiseGenerator;
	float UndergroundGravelThreshold;

	INoiseGenerator* UndergroundDirtNoiseGenerator;
	float UndergroundDirtThreshold;

	int WaterHeight;

	// Ore Generators
	OreGenerator CoalGenerator;
	OreGenerator IronGenerator;
	OreGenerator GoldGenerator;
	OreGenerator RedstoneGenerator;
	OreGenerator DiamondGenerator;

	GenerationSettings();
	~GenerationSettings();

private :
	void CreateOreGenerators();
};

