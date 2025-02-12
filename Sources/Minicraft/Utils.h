#pragma once

using namespace DirectX::SimpleMath;

std::vector<std::array<int, 3>> Raycast(Vector3 pos, Vector3 dir, float maxDist);