#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"
#include "Material.h"
#include "WICTextureLoader.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <iostream>

#define PI 3.14159265359

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true),				// Show extra stats (fps) in title bar?
	ambient(0.5f, 0.5f, 0.5f)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();

	// Creating the camera
	camera = std::make_shared<Camera>(0.0f, 10.0f, -55.0f, (float)windowWidth / windowHeight, 5.0f, 5.0f, XM_PI / 3, 0.01f, 150.0f, true);
	
	// Set up the pause toggle
	isPaused = false;

	// ImGui stuff
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	ImGui::StyleColorsDark();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	// unsigned int size = sizeof(VertexShaderExternalData);
	// size = (size + 15) / 16 * 16;
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// Setting up shaders
	vertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader.cso").c_str());
	triangleVertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"TriangleVS.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader.cso").c_str());
	celPixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"CelShadingPixel.cso").c_str());
	depthNormalPixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"DepthNormalPS.cso").c_str());

	CalcPostProcessing();
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&sampDesc, clamp.GetAddressOf());

	// Importing primitives from Assets
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/cube.obj").c_str(), device);
	std::shared_ptr<Mesh> cylinderMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/cylinder.obj").c_str(), device);
	std::shared_ptr<Mesh> helixMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/helix.obj").c_str(), device);
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/sphere.obj").c_str(), device);
	std::shared_ptr<Mesh> torusMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/torus.obj").c_str(), device);
	std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/quad.obj").c_str(), device);
	std::shared_ptr<Mesh> quad2sidedMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/quad_double_sided.obj").c_str(), device);

	// Creating our lights
	{
		lightCount = 5;

		Light light1 = {};
		light1.Type = LIGHT_TYPE_DIRECTIONAL;
		light1.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
		light1.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		light1.Intensity = 0.0f;
		lights.push_back(light1);

		Light light2 = {};
		light2.Type = LIGHT_TYPE_DIRECTIONAL;
		light2.Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
		light2.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		light2.Intensity = 0.0f;
		lights.push_back(light2);

		Light light3 = {};
		light3.Type = LIGHT_TYPE_POINT;
		light3.Position = XMFLOAT3(3.0f, 3.0f, 0.0f);
		light3.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		light3.Intensity = 0.0f;
		lights.push_back(light3);

		Light light4 = {};
		light4.Type = LIGHT_TYPE_POINT;
		light4.Position = XMFLOAT3(-7.0f, 1.7f, -6.5f);
		light4.Range = 0.0f;
		light4.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		light4.Intensity = 0.0f;
		lights.push_back(light4);

		Light light5 = {};
		light5.Type = LIGHT_TYPE_POINT;
		light5.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		light5.Range = 100.0f;
		light5.Color = XMFLOAT3(1.0f, 0.5f, 0.8f);
		light5.Intensity = 1.0f;
		lights.push_back(light5);
	}

	// Sky Texturing
	{
		std::shared_ptr<SimpleVertexShader> skyboxVS = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"SkyboxVS.cso").c_str());
		std::shared_ptr<SimplePixelShader> skyboxPS = std::make_shared<SimplePixelShader>(device, context, FixPath(L"SkyboxPS.cso").c_str());

		// Skybox Auth: StumpyStrust on OpenGameArt.org
		//	- https://opengameart.org/content/space-skyboxes-0
		sky = std::make_shared<Sky>(
			FixPath(L"../../Assets/skies/planet/right.png").c_str(),
			FixPath(L"../../Assets/skies/planet/left.png").c_str(),
			FixPath(L"../../Assets/skies/planet/up.png").c_str(),
			FixPath(L"../../Assets/skies/planet/down.png").c_str(),
			FixPath(L"../../Assets/skies/planet/front.png").c_str(),
			FixPath(L"../../Assets/skies/planet/back.png").c_str(),
			skyboxVS,
			skyboxPS,
			cubeMesh,
			sampler,
			context,
			device);
	}

	// Texture Stuff
	{
		// Loading Textures
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet1AlbedoSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-1.png").c_str(), 0, planet1AlbedoSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet1NormalsSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-1-normal.png").c_str(), 0, planet1NormalsSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet1RoughnessSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-1-roughness.png").c_str(), 0, planet1RoughnessSRV.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet2AlbedoSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-2.png").c_str(), 0, planet2AlbedoSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet2NormalsSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-2-normal.png").c_str(), 0, planet2NormalsSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet2RoughnessSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-2-roughness.png").c_str(), 0, planet2RoughnessSRV.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet3AlbedoSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-3.png").c_str(), 0, planet3AlbedoSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet3NormalsSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-3-normal.png").c_str(), 0, planet3NormalsSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet3RoughnessSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-3-roughness.png").c_str(), 0, planet3RoughnessSRV.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet4AlbedoSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-4.png").c_str(), 0, planet4AlbedoSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet4NormalsSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-4-normal.png").c_str(), 0, planet4NormalsSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planet4RoughnessSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/planet-texture-4-roughness.png").c_str(), 0, planet4RoughnessSRV.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sunAlbedoSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/sun-texture.png").c_str(), 0, sunAlbedoSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sunNormalsSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/flat_normals.png").c_str(), 0, sunNormalsSRV.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sunRoughnessSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/planets/sun-texture-roughness.png").c_str(), 0, sunRoughnessSRV.GetAddressOf());


		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rampSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rampSpecSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/ramps/ramptexture3.png").c_str(), 0, rampSRV.GetAddressOf());
		CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/ramps/specramptexture.png").c_str(), 0, rampSpecSRV.GetAddressOf());

		



		// Creating materials

		shared_ptr<Material> matPlanet1 = make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), celPixelShader, vertexShader, 0.8f);
		matPlanet1->AddSampler("Sampler", sampler);
		matPlanet1->AddSampler("Clamp", clamp);
		matPlanet1->AddTextureSRV("Albedo", planet1AlbedoSRV);
		matPlanet1->AddTextureSRV("NormalMap", planet1NormalsSRV);
		matPlanet1->AddTextureSRV("RoughnessMap", planet1RoughnessSRV);

		shared_ptr<Material> matPlanet2 = make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), celPixelShader, vertexShader, 0.8f);
		matPlanet2->AddSampler("Sampler", sampler);
		matPlanet2->AddSampler("Clamp", clamp);
		matPlanet2->AddTextureSRV("Albedo", planet2AlbedoSRV);
		matPlanet2->AddTextureSRV("NormalMap", planet2NormalsSRV);
		matPlanet2->AddTextureSRV("RoughnessMap", planet2RoughnessSRV);

		shared_ptr<Material> matPlanet3 = make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), celPixelShader, vertexShader, 0.8f);
		matPlanet3->AddSampler("Sampler", sampler);
		matPlanet3->AddSampler("Clamp", clamp);
		matPlanet3->AddTextureSRV("Albedo", planet3AlbedoSRV);
		matPlanet3->AddTextureSRV("NormalMap", planet3NormalsSRV);
		matPlanet3->AddTextureSRV("RoughnessMap", planet3RoughnessSRV);

		shared_ptr<Material> matPlanet4 = make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), celPixelShader, vertexShader, 0.8f);
		matPlanet4->AddSampler("Sampler", sampler);
		matPlanet4->AddSampler("Clamp", clamp);
		matPlanet4->AddTextureSRV("Albedo", planet4AlbedoSRV);
		matPlanet4->AddTextureSRV("NormalMap", planet4NormalsSRV);
		matPlanet4->AddTextureSRV("RoughnessMap", planet4RoughnessSRV);

		shared_ptr<Material> matSun = make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), celPixelShader, vertexShader, 0.8f);
		matSun->AddSampler("Sampler", sampler);
		matSun->AddSampler("Clamp", clamp);
		matSun->AddTextureSRV("Albedo", sunAlbedoSRV);
		matSun->AddTextureSRV("NormalMap", sunNormalsSRV);
		matSun->AddTextureSRV("RoughnessMap", sunRoughnessSRV);


		// Creating a set of entities
		entities.push_back(std::make_shared<GameEntity>(sphereMesh, matSun));
		entities.push_back(std::make_shared<GameEntity>(sphereMesh, matPlanet1));
		entities.push_back(std::make_shared<GameEntity>(sphereMesh, matPlanet2));
		entities.push_back(std::make_shared<GameEntity>(sphereMesh, matPlanet3));
		entities.push_back(std::make_shared<GameEntity>(sphereMesh, matPlanet4));

		entities[0]->GetTransform()->Scale(10, 10, 10);
		entities[1]->GetTransform()->Scale(1.2f, 1.2f, 1.2f);
		entities[1]->GetTransform()->MoveAbsolute(15, 0, 0);
		entities[2]->GetTransform()->Scale(5, 5, 5);
		entities[2]->GetTransform()->MoveAbsolute(20, 0, 0);
		entities[3]->GetTransform()->MoveAbsolute(30, 0, 0);
		entities[4]->GetTransform()->Scale(3, 3, 3);
		entities[4]->GetTransform()->MoveAbsolute(50, 0, 0);
	}
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Adjust the camera for resizing
	if (camera) camera->UpdateProjectionMatrix((float)windowWidth / windowHeight);

	CalcPostProcessing();
}

// Adjust necessary post-processing resources when the window resizes and on startup
void Game::CalcPostProcessing() {
	// Discard ancy existing values
	postProcessingRTV.Reset();
	postProcessingSRV.Reset();
	normalsRTV.Reset();
	normalsSRV.Reset();
	depthRTV.Reset();
	depthSRV.Reset();

	// Describe the new textures
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	// Set up textures
	Microsoft::WRL::ComPtr<ID3D11Texture2D> postProcessingTexture;
	device->CreateTexture2D(&textureDesc, 0, postProcessingTexture.GetAddressOf());

	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> normalsTexture;
	device->CreateTexture2D(&textureDesc, 0, normalsTexture.GetAddressOf());

	textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthsTexture;
	device->CreateTexture2D(&textureDesc, 0, depthsTexture.GetAddressOf());

	// Setting up RTVs
	device->CreateRenderTargetView(postProcessingTexture.Get(), 0, postProcessingRTV.GetAddressOf());
	device->CreateRenderTargetView(normalsTexture.Get(), 0, normalsRTV.GetAddressOf());
	device->CreateRenderTargetView(depthsTexture.Get(), 0, depthRTV.GetAddressOf());

	// Same for SRVs
	device->CreateShaderResourceView(postProcessingTexture.Get(), 0, postProcessingSRV.GetAddressOf());
	device->CreateShaderResourceView(normalsTexture.Get(), 0, normalsSRV.GetAddressOf());
	device->CreateShaderResourceView(depthsTexture.Get(), 0, depthSRV.GetAddressOf());
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	// Handle planetary motion
	if (!isPaused) {
		if (angle >= 360.0f) angle = 0.0f;
		for (int i = 1; i < entities.size(); i++) {
			entities[1]->GetTransform()->Rotate(0, -0.001f, 0);
			float radius = 0.0f;
			if (i == 1) radius = 15.0f;
			else if (i == 2) radius = 40.0f;
			else if (i == 3) radius = 60.0f;
			else if (i == 4) radius = 75.0f;
			XMFLOAT3 offset = XMFLOAT3(sin((angle * i) * (PI / 180)) * radius, 0.0f, cos((angle * i) * (PI / 180)) * radius);
			// std::cout << offset.x << " " << offset.y << " " << offset.z << endl;
			std::cout << sin(angle * (PI / 180)) << endl;
			entities[i]->GetTransform()->SetPosition(offset.x, offset.y, offset.z);
		}
		angle += 20.0f * deltaTime;
	}

	camera->Update(deltaTime);

	// ImGui
	{
		// Get a reference to our custom input manager
		Input& input = Input::GetInstance();
		// Reset input manager's gui state so we don’t
		// taint our own input (you’ll uncomment later)
		input.SetKeyboardCapture(false);
		input.SetMouseCapture(false);
		// Feed fresh input data to ImGui
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = deltaTime;
		io.DisplaySize.x = (float)this->windowWidth;
		io.DisplaySize.y = (float)this->windowHeight;
		io.KeyCtrl = input.KeyDown(VK_CONTROL);
		io.KeyShift = input.KeyDown(VK_SHIFT);
		io.KeyAlt = input.KeyDown(VK_MENU);
		io.MousePos.x = (float)input.GetMouseX();
		io.MousePos.y = (float)input.GetMouseY();
		io.MouseDown[0] = input.MouseLeftDown();
		io.MouseDown[1] = input.MouseRightDown();
		io.MouseDown[2] = input.MouseMiddleDown();
		io.MouseWheel = input.GetMouseWheel();
		input.GetKeyArray(io.KeysDown, 256);
		// Reset the frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// Determine new input capture (you’ll uncomment later)
		input.SetKeyboardCapture(io.WantCaptureKeyboard);
		input.SetMouseCapture(io.WantCaptureMouse);
		// Show the demo window
		// ImGui::ShowDemoWindow();

		ImGui::Begin("System Stats");
		ImGui::Text("fps: %f", io.Framerate);
		ImGui::Text("Window Width: %f", io.DisplaySize.x);
		ImGui::Text("Window Height: %f", io.DisplaySize.y);
		ImGui::End();
		ImGui::Begin("Orbit Controller");
		XMFLOAT3 pos = entities[0]->GetTransform()->GetPosition();
		ImGui::Checkbox("Toggle Orbit", &isPaused);
		ImGui::End();
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// 
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	PreProcess();
	/*
	context->VSSetConstantBuffers(
		0, // Which slot (register) to bind the buffer to?
		1, // How many are we activating? Can do multiple at once
		vsConstantBuffer.GetAddressOf());
	*/
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rampSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rampSpecSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/ramps/ramptexture3.png").c_str(), 0, rampSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/ramps/specramptexture.png").c_str(), 0, rampSpecSRV.GetAddressOf());

	// Draw loop
	for (auto& e : entities) {
		// Setting material properties that need to be updated with data from Game
		e->GetMaterial()->GetPixelShader()->SetFloat("time", totalTime);
		e->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambient);
		e->GetMaterial()->GetPixelShader()->SetInt("lightCount", lightCount);
		e->GetMaterial()->GetPixelShader()->SetData(
			"lights", // The name of the (eventual) variable in the shader
			&lights[0], // The address of the data to set
			sizeof(Light) * (int)lights.size()); // The size of the data (the whole struct!) to set
		e->GetMaterial()->GetPixelShader()->SetShaderResourceView("CelRamp", rampSRV);
		e->GetMaterial()->GetPixelShader()->SetShaderResourceView("CelRampSpec", rampSpecSRV);
		e->Draw(context, camera);
	}

	sky->Draw(camera);

	PostProcess();
	// ImGui
	{
		// Draw ImGui
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		swapChain->Present(vsync ? 1 : 0, 0);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());

	}
}

// Handle anything that needs to be done immediately before before draw code happens
void Game::PreProcess() {
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*

	// Clear the back buffer (erases what's on the screen)
	const float bgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

	// Clear the depth buffer (resets per-pixel occlusion information)
	context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Clear and set RTVs
	context->ClearRenderTargetView(postProcessingRTV.Get(), bgColor);
	context->ClearRenderTargetView(normalsRTV.Get(), bgColor);
	context->ClearRenderTargetView(depthRTV.Get(), bgColor);

	ID3D11RenderTargetView* rtvs[3] =
	{
		postProcessingRTV.Get(),
		normalsRTV.Get(),
		depthRTV.Get()
	};

	context->OMSetRenderTargets(3, rtvs, depthBufferDSV.Get());
}

// Handle anything that gets applied right after draw code
void Game::PostProcess() {
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), 0);
	triangleVertexShader->SetShader();

	depthNormalPixelShader->SetShaderResourceView("Pixels", postProcessingSRV.Get());
	depthNormalPixelShader->SetShaderResourceView("Normals", normalsSRV.Get());
	depthNormalPixelShader->SetShaderResourceView("Depth", depthSRV.Get());
	depthNormalPixelShader->SetSamplerState("Sampler", clamp.Get());
	depthNormalPixelShader->SetShader();
	depthNormalPixelShader->SetFloat("width", 1.0f / windowWidth);
	depthNormalPixelShader->SetFloat("height", 1.0f / windowHeight);
	depthNormalPixelShader->SetFloat("normal", 5.0f);
	depthNormalPixelShader->SetFloat("depth", 5.0f);
	depthNormalPixelShader->CopyAllBufferData();
	context->Draw(3, 0);

	ID3D11ShaderResourceView* clearSRVs[128] = {};
	context->PSSetShaderResources(0, ARRAYSIZE(clearSRVs), clearSRVs);
}