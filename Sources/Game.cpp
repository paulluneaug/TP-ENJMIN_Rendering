//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"
#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Engine/Texture.h"
#include "Minicraft/World.h"
#include "Minicraft/Camera.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

float sign(float v) {
	if (v < 0) return -1;
	else return 1;
}

std::vector<std::array<int, 3>> Raycast(Vector3 pos, Vector3 dir, float maxDist) {
	std::map<float, std::array<int, 3>> cubes;

	if (dir.x != 0) {
		float deltaYX = dir.y / dir.x;
		float deltaZX = dir.z / dir.x;
		float offsetYX = pos.y - pos.x * deltaYX;
		float offsetZX = pos.z - pos.x * deltaZX;

		float cubeX = (dir.x > 0) ? ceil(pos.x) : floor(pos.x);
		do {
			Vector3 collision(cubeX, deltaYX * cubeX + offsetYX, deltaZX * cubeX + offsetZX);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDist) break;

			cubes[dist] = {
				(int)floor(cubeX - ((dir.x < 0) ? 1 : 0)),
				(int)floor(collision.y),
				(int)floor(collision.z)
			};
			cubeX = cubeX + sign(dir.x);
		} while (true);
	}
	if (dir.y != 0) {
		float deltaXY = dir.x / dir.y;
		float deltaZY = dir.z / dir.y;
		float offsetXY = pos.x - pos.y * deltaXY;
		float offsetZY = pos.z - pos.y * deltaZY;

		float cubeY = (dir.y > 0) ? ceil(pos.y) : floor(pos.y);
		do {
			Vector3 collision(deltaXY * cubeY + offsetXY, cubeY, deltaZY * cubeY + offsetZY);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDist) break;

			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(cubeY - ((dir.y < 0) ? 1 : 0)),
				(int)floor(collision.z)
			};
			cubeY = cubeY + sign(dir.y);
		} while (true);
	}
	if (dir.z != 0) {
		float deltaXZ = dir.x / dir.z;
		float deltaYZ = dir.y / dir.z;
		float offsetXZ = pos.x - pos.z * deltaXZ;
		float offsetYZ = pos.y - pos.z * deltaYZ;

		float cubeZ = (dir.z > 0) ? ceil(pos.z) : floor(pos.z);
		do {
			Vector3 collision(deltaXZ * cubeZ + offsetXZ, deltaYZ * cubeZ + offsetYZ, cubeZ);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDist) break;

			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(collision.y),
				(int)floor(cubeZ - ((dir.z < 0) ? 1 : 0)),
			};
			cubeZ = cubeZ + sign(dir.z);
		} while (true);
	}

	std::vector<std::array<int, 3>> res;
	std::transform(
		cubes.begin(), cubes.end(),
		std::back_inserter(res),
		[](auto& v) { return v.second; });
	return res;
}

// Global stuff
Shader basicShader(L"Basic");
Shader blockShader(L"Block");
VertexBuffer<VertexLayout_PositionColor> debugLine;

Texture texture(L"terrain");
Camera camera(75, 1);
World world;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader.Create(m_deviceResources.get());
	blockShader.Create(m_deviceResources.get());
	GenerateInputLayout<VertexLayout_PositionColor>(m_deviceResources.get(), &basicShader);
	GenerateInputLayout<VertexLayout_PositionUV>(m_deviceResources.get(), &blockShader);

	texture.Create(m_deviceResources.get());

	camera.UpdateAspectRatio((float)width / (float)height);

	world.Generate(m_deviceResources.get());

	Vector3 dir(-0.5, -0.8, -0.2);
	dir.Normalize();
	auto res = Raycast(Vector3(20, 15, 20) + Vector3(0.5, 0.5, 0.5), dir, 20);

	for (auto& cube : res) {
		world.UpdateBlock(cube[0], cube[1], cube[2], STONE);
	}

	debugLine.PushVertex({ {20, 15, 20, 1}, {1, 0, 0, 1} });
	debugLine.PushVertex({ Vector4(20, 15, 20, 1) + Vector4(dir.x, dir.y, dir.z, 0) * 20, {0, 0, 0, 1}});
	debugLine.Create(m_deviceResources.get());
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render(m_timer);
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	
	camera.Update(timer.GetElapsedSeconds(), kb, m_mouse.get());
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render(DX::StepTimer const& timer) {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ApplyInputLayout<VertexLayout_PositionUV>(m_deviceResources.get());

	camera.ApplyCamera(m_deviceResources.get());

	blockShader.Apply(m_deviceResources.get());
	texture.Apply(m_deviceResources.get());
	world.Draw(&camera, m_deviceResources.get());

	// todo debug line
	ApplyInputLayout<VertexLayout_PositionColor>(m_deviceResources.get());
	basicShader.Apply(m_deviceResources.get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	debugLine.Apply(m_deviceResources.get());
	context->Draw(2, 0);

	m_deviceResources->Present();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
	camera.UpdateAspectRatio((float)width / (float)height);
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion
