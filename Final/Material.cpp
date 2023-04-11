#include "Material.h"

// ctor
Material::Material(DirectX::XMFLOAT3 colorTint,
	shared_ptr<SimplePixelShader> pixelShader,
	shared_ptr<SimpleVertexShader> vertexShader,
	float roughness,
	DirectX::XMFLOAT2 uvScale,
	DirectX::XMFLOAT2 uvOffset) :
	colorTint(colorTint),
	pixelShader(pixelShader),
	vertexShader(vertexShader),
	roughness(roughness),
	uvScale(uvScale),
	uvOffset(uvOffset)
{}

// getters
DirectX::XMFLOAT3 Material::GetColor() {
	return colorTint;
}
shared_ptr<SimplePixelShader> Material::GetPixelShader() {
	return pixelShader;
}
shared_ptr<SimpleVertexShader> Material::GetVertexShader() {
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV(std::string name)
{
	auto it = textureSRVs.find(name);
	if (it == textureSRVs.end())
	return it->second;
}
Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::GetSampler(std::string name)
{
	auto it = samplers.find(name);
	if (it == samplers.end())
		return it->second;
}

// setters
void Material::SetColor(DirectX::XMFLOAT3 clr) {
	colorTint = clr;
}
void Material::SetPixelShader(shared_ptr<SimplePixelShader> pxShader) {
	pixelShader = pxShader;
}
void Material::SetVertexShader(shared_ptr<SimpleVertexShader> vtShader) {
	vertexShader = vtShader;
}
void Material::AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ name, srv });
}
void Material::AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ name, sampler });
}
void Material::RemoveTextureSRV(std::string name)
{
	textureSRVs.erase(name);
}
void Material::RemoveSampler(std::string name)
{
	samplers.erase(name);
}

// Configure the shaders
void Material::SetUpShaders(Transform* transform, std::shared_ptr<Camera> camera)
{
	// Activation
	vertexShader->SetShader();
	pixelShader->SetShader();
	
	// Sending data to the shaders and updating buffers via simpleshader
	vertexShader->SetMatrix4x4("world", transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());
	vertexShader->SetMatrix4x4("worldInvTranspose", transform->GetWorldInverseTransposeMatrix());
	vertexShader->CopyAllBufferData();
	
	pixelShader->SetFloat("roughness", roughness);
	pixelShader->SetFloat3("colorTint", colorTint);
	pixelShader->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());
	pixelShader->SetFloat2("uvScale", uvScale);
	pixelShader->SetFloat2("uvOffset", uvOffset);
	pixelShader->CopyAllBufferData();
	
	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second.Get()); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second.Get()); }
}