#include "pch.h"

#include "Cube.h"

Cube::Cube(Vector3 pos) {
	model = Matrix::CreateTranslation(pos);
}

void Cube::Generate(DeviceResources* deviceRes) {
	auto& data = BlockData::Get(id);
	float uvxSide = (data.texIdSide % 16) / BLOCK_TEXSIZE;
	float uvySide = (data.texIdSide / 16) / BLOCK_TEXSIZE;

	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right, data.texIdSide);
	PushFace({  0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward, data.texIdSide);
	PushFace({  0.5f, -0.5f,-0.5f }, Vector3::Up, Vector3::Left, data.texIdSide);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Up, Vector3::Backward, data.texIdSide);
	PushFace({ -0.5f,  0.5f, 0.5f }, Vector3::Forward, Vector3::Right, data.texIdTop);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Backward, Vector3::Right, data.texIdBottom);
	
	vb.Create(deviceRes);
	ib.Create(deviceRes);
}

void Cube::Draw(DeviceResources* deviceRes) {
	vb.Apply(deviceRes, 0);
	ib.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(ib.Size(), 0, 0);
}

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}


void Cube::PushFace(Vector3 pos, Vector3 up, Vector3 right, int id) {
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto a = vb.PushVertex({ ToVec4(pos), uv + Vector2::UnitY * BLOCK_TEXSIZE });
	auto b = vb.PushVertex({ ToVec4(pos + up), uv });
	auto c = vb.PushVertex({ ToVec4(pos + right), uv + Vector2::UnitX * BLOCK_TEXSIZE + Vector2::UnitY * BLOCK_TEXSIZE });
	auto d = vb.PushVertex({ ToVec4(pos + up + right), uv + Vector2::UnitX * BLOCK_TEXSIZE });
	ib.PushTriangle(a, b, c);
	ib.PushTriangle(c, b, d);
}
