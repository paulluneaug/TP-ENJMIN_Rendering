#include "pch.h"
#include "GenerationSettings.h"
#include "Noise/PerlinNoiseGenerator.h"
#include "Noise/VoronoiNoiseGenerator.h"

GenerationSettings::GenerationSettings()
{
	std::vector<PerlinNoiseGenerator::NoiseLayer> stoneNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 8.0f, 32 },
		PerlinNoiseGenerator::NoiseLayer{ 2.0f, 16 },
		PerlinNoiseGenerator::NoiseLayer{ 1.0f, 8 },
	};
	StoneNoiseGenerator = new PerlinNoiseGenerator(stoneNoiseLayers);
	StoneWidthRange = { 10, 80 };

	std::vector<PerlinNoiseGenerator::NoiseLayer> dirtNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 8.0f, 10 }
	};
	DirtNoiseGenerator = new PerlinNoiseGenerator(dirtNoiseLayers);
	DirtWidthRange = { 1, 5 };


	std::vector<PerlinNoiseGenerator::NoiseLayer> caveNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 8.0f, 16},
		PerlinNoiseGenerator::NoiseLayer{ 4.0f, 8 },
		PerlinNoiseGenerator::NoiseLayer{ 2.0f, 4 },
	};
	CaveNoiseGenerator = new PerlinNoiseGenerator(caveNoiseLayers);
	CaveThreshold = 0.6f;

	std::vector<PerlinNoiseGenerator::NoiseLayer> snowNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 8.0f, 8},
	};
	SnowNoiseGenerator = new PerlinNoiseGenerator(snowNoiseLayers);
	SnowThreshold = 0.1f;
	SnowHeightRange = {45, 60};

	std::vector<PerlinNoiseGenerator::NoiseLayer> undergroundDirtNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 4.0f, 8 }
	};
	UndergroundDirtNoiseGenerator = new PerlinNoiseGenerator(undergroundDirtNoiseLayers);
	UndergroundDirtThreshold = 0.6f;


	std::vector<PerlinNoiseGenerator::NoiseLayer> undergroundGravelNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 4.0f, 8, 200 }
	};
	UndergroundGravelNoiseGenerator = new PerlinNoiseGenerator(undergroundGravelNoiseLayers);
	UndergroundGravelThreshold = 0.6f;

	WaterHeight = 40;

	CreateOreGenerators();
}

GenerationSettings::~GenerationSettings()
{
	delete StoneNoiseGenerator;
	delete DirtNoiseGenerator;
	delete CaveNoiseGenerator;
}

void GenerationSettings::CreateOreGenerators()
{
	// Coal
	{
		CoalGenerator.OreGenerationRange = { 10, 50, 90 };

		std::vector<PerlinNoiseGenerator::NoiseLayer> perlinNoiseLayers =
		{
			PerlinNoiseGenerator::NoiseLayer{ 4.0f, 4, 400 }
		};
		CoalGenerator.PerlinNoiseGenerator = new PerlinNoiseGenerator(perlinNoiseLayers);
		CoalGenerator.PerlinTreshold = 0.65f;

		std::vector<VoronoiNoiseGenerator::NoiseLayer> voronoiNoiseLayers =
		{
			VoronoiNoiseGenerator::NoiseLayer{ 4.0f, 4, 0, false, true }
		};
		CoalGenerator.VoronoiNoiseGenerator = new VoronoiNoiseGenerator(voronoiNoiseLayers);
		CoalGenerator.VoronoiTreshold = 0.1f;
	}

	// Iron
	{
		IronGenerator.OreGenerationRange = { 1, 35, 90 };

		std::vector<PerlinNoiseGenerator::NoiseLayer> perlinNoiseLayers =
		{
			PerlinNoiseGenerator::NoiseLayer{ 4.0f, 3, 500 }
		};
		IronGenerator.PerlinNoiseGenerator = new PerlinNoiseGenerator(perlinNoiseLayers);
		IronGenerator.PerlinTreshold = 0.66f;

		std::vector<VoronoiNoiseGenerator::NoiseLayer> voronoiNoiseLayers =
		{
			VoronoiNoiseGenerator::NoiseLayer{ 4.0f, 4, 30, false, true }
		};
		IronGenerator.VoronoiNoiseGenerator = new VoronoiNoiseGenerator(voronoiNoiseLayers);
		IronGenerator.VoronoiTreshold = 0.1f;
	}

	// Gold
	{
		GoldGenerator.OreGenerationRange = { 0, 10, 30 };

		std::vector<PerlinNoiseGenerator::NoiseLayer> perlinNoiseLayers =
		{
			PerlinNoiseGenerator::NoiseLayer{ 4.0f, 4, 1100 }
		};
		GoldGenerator.PerlinNoiseGenerator = new PerlinNoiseGenerator(perlinNoiseLayers);
		GoldGenerator.PerlinTreshold = 0.68f;

		std::vector<VoronoiNoiseGenerator::NoiseLayer> voronoiNoiseLayers =
		{
			VoronoiNoiseGenerator::NoiseLayer{ 4.0f, 4, 60, false, true }
		};
		GoldGenerator.VoronoiNoiseGenerator = new VoronoiNoiseGenerator(voronoiNoiseLayers);
		GoldGenerator.VoronoiTreshold = 0.1f;
	}

	// Redstone
	{
		RedstoneGenerator.OreGenerationRange = { 0, 10, 60 };

		std::vector<PerlinNoiseGenerator::NoiseLayer> perlinNoiseLayers =
		{
			PerlinNoiseGenerator::NoiseLayer{ 4.0f, 3, 800 }
		};
		RedstoneGenerator.PerlinNoiseGenerator = new PerlinNoiseGenerator(perlinNoiseLayers);
		RedstoneGenerator.PerlinTreshold = 0.65f;

		std::vector<VoronoiNoiseGenerator::NoiseLayer> voronoiNoiseLayers =
		{
			VoronoiNoiseGenerator::NoiseLayer{ 4.0f, 16, 70, false, true }
		};
		RedstoneGenerator.VoronoiNoiseGenerator = new VoronoiNoiseGenerator(voronoiNoiseLayers);
		RedstoneGenerator.VoronoiTreshold = 0.1f;
	}

	// Diamond
	{
		DiamondGenerator.OreGenerationRange = { 0, 5, 12 };

		std::vector<PerlinNoiseGenerator::NoiseLayer> perlinNoiseLayers =
		{
			PerlinNoiseGenerator::NoiseLayer{ 4.0f, 4, 1000 }
		};
		DiamondGenerator.PerlinNoiseGenerator = new PerlinNoiseGenerator(perlinNoiseLayers);
		DiamondGenerator.PerlinTreshold = 0.68f;

		std::vector<VoronoiNoiseGenerator::NoiseLayer> voronoiNoiseLayers =
		{
			VoronoiNoiseGenerator::NoiseLayer{ 4.0f, 4, 60, false, true }
		};
		DiamondGenerator.VoronoiNoiseGenerator = new VoronoiNoiseGenerator(voronoiNoiseLayers);
		DiamondGenerator.VoronoiTreshold = 0.1f;
	}
	
}
