#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include "Transform.h"
#include "Camera.h"
#include <DirectXMath.h>
#include <memory>

using namespace std;


class Material
{
private:
	DirectX::XMFLOAT3 colorTint;
	shared_ptr<SimplePixelShader> pixelShader;
	shared_ptr<SimpleVertexShader> vertexShader;
	float roughness;

	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT2 uvScale;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
public:
	Material(DirectX::XMFLOAT3 colorTint,
		shared_ptr<SimplePixelShader> pixelShader,
		shared_ptr<SimpleVertexShader> vertexShader,
		float roughness,
		DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2(1, 1),
		DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2(0, 0));
	DirectX::XMFLOAT3 GetColor();
	shared_ptr<SimplePixelShader> GetPixelShader();
	shared_ptr<SimpleVertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV(std::string name);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSampler(std::string name);
	void SetColor(DirectX::XMFLOAT3 clr);
	void SetPixelShader(shared_ptr<SimplePixelShader> pxShader);
	void SetVertexShader(shared_ptr<SimpleVertexShader> vtShader);

	void AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void RemoveTextureSRV(std::string name);
	void RemoveSampler(std::string name);

	void SetUpShaders(Transform* transform, std::shared_ptr<Camera> camera);
};

