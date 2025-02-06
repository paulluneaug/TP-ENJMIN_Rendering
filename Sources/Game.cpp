//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <iostream>

#include "Engine/Shader.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/VertexLayout.h"
#include "PerlinNoise.hpp"

#include "Engine/Buffers.h"
#include <Engine/Chunk.h>
#include <Minicraft/Camera.h>
#include <Minicraft/World.h>

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

using VertexLayout = VertexLayout_PositionUV;

struct CameraData
{
	Matrix View;
	Matrix Projection;
};

// Global stuff
Shader* basicShader;

//ConstantBuffer<CameraData> m_constantBufferCamera;

World m_world;

Camera m_camera{ 60, 1.0f };

Texture m_texture(L"terrain");

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
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

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());
	GenerateInputLayout<VertexLayout>(m_deviceResources.get(), basicShader);

	m_texture.Create(m_deviceResources.get());

	m_camera.UpdateAspectRatio(float(width) / float(height));

	m_world.Generate(m_deviceResources.get());
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	auto const kb = m_keyboard->GetState();

	// add kb/mouse interact here

	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);

	auto context = m_deviceResources->GetD3DDeviceContext();

	float cameraSpeed = 1.0f;
	float cameraRadius = 4.0f;

	float seconds = timer.GetTotalSeconds();
	Vector3 position = Vector3
	{
		std::cosf(seconds * cameraSpeed) * cameraRadius,
		std::cosf(2 * seconds * cameraSpeed) * cameraRadius,
		std::sinf(seconds * cameraSpeed) * cameraRadius
	};

	m_camera.Update(timer.GetElapsedSeconds(), kb, m_mouse.get());
	//m_constantBufferCamera.Data.View = Matrix::CreateLookAt(position, Vector3::Zero, Vector3::Up).Transpose();
}

// Draws the scene.
void Game::Render() {
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

	ApplyInputLayout<VertexLayout>(m_deviceResources.get());

	basicShader->Apply(m_deviceResources.get());

	m_camera.ApplyCamera(m_deviceResources.get());
	// m_constantBufferCamera.UpdateBuffer(m_deviceResources.get());
	// m_constantBufferCamera.ApplyToVS(m_deviceResources.get(), 1);

	m_texture.Apply(m_deviceResources.get());

	m_world.Draw(m_deviceResources.get());


	// envoie nos commandes au GPU pour etre afficher � l'�cran
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
	m_camera.UpdateAspectRatio(float(width) / float(height));
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
