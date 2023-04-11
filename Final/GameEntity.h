#pragma once
#include <memory>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "Transform.h"
#include "Mesh.h"
#include "DXCore.h"
#include "Camera.h"
#include "Material.h"

using namespace std;

class GameEntity
{
private:
	Transform transform;
	shared_ptr<Mesh> mesh;
	shared_ptr<Material> material;
public:
	GameEntity(std::shared_ptr<Mesh> mesh, shared_ptr<Material> material);
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();
	shared_ptr<Material> GetMaterial();
	void SetMaterial(shared_ptr<Material> material);
	void SetMesh(shared_ptr<Mesh> mesh);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);
};

