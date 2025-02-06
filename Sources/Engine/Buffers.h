#pragma once
#include <pch.h>
using Microsoft::WRL::ComPtr;

template<typename TVertex>
class VertexBuffer {
private:
	ComPtr<ID3D11Buffer> m_buffer;
	std::vector<TVertex> m_data;

public:
	VertexBuffer() {};

	uint32_t PushVertex(TVertex vertex)
	{
		m_data.push_back(vertex);
		return m_data.size() - 1;
	}

	void Create(DeviceResources* deviceRes)
	{
		if (m_data.size() == 0) 
		{
			return;
		}
		CD3D11_BUFFER_DESC bufferDescription
		(
			sizeof(TVertex) * m_data.size(),
			D3D11_BIND_VERTEX_BUFFER
		);

		D3D11_SUBRESOURCE_DATA initalDatas = {};
		initalDatas.pSysMem = m_data.data();

		deviceRes->GetD3DDevice()->CreateBuffer(&bufferDescription, &initalDatas, m_buffer.ReleaseAndGetAddressOf());
	}

	void Apply(DeviceResources* deviceRes, int slot = 0)
	{
		ID3D11Buffer* vbs[] = { m_buffer.Get() };
		const UINT strides[] = { sizeof(TVertex) };
		const UINT offsets[] = { 0 };
		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(slot, 1, vbs, strides, offsets);
	}
};

class IndexBuffer {
private:
	ComPtr<ID3D11Buffer> m_buffer;
	std::vector<uint32_t> m_data;

public:
	IndexBuffer() {};

	void PushTriangle(uint32_t a, uint32_t b, uint32_t c)
	{
		m_data.push_back(a);
		m_data.push_back(b);
		m_data.push_back(c);
	}

	size_t Size() const 
	{
		return m_data.size();
	}

	void Create(DeviceResources* deviceRes)
	{
		if (m_data.size() == 0)
		{
			return;
		}
		CD3D11_BUFFER_DESC bufferDescription
		(
			sizeof(uint32_t) * m_data.size(),
			D3D11_BIND_INDEX_BUFFER
		);

		D3D11_SUBRESOURCE_DATA initalDatas = {};
		initalDatas.pSysMem = m_data.data();

		deviceRes->GetD3DDevice()->CreateBuffer(&bufferDescription, &initalDatas, m_buffer.ReleaseAndGetAddressOf());
	}

	void Apply(DeviceResources* deviceRes)
	{
		ID3D11Buffer* vbs[] = { m_buffer.Get() };
		const UINT strides[] = { sizeof(uint32_t) };
		const UINT offsets[] = { 0 };
		deviceRes->GetD3DDeviceContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
}; 

template<typename TData>
class ConstantBuffer {
public:
	TData Data;

private:
	ComPtr<ID3D11Buffer> m_buffer;


public:
	ConstantBuffer() {};

	void Create(DeviceResources* deviceRes)
	{
		CD3D11_BUFFER_DESC bufferDescription
		(
			sizeof(TData),
			D3D11_BIND_CONSTANT_BUFFER
		);

		D3D11_SUBRESOURCE_DATA initalDatas = {};
		initalDatas.pSysMem = &Data;

		deviceRes->GetD3DDevice()->CreateBuffer(&bufferDescription, &initalDatas, m_buffer.ReleaseAndGetAddressOf());
	}

	void UpdateBuffer(DeviceResources* deviceRes)
	{
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(m_buffer.Get(), 0, nullptr, &Data, 0, 0);
	}

	void ApplyToVS(DeviceResources* deviceRes, int slot = 0)
	{
		ID3D11Buffer* cbs[] = { m_buffer.Get() };
		deviceRes->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
	}
};
