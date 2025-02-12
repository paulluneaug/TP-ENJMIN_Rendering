#pragma once

#include "Engine/Camera.h"
#include "Minicraft/World.h"

using namespace DirectX::SimpleMath;

class Player {
	World* world = nullptr;

	Vector3 position = Vector3();
	float velocityY = 0;

	float walkSpeed = 10.0f;

	PerspectiveCamera camera = PerspectiveCamera(75, 1);

	DirectX::Mouse::ButtonStateTracker      mouseTracker;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
public:
	Player(World* w, Vector3 pos) : world(w), position(pos) {}
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms);

	PerspectiveCamera* GetCamera() { return &camera; }
};
