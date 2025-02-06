#pragma once
#include "INoiseGenerator.h"

class PerlinNoiseGenerator : public INoiseGenerator
{
public:
    struct NoiseLayer
    {
        float LayerWeigth;

        int GradientOffset;
        int NoiseScale;

        bool UseSmootherStep;

        NoiseLayer(float layerWeigth, int noiseScale, int gradientOffset = 0, bool useSmootherStep = false)
            : LayerWeigth(layerWeigth), GradientOffset(gradientOffset), NoiseScale(noiseScale), UseSmootherStep(useSmootherStep)
        {
        }
    };

private:
    std::vector<NoiseLayer> m_layers;
    float m_layersWeightsMultiplier;


public:
    PerlinNoiseGenerator(std::vector<NoiseLayer> layers);

    float Sample3DNoiseAtPosition(int xPos, int yPos, int zPos);
    float Sample2DNoiseAtPosition(int xPos, int yPos);
};

