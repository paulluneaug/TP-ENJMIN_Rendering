#include "pch.h"
#include "OreGenerator.h"
#include "Engine/Vector2Int.h"
#include <Engine/Utils.h>

bool OreGenerator::ShouldGenerateOre(int worldX, int worldY, int worldZ)
{
    float heightMultiplier;

    if (worldY < OreGenerationRange.X || worldY > OreGenerationRange.Z) 
    {
        return false;
    }

    Vector2Int range;
    if (worldY < OreGenerationRange.Y)
    {
        range = { OreGenerationRange.X, OreGenerationRange.Y };
    }
    else 
    {
        range = { OreGenerationRange.Y, OreGenerationRange.Z };
    }
    heightMultiplier = Utils::InverseLerp(worldY, range);

    float perlinNoiseValue = PerlinNoiseGenerator->Sample3DNoiseAtPosition(worldX, worldY, worldZ);
    float voronoiNoiseValue = VoronoiNoiseGenerator->Sample3DNoiseAtPosition(worldX, worldY, worldZ);

    if (perlinNoiseValue < PerlinTreshold) 
    {
        return false;
    }
    return voronoiNoiseValue * heightMultiplier > VoronoiTreshold;
}
