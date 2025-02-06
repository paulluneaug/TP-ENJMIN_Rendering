#include "pch.h"
#include "PerlinNoiseGenerator.h"
#include "NoiseUtils.h"

PerlinNoiseGenerator::PerlinNoiseGenerator(std::vector<NoiseLayer> layers) :
	m_layers(layers)
{
    float layersWeightsSum = 0.0f;
    for (NoiseLayer& layer : m_layers) 
    {
        layersWeightsSum += layer.LayerWeigth;
    }
    m_layersWeightsMultiplier = 1.0f / layersWeightsSum;
}

float PerlinNoiseGenerator::Sample3DNoiseAtPosition(int xPos, int yPos, int zPos)
{
    float value = 0;

    for (size_t i = 0; i < m_layers.size(); i++)
    {
        NoiseLayer currentLayer = m_layers[i];

        float x = float(xPos) / currentLayer.NoiseScale;
        float y = float(yPos) / currentLayer.NoiseScale;
        float z = float(zPos) / currentLayer.NoiseScale;

        // Determine grid cell coordinates
        int x0 = int(floor(x));
        int x1 = x0 + 1;
        int y0 = int(floor(y));
        int y1 = y0 + 1;
        int z0 = int(floor(z));
        int z1 = z0 + 1;

        // Determine interpolation weights
        // Could also use higher order polynomial/s-curve here
        float sx = x - (float)x0;
        float sy = y - (float)y0;
        float sz = z - (float)z0;

        // Interpolate between grid point gradients
        float n0 = NoiseUtils::DotGridGradient3D(x0, y0, z0, x, y, z, currentLayer.GradientOffset);
        float n1 = NoiseUtils::DotGridGradient3D(x1, y0, z0, x, y, z, currentLayer.GradientOffset);
        float iy0z0 = NoiseUtils::Interpolate(n0, n1, sx, currentLayer.UseSmootherStep);

        n0 = NoiseUtils::DotGridGradient3D(x0, y1, z0, x, y, z, currentLayer.GradientOffset);
        n1 = NoiseUtils::DotGridGradient3D(x1, y1, z0, x, y, z, currentLayer.GradientOffset);
        float iy1z0 = NoiseUtils::Interpolate(n0, n1, sx, currentLayer.UseSmootherStep);

        float iz0 = NoiseUtils::Interpolate(iy0z0, iy1z0, sy, currentLayer.UseSmootherStep);


        n0 = NoiseUtils::DotGridGradient3D(x0, y0, z1, x, y, z, currentLayer.GradientOffset);
        n1 = NoiseUtils::DotGridGradient3D(x1, y0, z1, x, y, z, currentLayer.GradientOffset);
        float iy0z1 = NoiseUtils::Interpolate(n0, n1, sx, currentLayer.UseSmootherStep);

        n0 = NoiseUtils::DotGridGradient3D(x0, y1, z1, x, y, z, currentLayer.GradientOffset);
        n1 = NoiseUtils::DotGridGradient3D(x1, y1, z1, x, y, z, currentLayer.GradientOffset);
        float iy1z1 = NoiseUtils::Interpolate(n0, n1, sx, currentLayer.UseSmootherStep);

        float iz1 = NoiseUtils::Interpolate(iy0z1, iy1z1, sy, currentLayer.UseSmootherStep);

        float layerValue = NoiseUtils::Interpolate(iz0, iz1, sz, currentLayer.UseSmootherStep);

        layerValue = layerValue / 2 + 0.5f;

        value += layerValue * currentLayer.LayerWeigth * m_layersWeightsMultiplier;
    }

    return value;
}

float PerlinNoiseGenerator::Sample2DNoiseAtPosition(int xPos, int yPos)
{
    float value = 0;

    for (size_t i = 0; i < m_layers.size(); i++)
    {
        NoiseLayer currentLayer = m_layers[i];

        float x = float(xPos) / currentLayer.NoiseScale;
        float y = float(yPos) / currentLayer.NoiseScale;

        // Determine grid cell coordinates
        int x0 = int(floor(x));
        int x1 = x0 + 1;
        int y0 = int(floor(y));
        int y1 = y0 + 1;

        // Determine interpolation weights
        // Could also use higher order polynomial/s-curve here
        float sx = x - (float)x0;
        float sy = y - (float)y0;

        // Interpolate between grid point gradients
        float n0 = NoiseUtils::DotGridGradient2D(x0, y0, x, y, currentLayer.GradientOffset);
        float n1 = NoiseUtils::DotGridGradient2D(x1, y0, x, y, currentLayer.GradientOffset);
        float ix0 = NoiseUtils::Interpolate(n0, n1, sx, currentLayer.UseSmootherStep);

        n0 = NoiseUtils::DotGridGradient2D(x0, y1, x, y, currentLayer.GradientOffset);
        n1 = NoiseUtils::DotGridGradient2D(x1, y1, x, y, currentLayer.GradientOffset);
        float ix1 = NoiseUtils::Interpolate(n0, n1, sx, currentLayer.UseSmootherStep);

        float layerValue = NoiseUtils::Interpolate(ix0, ix1, sy, currentLayer.UseSmootherStep);

        layerValue = layerValue / 2 + 0.5f;

        value += layerValue * currentLayer.LayerWeigth * m_layersWeightsMultiplier;
    }
    return value;
}
