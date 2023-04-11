#include "GameEntity.h"

// ctors account for varying color inputs
GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, shared_ptr<Material> material) : mesh(mesh), material(material) {}

// getters
shared_ptr<Mesh> GameEntity::GetMesh() {
	return mesh;
}
Transform* GameEntity::GetTransform() {
	return &transform;
}
shared_ptr<Material> GameEntity::GetMaterial() {
	return material;
}

// Setters
void GameEntity::SetMesh(std::shared_ptr<Mesh> mesh) { 
	this->mesh = mesh; 
}
void GameEntity::SetMaterial(std::shared_ptr<Material> material) { 
	this->material = material; 
}

// Draw handles buffers locally to free up space in Game
void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera) {
	material->SetUpShaders(&transform, camera);
	mesh->Draw(context);
}