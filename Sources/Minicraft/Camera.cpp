#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : m_fov(fov) {
	// TP initialiser matrices
	m_nearPlane = 0.01f;
	m_farPlane = 1000.0f;

	m_projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, m_nearPlane, m_farPlane);
	m_view = Matrix::CreateFromQuaternion(m_camRotation) * Matrix::CreateTranslation(m_camPosition);

}

Camera::~Camera() {
	if (m_cameraConstantBuffer) delete m_cameraConstantBuffer;
	m_cameraConstantBuffer = nullptr;
}

void Camera::UpdateAspectRatio(float aspectRatio)
{
	// TP updater matrice proj
	m_projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse) {
	float camSpeedRot = 0.25f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mouseState = mouse->GetState();
	const Matrix inverseView = m_view.Invert();

	Vector3 localRight = Vector3::TransformNormal(Vector3::Right, inverseView);
	Vector3 localUp = Vector3::TransformNormal(Vector3::Up, inverseView);
	Vector3 localForward = Vector3::TransformNormal(Vector3::Forward, inverseView);

	// TP: deplacement par clavier 
	Vector2 keyboardInput =
	{
		float((kb.Q ? -1 : 0) + (kb.D ? 1 : 0)),
		float((kb.S ? -1 : 0) + (kb.Z ? 1 : 0))
	};
	m_camPosition += (localRight * keyboardInput.x + localForward * keyboardInput.y) * (camSpeed * dt);

	// astuce: Vector3::TransformNormal(vecteur, im); transforme un vecteur de l'espace cameravers l'espace monde

	if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mouseState.x;
		float dy = mouseState.y;
		if (mouseState.rightButton)
		{
			// TP Translate camera a partir de dx/dy
			m_camPosition += (-localRight * dx + localUp * dy) * camSpeedMouse * dt;
		}
		else if (mouseState.leftButton)
		{
			// TP Rotate camera a partir de dx/dy
			m_camRotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, dx * camSpeedRot * dt) * Quaternion::CreateFromAxisAngle(localRight, dy * camSpeedRot * dt);
		}
		else
		{
			mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
	}
	else if (mouseState.rightButton || mouseState.leftButton)
	{
		mouse->SetMode(Mouse::MODE_RELATIVE);
	}

	Vector3 newForward = Vector3::Transform(Vector3::Forward, m_camRotation);
	Vector3 newUp = Vector3::Transform(Vector3::Up, m_camRotation);

	// TP updater matrice view
	m_view = Matrix::CreateLookAt(m_camPosition, m_camPosition + newForward, newUp);
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!m_cameraConstantBuffer) {
		m_cameraConstantBuffer = new ConstantBuffer<MatrixData>();
		m_cameraConstantBuffer->Create(deviceRes);
	}

	m_cameraConstantBuffer->Data.ProjectionMatrix = m_projection.Transpose();
	m_cameraConstantBuffer->Data.ViewMatrix = m_view.Transpose();


	m_cameraConstantBuffer->UpdateBuffer(deviceRes);
	m_cameraConstantBuffer->ApplyToVS(deviceRes, 1);

	// TP envoyer data
}
