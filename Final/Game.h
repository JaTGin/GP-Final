#pragma once

#include "DXCore.h"
#include "GameEntity.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Sky.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>

using namespace std;

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	void CalcPostProcessing();
	void PreProcess();
	void PostProcess();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	shared_ptr<SimplePixelShader> pixelShader;
	shared_ptr<SimplePixelShader> celPixelShader;
	shared_ptr<SimplePixelShader> depthNormalPixelShader;

	shared_ptr<SimpleVertexShader> vertexShader;
	shared_ptr<SimpleVertexShader> triangleVertexShader;

	std::shared_ptr<Camera> camera;

	std::vector<std::shared_ptr<GameEntity>> entities;

	DirectX::XMFLOAT3 ambient;

	std::vector<Light> lights;
	int lightCount;
	float angle;
	bool isPaused;

	std::shared_ptr<Sky> sky;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> clamp;

	// Post-processing stuff
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> postProcessingRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> postProcessingSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> depthRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> normalsRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalsSRV;
};

