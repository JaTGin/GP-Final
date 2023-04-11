#pragma once
#include <DirectXMath.h>
#include "transform.h"

class Camera
{
private:
	Transform transform;
	DirectX::XMFLOAT4X4 view, projection;
	float aspectRatio, fov, nearClip, farClip, moveSpeed, mouseSensitivity;
	bool isPerspective;
public:
	Camera(float x, float y, float z, float aspectRatio, float moveSpeed, float mouseSensitivity, float fov, float nearClip, float farClip, bool isPerspective);
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	Transform* GetTransform();
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};

