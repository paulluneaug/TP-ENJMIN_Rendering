//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <iostream>

#include "Engine/Shader.h"
#include "PerlinNoise.hpp"
#include "Engine/Shader.h"
#include "Engine/VertexLayout.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

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

// a terme a mettre dans une class Camera:
ModelData m_modelDatas;
CameraData m_cameraDatas;

ComPtr<ID3D11Buffer> m_vertexBuffer;
ComPtr<ID3D11Buffer> m_indexBuffer;
ComPtr<ID3D11Buffer> m_constantBufferModel;
ComPtr<ID3D11Buffer> m_constantBufferCamera;

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
	GenerateInputLayout<VertexLayout_Position>(m_deviceResources.get(), basicShader);

	m_cameraDatas.Projection = Matrix::CreatePerspectiveFieldOfView(75.0f * XM_PI / 180.0f, (float)width / (float)height, 0.01f, 100.0f).Transpose();

	auto device = m_deviceResources->GetD3DDevice();

	{
		CD3D11_BUFFER_DESC descModel(sizeof(ModelData), D3D11_BIND_CONSTANT_BUFFER);
		device->CreateBuffer(
			&descModel, nullptr,
			m_constantBufferModel.ReleaseAndGetAddressOf()
		);
		CD3D11_BUFFER_DESC descCamera(sizeof(CameraData), D3D11_BIND_CONSTANT_BUFFER);
		device->CreateBuffer(
			&descCamera, nullptr,
			m_constantBufferCamera.ReleaseAndGetAddressOf()
		);
	}

	CreateCircle(device, 0.0f, 0.0f, 0.4f, 200);
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
	float cameraRadius = 1.0f;

	float seconds = timer.GetTotalSeconds();
	Vector3 position = Vector3
	{
		std::cosf(seconds * cameraSpeed) * cameraRadius,
		std::sinf(seconds * cameraSpeed) * cameraRadius,
		2.0f
	};
	m_cameraDatas.View = Matrix::CreateLookAt(position, Vector3::Zero, Vector3::Up).Transpose();


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
	
	ApplyInputLayout<VertexLayout_Position>(m_deviceResources.get());

	basicShader->Apply(m_deviceResources.get());

	// Update matrix
	context->UpdateSubresource(m_constantBufferModel.Get(), 0, nullptr, &m_modelDatas, 0, 0);
	context->UpdateSubresource(m_constantBufferCamera.Get(), 0, nullptr, &m_cameraDatas, 0, 0);

	ID3D11Buffer* cbs[] = { m_constantBufferModel.Get(), m_constantBufferCamera.Get() };
	context->VSSetConstantBuffers(0, 2, cbs);

	// TP: Tracer votre vertex buffer ici
	ID3D11Buffer* vbs[] = { m_vertexBuffer.Get() };
	const UINT strides[] = { sizeof(VertexLayout_Position) };
	const UINT offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(m_indicesCount * 3, 0, 0);

	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
}

void Game::CreateCircle(ID3D11Device1* device, float cx, float cy, float radius, int resolution)
{
	resolution = std::max(3, resolution);

	m_verticesCount = resolution + 1;
	m_indicesCount = resolution;

	float* vertexData = new float[m_verticesCount * 4];
	UINT* indexData = new UINT[m_indicesCount * 3];

	float angleStep = XM_2PI / resolution;

	int vertexOffset = 0;
	int indexOffset = 0;

	printf("AngleStep : %f", angleStep);

	for (int i = 0; i < resolution; ++i)
	{
		float angle = angleStep * i;
		float x = std::cosf(angle) * radius + cx;
		float y = std::sinf(angle) * radius + cy;

		std::cout << "X : " << x << std::endl;

		vertexData[vertexOffset++] = x;
		vertexData[vertexOffset++] = y;
		vertexData[vertexOffset++] = 0.0f;
		vertexData[vertexOffset++] = 1.0f;

		indexData[indexOffset++] = resolution;
		indexData[indexOffset++] = (i + 1) % resolution;
		indexData[indexOffset++] = (i + 0) % resolution;
	}

	vertexData[vertexOffset++] = cx;
	vertexData[vertexOffset++] = cy;
	vertexData[vertexOffset++] = 0.0f;
	vertexData[vertexOffset++] = 1.0f;

	// Vertex Buffer
	CD3D11_BUFFER_DESC vertexBufferDescription{ m_verticesCount * UINT(sizeof(float)), D3D11_BIND_VERTEX_BUFFER };

	D3D11_SUBRESOURCE_DATA vertexSubResData = {};
	vertexSubResData.pSysMem = vertexData;

	device->CreateBuffer(&vertexBufferDescription, &vertexSubResData, m_vertexBuffer.ReleaseAndGetAddressOf());

	// Index Buffer
	CD3D11_BUFFER_DESC indexBufferDescription{ m_indicesCount * UINT(sizeof(UINT)), D3D11_BIND_INDEX_BUFFER };

	D3D11_SUBRESOURCE_DATA indexSubResData = {};
	indexSubResData.pSysMem = indexData;

	device->CreateBuffer(&indexBufferDescription, &indexSubResData, m_indexBuffer.ReleaseAndGetAddressOf());

	delete[] vertexData;
	delete[] indexData;
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

	m_cameraDatas.Projection = Matrix::CreatePerspectiveFieldOfView(75.0f * XM_PI / 180.0f, (float)width / (float)height, 0.01f, 100.0f).Transpose();
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
