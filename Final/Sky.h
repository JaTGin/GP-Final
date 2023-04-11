#pragma once
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

#include <memory>
#include <wrl/client.h>
class Sky
{
private:
	std::shared_ptr<SimpleVertexShader> skyboxVS;
	std::shared_ptr<SimplePixelShader> skyboxPS;

	std::shared_ptr<Mesh> skyboxMesh;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyboxRasterizer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skyboxDepth;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyboxSRV;

	// Auth: Chris Cascioli
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
	
	void SetupRenderStates();

public:
	Sky(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap,
		std::shared_ptr<SimpleVertexShader> skyboxVS,
		std::shared_ptr<SimplePixelShader> skyboxPS,
		std::shared_ptr<Mesh> skyboxMesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11Device> device
	);
	Sky(
		const wchar_t* cubeMapFromDDS,
		std::shared_ptr<SimpleVertexShader> skyboxVS,
		std::shared_ptr<SimplePixelShader> skyboxPS,
		std::shared_ptr<Mesh> skyboxMesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11Device> device
	);
	Sky(
		const wchar_t* r,
		const wchar_t* l,
		const wchar_t* u,
		const wchar_t* d,
		const wchar_t* f,
		const wchar_t* b,
		std::shared_ptr<SimpleVertexShader> skyboxVS,
		std::shared_ptr<SimplePixelShader> skyboxPS,
		std::shared_ptr<Mesh> skyboxMesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11Device> device
	);
	~Sky();
	void Draw(std::shared_ptr<Camera> camera);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture();

};

