#pragma once
#include "INoiseGenerator.h"

#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class VoronoiNoiseGenerator :
	public INoiseGenerator
{
public:
	struct NoiseLayer
	{
		float LayerWeight;
		int Scale;
		int GradientOffset;

		bool UseSmootherStep;
		bool Inverse;

		bool SameCellSameValue;

		NoiseLayer(float layerWeight, int scale, int gradientOffset = 0,  bool useSmootherStep = false, bool inverse = false, bool sameCellSameValue = false) :
			LayerWeight(layerWeight),
			Scale(scale),
			GradientOffset(gradientOffset),
			UseSmootherStep(useSmootherStep),
			Inverse(inverse),
			SameCellSameValue(sameCellSameValue)
		{
		}
	};

private:
	std::vector<NoiseLayer> m_layers;
	float m_layersWeightsMultiplier;


public:
	VoronoiNoiseGenerator(std::vector<NoiseLayer> layers);

	float Sample3DNoiseAtPosition(int xPos, int yPos, int zPos);
	float Sample2DNoiseAtPosition(int xPos, int yPos);

private:
	float SqrDist(float ax, float ay, float bx, float by);
	float SqrDist(float ax, float ay, float az, float bx, float by, float bz);
	static Vector2 GetCellPointCoordinates(int cellX, int cellY, uint32_t cellSeed);
	static Vector3 GetCellPointCoordinates(int cellX, int cellY, int cellZ, uint32_t cellSeed);
};

