#pragma once
#include <RenderingPipelineDX11PCH.h>
#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx, std::vector<V> vertices)
		: stride(sizeof(V))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC vBufferDesc = {};
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.CPUAccessFlags = 0u;
		vBufferDesc.MiscFlags = 0u;
		vBufferDesc.ByteWidth = UINT(sizeof(V) * vertices.size());
		vBufferDesc.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA vSubresourceData = {};
		vSubresourceData.pSysMem = vertices.data();

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vBufferDesc, &vSubresourceData, &pVertexBuffer));
	}
	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};
 

