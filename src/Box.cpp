#include <RenderingPipelineDX11PCH.h>
#include "Box.h"
#include "BindableBase.h"

Box::Box(
	Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist
	) : r(rdist(rng)), 
	droll(ddist(rng)), dpitch(ddist(rng)), dyaw(ddist(rng)), 
	dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)),
	chi(adist(rng)), theta(adist(rng)), phi(adist(rng))
{

	namespace DX11 = DirectX;

	if (!IsStaticInitialized())
	{
		// Binding a VertexBuffer with box triangles

		struct Vertex
		{
			DX11::XMFLOAT3 Position;
		};

		const std::vector<Vertex> vertices =
		{
			{ DX11::XMFLOAT3(-1.0f, -1.0f, -1.0f)},// DX11::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
			{ DX11::XMFLOAT3(1.0f,  -1.0f, -1.0f)},// DX11::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
			{ DX11::XMFLOAT3(-1.0f,  1.0f, -1.0f)},// DX11::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
			{ DX11::XMFLOAT3(1.0f, 1.0f, -1.0f)},// DX11::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
			{ DX11::XMFLOAT3(-1.0f, -1.0f,  1.0f)},// DX11::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
			{ DX11::XMFLOAT3(1.0f,  -1.0f,  1.0f)},// DX11::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
			{ DX11::XMFLOAT3(-1.0f,  1.0f,  1.0f)},// DX11::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
			{ DX11::XMFLOAT3(1.0f, 1.0f,  1.0f)}, //DX11::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
		};


		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		// Binding a index buffer for triangle sorting

		const std::vector<unsigned short> indices =
		{
			0, 2, 1,  2, 3, 1,
			1, 3, 5,  3, 7, 5,
			2, 6, 3,  3, 6, 7,
			4, 5, 7,  4, 7, 6,
			0, 4, 2,  2, 4, 6,
			0, 1, 4,  1, 5, 4
		};

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		// Binding Vertex Shader
		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"../VertexShader.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));


		// Binding an Input Layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pVertexShaderByteCode));

		// Binding Primitive Topology
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));



		// Binding a Pixel Constant buffer with face colors
		struct ConstantBuffer2
		{
			DX11::XMFLOAT4 face_colors[6];
		};

		const ConstantBuffer2 cb2 =
		{
			{
				{1.0f, 0.0f, 1.0f, 1.0f},
				{1.0f, 0.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 0.0f, 1.0f},
				{0.0f, 0.0f, 1.0f, 1.0f},
				{1.0f, 1.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 1.0f, 1.0f}
			}
		};

		AddStaticBind(std::make_unique<PixelContextBuffer<ConstantBuffer2>>(gfx, cb2));

		// Binding Pixel Shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"../PixelShader.cso"));
	}
	else
	{
		SetIndexFromStatic();
	}
	
	// Binding the transform constant buffer
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0, 0.0f, 20.0f);
}