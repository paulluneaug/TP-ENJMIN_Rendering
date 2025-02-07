#include "pch.h"
#include "VoronoiNoiseGenerator.h"
#include <algorithm>

#include "NoiseUtils.h"

VoronoiNoiseGenerator::VoronoiNoiseGenerator(std::vector<NoiseLayer> layers):
    m_layers(layers)
{
    float layersWeightsSum = 0.0f;
    for (NoiseLayer& layer : m_layers)
    {
        layersWeightsSum += layer.LayerWeight;
    }
    m_layersWeightsMultiplier = 1.0f / layersWeightsSum;
}

float VoronoiNoiseGenerator::Sample3DNoiseAtPosition(int xPos, int yPos, int zPos)
{
    float value = 0;

    for (size_t i = 0; i < m_layers.size(); i++)
    {
        NoiseLayer& currentLayer = m_layers.at(i);

        float x = (float)xPos / currentLayer.Scale;
        float y = (float)yPos / currentLayer.Scale;
        float z = (float)zPos / currentLayer.Scale;

        // Determine grid cell coordinates
        int x0 = floor(x);
        int y0 = floor(y);
        int z0 = floor(z);

        float layerValue;

        float nearestDistance = 1000.0f;
        uint32_t nearestSeed;


        for (int jx = -1; jx <= 1; ++jx)
        {
            for (int jy = -1; jy <= 1; ++jy)
            {
                for (int jz = -1; jz <= 1; ++jz)
                {
                    if ((jx | jy | jz) == 0) 
                    {
                        continue;
                    }
                    uint32_t cellSeed = NoiseUtils::HashCoordinates3D(x0 + jx, y0 + jy, z0 + jz, currentLayer.GradientOffset);
                    Vector3 cellPoint = GetCellPointCoordinates(x0 + jx, y0 + jy, z0 + jz, cellSeed);
                    float distance = SqrDist(cellPoint.x, cellPoint.y, cellPoint.z, x, y, z);

                    if (distance < nearestDistance)
                    {
                        nearestDistance = distance;
                        nearestSeed = cellSeed;
                    }
                }
            }
        }


        if (currentLayer.SameCellSameValue)
        {
            layerValue = NoiseUtils::RandomFloat01(nearestSeed);
        }
        else
        {
            layerValue = nearestDistance / 2;
        }

        if (currentLayer.Inverse)
        {
            layerValue = 1.0f - layerValue;
        }

        value += layerValue * currentLayer.LayerWeight * m_layersWeightsMultiplier;
    }
    return value;
}

float VoronoiNoiseGenerator::Sample2DNoiseAtPosition(int xPos, int yPos)
{
    float value = 0;

    for (size_t i = 0; i < m_layers.size(); i++)
    {
        NoiseLayer& currentLayer = m_layers.at(i);

        float x = (float)xPos / currentLayer.Scale;
        float y = (float)yPos / currentLayer.Scale;

        // Determine grid cell coordinates
        int x0 = floor(x);
        int y0 = floor(y);

        float layerValue;

        float nearestDistance = 1000.0f;
        uint32_t nearestSeed;


        for (int jx = -1; jx <= 1; ++jx)
        {
            for (int jy = -1; jy <= 1; ++jy)
            {
                if ((jx | jy) == 0)
                {
                    continue;
                }
                uint32_t cellSeed = NoiseUtils::HashCoordinates2D(x0 + jx, y0 + jy, currentLayer.GradientOffset);
                Vector2 cellPoint = GetCellPointCoordinates(x0 + jx, y0 + jy, cellSeed);
                float distance = SqrDist(cellPoint.x, cellPoint.y, x, y);

                if (distance < nearestDistance) 
                {
                    nearestDistance = distance;
                    nearestSeed = cellSeed;
                }
            }
        }


        if (currentLayer.SameCellSameValue)
        {
            layerValue = NoiseUtils::RandomFloat01(nearestSeed);
        }
        else
        {
            layerValue = nearestDistance / 2;
        }

        if (currentLayer.Inverse)
        {
            layerValue = 1.0f - layerValue;
        }

        value += layerValue * currentLayer.LayerWeight * m_layersWeightsMultiplier;
    }
    return value;
}

float VoronoiNoiseGenerator::SqrDist(float ax, float ay, float bx, float by)
{
    float dx = ax - bx;
    float dy = ay - by;
    return abs(dx * dx + dy * dy);
}

float VoronoiNoiseGenerator::SqrDist(float ax, float ay, float az, float bx, float by, float bz)
{
    float dx = ax - bx;
    float dy = ay - by;
    float dz = az - bz;
    return abs(dx * dx + dy * dy + dz * dz);
}

Vector2 VoronoiNoiseGenerator::GetCellPointCoordinates(int cellX, int cellY, uint32_t cellSeed)
{
    return { cellX + NoiseUtils::RandomFloat01(cellSeed), cellY + NoiseUtils::RandomFloat01(cellSeed) };
}

Vector3 VoronoiNoiseGenerator::GetCellPointCoordinates(int cellX, int cellY, int cellZ, uint32_t cellSeed)
{
    return { cellX + NoiseUtils::RandomFloat01(cellSeed), cellY + NoiseUtils::RandomFloat01(cellSeed), cellZ + NoiseUtils::RandomFloat01(cellSeed) };
}
