//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <iostream>

#include "Engine/Shader.h"
#include "Engine/Shader.h"
#include "Engine/VertexLayout.h"
#include "PerlinNoise.hpp"

#include "Engine/Buffers.h"
#include <Engine/Chunk.h>

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

using VertexLayout = VertexLayout_PositionUV;

struct ModelData
{
	Matrix Model;
};

struct CameraData
{
	Matrix View;
	Matrix Projection;
};

// Global stuff
Shader* basicShader;

int m_verticesCount;
int m_indicesCount;

VertexBuffer<VertexLayout> m_vertexBuffer;
IndexBuffer m_indexBuffer;
ConstantBuffer<ModelData> m_constantBufferModel;
ConstantBuffer<CameraData> m_constantBufferCamera;

Chunk m_chunk;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
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

	m_constantBufferCamera.Data.Projection = Matrix::CreatePerspectiveFieldOfView(75.0f * XM_PI / 180.0f, (float)width / (float)height, 0.01f, 100.0f).Transpose();

	m_constantBufferModel.Create(m_deviceResources.get());
	m_constantBufferCamera.Create(m_deviceResources.get());

	CreateCircle(m_deviceResources->GetD3DDevice(), 0.0f, 0.0f, 0.4f, 12);
	m_vertexBuffer.Create(m_deviceResources.get());
	m_indexBuffer.Create(m_deviceResources.get());

	m_chunk.Generate(m_deviceResources.get());
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();

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
	m_constantBufferCamera.Data.View = Matrix::CreateLookAt(position, Vector3::Zero, Vector3::Up).Transpose();
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

	// TP: Tracer votre vertex buffer ici
	m_vertexBuffer.Apply(m_deviceResources.get());
	m_indexBuffer.Apply(m_deviceResources.get());

	for (float offset = -0.5f; offset < 0.5f; offset += 0.2f) 
	{
		m_constantBufferModel.Data.Model = m_chunk.ModelMatrix.Transpose();

		m_chunk.Draw(m_deviceResources.get());

		// Update matrix
		m_constantBufferModel.UpdateBuffer(m_deviceResources.get());
		m_constantBufferCamera.UpdateBuffer(m_deviceResources.get());

		m_constantBufferModel.ApplyToVS(m_deviceResources.get(), 0);
		m_constantBufferCamera.ApplyToVS(m_deviceResources.get(), 1);
	}


	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
}

void Game::CreateCircle(ID3D11Device1* device, float cx, float cy, float radius, int resolution)
{
	resolution = std::max(3, resolution);

	float angleStep = XM_2PI / resolution;

	for (int i = 0; i < resolution; ++i)
	{
		float angle = angleStep * i;
		float x = std::cosf(angle) * radius + cx;
		float y = std::sinf(angle) * radius + cy;

		m_vertexBuffer.PushVertex({ {x, y, 0.0f, 1.0f}, {x / 2 + 0.5f, y / 2 + 0.5f} });
		m_indexBuffer.PushTriangle(resolution, (i + 1) % resolution, i);
	}

	m_vertexBuffer.PushVertex({ {cx, cy, 0.0f, 1.0f}, {0.5f, 0.5f} });
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

	m_constantBufferCamera.Data.Projection = Matrix::CreatePerspectiveFieldOfView(75.0f * XM_PI / 180.0f, (float)width / (float)height, 0.01f, 100.0f).Transpose();
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
