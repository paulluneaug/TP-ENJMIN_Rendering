#include "pch.h"
#include "Chunk.h"

Chunk::Chunk(Vector3 position)
{
	ModelMatrix = Matrix::CreateTranslation(position);
}

void Chunk::Generate(DeviceResources* deviceRes)
{
	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right);
	PushFace({ 0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward);
	PushFace({ 0.5f, -0.5f, -0.5f }, Vector3::Up, -Vector3::Right);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Up, -Vector3::Forward);

	PushFace({ -0.5f, 0.5f, 0.5f }, Vector3::Forward, Vector3::Right); // Top	
	PushFace({ 0.5f, -0.5f, 0.5f }, Vector3::Forward, -Vector3::Right); // Bottom

	m_vertexBuffer.Create(deviceRes);
	m_indexBuffer.Create(deviceRes);
}

void Chunk::PushFace(Vector3 position, Vector3 up, Vector3 right)
{
	uint32_t a = m_vertexBuffer.PushVertex({ ToVec4(position),				{0.0f, 0.0f} });
	uint32_t b = m_vertexBuffer.PushVertex({ ToVec4(position + right),		{1.0f, 0.0f} });
	uint32_t c = m_vertexBuffer.PushVertex({ ToVec4(position + up),			{0.0f, 1.0f} });
	uint32_t d = m_vertexBuffer.PushVertex({ ToVec4(position + up + right), {1.0f, 1.0f} });

	m_indexBuffer.PushTriangle(a, c, b);
	m_indexBuffer.PushTriangle(b, c, d);
}

void Chunk::Draw(DeviceResources* deviceRes)
{
	m_vertexBuffer.Apply(deviceRes, 0);
	m_indexBuffer.Apply(deviceRes);

	deviceRes->GetD3DDeviceContext()->DrawIndexed(m_indexBuffer.Size(), 0, 0);
}

inline Vector4 Chunk::ToVec4(Vector3 v3)
{
	return Vector4(v3.x, v3.y, v3.z, 1.0f);
}
