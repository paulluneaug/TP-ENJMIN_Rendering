#pragma once
class INoiseGenerator
{
public:
    virtual float Sample3DNoiseAtPosition(int xPos, int yPos, int zPos) = 0;
    virtual float Sample2DNoiseAtPosition(int xPos, int yPos) = 0;
};

