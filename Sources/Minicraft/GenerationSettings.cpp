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
	StoneWidthRange = { 20, 60 };

	std::vector<PerlinNoiseGenerator::NoiseLayer> dirtNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 8.0f, 10 }
	};
	DirtNoiseGenerator = new PerlinNoiseGenerator(dirtNoiseLayers);
	DirtWidthRange = { 1, 5 };


	std::vector<PerlinNoiseGenerator::NoiseLayer> caveNoiseLayers =
	{
		PerlinNoiseGenerator::NoiseLayer{ 8.0f, 0, 16, false },
		PerlinNoiseGenerator::NoiseLayer{ 4.0f, 0, 8, false },
		PerlinNoiseGenerator::NoiseLayer{ 2.0f, 0, 4, false },
	};
	CaveNoiseGenerator = new PerlinNoiseGenerator(caveNoiseLayers);
	CaveThreshold = 0.6f;


}

GenerationSettings::~GenerationSettings()
{
	delete StoneNoiseGenerator;
	delete DirtNoiseGenerator;
	delete CaveNoiseGenerator;
}
