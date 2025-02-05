#pragma once

#include "Engine/Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera 
{
private:
	float m_fov;
	float m_nearPlane = 0.01f;
	float m_farPlane = 500.0f;
	
	Vector3 m_camPosition = Vector3(0, 0, 0);
	Quaternion m_camRotation = Quaternion();
	Matrix m_projection;
	Matrix m_view;

	struct MatrixData 
	{
		Matrix ViewMatrix;
		Matrix ProjectionMatrix;
	};
	ConstantBuffer<MatrixData>* m_cameraConstantBuffer = nullptr;
public:
	Camera(float fov, float aspectRatio);
	~Camera();

	void UpdateAspectRatio(float aspectRatio);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse* ms);

	void ApplyCamera(DeviceResources* deviceRes);
};