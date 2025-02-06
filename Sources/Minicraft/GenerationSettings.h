#pragma once
#include "Noise/INoiseGenerator.h"
#include "Engine/Vector2Int.h"

class GenerationSettings
{
public:
	INoiseGenerator* StoneNoiseGenerator;
	Vector2Int StoneWidthRange;

	INoiseGenerator* DirtNoiseGenerator;
	Vector2Int DirtWidthRange;

	INoiseGenerator* CaveNoiseGenerator;
	float CaveThreshold;

	GenerationSettings();
	~GenerationSettings();


};

